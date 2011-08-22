#include "qdeclarativesearchresultmodel_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include <qplacesearchreply.h>

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>

QT_USE_NAMESPACE

/*!
    \qmlclass SearchResultModel QDeclarativeSearchResultModel
    \brief The SearchResultModel element provides access to search results.
    \inherits QAbstractListModel
    \ingroup qml-places

    SearchResultModel provides a model of search results from PlaceManager.
    The contents of the model is SearchResult list. User can add additional parameters
    which make search more satisfactory. At least position of search center
    should be set. Other parameters are start and limit of returned items, and bounding box
    for the items returned. Also user might set \l dym property if "did you mean"
    items should be returned.

    There are two ways of accessing the data: through model by using views and delegates,
    or alternatively via \l results list property. Of the two, the model access is preferred.

    At the moment only data role provided by the model is \c searchResult.

    Model might be use for different kind of requests. It mighe be used for search request
    with search term where user gives string with query. It might be used for category search.
    User might also use it for recomendation search (search similar places in requested area).
    In that case user pass place id as an input parameter.

    To use SearchResultModel user need to create it in qml file and connect it to some view
    \code
    import places 1.0

    SearchResultModel {
        id: searchModel
        searchArea: BoundingCircle {
            id: proximity
            center: Coordinate {
                longitude: 53
                latitude: 100
            }
            radius:5000
        }
        start: 0
        limit: 15
    }

    ...
    searchModel.executeQuery()
    ...

    ListView {
        id: suggestionList
        model: searchModel
        delegate: Text {
            text: 'Name: ' + searchResult.place.name }
        }
    }
    \endcode

    \sa SuggestionModel, SupportedCategoryModel, {QPlaceManager}
*/
QDeclarativeSearchResultModel::QDeclarativeSearchResultModel(QObject *parent)
:   QAbstractListModel(parent), m_response(0), m_plugin(0), m_complete(false)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(SearchResultRole, "searchResult");
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

/*!
    \qmlproperty QDeclarativeListProperty<QDeclarativeSearchResult> SearchResultModel::results

    This element holds the list of search results (place or "did you mean" strings)
    that the model currently has.
*/
QDeclarativeListProperty<QDeclarativeSearchResult> QDeclarativeSearchResultModel::results()
{
    return QDeclarativeListProperty<QDeclarativeSearchResult>(this,
                                                          0, // opaque data parameter
                                                          results_append,
                                                          results_count,
                                                          results_at,
                                                          results_clear);
}

void QDeclarativeSearchResultModel::results_append(QDeclarativeListProperty<QDeclarativeSearchResult> *prop,
                                                             QDeclarativeSearchResult* category)
{
    Q_UNUSED(prop);
    Q_UNUSED(category);
}

int QDeclarativeSearchResultModel::results_count(QDeclarativeListProperty<QDeclarativeSearchResult> *prop)
{
    return static_cast<QDeclarativeSearchResultModel*>(prop->object)->m_results.count();
}

QDeclarativeSearchResult* QDeclarativeSearchResultModel::results_at(QDeclarativeListProperty<QDeclarativeSearchResult> *prop,
                                                                          int index)
{
    QDeclarativeSearchResultModel* model = static_cast<QDeclarativeSearchResultModel*>(prop->object);
    QDeclarativeSearchResult *res = NULL;
    if (model->m_results.count() > index && index > -1) {
        res = model->m_results[index];
    }
    return res;
}

void QDeclarativeSearchResultModel::results_clear(QDeclarativeListProperty<QDeclarativeSearchResult> *prop)
{
    Q_UNUSED(prop)
}

void QDeclarativeSearchResultModel::convertResultsToDeclarative()
{
    qDeleteAll(m_results);
    m_results.clear();

    foreach (const QPlaceSearchResult& result, m_response->results()) {
        QDeclarativeSearchResult* declarative = new QDeclarativeSearchResult(result, this);
        m_results.append(declarative);
    }
}

int QDeclarativeSearchResultModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (m_response) {
        return m_response->results().count();
    } else {
        return 0;
    }
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeSearchResultModel::data(const QModelIndex& index, int role) const
{
    QDeclarativeSearchResult *result = NULL;
    if (m_response && m_response->results().count() > index.row()) {
       result = m_results[index.row()];
    }

    switch (role) {
        case Qt::DisplayRole:
            if (result && result->type() == QDeclarativeSearchResult::Place) {
                return result->place()->name();
            } else if (result && result->type() == QDeclarativeSearchResult::DidYouMeanSuggestion) {
                return result->didYouMeanSuggestion();
            } else {
                return QString();
            }
        case SearchResultRole:
            if (result) {
                return QVariant::fromValue(static_cast<QObject *>(result));
            } else {
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
        convertResultsToDeclarative();
        endResetModel();
        emit resultsChanged();

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
