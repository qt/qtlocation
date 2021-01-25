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

#include "qdeclarativeratings_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype Ratings
    \instantiates QDeclarativeRatings
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-data
    \since QtLocation 5.5

    \brief The Ratings type holds place rating information.

    Rating information is used to describe how \e good a place is conceived to be.  Typically this
    information is visualized as a number of stars.  The \l average property gives an aggregated
    ratings value out of a possible maximum as given by the \l maximum property.

    \snippet declarative/places.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml Ratings
*/

QDeclarativeRatings::QDeclarativeRatings(QObject *parent)
        : QObject(parent) {}

QDeclarativeRatings::QDeclarativeRatings(const QPlaceRatings &rating,
        QObject *parent)
        : QObject(parent),
        m_ratings(rating) {}

QDeclarativeRatings::~QDeclarativeRatings() {}

/*!
    \qmlproperty QPlaceRatings Ratings::ratings

    For details on how to use this property to interface between C++ and QML see
    "\l {Ratings - QPlaceRatings} {Interfaces between C++ and QML Code}".
*/
void QDeclarativeRatings::setRatings(const QPlaceRatings &ratings)
{
    QPlaceRatings previous = m_ratings;
    m_ratings = ratings;

    if (ratings.average() != previous.average()) {
        emit averageChanged();
    }
    if (ratings.count() != previous.count()) {
        emit countChanged();
    }
}

QPlaceRatings QDeclarativeRatings::ratings() const
{
    return m_ratings;
}

/*!
    \qmlproperty real Ratings::average

    This property holds the average of the individual ratings.

    \sa maximum
*/
void QDeclarativeRatings::setAverage(qreal average)
{
    if (m_ratings.average() != average) {
        m_ratings.setAverage(average);
        emit averageChanged();
    }
}

qreal QDeclarativeRatings::average() const
{
    return m_ratings.average();
}

/*!
    \qmlproperty real Ratings::maximum

    This property holds the maximum rating value.
*/
void QDeclarativeRatings::setMaximum(qreal max)
{
    if (m_ratings.maximum() == max)
        return;

    m_ratings.setMaximum(max);
    emit maximumChanged();
}

qreal QDeclarativeRatings::maximum() const
{
    return m_ratings.maximum();
}

/*!
    \qmlproperty int Ratings::count

    This property holds the total number of individual user ratings
    used in determining the overall ratings \l average.
*/
void QDeclarativeRatings::setCount(int count)
{
    if (m_ratings.count() != count) {
        m_ratings.setCount(count);
        emit countChanged();
    }
}

int QDeclarativeRatings::count() const
{
    return m_ratings.count();
}

QT_END_NAMESPACE
