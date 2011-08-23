#include "qdeclarativesearchresultmodel_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include <qplacesearchreply.h>

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>

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
            \o matchType
            \o SearchResultModel.LocationMatchType
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

QDeclarativeSearchResultModel::QDeclarativeSearchResultModel(QObject *parent)
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

QDeclarativeSearchResultModel::~QDeclarativeSearchResultModel()
{
}

// From QDeclarativeParserStatus
void QDeclarativeSearchResultModel::componentComplete()
{
    m_complete = true;
}

void QDeclarativeSearchResultModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
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

QDeclarativeGeoServiceProvider* QDeclarativeSearchResultModel::plugin() const
{
    return m_plugin;
}

/*!
    \qmlsignal SearchResultModel::queryFinished(const int &error)

    This handler is called when the request processing is finished.
    0 means that no error occurs during processing and new list is
    available.
*/

int QDeclarativeSearchResultModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_results.count();
}

// Returns the stored under the given role for the item referred to by the index.
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
    \qmlproperty string SearchResultModel::searchTerm

    This element holds search term used in query.
*/

QString QDeclarativeSearchResultModel::searchTerm() const
{
    return m_queryParameters.searchTerm();
}

void QDeclarativeSearchResultModel::setSearchTerm(const QString &searchTerm)
{
    if (m_queryParameters.searchTerm() == searchTerm) {
        return;
    }
    m_queryParameters.setSearchTerm(searchTerm);
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
    if (m_queryParameters.categories().count()
            && m_queryParameters.categories()[0] == searchCategory->category()) {
        return;
    }
    m_queryParameters.setCategory(searchCategory->category());
    m_category.setCategory(m_queryParameters.categories()[0]);
    emit searchCategoryChanged();
}

/*!
    \qmlproperty GeoCoordinate SearchResultModel::searchArea

    This element holds the search area.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeGeoBoundingArea *QDeclarativeSearchResultModel::searchArea() const
{
    return m_searchArea;
}

void QDeclarativeSearchResultModel::setSearchArea(QDeclarativeGeoBoundingArea *searchArea)
{
    if (m_searchArea == searchArea)
        return;
    m_searchArea = searchArea;
    emit searchAreaChanged();
}

/*!
    \qmlproperty int SearchResultModel::offset

    This element holds offset for items that would be returned.
    Less then 0 means that it is undefined.
*/
int QDeclarativeSearchResultModel::offset() const
{
    return m_queryParameters.offset();
}

void QDeclarativeSearchResultModel::setOffset(const int &offsetValue)
{
    if (m_queryParameters.offset() == offsetValue){
        return;
    }
    m_queryParameters.setOffset(offsetValue);
    emit offsetChanged();
}

/*!
    \qmlproperty int SearchResultModel::limit

    This element holds limit of items that would be returned.
    Less then -1 means that limit is undefined.
*/
int QDeclarativeSearchResultModel::limit() const
{
    return m_queryParameters.limit();
}

void QDeclarativeSearchResultModel::setLimit(const int &limit)
{
    if (m_queryParameters.limit() == limit){
        return;
    }
    m_queryParameters.setLimit(limit);
    emit limitChanged();
}

/*!
    \qmlproperty int SearchResultModel::didYouMean

    This element holds maximum number of "did you mean" suggestions returned by search query.
*/
int QDeclarativeSearchResultModel::didYouMean() const
{
    return m_queryParameters.didYouMeanSuggestionNumber();
}

void QDeclarativeSearchResultModel::setDidYouMean(const int &didYouMeanSuggestionNumber)
{
    if (m_queryParameters.didYouMeanSuggestionNumber() == didYouMeanSuggestionNumber){
        return;
    }
    m_queryParameters.setDidYouMeanSuggestionNumber(didYouMeanSuggestionNumber);
    emit didYouMeanChanged();
}

/*!
    \qmlproperty bool SearchResultModel::executing

    This property indicates whether a search query is currently being executed.
*/
bool QDeclarativeSearchResultModel::executing() const
{
    return m_response;
}

/*!
    \qmlmethod SearchResultModel::executeQuery()
    Parameter searchTerm should contain string for which search should be
    started.
    Updates the items represented by the model from the underlying proivider.
*/
void QDeclarativeSearchResultModel::executeQuery()
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

    m_queryParameters.setSearchArea(m_searchArea->area());
    connectNewResponse(placeManager->searchForPlaces(m_queryParameters));
}

/*!
    \qmlmethod SearchResultModel::cancelRequest()
    Cancels ongoing request.
*/
void QDeclarativeSearchResultModel::cancelRequest()
{
    cancelPreviousRequest();
}

/*!
    \qmlmethod SearchResultModel::clearCategories()
    Clear search categories.
*/
void QDeclarativeSearchResultModel::clearCategories()
{
    if (m_queryParameters.categories().isEmpty())
        return;

    m_queryParameters.setCategory(QPlaceCategory());
    m_category.setCategory(QPlaceCategory());
    emit searchCategoryChanged();
}

/*!
    \qmlmethod SearchResultModel::clearSearchTerm()
    Cancels search term.
*/
void QDeclarativeSearchResultModel::clearSearchTerm()
{
    if (!m_queryParameters.searchTerm().isEmpty()) {
        m_queryParameters.setSearchTerm(QString());
        emit searchTermChanged();
    }
}

void QDeclarativeSearchResultModel::replyFinished()
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

void QDeclarativeSearchResultModel::replyError(QPlaceReply::Error error,
                                                 const QString &errorString)
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    emit queryFinished(-1);
}

void QDeclarativeSearchResultModel::cancelPreviousRequest()
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

void QDeclarativeSearchResultModel::connectNewResponse(QPlaceSearchReply *newResponse)
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
