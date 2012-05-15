/****************************************************************************
 **
 ** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/
 **
 ** This file is part of the QtLocation module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** GNU Lesser General Public License Usage
 ** This file may be used under the terms of the GNU Lesser General Public
 ** License version 2.1 as published by the Free Software Foundation and
 ** appearing in the file LICENSE.LGPL included in the packaging of this
 ** file. Please review the following information to ensure the GNU Lesser
 ** General Public License version 2.1 requirements will be met:
 ** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights. These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU General
 ** Public License version 3.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of this
 ** file. Please review the following information to ensure the GNU General
 ** Public License version 3.0 requirements will be met:
 ** http://www.gnu.org/copyleft/gpl.html.
 **
 ** Other Usage
 ** Alternatively, this file may be used in accordance with the terms and
 ** conditions contained in a signed written agreement between you and Nokia.
 **
 **
 **
 **
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#ifndef QGEOMAPITEMGEOMETRY_H
#define QGEOMAPITEMGEOMETRY_H

#include <QPainterPath>
#include <QPolygonF>
#include <QPointF>
#include <QRectF>
#include <QVector>
#include <QGeoCoordinate>
#include <QVector2D>
#include <QList>

QT_BEGIN_NAMESPACE

class QSGGeometry;

class QGeoMapItemGeometry : public QObject
{
    Q_OBJECT

public:

    struct Point
    {
        inline Point(qreal x, qreal y) : x(x), y(y) {}
        inline Point() : x(0), y(0) {}
        inline Point(const QPointF &pt) : x(pt.x()), y(pt.y()) {}
        qreal x, y;
    };

    explicit QGeoMapItemGeometry(QObject *parent = 0);

    inline bool isSourceDirty() const { return sourceDirty_; }
    inline bool isScreenDirty() const { return screenDirty_; }
    inline void markSourceDirty() { sourceDirty_ = true; screenDirty_ = true; }
    inline void markScreenDirty() { screenDirty_ = true; }
    inline void markClean() { screenDirty_ = (sourceDirty_ = false); }

    inline QRectF sourceBoundingBox() const { return sourceBounds_; }
    inline QRectF screenBoundingBox() const { return screenBounds_; }

    inline QPointF firstPointOffset() const { return firstPointOffset_; }
    void translate(const QPointF &offset);

    inline const QGeoCoordinate &origin() const { return srcOrigin_; }

    inline bool contains(const QPointF &screenPoint) const {
        return screenOutline_.contains(screenPoint);
    }

    inline QVector2D vertex(quint32 index) const {
        return QVector2D(screenVertices_[index].x, screenVertices_[index].y);
    }

    inline QVector<Point> vertices() const { return screenVertices_; }
    inline QVector<quint32> indices() const { return screenIndices_; }

    inline bool isIndexed() const { return (!screenIndices_.isEmpty()); }

    /* Size is # of triangles */
    inline quint32 size() const
    {
        if (isIndexed())
            return screenIndices_.size() / 3;
        else
            return screenVertices_.size() / 3;
    }

    inline void clear() { firstPointOffset_ = QPointF(0,0);
                          screenVertices_.clear(); screenIndices_.clear(); }

    void allocateAndFill(QSGGeometry *geom) const;

    static QRectF translateToCommonOrigin(const QList<QGeoMapItemGeometry *> &geoms);

protected:
    bool sourceDirty_, screenDirty_;

    QPointF firstPointOffset_;

    QPainterPath screenOutline_;

    QRectF sourceBounds_;
    QRectF screenBounds_;

    QGeoCoordinate srcOrigin_;

    QVector<Point> screenVertices_;
    QVector<quint32> screenIndices_;
};

QT_END_NAMESPACE

#endif // QGEOMAPITEMGEOMETRY_H
