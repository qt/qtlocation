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
****************************************************************************/

#include "qdeclarativeratings_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass Ratings QDeclarativeRatings
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The Ratings element holds place rating information.

    Rating information is used to describe how \e good a place is conceived to be.  Typically this
    information is visualized as a number of stars.  The \l average property gives an aggregated
    ratings value out of a possible maximum as given by the \l maximum property.

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml Ratings
*/

QDeclarativeRatings::QDeclarativeRatings(QObject* parent)
        : QObject(parent) {}

QDeclarativeRatings::QDeclarativeRatings(const QPlaceRatings &rating,
        QObject *parent)
        : QObject(parent),
        m_ratings(rating) {}

QDeclarativeRatings::~QDeclarativeRatings() {}

/*!
    \qmlproperty QPlaceRatings Ratings::ratings

    This property is used to provide an interface between C++ and QML code.  First a pointer to a
    Ratings object must be obtained from C++, then use the \l {QObject::property()}{property()} and
    \l {QObject::setProperty()}{setProperty()} functions to get and set the \c ratings property.

    The following gets the QPlaceRating representing this object from C++:

    \snippet snippets/cpp/cppqml.cpp Ratings get

    The following sets the properties of this object based on a QPlaceRatings object from C++:

    \snippet snippets/cpp/cppqml.cpp Ratings set
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
    \qmlproperty qreal Ratings::average

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
    \qmlproperty qreal Ratings::maximum

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

