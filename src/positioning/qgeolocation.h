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

#ifndef QGEOLOCATION_H
#define QGEOLOCATION_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QMetaType>
#include <QtPositioning/qpositioningglobal.h>

QT_BEGIN_NAMESPACE

class QGeoAddress;
class QGeoCoordinate;
class QGeoRectangle;
class QGeoLocationPrivate;

class Q_POSITIONING_EXPORT QGeoLocation
{
public:
    QGeoLocation();
    QGeoLocation(const QGeoLocation &other);

    ~QGeoLocation();

    QGeoLocation &operator=(const QGeoLocation &other);

    bool operator==(const QGeoLocation &other) const;
    bool operator!=(const QGeoLocation &other) const {
        return !(other == *this);
    }

    QGeoAddress address() const;
    void setAddress(const QGeoAddress &address);
    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &position);
    QGeoRectangle boundingBox() const; // ### Qt6: change this into QGeoShape geometry
    void setBoundingBox(const QGeoRectangle &box);
    QVariantMap extendedAttributes() const;
    void setExtendedAttributes(const QVariantMap &data);

    bool isEmpty() const;

private:
    QSharedDataPointer<QGeoLocationPrivate> d;
};

Q_DECLARE_TYPEINFO(QGeoLocation, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoLocation)

#endif
