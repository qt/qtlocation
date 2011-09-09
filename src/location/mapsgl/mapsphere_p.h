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
#ifndef MAPSPHERE_H
#define MAPSPHERE_H

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

#include "qglobal.h"

#include <QObject>
#include <QSet>
#include <QHash>
#include <QList>

#include <QMutex>

QT_BEGIN_NAMESPACE

class QGLSceneNode;

class TileSpec;
class TileCache;

class Map;
class MapPrivate;

class QGeoMappingManager;

class Q_LOCATION_EXPORT MapSphere : public QObject
{
    Q_OBJECT
public:
    MapSphere(Map* map, MapPrivate *mapPrivate, TileCache *tileCache);
    ~MapSphere();

    void setMappingManager(QGeoMappingManager *manager);

    QGLSceneNode* sphereSceneNode() const;

    // TODO Quick temporary thread protection.
    QMutex updateMutex;
    // when running as QML app we can't access GL context anywhere
    // but QSG rendering thread.
    void GLContextAvailable();

public slots:
    void clearCache();
    void update(const QList<TileSpec> &tiles);

private slots:
    void tileFetched(const TileSpec &spec);
    void prefetchingFinished();

signals:
    void tileUpdated();
    void sphereUpdated();

private:
    void displayTile(const TileSpec &spec);

    TileCache *tileCache_;
    int minZoom_;

    QSet<TileSpec> requested_;
    QHash<TileSpec, QGLSceneNode*> built_;

    QGLSceneNode* sphereNode_;

    MapPrivate* mapPrivate_;
    QList<QGLSceneNode*> obsoleteNodes_;
};

QT_END_NAMESPACE

#endif // MAPSPHERE_H
