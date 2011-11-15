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

#include "qplacemanagerengine.h"
#include "qplacemanagerengine_p.h"

QT_USE_NAMESPACE

/*!
    \class QPlaceManagerEngine
    \inmodule QtLocation
    \ingroup QtLocation-impl
    \ingroup QtLocation-places
    \ingroup QtLocation-places-manager
    \since QtLocation 5.0

    \brief The QPlaceManagerEngine class provides an interface for
    implementers of QGeoServiceProvider plugins who want to provide access to place
    functionality.

    Application developers need not concern themselves with the QPlaceManagerEngine.
    Backend implementers however will need to derive from QPlaceManagerEngine and provide
    implementations for the abstract virtual functions.

    For more information on writing a backend see the \l {Places Backend} documentation.

    \sa QPlaceManager
*/

/*!
    Constructs a new engine with the specified \a parent, using \a parameters to pass any
    implementation specific data to the engine.
*/
QPlaceManagerEngine::QPlaceManagerEngine(const QMap<QString, QVariant> &parameters,
                                         QObject *parent)
:   QObject(parent), d_ptr(new QPlaceManagerEnginePrivate)
{
    Q_UNUSED(parameters)
}

/*!
    Destroys this engine.
*/
QPlaceManagerEngine::~QPlaceManagerEngine()
{
    delete d_ptr;
}

/*!
    \internal
    Sets the name which this engine implementation uses to distinguish itself
    from the implementations provided by other plugins to \a managerName.

    This function does not need to be called by engine implementers,
    it is implicitly called by QGeoServiceProvider to set the manager
    name to be the same as the provider's.
*/
void QPlaceManagerEngine::setManagerName(const QString &managerName)
{
    d_ptr->managerName = managerName;
}

/*!
    Returns the name which this engine implementation uses to distinguish
    itself from the implementations provided by other plugins.

    The manager name is automatically set to be the same
    as the QGeoServiceProviderFactory::providerName().
*/
QString QPlaceManagerEngine::managerName() const
{
    return d_ptr->managerName;
}

/*!
    \internal
    Sets the version of this engine implementation to \a managerVersion.

    The combination of managerName() and managerVersion() should be unique
    amongst plugin implementations.
*/
void QPlaceManagerEngine::setManagerVersion(int managerVersion)
{
    d_ptr->managerVersion = managerVersion;
}

/*!
    Returns the version of this engine implementation.

    The manager version is automatically set to be the same
    as the QGeoServiceProviderFactory::providerVersion().
*/
int QPlaceManagerEngine::managerVersion() const
{
    return d_ptr->managerVersion;
}

/*!
    Returns the manager instance used to create this engine.
*/
QPlaceManager *QPlaceManagerEngine::manager() const
{
    return d_ptr->manager;
}

QPlaceManagerEnginePrivate::QPlaceManagerEnginePrivate()
    :   managerVersion(-1), manager(0)
{
}

QPlaceManagerEnginePrivate::~QPlaceManagerEnginePrivate()
{
}

/*!
    \fn QPlaceDetailsReply *QPlaceManagerEngine::getPlaceDetails(const QString &placeId)

    Retrieves a details of place corresponding to the given \a placeId.
*/

/*! \fn QPlaceContentReply *QPlaceManagerEngine::getContent(const QPlace &place, const QPlaceContentRequest &request)

    Retrieves content for a given \a place according to the parameters specified in
    \a request.
*/

/*!
    \fn QPlaceSearchReply *QPlaceManagerEngine::search(const QPlaceSearchRequest &request)

    Searches for places according to the parameters specified in \a request.
*/

/*!
    \fn QPlaceSearchReply *QPlaceManagerEngine::recommendations(const QString &placeId, const QPlaceSearchRequest &request)

    Provides recommendations for places that are similar to the place identified by \a placeId,
    and using the parameters as specified in \a request.
*/

/*!
    \fn QPlaceSearchSuggestionReply *QPlaceManagerEngine::searchSuggestions(const QPlaceSearchRequest &request)

    Requests a set of search term suggestions according to the parameters specified in \a request.
*/

/*!
    \fn QPlaceIdReply *QPlaceManagerEngine::savePlace(const QPlace &place)

    Saves a specified \a place to the manager engine's datastore.
*/

/*!
    \fn QPlaceIdReply *QPlaceManagerEngine::removePlace(const QString &placeId)

    Removes the place corresponding to \a placeId from the manager engine's datastore.
*/

/*!
    \fn QPlaceIdReply *QPlaceManagerEngine::saveCategory(const QPlaceCategory &category, const QString &parentId)

    Saves a \a category that is a child of the category specified by \a parentId.
    An empty \a parentId means \a category is saved as a top level category.
*/

/*!
    \fn QPlaceIdReply *QPlaceManagerEngine::removeCategory(const QString &categoryId)

    Removes the category corresponding to \a categoryId from the manager engine's datastore.
*/

/*!
    \fn QPlaceReply *QPlaceManagerEngine::initializeCategories()

    Initializes the categories of the manager engine.
*/

/*!
    \fn QString QPlaceManagerEngine::parentCategoryId(const QString &categoryId) const

    Returns the parent category id of the category corresponding to \a categoryId.
*/

/*!
    \fn QStringList QPlaceManagerEngine::childrenCategoryIds(const QString &categoryId) const

    Returns the children category ids of the category corresponding to \a categoryId.
    If \a categoryId is empty then all top level category ids are returned.
*/

/*!
    \fn QPlaceCategory QPlaceManagerEngine::category(const QString &categoryId) const

    Returns the category corresponding to the given \a categoryId.
*/

/*!
    \fn QList<QPlaceCategory> QPlaceManagerEngine::childCategories(const QString &parentId) const

    Returns a list of categories that are children of the category corresponding to \a parentId.
    If \a parentId is empty, all the top level categories are returned.
*/

/*!
    \fn QLocale QPlaceManagerEngine ::locale() const

    Returns the locale of the manager engine. The locale is used as a hint to determine
    what language place data should be returned in.
*/

/*!
    \fn void QPlaceManagerEngine::setLocale(const QLocale &locale)

    Sets the \a locale of the manager engine.
*/

/*!
    \fn QUrl QPlaceManagerEngine::constructIconUrl(const QPlaceIcon &icon, const QSize &size, QPlaceIcon::IconFlags flags)

    Constructs an icon url from a given \a icon, \a size and \a flags.  The URL of the icon
    image that most closely matches the given parameters is returned.
*/

/*!
    \fn QPlaceManager::ManagerFeatures QPlaceManagerEngine::supportedFeatures() const

    Returns a set of flags indicating what particular features this manager engine instance supports.
*/

/*!
    \fn void QPlaceManagerEngine::finished(QPlaceReply* reply)

    This signal is emitted when \a reply has finished processing.

    If reply->error() equals QPlaceReply::NoError then the processing
    finished successfully.

    This signal and QPlaceReply::finished() will be emitted at the same time.

    \note Do no delete the \a reply object in the slot connected to this signal.
    Use deleteLater() instead.
*/

/*!
    \fn void QPlaceManagerEngine::error(QPlaceReply * reply, QPlaceReply::Error error, const QString &errorString = QString());

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
    \fn void QPlaceManagerEngine::placeAdded(const QString &placeId)

    This signal is emitted if a place has been added to the manager engine's datastore.
    The particular added place is specified by \a placeId.

    This signal is only emitted by manager engines that support the QPlaceManager::NotificationsFeature.
*/

/*!
    \fn void QPlaceManagerEngine::placeUpdated(const QString &placeId)

    This signal is emitted if a place has been modified in the manager engine's datastore.
    The particular modifed place is specified by \a placeId.

    This signal is only emitted by manager engines that support the QPlaceManager::NotificationsFeature.
*/

/*!
    \fn void QPlaceManagerEngine::placeRemoved(const QString &placeId)

    This signal is emitted if a place has been removed from the manager engine's datastore.
    The particular place that has been removed is specified by \a placeId.

    This signal is only emitted by manager engines that support the QPlaceManager::NotificationsFeature.
*/

/*!
    \fn void QPlaceManagerEngine::categoryAdded(const QPlaceCategory &category, const QString &parentId)

    This signal is emitted if a \a category has been added to the manager engine's datastore.
    The parent of the \a category is specified by \a parentId.

    This signal is only emitted by manager engines that support the QPlaceManager::NotificationsFeature.

*/

/*!
    \fn void QPlaceManagerEngine::categoryUpdated(const QPlaceCategory &category, const QString &parentId)

    This signal is emitted if a \a category has been modified in the manager engine's datastore.
    The parent of the modified category is specified by \a parentId.

    This signal is only emitted by manager engines that support the QPlaceManager::NotificationsFeature.
*/

/*!
    \fn void QPlaceManagerEngine::categoryRemoved(const QString &categoryId, const QString &parentId)

    This signal is emitted when the category correspoinding to \a categoryId has
    been removed from the manager engine's datastore.  The parent of the removed category
    is specified by \a parentId.

    This signal is only emitted by manager engines that support the QPlaceManager::NotificationsFeature.
*/

#include "moc_qplacemanagerengine.cpp"

QT_END_NAMESPACE

