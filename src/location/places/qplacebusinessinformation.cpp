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

#include "qplacebusinessinformation.h"
#include "qplacebusinessinformation_p.h"

QTM_USE_NAMESPACE

QPlaceBusinessInformationPrivate::QPlaceBusinessInformationPrivate() : QSharedData()
{
}

QPlaceBusinessInformationPrivate::QPlaceBusinessInformationPrivate(const QPlaceBusinessInformationPrivate &other)
    : QSharedData()
{
    this->additionalData = other.additionalData;
    this->annualClosings = other.annualClosings;
    this->openingHours = other.openingHours;
    this->features = other.features;
    this->annualClosingNote = other.annualClosingNote;
    this->openingNote = other.openingNote;
    this->paymentMethods = other.paymentMethods;
}

QPlaceBusinessInformationPrivate::~QPlaceBusinessInformationPrivate()
{
}

bool QPlaceBusinessInformationPrivate::operator==(const QPlaceBusinessInformationPrivate &other) const
{
    return (
            this->additionalData == other.additionalData
            && this->annualClosings == other.annualClosings
            && this->openingHours == other.openingHours
            && this->features == other.features
            && this->annualClosingNote == other.annualClosingNote
            && this->openingNote == other.openingNote
            && this->paymentMethods == other.paymentMethods
    );
}

/*!
    \class QPlaceBusinessInformation

    \inmodule QPlaces

    \brief The QPlaceBusinessInformation class represents a business information object.

    Each QPlaceBusinessInformation represents a business information object with a number of attributes
    such as opening hours, payment methods etc. Each QPlaceBusinessInformation is associated with place.

    Business info objects are read-only, e.g. user of API might get business info object
    associated to specific place but can not edit its content. User might also create new
    business info object and add it to place.

    QPlaceBusinessInformation is an in memory representation of a business info object.
*/


/*
    Constructs an new "empty" business info object.
*/
QPlaceBusinessInformation::QPlaceBusinessInformation()
    : d(new QPlaceBusinessInformationPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceBusinessInformation::QPlaceBusinessInformation(const QPlaceBusinessInformation &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceBusinessInformation::~QPlaceBusinessInformation()
{
}

QPlaceBusinessInformation &QPlaceBusinessInformation::operator =(const QPlaceBusinessInformation &other) {
    d = other.d;
    return *this;
}

bool QPlaceBusinessInformation::operator==(const QPlaceBusinessInformation &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns additional data.
*/
QVariantHash QPlaceBusinessInformation::additionalData() const
{
    return d->additionalData;
}

/*!
    Sets additional data.
*/
void QPlaceBusinessInformation::setAdditionalData(const QVariantHash &data)
{
    d->additionalData = data;
}

/*!
    Returns annual closing.
*/
QList<QPlacePeriod> QPlaceBusinessInformation::annualClosings() const
{
    return d->annualClosings;
}

/*!
    Sets annual closing.
*/
void QPlaceBusinessInformation::setAnnualClosings(const QList<QPlacePeriod> &data)
{
    d->annualClosings = data;
}

/*!
    Returns opening hours.
*/
QList<QPlaceWeekdayHours> QPlaceBusinessInformation::openingHours() const
{
    return d->openingHours;
}

/*!
    Sets opening hours.
*/
void QPlaceBusinessInformation::setOpeningHours(const QList<QPlaceWeekdayHours> &data)
{
    d->openingHours = data;
}

/*!
    Returns annual closing note.
*/
QString QPlaceBusinessInformation::annualClosingNote() const
{
    return d->annualClosingNote;
}

/*!
    Sets annual closing note.
*/
void QPlaceBusinessInformation::setAnnualClosingNote(const QString &data)
{
    d->annualClosingNote = data;
}

/*!
    Returns features.
*/
QList<QPlaceBusinessFeature> QPlaceBusinessInformation::features() const
{
    return d->features;
}

/*!
    Sets features.
*/
void QPlaceBusinessInformation::setFeatures(const QList<QPlaceBusinessFeature> &data)
{
    d->features = data;
}

/*!
    Returns opening note.
*/
QString QPlaceBusinessInformation::openingNote() const
{
    return d->openingNote;
}

/*!
    Sets opening note.
*/
void QPlaceBusinessInformation::setOpeningNote(const QString &data)
{
    d->openingNote = data;
}

/*!
    Returns payment methods.
*/
QStringList QPlaceBusinessInformation::paymentMethods() const
{
    return d->paymentMethods;
}

/*!
    Sets paymant methods.
*/
void QPlaceBusinessInformation::setPaymentMethods(const QStringList &data)
{
    d->paymentMethods = data;
}
