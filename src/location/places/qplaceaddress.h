/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QPLACEADDRESS_H
#define QPLACEADDRESS_H

#include "qmobilityglobal.h"
#include <QSharedDataPointer>
#include <QString>
#include <QVariant>
#include <QList>
#include "qplacealternativevalue.h"

QTM_BEGIN_NAMESPACE

class QPlaceAddressPrivate;

class Q_LOCATION_EXPORT QPlaceAddress
{
public:
    QPlaceAddress();
    QPlaceAddress(const QPlaceAddress &other);

    virtual ~QPlaceAddress();

    QPlaceAddress &operator=(const QPlaceAddress &other);

    bool operator==(const QPlaceAddress &other) const;
    bool operator!=(const QPlaceAddress &other) const {
        return !(other == *this);
    }

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QList<QPlaceAlternativeValue> alternativeAttributes() const;
    void setAlternativeAttributes(const QList<QPlaceAlternativeValue> &alternativeAttributes);
    QString city() const;
    void setCity(const QString &city);
    QString country() const;
    void setCountry(const QString &country);
    QString countryName() const;
    void setCountryName(const QString &countryName);
    QString county() const;
    void setCounty(const QString &county);
    QString district() const;
    void setDistrict(const QString &district);
    QString floor() const;
    void setFloor(const QString &floor);
    QString houseNumber() const;
    void setHouseNumber(const QString &houseNumber);
    QString label() const;
    void setLabel(const QString &label);
    QString postalCode() const;
    void setPostalCode(const QString &postalCode);
    QString state() const;
    void setState(const QString &state);
    QString street() const;
    void setStreet(const QString &street);
    QString suite() const;
    void setSuite(const QString &suite);

private:
    QSharedDataPointer<QPlaceAddressPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACEADDRESS_H
