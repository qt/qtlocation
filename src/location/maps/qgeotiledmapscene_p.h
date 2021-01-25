/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/
#ifndef QGEOTILEDMAPSCENE_P_H
#define QGEOTILEDMAPSCENE_P_H

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

#include <QObject>
#include <QtLocation/private/qlocationglobal_p.h>
#include <QtPositioning/QGeoCoordinate>

QT_BEGIN_NAMESPACE

class QGeoCameraData;
class QGeoTileSpec;
class QDoubleVector2D;
class QGeoTileTexture;
class QSGNode;
class QQuickWindow;
class QGeoTiledMapScenePrivate;

class Q_LOCATION_PRIVATE_EXPORT QGeoTiledMapScene : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGeoTiledMapScene)
public:
    explicit QGeoTiledMapScene(QObject *parent = 0);
    virtual ~QGeoTiledMapScene();

    void setScreenSize(const QSize &size);
    void setTileSize(int tileSize);
    void setCameraData(const QGeoCameraData &cameraData);
    void setVisibleArea(const QRectF &visibleArea);

    void setVisibleTiles(const QSet<QGeoTileSpec> &tiles);
    const QSet<QGeoTileSpec> &visibleTiles() const;

    void addTile(const QGeoTileSpec &spec, QSharedPointer<QGeoTileTexture> texture);

    QSGNode *updateSceneGraph(QSGNode *oldNode, QQuickWindow *window);

    QSet<QGeoTileSpec> texturedTiles();

    void clearTexturedTiles();

Q_SIGNALS:
    void newTilesVisible(const QSet<QGeoTileSpec> &newTiles);

private:
    void updateSceneParameters();

    Q_DISABLE_COPY(QGeoTiledMapScene)
};

QT_END_NAMESPACE

#endif // QGEOTILEDMAPSCENE_P_H
