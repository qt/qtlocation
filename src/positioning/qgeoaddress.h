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

#ifndef QGEOADDRESS_H
#define QGEOADDRESS_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>
#include <QtPositioning/qpositioningglobal.h>

QT_BEGIN_NAMESPACE

class QString;
class QGeoAddressPrivate;
class Q_POSITIONING_EXPORT QGeoAddress
{
public:
    QGeoAddress();
    QGeoAddress(const QGeoAddress &other);
    ~QGeoAddress();

    QGeoAddress &operator=(const QGeoAddress &other);
    bool operator==(const QGeoAddress &other) const;
    bool operator!=(const QGeoAddress &other) const {
        return !(other == *this);
    }

    QString text() const;
    void setText(const QString &text);

    QString country() const;
    void setCountry(const QString &country);

    QString countryCode() const;
    void setCountryCode(const QString &countryCode);

    QString state() const;
    void setState(const QString &state);

    QString county() const;
    void setCounty(const QString &county);

    QString city() const;
    void setCity(const QString &city);

    QString district() const;
    void setDistrict(const QString &district);

    QString postalCode() const;
    void setPostalCode(const QString &postalCode);

    QString street() const;
    void setStreet(const QString &street);

    bool isEmpty() const;
    void clear();

    bool isTextGenerated() const;

private:
    QSharedDataPointer<QGeoAddressPrivate> d;
};

Q_DECLARE_TYPEINFO(QGeoAddress, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoAddress)

#endif
