#include "qdeclarativerecommendationmodel_p.h"
#include "qdeclarativegeoserviceprovider_p.h"

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

QDeclarativeRecommendationModel::QDeclarativeRecommendationModel(QObject *parent)
:   QAbstractListModel(parent), m_response(0), m_plugin(0), m_complete(false)
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

// From QDeclarativeParserStatus
void QDeclarativeRecommendationModel::componentComplete()
{
    m_complete = true;
}


void QDeclarativeRecommendationModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    reset(); // reset the model
    m_plugin = plugin;
    if (m_complete)
        emit pluginChanged();
    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support places.");
        return;
    }
}

QDeclarativeGeoServiceProvider* QDeclarativeRecommendationModel::plugin() const
{
    return m_plugin;
}

/*!
    \qmlsignal RecommendationModel::queryFinished(const int &error)

    This handler is called when the request processing is finished.
    0 means that no error occurs during processing and new list is
    available.
*/

int QDeclarativeRecommendationModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return m_results.count();
}

// Returns the stored under the given role for the item referred to by the index.
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

/*!
    \qmlproperty string RecommendationModel::placeId

    This element holds place id used in query.
*/

QString QDeclarativeRecommendationModel::placeId() const
{
    return m_queryParameters.searchTerm();
}

void QDeclarativeRecommendationModel::setPlaceId(const QString &placeId)
{
    if (m_queryParameters.searchTerm() == placeId) {
        return;
    }
    m_queryParameters.setSearchTerm(placeId);
    emit placeIdChanged();
}

/*!
    \qmlproperty GeoCoordinate RecommendationModel::searchArea

    This element holds the search area.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeGeoBoundingArea *QDeclarativeRecommendationModel::searchArea() const
{
    return m_searchArea;
}

void QDeclarativeRecommendationModel::setSearchArea(QDeclarativeGeoBoundingArea *searchArea)
{
    if (m_searchArea == searchArea)
        return;
    m_searchArea = searchArea;
    emit searchAreaChanged();
}

/*!
    \qmlproperty int RecommendationModel::offset

    This element holds offset for items that would be returned.
    Less then 0 means that it is undefined.
*/
int QDeclarativeRecommendationModel::offset() const
{
    return m_queryParameters.offset();
}

void QDeclarativeRecommendationModel::setOffset(const int &offsetValue)
{
    if (m_queryParameters.offset() == offsetValue){
        return;
    }
    m_queryParameters.setOffset(offsetValue);
    emit offsetChanged();
}

/*!
    \qmlproperty int RecommendationModel::limit

    This element holds limit of items that would be returned.
    Less then -1 means that limit is undefined.
*/
int QDeclarativeRecommendationModel::limit() const
{
    return m_queryParameters.limit();
}

void QDeclarativeRecommendationModel::setLimit(const int &limit)
{
    if (m_queryParameters.limit() == limit){
        return;
    }
    m_queryParameters.setLimit(limit);
    emit limitChanged();
}

/*!
    \qmlproperty bool RecommendationModel::executing

    This property indicates whether a search query is currently being executed.
*/
bool QDeclarativeRecommendationModel::executing() const
{
    return m_response;
}

/*!
    \qmlmethod RecommendationModel::executeQuery()
    Parameter placeId should contain string for which search should be
    started.
    Updates the items represented by the model from the underlying proivider.
*/
void QDeclarativeRecommendationModel::executeQuery()
{
    if (!m_plugin) {
        qmlInfo(this) << "plugin not set.";
        return;
    }

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << tr("Places not supported by %1 Plugin.").arg(m_plugin->name());
        return;
    }

    cancelPreviousRequest();

    QGeoPlace target;
    target.setPlaceId(m_queryParameters.searchTerm());
    m_queryParameters.setSearchArea(m_searchArea->area());
    connectNewResponse(placeManager->recommendations(target, m_queryParameters));
}

/*!
    \qmlmethod RecommendationModel::cancelRequest()
    Cancels ongoing request.
*/
void QDeclarativeRecommendationModel::cancelRequest()
{
    cancelPreviousRequest();
}

void QDeclarativeRecommendationModel::replyFinished()
{
    if (m_response) {
        beginResetModel();

        qDeleteAll(m_places);
        m_places.clear();

        m_results = m_response->results();

        foreach (const QPlaceSearchResult &result, m_results) {
            const QString id = result.place().placeId();
            QDeclarativePlace *place = new QDeclarativePlace(result.place(), this);
            place->setPlugin(m_plugin);
            m_places.insert(id, place);
        }

        endResetModel();

        m_response->deleteLater();
        m_response = 0;
        emit executingChanged();
    }
    emit queryFinished(0);
}

void QDeclarativeRecommendationModel::replyError(QPlaceReply::Error error,
                                                 const QString &errorString)
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    emit queryFinished(-1);
}

void QDeclarativeRecommendationModel::cancelPreviousRequest()
{
    if (m_response) {
        if (!m_response->isFinished()) {
            m_response->abort();
        }
        m_response->deleteLater();
        m_response = 0;
        emit executingChanged();
    }
}

void QDeclarativeRecommendationModel::connectNewResponse(QPlaceSearchReply *newResponse)
{
    if (newResponse) {
        m_response = newResponse;
        m_response->setParent(this);
        connect(m_response, SIGNAL(finished()), this, SLOT(replyFinished()));
        connect(m_response, SIGNAL(error(QPlaceReply::Error,QString)),
                this, SLOT(replyError(QPlaceReply::Error,QString)));

        emit executingChanged();
    } else {
        emit queryFinished(-1);
    }
}
