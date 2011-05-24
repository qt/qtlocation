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
***************************************************************************/

#include "qdeclarativelandmark_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass Landmark QDeclarativeLandmark
    \brief The Landmark element presents one landmark.
    \ingroup qml-location-landmarks
    \inherits Place

    This element is part of the \bold{QtMobility.location 1.1} module.

    The Landmark element presents one landmark.
    The primary source for landmarks is the \l LandmarkModel.
    To get Categories of a certain landmark, see the
    LandmarkCategoryModel::landmark. Currently saving Landmarks
    is not supported.

    \snippet doc/src/snippets/declarative/declarative-landmark.qml User declared landmark

    \sa Place, LandmarkCategory, LandmarkModel, LandmarkCategoryModel, {QLandmark}
*/

QDeclarativeLandmark::QDeclarativeLandmark(QObject* parent) :
        QDeclarativeGeoPlace(parent)
{
}

QDeclarativeLandmark::QDeclarativeLandmark(const QLandmark& landmark, QObject* parent) :
    QDeclarativeGeoPlace(landmark, parent), m_landmark(landmark)
{
}

QString QDeclarativeLandmark::name()
{
    return m_landmark.name();
}

/*!
  \qmlproperty string Landmark::name

  This property holds the name of the landmark.

  */

void QDeclarativeLandmark::setName(const QString& name)
{
    if (name == m_landmark.name())
        return;
    m_landmark.setName(name);
    emit nameChanged();
}

QString QDeclarativeLandmark::phoneNumber()
{
    return m_landmark.phoneNumber();
}

/*!
  \qmlproperty string Landmark::phoneNumber

  This property holds the phoneNumber of the landmark.

  */

void QDeclarativeLandmark::setPhoneNumber(const QString& phoneNumber)
{
    if (phoneNumber == m_landmark.phoneNumber())
        return;
    m_landmark.setPhoneNumber(phoneNumber);
    emit phoneNumberChanged();
}

QString QDeclarativeLandmark::description()
{
    return m_landmark.description();
}

/*!
  \qmlproperty string Landmark::description

  This property holds the description of the landmark.

  */

void QDeclarativeLandmark::setDescription(const QString& description)
{
    if (description == m_landmark.description())
        return;
    m_landmark.setDescription(description);
    emit descriptionChanged();
}

double QDeclarativeLandmark::radius()
{
    return m_landmark.radius();
}

/*!
  \qmlproperty double Landmark::radius

  This property holds the coverage radius of the landmark (in metres).
  The coverage radius is relevant for large landmarks such as cities.

  Note that landmark searches over a given area do not factor in the coverage radius.

  */

void QDeclarativeLandmark::setRadius(const double& radius)
{
    if (radius == m_landmark.radius())
        return;
    m_landmark.setRadius(radius);
    emit radiusChanged();
}

QUrl QDeclarativeLandmark::iconSource()
{
    return m_landmark.iconUrl();
}

/*!
  \qmlproperty url Landmark::iconSource

  This property holds the landmark's icon's URL.

  */

void QDeclarativeLandmark::setIconSource(const QUrl& iconSource)
{
    if (iconSource == m_landmark.iconUrl())
        return;
    m_landmark.setIconUrl(iconSource);
    emit iconSourceChanged();
}

QUrl QDeclarativeLandmark::url()
{
    return m_landmark.url();
}

/*!
  \qmlproperty url Landmark::url

  This property holds the URL of the landmark.

  */

void QDeclarativeLandmark::setUrl(const QUrl& url)
{
    if (url == m_landmark.url())
        return;
    m_landmark.setUrl(url);
    emit urlChanged();
}

// Initializes this landmark from the given landmark
void QDeclarativeLandmark::setLandmark(const QLandmark& landmark)
{
    // Elaborate but makes sure appropriate signals are sent
    // (this function is called when landmark updates).
    setPlace(landmark); // viewport, address, coordinate etc.
    setName(landmark.name());
    setPhoneNumber(landmark.phoneNumber());
    setDescription(landmark.description());
    setRadius(landmark.radius());
    setIconSource(landmark.iconUrl());
    setUrl(landmark.url());
    m_landmark = landmark;
}

QList<QLandmarkCategoryId> QDeclarativeLandmark::categoryIds () const
{
    return m_landmark.categoryIds();
}

QLandmark QDeclarativeLandmark::landmark()
{
    return m_landmark;
}

#include "moc_qdeclarativelandmark_p.cpp"

QTM_END_NAMESPACE
