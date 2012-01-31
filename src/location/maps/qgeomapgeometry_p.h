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
#ifndef QGEOMAPGEOMETRY_P_H
#define QGEOMAPGEOMETRY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QSet>
#include <QSize>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

class QGeoCameraData;
class QGeoTileSpec;
class QGeoProjection;

class QDoubleVector2D;

class QGLSceneNode;
class QGLCamera;
class QGLPainter;
class QGLTexture2D;

class QPointF;

class QGeoMapGeometryPrivate;

class QGeoMapGeometry {
public:
    QGeoMapGeometry(QSharedPointer<QGeoProjection> projection);
    ~QGeoMapGeometry();

    void setScreenSize(const QSize &size);
    void setTileSize(int tileSize);
    void setCameraData(const QGeoCameraData &cameraData_);
    void setVisibleTiles(const QSet<QGeoTileSpec> &tiles);

    void setUseVerticalLock(bool lock);

    void addTile(const QGeoTileSpec &spec, QGLTexture2D *texture);

    QDoubleVector2D screenPositionToMercator(const QPointF &pos) const;
    QPointF mercatorToScreenPosition(const QDoubleVector2D &mercator) const;

    QGLCamera *camera() const;
    QGLSceneNode *sceneNode() const;
    void paintGL(QGLPainter *painter);

    bool verticalLock() const;

private:
    QGeoMapGeometryPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoMapGeometry)
};

QT_END_NAMESPACE

#endif // QGEOMAPGEOMETRY_P_H
