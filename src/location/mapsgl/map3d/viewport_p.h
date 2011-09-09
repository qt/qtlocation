/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef VIEWPORT_P_H
#define VIEWPORT_P_H

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

#include "spheregeometry_p.h"
#include "cameradata.h"
#include "tilespec.h"
#include "frustum_p.h"

#include "viewportcamera_p.h"
#include "viewporttiles_p.h"

#include <QList>
#include <QVector2D>
#include <QVector3D>
#include <Qt3D/qplane3d.h>

QT_BEGIN_NAMESPACE

class QGLCamera;
class QGLSceneNode;

class Arc;

class Projection3D;

class Q_LOCATION_EXPORT Viewport
{
public:
    Viewport();
    ~Viewport();

    void setProjection(Projection3D *projection);

    void setZoomLevel(int zoomLevel);
    int zoomLevel() const;

    void setCameraData(const CameraData &cameraData);

    QGLCamera* camera() const;

    QList<TileSpec> visibleTiles() const;

private:
    void calculateVisibleTiles();

    ViewportCamera viewportCamera_;
    ViewportTiles viewportTiles_;

    QList<TileSpec> visibleTiles_;
};

QT_END_NAMESPACE

#endif // VIEWPORT_P_H

