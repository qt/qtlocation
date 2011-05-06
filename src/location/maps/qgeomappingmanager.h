/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOMAPPINGMANAGER_H
#define QGEOMAPPINGMANAGER_H

#include "qgraphicsgeomap.h"

#include <QObject>
#include <QSize>
#include <QPair>

class QLocale;

QTM_BEGIN_NAMESPACE

class QGeoBoundingBox;
class QGeoCoordinate;
class QGeoMapData;
class QGeoMappingManagerPrivate;
class QGeoMapRequestOptions;

class QGeoMappingManagerEngine;

class Q_LOCATION_EXPORT QGeoMappingManager : public QObject
{
    Q_OBJECT

public:
    ~QGeoMappingManager();

    QString managerName() const;
    int managerVersion() const;

    QGeoMapData* createMapData();

    QList<QGraphicsGeoMap::MapType> supportedMapTypes() const;
    QList<QGraphicsGeoMap::ConnectivityMode> supportedConnectivityModes() const;

    qreal minimumZoomLevel() const;
    qreal maximumZoomLevel() const;

    bool supportsBearing() const;

    bool supportsTilting() const;
    qreal minimumTilt() const;
    qreal maximumTilt() const;

    bool supportsCustomMapObjects() const;

    void setLocale(const QLocale &locale);
    QLocale locale() const;

private:
    QGeoMappingManager(QGeoMappingManagerEngine *engine, QObject *parent = 0);

    QGeoMappingManagerPrivate* d_ptr;
    Q_DISABLE_COPY(QGeoMappingManager)

    friend class QGeoServiceProvider;
};

QTM_END_NAMESPACE

#endif
