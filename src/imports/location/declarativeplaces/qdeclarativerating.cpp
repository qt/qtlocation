/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#include "qdeclarativerating_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass Rating QDeclarativeRating
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The Rating element holds place rating information.
*/

QDeclarativeRating::QDeclarativeRating(QObject* parent)
        : QObject(parent) {}

QDeclarativeRating::QDeclarativeRating(const QPlaceRating &rating,
        QObject *parent)
        : QObject(parent),
        m_rating(rating) {}

QDeclarativeRating::~QDeclarativeRating() {}

void QDeclarativeRating::setRating(const QPlaceRating &rating)
{
    QPlaceRating previous = m_rating;
    m_rating = rating;

    if (rating.value() != previous.value()) {
        emit valueChanged();
    }
    if (rating.count() != previous.count()) {
        emit countChanged();
    }
}

QPlaceRating QDeclarativeRating::rating() const
{
    return m_rating;
}

/*!
    \qmlproperty qreal Rating::value

    This property holds the value of rating
*/

void QDeclarativeRating::setValue(qreal value)
{
    if (m_rating.value() != value) {
        m_rating.setValue(value);
        emit valueChanged();
    }
}

qreal QDeclarativeRating::value() const
{
    return m_rating.value();
}

/*!
    \qmlproperty qreal Rating::maximum

    This property holds the maximum rating value.
*/
void QDeclarativeRating::setMaximum(qreal max)
{
    if (m_rating.maximum() == max)
        return;

    m_rating.setMaximum(max);
    emit maximumChanged();
}

qreal QDeclarativeRating::maximum() const
{
    return m_rating.maximum();
}

/*!
    \qmlproperty int Rating::count

    This property holds number of votes with rate.
*/

void QDeclarativeRating::setCount(int count)
{
    if (m_rating.count() != count) {
        m_rating.setCount(count);
        emit countChanged();
    }
}

int QDeclarativeRating::count() const
{
    return m_rating.count();
}

