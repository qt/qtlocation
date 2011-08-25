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

/*!
    \qmlproperty Plugin TextPredictionModel::plugin

    This property holds the provider Plugin used by this model.
*/

/*!
    \qmlproperty GeoCoordinate TextPredictionModel::searchArea

    This element holds the search area.

    Note: this property's changed() signal is currently emitted only if the whole element changes,
    not if only the contents of the element change.
*/

/*!
    \qmlproperty int TextPredictionModel::offset

    This element holds offset for items that would be returned.  Less then 0 means that it is
    undefined.
*/

/*!
    \qmlproperty int TextPredictionModel::limit

    This element holds limit of items that would be returned. Less then -1 means that limit is
    undefined.
*/

/*!
    \qmlmethod TextPredictionModel::executeQuery()

    Parameter searchTerm should contain string for which suggestion search should be started.
    Updates the items represented by the model from the underlying provider.
*/

/*!
    \qmlmethod TextPredictionModel::cancelRequest()

    Cancels ongoing request.
*/

QDeclarativeTextPredictionModel::QDeclarativeTextPredictionModel(QObject *parent)
:   QDeclarativeSearchModelBase(parent)
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
    \qmlproperty string TextPredictionModel::searchTerm

    This element holds search term used in query.
*/
QString QDeclarativeTextPredictionModel::searchTerm() const
{
    return m_request.searchTerm();
}

void QDeclarativeTextPredictionModel::setSearchTerm(const QString &searchTerm)
{
    if (m_request.searchTerm() == searchTerm)
        return;

    m_request.setSearchTerm(searchTerm);
    emit searchTermChanged();
}

/*!
    \qmlproperty QStringList TextPredictionModel::predictions

    This element holds the list of string that the model currently has.
*/
QStringList QDeclarativeTextPredictionModel::predictions() const
{
    return m_predictions;
}

void QDeclarativeTextPredictionModel::clearData()
{
    m_predictions.clear();
}

void QDeclarativeTextPredictionModel::updateSearchRequest()
{
    QDeclarativeSearchModelBase::updateSearchRequest();
}

void QDeclarativeTextPredictionModel::processReply(QPlaceReply *reply)
{
    QPlaceTextPredictionReply *predictionReply = qobject_cast<QPlaceTextPredictionReply *>(reply);
    m_predictions = predictionReply->textPredictions();
    emit predictionsChanged();
}

int QDeclarativeTextPredictionModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return m_predictions.count();
}

QVariant QDeclarativeTextPredictionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case TextPredictionRole:
        return m_predictions.at(index.row());
    }

    return QVariant();
}

QPlaceReply *QDeclarativeTextPredictionModel::sendQuery(QPlaceManager *manager,
                                                        const QPlaceSearchRequest &request)
{
    return manager->textPredictions(request);
}
