/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
***************************************************************************/

#ifndef QDECLARATIVECOORDINATE_H
#define QDECLARATIVECOORDINATE_H

#include <qgeocoordinate.h>
#include <QtDeclarative/qdeclarative.h>

#include <QObject>

QT_BEGIN_NAMESPACE

class QDeclarativeCoordinate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude NOTIFY longitudeChanged)
    Q_PROPERTY(double altitude READ altitude WRITE setAltitude NOTIFY altitudeChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY validityChanged)

public:
    QDeclarativeCoordinate(QObject* parent = 0);
    QDeclarativeCoordinate(const QGeoCoordinate &coordinate, QObject* parent = 0);
    ~QDeclarativeCoordinate();

    Q_INVOKABLE qreal distanceTo(QObject* coordinate);
    Q_INVOKABLE qreal azimuthTo(QObject *coordinate);

    Q_INVOKABLE QDeclarativeCoordinate *atDistanceAndAzimuth(qreal distance, qreal azimuth);

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);

    double latitude() const;
    void setLatitude(double latitude);

    double longitude() const;
    void setLongitude(double longitude);

    double altitude() const;
    void setAltitude(double altitude);

    bool isValid() const;

Q_SIGNALS:
    void latitudeChanged(double latitude);
    void longitudeChanged(double longitude);
    void altitudeChanged(double altitude);
    void validityChanged(bool valid);
    void coordinateChanged(const QGeoCoordinate &coord);

private:
    QGeoCoordinate m_coordinate;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeCoordinate));

#endif
