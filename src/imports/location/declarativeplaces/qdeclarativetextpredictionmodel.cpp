#include "qdeclarativetextpredictionmodel_p.h"
#include "qdeclarativegeoserviceprovider_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>

#include <qplacemanager.h>
#include <qplacesearchrequest.h>

QT_USE_NAMESPACE

/*!
    \qmlclass TextPredictionModel QDeclarativeTextPredictionModel
    \brief The TextPredictionModel element provides access to text predictions.
    \inherits QAbstractListModel
    \ingroup qml-places

    TextPredictionModel provides a model of text prediction from PlaceManager.
    The contents of the model is string list. User can add additional parameters
    which make suggestion search more satisfactory. At least position of search center
    should be set. Other parameters are start and limit of returned items, and bounding box
    for the items returned.

    There are two ways of accessing the data: through model by using views and delegates,
    or alternatively via \l suggestions list property. Of the two, the model access is preferred.

    At the moment only data role provided by the model is \c suggestion.

    To use TextPredictionModel user need to create it in qml file and connect it to some view
    \code
    import places 1.0

    TextPredictionModel {
        id: textPredictionModel
        searchArea: BoundingCircle {
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
    // Starts suggestion request
    textPredictionModel.executeQuery(search_term)
    ...

    ListView {
        id: textPredictionList
        model: textPredictionModel
        delegate: Text {
            text: textPrediction }
        }
    }
    \endcode

    \sa {QPlaceManager}
*/
QDeclarativeTextPredictionModel::QDeclarativeTextPredictionModel(QObject *parent)
:   QAbstractListModel(parent), m_response(0), m_plugin(0), m_complete(false)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(TextPredictionRole, "textPrediction");
    setRoleNames(roleNames);
}

QDeclarativeTextPredictionModel::~QDeclarativeTextPredictionModel()
{
}

/*!
    \qmlproperty Plugin MediaModel::plugin

    This property holds the provider Plugin used by this model.
*/
void QDeclarativeTextPredictionModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
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

QDeclarativeGeoServiceProvider* QDeclarativeTextPredictionModel::plugin() const
{
    return m_plugin;
}

/*!
    \qmlsignal TextPredictionModel::queryFinished(const int &error)

    This handler is called when the request processing is finished.
    0 means that no error occurs during processing and new list is
    available.
*/

/*!
    \qmlproperty QStringList TextPredictionModel::predictions

    This element holds the list of string that the model currently has.
*/
QStringList QDeclarativeTextPredictionModel::predictions() const
{
    if (m_response) {
        return m_response->textPredictions();
    } else {
        return QStringList();
    }
}

int QDeclarativeTextPredictionModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if (m_response) {
        return m_response->textPredictions().count();
    } else {
        return 0;
    }
}

// Returns the stored under the given role for the item referred to by the index.
QVariant QDeclarativeTextPredictionModel::data(const QModelIndex& index, int role) const
{
    QString result;
    if (m_response && m_response->textPredictions().count() > index.row()) {
       result = m_response->textPredictions()[index.row()];
    }

    switch (role) {
        case Qt::DisplayRole:
        case TextPredictionRole:
            return result;
        }
    return QVariant();
}

/*!
    \qmlproperty string TextPredictionModel::searchTerm

    This element holds search term used in query.
*/

QString QDeclarativeTextPredictionModel::searchTerm() const
{
    return m_queryParameters.searchTerm();
}

void QDeclarativeTextPredictionModel::setSearchTerm(const QString &searchTerm)
{
    if (m_queryParameters.searchTerm() == searchTerm) {
        return;
    }
    m_queryParameters.setSearchTerm(searchTerm);
    emit searchTermChanged();
}

/*!
    \qmlproperty GeoCoordinate TextPredictionModel::searchArea

    This element holds the search area.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
QDeclarativeGeoBoundingArea *QDeclarativeTextPredictionModel::searchArea() const
{
    return m_searchArea;
}

void QDeclarativeTextPredictionModel::setSearchArea(QDeclarativeGeoBoundingArea *searchArea)
{
    if (m_searchArea == searchArea)
        return;
    m_searchArea = searchArea;
    emit searchAreaChanged();
}

/*!
    \qmlproperty int TextPredictionModel::offset

    This element holds offset for items that would be returned.
    Less then 0 means that it is undefined.
*/
int QDeclarativeTextPredictionModel::offset() const
{
    return m_queryParameters.offset();
}

/*!
    Sets offset.
*/
void QDeclarativeTextPredictionModel::setOffset(const int &offsetValue)
{
    if (m_queryParameters.offset() == offsetValue){
        return;
    }
    m_queryParameters.setOffset(offsetValue);
    emit offsetChanged();
}

/*!
    \qmlproperty int TextPredictionModel::limit

    This element holds limit of items that would be returned.
    Less then -1 means that limit is undefined.
*/
int QDeclarativeTextPredictionModel::limit() const
{
    return m_queryParameters.limit();
}

void QDeclarativeTextPredictionModel::setLimit(const int &limit)
{
    if (m_queryParameters.limit() == limit){
        return;
    }
    m_queryParameters.setLimit(limit);
    emit limitChanged();
}

/*!
    \qmlmethod TextPredictionModel::executeQuery()
    Parameter searchTerm should contain string for which suggestion search should be
    started.
    Updates the items represented by the model from the underlying proivider.
*/
void QDeclarativeTextPredictionModel::executeQuery()
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
    connectNewResponse(placeManager->textPredictions(m_queryParameters));
}

/*!
    \qmlmethod TextPredictionModel::cancelRequest()
    Cancels ongoing request.
*/
void QDeclarativeTextPredictionModel::cancelRequest()
{
    cancelPreviousRequest();
}

void QDeclarativeTextPredictionModel::replyFinished()
{
    if (m_response && m_response->textPredictions().count()) {
        beginResetModel();
        endResetModel();
        emit predictionsChanged();
    }
    emit queryFinished(0);
}

void QDeclarativeTextPredictionModel::replyError(QPlaceReply::Error error,
                                                 const QString &errorString)
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    emit queryFinished(-1);
}

void QDeclarativeTextPredictionModel::cancelPreviousRequest()
{
    if (m_response) {
        if (!m_response->isFinished()) {
            m_response->abort();
        }
        m_response->deleteLater();
        m_response = NULL;
    }
}

void QDeclarativeTextPredictionModel::connectNewResponse(QPlaceTextPredictionReply *newResponse)
{
    if (newResponse) {
        m_response = newResponse;
        m_response->setParent(this);
        connect(m_response, SIGNAL(finished()), this, SLOT(replyFinished()));
        connect(m_response, SIGNAL(error(QPlaceReply::Error,QString)),
                this, SLOT(replyError(QPlaceReply::Error,QString)));
    } else {
        emit queryFinished(-1);
    }
}

void QDeclarativeTextPredictionModel::classBegin()
{
}

void QDeclarativeTextPredictionModel::componentComplete()
{
    m_complete = true;
}
