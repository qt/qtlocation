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

#include "qdeclarativesearchresultmodel_p.h"
#include "qdeclarativeplace_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceSearchReply>

QT_USE_NAMESPACE

/*!
    \qmlclass SearchResultModel QDeclarativeSearchResultModel
    \brief The SearchResultModel element provides access to place search results.
    \inherits QAbstractListModel
    \ingroup qml-places

    SearchResultModel provides a model of place search results within the \l searchArea.  The
    \l searchTerm and \l searchCategory properties can be set to restrict the search results to
    places matching those criteria.

    The \l didYouMean property can be used to limit the maximum number of "did you mean" results
    that may be returned.  Settings \l didYouMean to 0 will prevent any "did you mean" results from
    being returned.

    The \l executing property indicates whether a query is currently executing.

    The model returns data for the following roles:

    \table
        \header
            \o Role
            \o Type
            \o Description
        \row
            \o type
            \o SearchResultModel.SearchResultType
            \o The type of search result.
        \row
            \o relevance
            \o real
            \o The relevence score of the result.
        \row
            \o distance
            \o real
            \o The distance to the place.
        \row
            \o heading
            \o real
            \o The heading to the place.
        \row
            \o additionalData
            \o
            \o Additional data related to the search result.
        \row
            \o place
            \o Place
            \o The Place.
        \row
            \o didYouMean
            \o string
            \o Valid only for did you mean search results, a suggested corrected search term.
    \endtable

    The following example shows how to use the SearchResultModel to search for Pizza restaurants
    within a 5km radius:

    \code
    import Qt.location 5.0

    SearchResultModel {
        id: searchModel

        searchTerm: "Pizza"
        searchArea: BoundingCircle {
            center: Coordinate {
                longitude: 53
                latitude: 100
            }
            radius:5000
        }

        Component.onCompleted: executeQuery()
    }

    ListView {
        model: searchModel
        delegate: Text { text: 'Name: ' + place.name }
    }
    \endcode

    \sa RecommendationModel, SupportedCategoryModel, {QPlaceManager}
*/

/*!
    \qmlproperty GeoCoordinate SearchResultModel::searchArea

    This element holds the search area.

    Note: this property's changed() signal is currently emitted only if the whole element changes,
    not if only the contents of the element change.
*/

/*!
    \qmlproperty int SearchResultModel::offset

    This element holds offset for items that would be returned.  Less then 0 means that it is
    undefined.
*/


/*!
    \qmlproperty int SearchResultModel::limit

    This element holds limit of items that would be returned.  Less then -1 means that limit is
    undefined.
*/

/*!
    \qmlproperty bool SearchResultModel::executing

    This property indicates whether a search query is currently being executed.
*/

/*!
    \qmlmethod SearchResultModel::executeQuery()

    Parameter searchTerm should contain string for which search should be started.  Updates the
    items represented by the model from the underlying proivider.
*/

/*!
    \qmlmethod SearchResultModel::cancelRequest()

    Cancels ongoing request.
*/

QDeclarativeSearchResultModel::QDeclarativeSearchResultModel(QObject *parent)
    : QDeclarativeSearchModelBase(parent), m_placeManager(0)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(SearchResultType, "type");
    roleNames.insert(SearchResultRelevance, "relevance");
    roleNames.insert(SearchResultDistance, "distance");
    roleNames.insert(SearchResultHeading, "heading");
    roleNames.insert(SearchResultAdditionalData, "additionalData");
    roleNames.insert(SearchResultPlace, "place");
    roleNames.insert(SearchResultDidYouMean, "didYouMean");
    setRoleNames(roleNames);
}

QDeclarativeSearchResultModel::~QDeclarativeSearchResultModel()
{
}

/*!
    \qmlproperty string SearchResultModel::searchTerm

    This element holds search term used in query.
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
    \qmlmethod SearchResultModel::clearSearchTerm()

    Clears the search term.
*/
void QDeclarativeSearchResultModel::clearSearchTerm()
{
    if (m_request.searchTerm().isEmpty())
        return;

    m_request.setSearchTerm(QString());
    emit searchTermChanged();
}

/*!
    \qmlproperty Category SearchResultModel::searchCategory

    This element holds search category. Search Category is used instead of search term.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeCategory *QDeclarativeSearchResultModel::searchCategory()
{
    return &m_category;
}

void QDeclarativeSearchResultModel::setSearchCategory(QDeclarativeCategory *searchCategory)
{
    if (m_request.categories().count() &&
        m_request.categories().first() == searchCategory->category()) {
        return;
    }

    m_request.setCategory(searchCategory->category());
    m_category.setCategory(m_request.categories().first());
    emit searchCategoryChanged();
}

/*!
    \qmlmethod SearchResultModel::clearCategories()

    Clear the search categories.
*/
void QDeclarativeSearchResultModel::clearCategories()
{
    if (m_request.categories().isEmpty())
        return;

    m_request.setCategory(QPlaceCategory());
    m_category.setCategory(QPlaceCategory());
    emit searchCategoryChanged();
}

/*!
    \qmlproperty enumeration SearchResultModel::relevanceHint

    This property holds a relevance hint for the search model.  The hint is given
    to the provider to help but not dictate the ranking of results. e.g a distance
    hint may give closer places a higher ranking but it does not neccessarily mean
    the results will be strictly ordered according to distance. A provider
    may ignore the hint altogether.

    \list
    \o SearchResultModel.NoHint - No relevance hint is given to the provider.
    \o SearchResultModel.DistanceHint - The distance to the user's curent location
        is important to the user.  This is only useful if a circular search area is used.
        Closer places are more highly ranked.
    \o SearchResultModel.LexicalPlaceNameHint - The lexical ordering of place names
        (in ascending alphabetical order) is relevant to the user.
    \endlist
*/
QDeclarativeSearchResultModel::RelevanceHint QDeclarativeSearchResultModel::relevanceHint() const
{
    return static_cast<QDeclarativeSearchResultModel::RelevanceHint>(m_request.relevanceHint());
}

void QDeclarativeSearchResultModel::setRelevanceHint(QDeclarativeSearchResultModel::RelevanceHint hint)
{
    if (m_request.relevanceHint() != hint) {
        m_request.setRelevanceHint(static_cast<QPlaceSearchRequest::RelevanceHint>(hint));
        emit relevanceHintChanged();
    }
}

/*!
    \qmlproperty int SearchResultModel::didYouMean

    This element holds maximum number of "did you mean" suggestions returned by search query.
*/
int QDeclarativeSearchResultModel::didYouMean() const
{
    return m_request.didYouMeanSuggestionNumber();
}

void QDeclarativeSearchResultModel::setDidYouMean(int didYouMeanSuggestionNumber)
{
    if (m_request.didYouMeanSuggestionNumber() == didYouMeanSuggestionNumber)
        return;

    m_request.setDidYouMeanSuggestionNumber(didYouMeanSuggestionNumber);
    emit didYouMeanChanged();
}

void QDeclarativeSearchResultModel::clearData()
{
    qDeleteAll(m_places);
    m_places.clear();
    m_results.clear();
}

void QDeclarativeSearchResultModel::updateSearchRequest()
{
    QDeclarativeSearchModelBase::updateSearchRequest();
}

void QDeclarativeSearchResultModel::processReply(QPlaceReply *reply)
{
    QPlaceSearchReply *searchReply = qobject_cast<QPlaceSearchReply *>(reply);
    if (!searchReply)
        return;

    m_results = searchReply->results();

    foreach (const QPlaceSearchResult &result, m_results) {
        QDeclarativePlace *place = new QDeclarativePlace(result.place(), this);
        place->setPlugin(plugin());
        m_places.insert(result.place().placeId(), place);
    }
}

int QDeclarativeSearchResultModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_results.count();
}

QVariant QDeclarativeSearchResultModel::data(const QModelIndex &index, int role) const
{
    if (index.row() > m_results.count())
        return QVariant();

    const QPlaceSearchResult &result = m_results.at(index.row());

    if (result.type() == QPlaceSearchResult::Place) {
        switch (role) {
        case Qt::DisplayRole:
            return result.place().name();
        case SearchResultType:
            return result.type();
        case SearchResultRelevance:
            return result.relevance();
        case SearchResultDistance:
            return result.distance();
        case SearchResultHeading:
            return result.heading();
        case SearchResultAdditionalData:
            return result.additionalData();
        case SearchResultPlace:
            return QVariant::fromValue(static_cast<QObject *>(m_places.value(result.place().placeId())));
        default:
            return QVariant();
        }
    } else if (result.type() == QPlaceSearchResult::DidYouMeanSuggestion) {
        switch (role) {
        case Qt::DisplayRole:
        case SearchResultDidYouMean:
            return result.didYouMeanSuggestion();
        case SearchResultType:
            return result.type();
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QPlaceReply *QDeclarativeSearchResultModel::sendQuery(QPlaceManager *manager,
                                                      const QPlaceSearchRequest &request)
{
    Q_ASSERT(manager);
    return manager->searchForPlaces(request);
}

void QDeclarativeSearchResultModel::initializePlugin(QDeclarativeGeoServiceProvider *oldPlugin,
                                                     QDeclarativeGeoServiceProvider *newPlugin)
{
    //The purpose of initialization is to connect to the place manager's signals
    //for place notifications so we can rexecute the query

    //disconnect the manager of the old plugin if we have one
    if (oldPlugin) {
        QGeoServiceProvider *serviceProvider = oldPlugin->sharedGeoServiceProvider();
        if (serviceProvider) {
            QPlaceManager *placeManager = serviceProvider->placeManager();
            if (placeManager) {
                disconnect(placeManager, SIGNAL(placeAdded(QString)),
                           this, SLOT(executeQuery()));
                disconnect(placeManager, SIGNAL(placeUpdated(QString)),
                           this, SLOT(executeQuery()));
                disconnect(placeManager, SIGNAL(placeRemoved(QString)),
                           this, SLOT(executeQuery()));
            }
        }
    }

    //connect to the manager of the new plugin.
    if (newPlugin) {
        QGeoServiceProvider *serviceProvider = newPlugin->sharedGeoServiceProvider();
        if (serviceProvider) {
            QPlaceManager *placeManager = serviceProvider->placeManager();
            if (placeManager) {
                connect(placeManager, SIGNAL(placeAdded(QString)),
                           this, SLOT(executeQuery()));
                connect(placeManager, SIGNAL(placeUpdated(QString)),
                           this, SLOT(executeQuery()));
                connect(placeManager, SIGNAL(placeRemoved(QString)),
                           this, SLOT(executeQuery()));
            }
        }
    }
}
