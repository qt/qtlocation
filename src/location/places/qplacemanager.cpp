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

#include "qplacemanager.h"
#include "qplacemanagerengine.h"

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

/*!
    \class QPlaceManager
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-manager
    \since QtLocation 5.0

    \brief The QPlaceManager class provides the interface which allows clients to access
    places stored in a particular backend.

    The following table gives an overview of the functionality provided by the QPlaceManager
    \table
        \header
            \o Functionality
            \o Description
        \row
            \o Searching for places
            \o Using set of parameters such as a seach term and search area, relevant places
               can be returned to the user.
        \row
            \o Categories
            \o Places can be classified as belonging to different categories.  The
               manager supports access to these categories.
        \row
            \o Search term suggestions
            \o Given a partially complete search term, a list of potential
               search terms can be given.
        \row
            \o Recommendations
            \o Given an existing place, a set of similar recommended places can
               be suggested to the user.
        \row
            \o Rich Content
            \o Rich content such as images, reviews etc can be retrieved in a paged
               fashion.
        \row
            \o Place/Category management
            \o Places and categories may be saved and removed.  It is possible
               for notifications to be given when this happens.
        \row
            \o Localization
            \o Different locales may be specified to return place
               data in different languages.
    \endtable

    \section1 Obtaining a QPlaceManager instance
    Creation of a QPlaceManager is facilitated by the QGeoServiceProvider.
    See \l {Initializing a manager} for an example on how to create a manager.


    \section1 Asynchronous interface.
    The QPlaceManager class provides an abstraction of the datastore which contains place information.
    The functions provided by the QPlaceManager and primarily asynchronous and follow
    a request-reply model.   Typically a request is given to the manager, consisting
    of a various set of parameters and a reply object is created.  The reply object
    has a signal to notify when the request is done, and once completed, the reply
    contains the results of the request, along with any errors that occurred, if any.

    An asynchonous request is generally handled as follows:
    \snippet snippets/places/requesthandler.h Simple search
    \dots
    \dots
    \snippet snippets/places/requesthandler.h Simple search handler

    See \l {Common Operations} for a list of examples demonstrating how the QPlaceManger
    is used.

    \section1 Category Initialization
    Sometime during startup of an application, the initializeCategories() function should
    be called to setup the categories.  Initializing the categories enables the usage of the
    following functions:

    \list
        \o QPlaceManager::childCategories()
        \o QPlaceManager::category()
        \o QPlaceManager::parentCategoryId()
        \o QPlaceManager::childrenCategoryIds();
    \endlist

    If the categories need to be refreshed or reloaded, the initializeCategories() function
    may be called again.

*/

/*!
    \enum QPlaceManager::ManagerFeature
    Defines the possible features that the place manager can have.
    \value NoFeatures No features specified/supported
    \value SavePlaceFeature The manager can be used to save places
    \value RemovePlaceFeature The manager can be used to remove places
    \value SaveCategoryFeature The manager can be used to save categories.
    \value RemoveCategoryFeature The manager can be used to remove categories.
    \value RecommendationsFeature The manager can be used to provide recommendations.
    \value SearchSuggestionsFeature The manager can be used to provide search term suggestions.
    \value CorrectionsFeature The manager can be used to provide search term corrections
    \value LocaleFeature The manager can be used to provide place data localized
                          according to locale
    \value NotificationsFeature The manager has signal notifications for when
                                places/categories are added/modified/removed.
*/

/*!
    Constructs a new manager with the specified \a parent and with the
    implementation provided by \a engine.

    This constructor is used internally by QGeoServiceProviderFactory. Regular
    users should acquire instances of QGeoRoutingManager with
    QGeoServiceProvider::routingManager();
*/
QPlaceManager::QPlaceManager(QPlaceManagerEngine *engine, QObject *parent)
    : QObject(parent), d(engine)
{
    if (d) {
        d->setParent(this);

        qRegisterMetaType<QPlaceCategory>("QPlaceCategory");

        connect(d, SIGNAL(finished(QPlaceReply*)), this, SIGNAL(finished(QPlaceReply*)));
        connect(d, SIGNAL(error(QPlaceReply*,QPlaceReply::Error)),
                this, SIGNAL(error(QPlaceReply*,QPlaceReply::Error)));

        connect(d, SIGNAL(placeAdded(QString)),
                this, SIGNAL(placeAdded(QString)), Qt::QueuedConnection);
        connect(d, SIGNAL(placeUpdated(QString)),
                this, SIGNAL(placeUpdated(QString)), Qt::QueuedConnection);
        connect(d, SIGNAL(placeRemoved(QString)),
                this, SIGNAL(placeRemoved(QString)), Qt::QueuedConnection);

        connect(d, SIGNAL(categoryAdded(QPlaceCategory,QString)),
                this, SIGNAL(categoryAdded(QPlaceCategory,QString)), Qt::QueuedConnection);
        connect(d, SIGNAL(categoryUpdated(QPlaceCategory,QString)),
                this, SIGNAL(categoryUpdated(QPlaceCategory,QString)), Qt::QueuedConnection);
        connect(d, SIGNAL(categoryRemoved(QString,QString)),
                this, SIGNAL(categoryRemoved(QString,QString)), Qt::QueuedConnection);
    } else {
        qFatal("The place manager engine that was set for this place manager was NULL.");
    }
}

/*!
    Destroys the manager.  This destructor is used internally by QGeoServiceProvider
    and should never need to be called in application code.
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
    return d->managerName();
}

/*!
    Returns the manager version.
*/
int QPlaceManager::managerVersion() const
{
    return d->managerVersion();
}

/*!
    Retrieves a details of place corresponding to the given \a placeId.

    See \l {Fetching Place Details} for an example of usage.
*/
QPlaceDetailsReply *QPlaceManager::getPlaceDetails(const QString &placeId) const
{
    return d->getPlaceDetails(placeId);
}

/*!
    Retrieves content for a given \a place according to the parameters specified in
    \a request.

    See \l {Fetching Rich Content} for an example of usage.
*/
QPlaceContentReply *QPlaceManager::getContent(const QPlace &place, const QPlaceContentRequest &request) const
{
    return d->getContent(place, request);
}

/*!
    Searches for places according to the parameters specified in \a request.

    See \l {Discovery/Search} for an example of usage.
*/
QPlaceSearchReply *QPlaceManager::search(const QPlaceSearchRequest &request) const
{
    return d->search(request);
}

/*!
    Provides recommendations for places that are similar to \a place, and using the parameters as specified in \a request.

    See \l {Recommendations} for an example of usage.
*/
QPlaceSearchReply *QPlaceManager::recommendations(const QPlace &place, const QPlaceSearchRequest &request) const
{
    return d->recommendations(place, request);
}

/*!
    Requests a set of search term suggestions  according to the parameters specified in \a request.
    The \a request can hold the incomplete search term, along with other data such
    as a search area to narrow down relevant results.

    See \l {Search Suggestions} for an example of usage.
*/
QPlaceSearchSuggestionReply *QPlaceManager::searchSuggestions(const QPlaceSearchRequest &request) const
{
    return d->searchSuggestions(request);
}

/*!
    Saves a specified \a place.

    See \l {Saving a place cpp} for an example of usage.
*/
QPlaceIdReply *QPlaceManager::savePlace(const QPlace &place)
{
    return d->savePlace(place);
}

/*!
    Removes the place corresponding to \a placeId from the manager.

    See \l {Removing a place cpp} for an example of usage.
*/
QPlaceIdReply *QPlaceManager::removePlace(const QString &placeId)
{
    return d->removePlace(placeId);
}

/*!
    Saves a \a category that is a child of the category specified by \a parentId.
    An empty \a parentId means \a category is saved as a top level category.

    See \l {Saving a category} for an example of usage.
*/
QPlaceIdReply *QPlaceManager::saveCategory(const QPlaceCategory &category, const QString &parentId)
{
    return d->saveCategory(category, parentId);
}

/*!
    Removes the category corresponding to \a categoryId from the manager.

    See \l {Removing a category} for an example of usage.
*/
QPlaceIdReply *QPlaceManager::removeCategory(const QString &categoryId)
{
    return d->removeCategory(categoryId);
}

/*!
    Initializes the categories of the manager.

    See \l {Using Categories} for an example of usage.
*/
QPlaceReply *QPlaceManager::initializeCategories()
{
    return d->initializeCategories();
}

/*!
    Returns the parent category id of the category corresponding to \a categoryId.
*/
QString QPlaceManager::parentCategoryId(const QString &categoryId) const
{
    return d->parentCategoryId(categoryId);
}

/*!
    Returns the children category ids of the category corresponding to \a categoryId.
    If \a categoryId is empty then all top level category ids are returned.
*/
QStringList QPlaceManager::childrenCategoryIds(const QString &categoryId) const
{
    return d->childrenCategoryIds(categoryId);
}

/*!
    Returns the category corresponding to the given \a categoryId.
*/
QPlaceCategory QPlaceManager::category(const QString &categoryId) const
{
    return d->category(categoryId);
}

/*!
    Returns a list of categories that are children of the category corresponding to \a parentId.
    If \a parentId is empty, all the top level categories are returned.
*/
QList<QPlaceCategory> QPlaceManager::childCategories(const QString &parentId) const
{
    return d->childCategories(parentId);
}

/*!
    Returns the locale of the manager.
    The locale is used as a hint to determine
    what language place data should be returned in.
*/
QLocale QPlaceManager::locale() const
{
    return d->locale();
}

/*!
    Sets the \a locale of the manager.
*/
void QPlaceManager::setLocale(const QLocale &locale)
{
    d->setLocale(locale);
}

/*!
    Returns a set of flags indicating what particular features this manager instance supports.
*/
QPlaceManager::ManagerFeatures QPlaceManager::supportedFeatures() const
{
    return d->supportedFeatures();
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

/*!
    \fn void QPlaceManager::placeAdded(const QString &placeId)

    This signal is emitted if a place has been added to the manager engine's datastore.
    The particular added place is specified by \a placeId.

    This signal is only emitted by managers that support the QPlaceManager::NotificationsFeature.

*/

/*!
    \fn void QPlaceManager::placeUpdated(const QString &placeId)

    This signal is emitted if a place has been modified in the manager's datastore.
    The particular modifed place is specified by \a placeId.

    This signal is only emitted by managers that support the QPlaceManager::NotificationsFeature.
*/

/*!
    \fn void QPlaceManager::placeRemoved(const QString &placeId)

    This signal is emitted if a place has been removed from the manager's datastore.
    The particular place that has been removed is specified by \a placeId.

    This signal is only emitted by managers that support the QPlaceManager::NotificationsFeature.
*/

/*!
    \fn void QPlaceManager::categoryAdded(const QPlaceCategory &category, const QString &parentId)

    This signal is emitted if a \a category has been added to the manager's datastore.
    The parent of the \a category is specified by \a parentId.

    This signal is only emitted by managers that support the QPlaceManager::NotificationsFeature.

*/

/*!
    \fn void QPlaceManager::categoryUpdated(const QPlaceCategory &category, const QString &parentId)

    This signal is emitted if a \a category has been modified in the manager's datastore.
    The parent of the modified category is specified by \a parentId.

    This signal is only emitted by managers that support the QPlaceManager::NotificationsFeature.
*/

/*!
    \fn void QPlaceManager::categoryRemoved(const QString &categoryId, const QString &parentId)

    This signal is emitted when the category correspoinding to \a categoryId has
    been removed from the manager's datastore.  The parent of the removed category
    is specified by \a parentId.

    This signal is only emitted by managers that support the QPlaceManager::NotificationsFeature.
*/

QT_END_NAMESPACE
