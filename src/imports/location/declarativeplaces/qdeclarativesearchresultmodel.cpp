/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativesearchresultmodel_p.h"
#include "qdeclarativeplace_p.h"
#include "qdeclarativeplaceicon_p.h"

#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceSearchReply>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceMatchRequest>
#include <QtLocation/QPlaceMatchReply>

QT_USE_NAMESPACE

/*!
    \qmlclass PlaceSearchModel QDeclarativeSearchResultModel
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-models
    \since QtLocation 5.0

    \brief The PlaceSearchModel element provides access to place search results.

    PlaceSearchModel provides a model of place search results within the \l searchArea.  The
    \l searchTerm and \l categories properties can be set to restrict the search results to
    places matching those criteria.

    The \l maximumCorrections property can be used to limit the maximum number of search term
    correction results that may be returned.  Setting \l maximumCorrections to 0 will prevent any
    search term correction results from being returned.

    The \l offset and \l limit properties can be used to access paged search results.  When the
    \l offset and \l limit properties are set the search results between \l offset and
    (\l offset + \l limit - 1) will be returned.

    The PlaceSearchModel returns both sponsored and
    \l {http://en.wikipedia.org/wiki/Organic_search}{organic search results}.  Sponsored search
    results will have the \c sponsored role set to true.

    The model returns data for the following roles:

    \table
        \header
            \li Role
            \li Type
            \li Description
        \row
            \li type
            \li enum
            \li The type of search result.
        \row
            \li distance
            \li real
            \li Valid only when the \c type role is \c PlaceResult, the distance to the place.
        \row
            \li place
            \li \l Place
            \li Valid only when the \c type role is \c PlaceResult, an object representing the
               place.
        \row
            \li sponsored
            \li bool
            \li Valid only when the \c type role is \c PlaceResult, true if the search result is a
               sponsored result.
        \row
            \li correction
            \li string
            \li Valid only when the \c type role is \c CorrectionResult, a suggested correction to
               the search term.
    \endtable

    \section2 Search result types

    The \c type role can take on the following values:

    \table
        \row
            \o PlaceSearchModel.PlaceResult
            \o The search result contains a place.
        \row
            \o PlaceSearchModel.CorrectionResult
            \o The search result contains a search term correction.
        \row
            \o PlaceSearchModel.UnknownSearchResult
            \o The contents of the search result are unknown.
    \endtable

    \section1 Example

    The following example shows how to use the PlaceSearchModel to search for Pizza restaurants in
    close proximity of a given position.

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml PlaceSearchModel

    \sa PlaceRecommendationModel, CategoryModel, {QPlaceManager}
*/

/*!
    \qmlproperty Plugin PlaceSearchModel::plugin

    This property holds the \l Plugin which will be used to perform the search.
*/

/*!
    \qmlproperty Plugin PlaceSearchModel::favoritesPlugin

    This property holds the \l Plugin which will be used to search for favorites.
    Any places from the search which can be cross-referenced/matched
    in the favorites \l Plugin will have their \l {Place::favorite}{favorite} property
    set with the \l Place from the favorites \l Plugin.

    If the favoritesPlugin is not set, the \l {Place::favorite}{favorite} property
    of the places in the results will always be null.

    \sa Favorites
*/

/*!
    \qmlproperty VariantMap PlaceSearchModel::favoritesMatchParameters

    This property holds a set of parameters used to specify how search result places
    are matched to favorites in the favoritesPlugin.

    By default the parameter map is empty and implies that the favorites plugin
    matches by \l {Alternative Id cross-referencing}{alternative ids}.  Generally,
    an application developer will not need to set this property.

    In cases where the favorites plugin does not support matching by alternative ids,
    then the \l {Information about plugins} {backend plugin documentation} should be consulted
    to see precisely what key-value parameters to set.
*/

/*!
    \qmlproperty BoundingArea PlaceSearchModel::searchArea

    This property holds the search area.  The search result returned by the model will be within
    the search area.

    If this property is set to a \l BoundingCircle its \l {BoundingCircle::radius}{radius} property
    may be left unset, in which case the \l Plugin will choose an appropriate radius for the
    search.
*/

/*!
    \qmlproperty int PlaceSearchModel::offset

    This property holds the index of the first search result in the model.

    \sa limit
*/

/*!
    \qmlproperty int PlaceSearchModel::limit

    This property holds the limit of the number of items that will be returned.

    \sa offset
*/

/*!
    \qmlproperty enum PlaceSearchModel::status

    This property holds the status of the model.  It can be one of:

    \table
        \row
            \li PlaceSearchModel.Ready
            \li The search query has completed and the result are available.
        \row
            \li PlaceSearchModel.Executing
            \li A search query is currently being executed.
        \row
            \li PlaceSearchModel.Error
            \li An error occurred when executing the previous search query.
    \endtable
*/

/*!
    \qmlmethod PlaceSearchModel::execute()

    Executes a search query using the element's properties as search parameters.  Once the query
    completes, the model items are updated with the search results.

    \sa cancel(), status
*/

/*!
    \qmlmethod PlaceSearchModel::cancel()

    Cancels an ongoing search query.

    \sa execute(), status
*/

/*!
    \qmlmethod PlaceSearchModel::clear()

    Clears the current search results stored in the model.
*/

/*!
    \qmlproperty string QtLocation5::PlaceSearchModel::errorString

    This read-only property holds the textual presentation of latest place search model error.
    If no error has occurred or if the model was cleared an empty string is returned.

    An empty string may also be returned if an error occurred which has no associated
    textual representation.
*/

QDeclarativeSearchResultModel::QDeclarativeSearchResultModel(QObject *parent)
:   QDeclarativeResultModelBase(parent)
{
    QHash<int, QByteArray> roles = roleNames();
    roles.insert(SearchResultTypeRole, "type");
    roles.insert(CorrectionRole, "correction");
    roles.insert(SponsoredRole, "sponsored");
    setRoleNames(roles);
}

QDeclarativeSearchResultModel::~QDeclarativeSearchResultModel()
{
}

/*!
    \qmlproperty string PlaceSearchModel::searchTerm

    This property holds search term used in query.  The search term is a free-form text string.
*/
QString QDeclarativeSearchResultModel::searchTerm() const
{
    return m_request.searchTerm();
}

void QDeclarativeSearchResultModel::setSearchTerm(const QString &searchTerm)
{
    if (m_request.searchTerm() == searchTerm)
        return;

    m_request.setSearchTerm(searchTerm);
    emit searchTermChanged();
}

/*!
    \qmlproperty list<Category> PlaceSearchModel::categories

    This property holds a list of categories to be used when searching.  Returned search results
    will be for places that match at least one of the categories.
*/
QDeclarativeListProperty<QDeclarativeCategory> QDeclarativeSearchResultModel::categories()
{
    return QDeclarativeListProperty<QDeclarativeCategory>(this,
                                                          0, // opaque data parameter
                                                          categories_append,
                                                          categories_count,
                                                          category_at,
                                                          categories_clear);
}

void QDeclarativeSearchResultModel::categories_append(QDeclarativeListProperty<QDeclarativeCategory> *list,
                                                  QDeclarativeCategory *declCategory)
{
    QDeclarativeSearchResultModel* searchModel = qobject_cast<QDeclarativeSearchResultModel*>(list->object);
    if (searchModel && declCategory) {
        searchModel->m_categories.append(declCategory);
        QList<QPlaceCategory> categories = searchModel->m_request.categories();
        categories.append(declCategory->category());
        searchModel->m_request.setCategories(categories);
        emit searchModel->categoriesChanged();
    }
}

int QDeclarativeSearchResultModel::categories_count(QDeclarativeListProperty<QDeclarativeCategory> *list)
{
    QDeclarativeSearchResultModel *searchModel = qobject_cast<QDeclarativeSearchResultModel*>(list->object);
    if (searchModel)
        return searchModel->m_categories.count();
    else
        return -1;
}

QDeclarativeCategory* QDeclarativeSearchResultModel::category_at(QDeclarativeListProperty<QDeclarativeCategory> *list,
                                                                          int index)
{
    QDeclarativeSearchResultModel *searchModel = qobject_cast<QDeclarativeSearchResultModel*>(list->object);
    if (searchModel && (searchModel->m_categories.count() > index) && (index > -1))
        return searchModel->m_categories.at(index);
    else
        return 0;
}

void QDeclarativeSearchResultModel::categories_clear(QDeclarativeListProperty<QDeclarativeCategory> *list)
{
    QDeclarativeSearchResultModel *searchModel = qobject_cast<QDeclarativeSearchResultModel*>(list->object);
    if (searchModel) {
        //note: we do not need to delete each of the elements in m_categories since the search model
        //should never be the parent of the categories anyway.
        searchModel->m_categories.clear();
        searchModel->m_request.setCategories(QList<QPlaceCategory>());
        emit searchModel->categoriesChanged();
    }
}

/*!
    \qmlproperty enumeration PlaceSearchModel::relevanceHint

    This property holds a relevance hint used in the search query.  The hint is given to the
    provider to help but not dictate the ranking of results.  For example, the distance hint may
    give closer places a higher ranking but it does not necessarily mean the results will be
    strictly ordered according to distance. A provider may ignore the hint altogether.

    \table
        \row
            \li SearchResultModel.UnspecifiedHint
            \li No relevance hint is given to the provider.
        \row
            \li SearchResultModel.DistanceHint
            \li The distance of the place from the user's current location is important to the user.
               This hint is only meaningful when a circular search area is used.
        \row
            \li SearchResultModel.LexicalPlaceNameHint
            \li The lexical ordering of place names (in ascending alphabetical order) is relevant to
               the user.  This hint is useful for providers based on a local data store.
    \endtable
*/
QDeclarativeSearchResultModel::RelevanceHint QDeclarativeSearchResultModel::relevanceHint() const
{
    return static_cast<QDeclarativeSearchResultModel::RelevanceHint>(m_request.relevanceHint());
}

void QDeclarativeSearchResultModel::setRelevanceHint(QDeclarativeSearchResultModel::RelevanceHint hint)
{
    if (m_request.relevanceHint() != static_cast<QPlaceSearchRequest::RelevanceHint>(hint)) {
        m_request.setRelevanceHint(static_cast<QPlaceSearchRequest::RelevanceHint>(hint));
        emit relevanceHintChanged();
    }
}

/*!
    \qmlproperty int PlaceSearchModel::maximumCorrections

    This property holds the maximum number of search term corrections that may be returned.
*/
int QDeclarativeSearchResultModel::maximumCorrections() const
{
    return m_request.maximumCorrections();
}

void QDeclarativeSearchResultModel::setMaximumCorrections(int corrections)
{
    if (m_request.maximumCorrections() == corrections)
        return;

    m_request.setMaximumCorrections(corrections);
    emit maximumCorrectionsChanged();
}

/*!
    \qmlproperty enum PlaceSearchModel::visibilityScope

    This property holds the visibility scope of the places to search.  Only places with the
    specified visibility will be returned in the search results.

    The visibility scope can be one of:

    \table
        \row
            \li Place.UnspecifiedVisibility
            \li No explicit visibility scope specified, places with any visibility may be part of
               search results.
        \row
            \li Place.DeviceVisibility
            \li Only places stored on the local device will be part of the search results.
        \row
            \li Place.PrivateVisibility
            \li Only places that are private to the current user will be part of the search results.
        \row
            \li Place.PublicVisibility
            \li Only places that are public will be part of the search results.
    \endtable
*/
QDeclarativePlace::Visibility QDeclarativeSearchResultModel::visibilityScope() const
{
    return QDeclarativePlace::Visibility(int(m_visibilityScope));
}

void QDeclarativeSearchResultModel::setVisibilityScope(QDeclarativePlace::Visibility visibilityScope)
{
    QtLocation::VisibilityScope scope = QtLocation::VisibilityScope(visibilityScope);

    if (m_visibilityScope == scope)
        return;

    m_visibilityScope = scope;
    emit visibilityScopeChanged();
}

QVariant QDeclarativeSearchResultModel::data(const QModelIndex &index, int role) const
{
    if (index.row() > m_results.count())
        return QVariant();

    switch (role) {
    case SearchResultTypeRole:
        return m_results.at(index.row()).type();
    case CorrectionRole:
        return m_results.at(index.row()).correction();
    case SponsoredRole:
        return m_results.at(index.row()).isSponsored();
    default:
        return QDeclarativeResultModelBase::data(index, role);
    }
}

QPlaceReply *QDeclarativeSearchResultModel::sendQuery(QPlaceManager *manager,
                                                      const QPlaceSearchRequest &request)
{
    Q_ASSERT(manager);
    return manager->search(request);
}

void QDeclarativeSearchResultModel::initializePlugin(QDeclarativeGeoServiceProvider *plugin)
{
    //disconnect the manager of the old plugin if we have one
    if (m_plugin) {
        QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
        if (serviceProvider) {
            QPlaceManager *placeManager = serviceProvider->placeManager();
            if (placeManager) {
                disconnect(placeManager, SIGNAL(placeUpdated(QString)), this, SLOT(placeUpdated(QString)));
                disconnect(placeManager, SIGNAL(placeRemoved(QString)), this, SLOT(placeRemoved(QString)));
            }
        }
    }

    //connect to the manager of the new plugin.
    if (plugin) {
        QGeoServiceProvider *serviceProvider = plugin->sharedGeoServiceProvider();
        if (serviceProvider) {
            QPlaceManager *placeManager = serviceProvider->placeManager();
            if (placeManager) {
                connect(placeManager, SIGNAL(placeUpdated(QString)), this, SLOT(placeUpdated(QString)));
                connect(placeManager, SIGNAL(placeRemoved(QString)), this, SLOT(placeRemoved(QString)));
            }
        }
    }
    QDeclarativeSearchModelBase::initializePlugin(plugin);
}

/*!
    \qmlmethod PlaceSearchModel::data(int index, string role)

    Returns the data for a given \a role at the specified row \a index.
*/

/*!
    \qmlproperty string PlaceSearchModel::count

    This property holds the number of results the model has.
*/

void QDeclarativeSearchResultModel::placeUpdated(const QString &placeId)
{
    int row = getRow(placeId);
    if (row < 0 || row > m_places.count())
        return;

    if (m_places.at(row))
        m_places.at(row)->getDetails();
}

void QDeclarativeSearchResultModel::placeRemoved(const QString &placeId)
{
    int row = getRow(placeId);
    if (row < 0 || row > m_places.count())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    delete m_places.at(row);
    m_places.removeAt(row);
    m_results.removeAt(row);
    endRemoveRows();

    emit rowCountChanged();
}

int QDeclarativeSearchResultModel::getRow(const QString &placeId) const
{
    for (int i = 0; i < m_places.count(); ++i) {
        if (!m_places.at(i))
            continue;
        else if (m_places.at(i)->placeId() == placeId)
            return i;
    }

    return -1;
}

