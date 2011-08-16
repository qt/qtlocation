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

#ifndef QPLACEMANAGERENGINE_TEST_H
#define QPLACEMANAGERENGINE_TEST_H

#include <qplacemanager.h>
#include <qplacemanagerengine.h>
#include <qplacereply.h>

QT_USE_NAMESPACE

class PlaceReplyTest : public QPlaceReply
{
    Q_OBJECT
public:
    PlaceReplyTest(QObject *parent = 0)
        : QPlaceReply(parent) {}

    void abort() { emit aborted(); }

Q_SIGNALS:
    void aborted();
};

class QPlaceManagerEngineTest : public QPlaceManagerEngine
{
    Q_OBJECT
public:
    QPlaceManagerEngineTest(const QMap<QString, QVariant> &parameters)
        : QPlaceManagerEngine(parameters)
    {
    }

    QPlaceDetailsReply *getPlaceDetails(const QString &placeId)
    {
        Q_UNUSED(placeId)

        return 0;
    }

    QPlaceMediaReply *getMedia(const QGeoPlace &place, const QPlaceRequest &query)
    {
        Q_UNUSED(place)
        Q_UNUSED(query)

        return 0;
    }

    QPlaceReply *postRating(const QString &placeId, qreal value)
    {
        Q_UNUSED(placeId)
        Q_UNUSED(value)

        return 0;
    }

    QPlaceReviewReply *getReviews(const QGeoPlace &place, const QPlaceRequest &query)
    {
        Q_UNUSED(place)
        Q_UNUSED(query)

        return 0;
    }

    QPlaceSearchReply *searchForPlaces(const QPlaceSearchRequest &query)
    {
        Q_UNUSED(query)

        return 0;
    }

    QPlaceSearchReply *recommendations(const QGeoPlace &place, const QPlaceSearchRequest &query)
    {
        Q_UNUSED(place)
        Q_UNUSED(query)

        return 0;
    }

    QPlaceTextPredictionReply *textPredictions(const QPlaceSearchRequest &query)
    {
        Q_UNUSED(query)

        return 0;
    }

    QPlaceManager::ConnectivityModes connectivityMode() const
    {
        return QPlaceManager::OnlineMode;
    }

    void setConnectivityMode(QPlaceManager::ConnectivityModes mode)
    {
        Q_UNUSED(mode)
    }

    QPlaceManager::ConnectivityModes supportedConnectivityModes() const
    {
        return QPlaceManager::OnlineMode;
    }

    QPlaceSaveReply *savePlace(const QGeoPlace &place, QPlaceManager::VisibilityScope scope)
    {
        Q_UNUSED(place)
        Q_UNUSED(scope)

        return 0;
    }

    QPlaceManager::VisibilityScopes supportedSaveVisibilityScopes() const
    {
        return QPlaceManager::PublicScope;
    }

    QPlaceReply *removePlace(const QGeoPlace &place)
    {
        Q_UNUSED(place)

        return 0;
    }

    QPlaceReply *initializeCategories()
    {
        return 0;
    }

    QList<QPlaceCategory> categories(const QPlaceCategory &parent) const
    {
        Q_UNUSED(parent)

        return QList<QPlaceCategory>();
    }

    QLocale locale() const
    {
        return QLocale(QLocale::French);
    }

    void setLocale(const QLocale &locale)
    {
        Q_UNUSED(locale)
    }
};

#endif
