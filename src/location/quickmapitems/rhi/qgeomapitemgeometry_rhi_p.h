/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOMAPITEMGEOMETRY_RHI_P_H
#define QGEOMAPITEMGEOMETRY_RHI_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QList>
#include <QtCore/QSharedPointer>
#include <QtGui/QColor>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include <QtLocation/private/qdeclarativegeomapitemutils_p.h>
#include <QtLocation/private/qgeomapitemgeometry_p.h>

QT_BEGIN_NAMESPACE

class QPointF;
class QSGGeometry;
class QGeoCircle;
class QGeoMap;
class QGeoPath;
class QGeoPolygon;
class QGeoProjectionWebMercator;
class QGeoRectangle;

class QGeoMapItemLODGeometry
{
    Q_DISABLE_COPY(QGeoMapItemLODGeometry);
public:
    mutable std::array<QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>, 7>
            m_verticesLOD; // fix it to 7,
                           // do not allow simplifications beyond ZL 20. This could actually be
                           // limited even further
    mutable QList<QDeclarativeGeoMapItemUtils::vec2> *m_screenVertices;
    mutable QSharedPointer<unsigned int> m_working;

    QGeoMapItemLODGeometry();

    void resetLOD();

    static unsigned int zoomToLOD(unsigned int zoom);

    static unsigned int zoomForLOD(unsigned int zoom);

    bool isLODActive(unsigned int lod) const;

    void selectLOD(unsigned int zoom, double leftBound, bool /*closed*/);

    static QList<QDeclarativeGeoMapItemUtils::vec2>
    getSimplified(QList<QDeclarativeGeoMapItemUtils::vec2> &wrappedPath, double leftBoundWrapped,
                  unsigned int zoom);

    static void enqueueSimplificationTask(
            const QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>
                    &input, // reference as it gets copied in the nested call
            const QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>> &output,
            double leftBound, unsigned int zoom, QSharedPointer<unsigned int> &working);

    void selectLODOnDataChanged(unsigned int zoom, double leftBound) const;
    bool selectLODOnLODMismatch(unsigned int zoom, double leftBound, bool closed) const;
};

class Q_LOCATION_PRIVATE_EXPORT QGeoMapPolylineGeometryOpenGL : public QGeoMapItemGeometry,
                                                                public QGeoMapItemLODGeometry
{
public:
    typedef struct {
        QList<QDoubleVector2D> wrappedBboxes;
    } WrappedPolyline;

    QGeoMapPolylineGeometryOpenGL()
    {
        m_working = QSharedPointer<unsigned int>(new unsigned int(0));
    }

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoPolygon &poly);

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoPath &poly);

    void updateSourcePoints(const QGeoProjectionWebMercator &p,
                            const QList<QDoubleVector2D> &wrappedPath,
                            const QGeoRectangle &boundingRectangle);

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoRectangle &rect);

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoCircle &circle);

    void updateScreenPoints(const QGeoMap &map,
                            qreal strokeWidth,
                            bool adjustTranslation = true);

    void updateQuickGeometry(const QGeoProjectionWebMercator &p, qreal strokeWidth = 0.0);

    bool allocateAndFillEntries(QSGGeometry *geom,
                                bool closed = false,
                                unsigned int zoom = 0) const;
    void allocateAndFillLineStrip(QSGGeometry *geom,
                                  int lod = 0) const;

    bool contains(const QPointF &point) const override
    {
        Q_UNUSED(point);
        return false;
    }

    static double distanceTo(const QDoubleVector2D &a, const QDoubleVector2D &b, const QDoubleVector2D &p)
    {
        double u = ((p.x() - a.x()) * (b.x() - a.x()) + (p.y() - a.y()) * (b.y() - a.y()))
                 / (b - a).lengthSquared();
        QDoubleVector2D intersection(a.x() + u * (b.x() - a.x()) , a.y() + u * (b.y() - a.y()));

        QDoubleVector2D candidate = ( (p-a).length() < (p-b).length() ) ? a : b;

        // And it falls in the segment
        if (u > 0 && u < 1 && (p-intersection).length() < (p-candidate).length()) {
                candidate = intersection;
        }

        return qAbs((candidate - p).length());
    }
    // Note: this is also slightly incorrect on joins and in the beginning/end of the line
    bool contains(const QPointF &point, qreal lineWidth, const QGeoProjectionWebMercator &p) const;

public:
    QDoubleVector2D m_bboxLeftBoundWrapped;
    QList<WrappedPolyline> m_wrappedPolygons;
    int m_wrapOffset;

    friend class QDeclarativeCircleMapItem;
    friend class QDeclarativePolygonMapItem;
    friend class QDeclarativeRectangleMapItem;
};

class Q_LOCATION_PRIVATE_EXPORT QGeoMapPolygonGeometryOpenGL : public QGeoMapItemGeometry
{
public:
    typedef struct {
        QList<QDoubleVector2D> wrappedBboxes;
    } WrappedPolygon;
    QGeoMapPolygonGeometryOpenGL();
    ~QGeoMapPolygonGeometryOpenGL() override {}

    // Temporary method for compatibility in MapCircleObject. Remove when MapObjects are ported.
    void updateSourcePoints(const QGeoMap &map,
                            const QList<QDoubleVector2D> &path);

    void updateSourcePoints(const QGeoMap &map,
                            const QList<QGeoCoordinate> &perimeter);

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoPolygon &poly);

    void updateSourcePoints(const QGeoMap &map,
                            const QGeoRectangle &rect);

    void updateScreenPoints(const QGeoMap &map, qreal strokeWidth = 0.0,
                            const QColor &strokeColor = Qt::transparent);
    void updateQuickGeometry(const QGeoProjectionWebMercator &p, qreal strokeWidth = 0.0);

    void allocateAndFillPolygon(QSGGeometry *geom) const;

    QList<QDeclarativeGeoMapItemUtils::vec2> m_screenVertices;
    QList<quint32> m_screenIndices;
    QDoubleVector2D m_bboxLeftBoundWrapped;
    QList<WrappedPolygon> m_wrappedPolygons;
    int m_wrapOffset = 0;
};

QT_END_NAMESPACE

#endif // QGEOMAPITEMGEOMETRY_RHI_P_H
