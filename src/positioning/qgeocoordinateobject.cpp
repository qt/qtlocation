/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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
**
**
**
****************************************************************************/

#include "qgeocoordinateobject_p.h"

QT_BEGIN_NAMESPACE

/*

  Note: This class only purpose is to enable conversion between QGeoCoordinate and QDeclarativeGeoWaypoint.
  Since QGeoCoordinate lives in the QtPositioning module, this class acts as a base for QDeclarativeGeoWaypoint,
  and contains the bare minimum to convert/compare to a QGeoCoordinate

*/

QGeoCoordinateObject::QGeoCoordinateObject(QObject *parent) : QObject(parent)
{
}

QGeoCoordinateObject::QGeoCoordinateObject(const QGeoCoordinate &c, QObject *parent) : QObject(parent)
{
    setCoordinate(c);
}

QGeoCoordinateObject::~QGeoCoordinateObject()
{

}

bool QGeoCoordinateObject::operator==(const QGeoCoordinateObject &other) const
{
    return m_coordinate == other.m_coordinate;
}

bool QGeoCoordinateObject::operator==(const QGeoCoordinate &other) const
{
    return m_coordinate == other;
}

QGeoCoordinate QGeoCoordinateObject::coordinate() const
{
    return m_coordinate;
}

void QGeoCoordinateObject::setCoordinate(const QGeoCoordinate &c)
{
    if (c == m_coordinate)
        return;

    m_coordinate = c;
    emit coordinateChanged();
}

QT_END_NAMESPACE


