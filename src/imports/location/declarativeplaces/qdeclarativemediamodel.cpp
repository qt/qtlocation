#include "qdeclarativemediamodel_p.h"
#include <qdeclarativeplace_p.h>

QT_USE_NAMESPACE

/*!
    \qmlclass MediaModel QDeclarativeMediaModel

    \brief The MediaModel element provides access to media items of a Place.
    \ingroup qml-places
    \since 5.0

    The MediaModel is a read-only model lused to fetch media items related to a Place.  The model
    incrementally fetches media item URLs.  The number of media items which are fetched at a time
    is specified by the batchSize property.  The total number of media items available can be
    accessed via the totalCount property and the number of fetched media items via the count
    property.

    The model provides a single data role, the "media" role, which returns a \l Media object.
*/

QDeclarativeMediaModel::QDeclarativeMediaModel(QObject* parent)
:   QAbstractListModel(parent), m_place(0), m_batchSize(1), m_mediaCount(-1),
    m_manager(new QPlaceManager(this)), m_reply(0)
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(MediaRole, "media");
    setRoleNames(roleNames);
}

QDeclarativeMediaModel::~QDeclarativeMediaModel()
{
    qDeleteAll(m_mediaObjects);
}

/*!
    \qmlproperty Place MediaModel::place

    This property holds the Place that the media items are for.
*/
QDeclarativePlace *QDeclarativeMediaModel::place() const
{
    return m_place;
}

void QDeclarativeMediaModel::setPlace(QDeclarativePlace *place)
{
    if (m_place != place) {
        beginResetModel();
        if (m_reply) {
            m_reply->abort();
            m_reply->deleteLater();
            m_reply = 0;
        }
        qDeleteAll(m_mediaObjects);
        endResetModel();

        m_mediaCount = -1;
        emit totalCountChanged();

        m_place = place;
        emit placeChanged();

        fetchMore(QModelIndex());
    }
}

/*!
    \qmlproperty int MediaModel::batchSize

    This property holds the batch size to use when fetching more media items.
*/
int QDeclarativeMediaModel::batchSize() const
{
    return m_batchSize;
}

void QDeclarativeMediaModel::setBatchSize(int batchSize)
{
    if (m_batchSize != batchSize) {
        m_batchSize = batchSize;
        emit batchSizeChanged();
    }
}

/*!
    \qmlproperty int MediaModel::totalCount

    This property holds the total number of media items for the place.
*/
int QDeclarativeMediaModel::totalCount() const
{
    return m_mediaCount;
}

static QPair<int, int> findMissingKey(const QMap<int, QDeclarativeMediaObject *> &map)
{
    int start = 0;
    while (map.value(start, 0) != 0)
        ++start;

    QMap<int, QDeclarativeMediaObject *>::const_iterator it = map.lowerBound(start);
    if (it == map.end())
        return qMakePair(start, -1);

    int end = start;
    while (map.value(end, 0) == 0)
        ++end;

    return qMakePair(start, end - 1);
}

bool QDeclarativeMediaModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid())
        return false;

    if (!m_place)
        return false;

    if (m_mediaCount == -1)
        return true;

    return m_mediaObjects.count() != m_mediaCount;
}

void QDeclarativeMediaModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;

    if (!m_place)
        return;

    if (m_reply)
        return;

    QPlaceQuery query;

    if (m_mediaCount == -1) {
        query.setOffset(0);
        query.setLimit(m_batchSize);
    } else {
        QPair<int, int> missing = findMissingKey(m_mediaObjects);
        query.setOffset(missing.first);
        if (missing.second == -1)
            query.setLimit(m_batchSize);
        else
            query.setLimit(qMin(m_batchSize, missing.second - missing.first + 1));
    }

    m_reply = m_manager->getMedia(m_place->place(), query);
    connect(m_reply, SIGNAL(finished()), this, SLOT(fetchFinished()), Qt::QueuedConnection);
}

void QDeclarativeMediaModel::clear()
{
    beginResetModel();
    m_mediaCount = -1;
    qDeleteAll(m_mediaObjects);
    m_mediaObjects.clear();
    delete m_reply;
    m_reply = 0;
    endResetModel();
}

void QDeclarativeMediaModel::classBegin()
{
}

void QDeclarativeMediaModel::componentComplete()
{
    fetchMore(QModelIndex());
}

void QDeclarativeMediaModel::fetchFinished()
{
    QPlaceMediaReply *reply = m_reply;
    m_reply = 0;

    if (m_mediaCount != reply->totalCount()) {
        m_mediaCount = reply->totalCount();
        emit totalCountChanged();
    }

    if (reply->mediaObjects().items() > 0) {
        int startIndex = reply->mediaObjects().start();

        QList<QPlaceMediaObject> medias = reply->mediaObjects().data();

        beginInsertRows(QModelIndex(), startIndex, startIndex + medias.length() - 1);
        for (int i = 0; i < medias.length(); ++i)
            m_mediaObjects.insert(startIndex + i, new QDeclarativeMediaObject(medias.at(i), this));
        endInsertRows();
    }

    reply->deleteLater();
}

int QDeclarativeMediaModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_mediaObjects.count();
}

QVariant QDeclarativeMediaModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == MediaRole)
        return QVariant::fromValue(static_cast<QObject *>(m_mediaObjects.value(index.row())));

    return QVariant();
}
