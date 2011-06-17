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

#include "qplacemanager.h"
#include "qplacemanager_p.h"
#include "qplacemanagerengine.h"

/*!
    \class QPlaceManager

    \brief The QPlaceManager class is responsible for the discovery and
    management of places.

    \inmodule QtLocation

    \ingroup places-main
*/

/*!
    \enum QPlaceManager::ConnectivityMode

    Defines the method of obtaining place data
    \value NoConnectivity There is no place data.
    \value OfflineMode The places data will come from an offline source.
    \value OnlineMode The place data will come from an online source.
    \value HybridMode The place data will come from a combination of offline and online sources.
*/

/*!
    \enum QPlaceManager::SearchVisibilityScope
    Defines the scope for searching places according to visibility.
    \value PublicSearch Searches will only be conducted on public places.
    \value PrivateSearch Searches will only be conducted on private places.
    \value PublicAndPrivateSearch Searches will be conducted on both public and private places
*/

/*!
    \enum QPlaceManager::ManagerFeature
    Defines the possible features that the place manager can possible.
    \value ImportFeature The manager supports import operations
    \value ExportFeature The manager supports export operations
    \value CheckInFeature The manaager supports check-in operations
    \value PostMediaFeature The manager supports posting media for places
    \value PostRatingFeature The manager supports posting ratings for places
    \value SuggestionFeature The manager supports the providing of suggestions
    \value ReportPlaceFeature The manager supports reporting a place if it is incorrect/inappropriate.
    \value AuthenticationFeature The manager supports authentication of a user.
    \value CreatePlaceFeature The manager supports the creation of places.
    \value UpdatePlaceFeature The manager supports the updating of places.
*/

/*!
    Constructs a new manager with the specified \a pareent and with the
    implementation provided by \a engine.

    This constructor is used internally by QGeoServiceProviderFactory. Regular
    users should acquire instances of QGeoRoutingManager with
    QGeoServiceProvider::routingManager();
*/
QPlaceManager::QPlaceManager(QObject *parent)
    : QObject(parent),d(new QPlaceManagerPrivate)
{
    QString managerName = "nokia";
    d->q_ptr = this;
    d->createEngine(managerName);
}

/*!
    Destroys the manager.
*/
QPlaceManager::~QPlaceManager()
{
    delete d;
}

/*!
    Returns the name of the manager
*/
QString QPlaceManager::managerName() const
{
    return d->engine->managerName();
}

/*!
    Retrieves a details of place with \a place id.
*/
QPlaceDetailsReply *QPlaceManager::getPlaceDetails(const QString &placeId) const
{
    return d->engine->getPlaceDetails(placeId);
}

/*!
    Retrieves media from a given \a place according to the parameters specified in \a query.
*/
QPlaceMediaReply *QPlaceManager::getMedia(const QPlace &place, const QPlaceQuery &query) const
{
    return d->engine->getMedia(place, query);
}

/*!
    Posts a \a rating to a \a place.
*/
QPlaceReply* QPlaceManager::postRating(const QPlace &place, qreal rating)
{
    return d->engine->postRating(place, rating);
}

/*!
    Returns reviews for a given \a place according to the parameters specified in \a query.
*/
QPlaceReviewReply *QPlaceManager::getReviews(const QPlace &place, const QPlaceQuery &query) const
{
    return d->engine->getReviews(place, query);
}

/*!
    Searches for places according to a given \a query and at the given \a scope.
*/
QPlaceSearchReply *QPlaceManager::searchForPlaces(const QPlaceSearchQuery &query, VisibilityScope scope) const
{
    return d->engine->searchForPlaces(query, scope);
}

/*!
    Returns the available scopes in which places can be searched
*/
QPlaceManager::VisibilityScopes QPlaceManager::supportedSearchVisibilityScopes() const
{
    return d->engine->supportedSearchVisibilityScopes();
}

/*!
    Provides recommendation based on a given \a place.
*/
QPlaceSearchReply *QPlaceManager::recommendations(const QPlace &place, const QPlaceSearchQuery &query) const
{
    return d->engine->recommendations(place, query);
}

/*!
    Requests a set of text predictions for a given \a query string.
*/
QPlaceTextPredictionReply *QPlaceManager::textPredictions(const QPlaceSearchQuery &query) const
{
    return d->engine->textPredictions(query);
}

/*!
    Returns the connectivity mode of the manager.
*/
QPlaceManager::ConnectivityModes QPlaceManager::connectivityMode() const
{
    return d->engine->connectivityMode();
}

/*!
    Sets the connectivity \a mode of the manager.
*/
void QPlaceManager::setConnectivityMode(QPlaceManager::ConnectivityModes mode)
{
    d->engine->setConnectivityMode(mode);
}

/*!
    Returns the list of connectivity modes that the manager supports.
*/
QPlaceManager::ConnectivityModes QPlaceManager::supportedConnectivityModes() const
{
    return d->engine->supportedConnectivityModes();
}

/*!
    Saves a \a place at the given \a scope.
*/
QPlaceReply *QPlaceManager::savePlace(QPlace *place, VisibilityScope scope)
{
    return d->engine->savePlace(place, scope);
}

/*
    Returns the available scopes in which places can be saved.
*/
QPlaceManager::VisibilityScopes QPlaceManager::supportedSaveVisibilityScopes()
{
    return d->engine->supportedSaveVisibilityScopes();
}

/*!
    Initializes the manager categories.
*/
QPlaceReply *QPlaceManager::initializeCategories()
{
    return d->engine->initializeCategories();
}

/*!
    Returns a list of top level categories.
*/
QList<QPlaceCategory> QPlaceManager::categories() const
{
    return d->engine->categories();
}

/*!
    Returns a list of names of available managers that
    can be used to instantiate manager instances.
*/
QStringList QPlaceManager::availableManagers() {
    return QPlaceManagerPrivate::factories().keys();
}

/*!
\fn void QPlaceManager::finished(QPlaceReply* reply)

This signal is emitted when \a reply has finished processing.

If reply->error() equals QPlaceReply::NoError then the processing
finished successfully.

This signal and QPlaceReply::finished() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/

/*!
\fn void QPlaceManager::error(QPlaceReply* reply, QPlaceReply::Error error, const QString &errorString)

This signal is emitted when an error has been detected in the processing of
\a reply.  The QPlaceManager::finished() signal will probably follow.

The error will be described by the error code \a error.  If \a errorString is
not empty it will contain a textual description of the error meant for developers
and not end users.

This signal and QPlaceReply::error() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/
