#include "qdeclarativerecommendationmodel_p.h"
#include "qdeclarativeplace_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>

#include <qplacesearchreply.h>

QT_USE_NAMESPACE

/*!
    \qmlclass RecommendationModel QDeclarativeRecommenadationModel
    \brief The RecommendationModel element provides access to place recommendations.
    \inherits QAbstractListModel
    \ingroup qml-places

    RecommendationModel provides a model of place recommendation results within the \l searchArea,
    that are similiar to the place identified by the \l placeId property.

    The \l executing property indicates whether a query is currently executing.

    The model returns data for the following roles:

    \table
        \header
            \o Role
            \o Type
            \o Description
        \row
            \o type
            \o RecommendationModel.SearchResultType
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
            \o matchType
            \o RecommendationModel.LocationMatchType
            \o The location match type of the result.
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

    The following example shows how to use the RecommendationModel to search for recommendations
    within a 5km radius:

    \code
    import Qt.location 5.0

    Place {
        id: place
        ...
    }

    RecommendationModel {
        id: recommendationModel

        placeId: place.placeId
        searchArea: BoundingCircle {
            center: Coordinate {
                longitude: 53
                latitude: 100
            }
            radius:5000
        }
    }

    ...
    resultModel.executeQuery()
    ...

    ListView {
        model: recommendationModel
        delegate: Text { text: 'Name: ' + place.name }
    }
    \endcode

    \sa SearchResultModel, SupportedCategoryModel, {QPlaceManager}
*/

/*!
    \qmlproperty GeoCoordinate RecommendationModel::searchArea

    This element holds the search area.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/

/*!
    \qmlproperty int RecommendationModel::offset

    This element holds offset for items that would be returned.
    Less then 0 means that it is undefined.
*/

/*!
    \qmlproperty int RecommendationModel::limit

    This element holds limit of items that would be returned.
    Less then -1 means that limit is undefined.
*/

/*!
    \qmlproperty bool RecommendationModel::executing

    This property indicates whether a search query is currently being executed.
*/



/*!
    \qmlmethod RecommendationModel::executeQuery()
    Parameter placeId should contain string for which search should be
    started.
    Updates the items represented by the model from the underlying proivider.
*/


/*!
    \qmlmethod RecommendationModel::cancelRequest()
    Cancels ongoing request.
*/

QDeclarativeRecommendationModel::QDeclarativeRecommendationModel(QObject *parent)
:   QDeclarativeSearchModelBase(parent)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(SearchResultType, "type");
    roleNames.insert(SearchResultRelevance, "relevance");
    roleNames.insert(SearchResultDistance, "distance");
    roleNames.insert(SearchResultHeading, "heading");
    roleNames.insert(SearchResultMatchType, "matchType");
    roleNames.insert(SearchResultAdditionalData, "additionalData");
    roleNames.insert(SearchResultPlace, "place");
    roleNames.insert(SearchResultDidYouMean, "didYouMean");
    setRoleNames(roleNames);
}

QDeclarativeRecommendationModel::~QDeclarativeRecommendationModel()
{
}

/*!
    \qmlproperty string RecommendationModel::placeId

    This element holds place id used in query.
*/

QString QDeclarativeRecommendationModel::placeId() const
{
    return m_request.searchTerm();
}

void QDeclarativeRecommendationModel::setPlaceId(const QString &placeId)
{
    if (m_request.searchTerm() == placeId)
        return;

    m_request.setSearchTerm(placeId);
    emit placeIdChanged();
}

void QDeclarativeRecommendationModel::clearData()
{
    qDeleteAll(m_places);
    m_places.clear();
    m_results.clear();
}

void QDeclarativeRecommendationModel::updateSearchRequest()
{
    QDeclarativeSearchModelBase::updateSearchRequest();
}

void QDeclarativeRecommendationModel::processReply(QPlaceReply *reply)
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

int QDeclarativeRecommendationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_results.count();
}

QVariant QDeclarativeRecommendationModel::data(const QModelIndex& index, int role) const
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
        case SearchResultMatchType:
            return result.matchType();
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

QPlaceReply *QDeclarativeRecommendationModel::sendQuery(QPlaceManager *manager,
                                                        const QPlaceSearchRequest &request)
{
    QGeoPlace target;
    target.setPlaceId(request.searchTerm());
    return manager->recommendations(target, request);
}
