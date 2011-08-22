#include "qdeclarativeplaceimagemodel_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativeplace_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>

QT_USE_NAMESPACE

/*!
    \qmlclass PlaceImageModel QDeclarativePlaceImage

    \brief The PlaceImageModel element provides access to place images.
    \ingroup qml-places
    \since 5.0

    The PlaceImageModel is a read-only model used to fetch images related to a Place.  The model
    performs fetches incrmentally.  The number of images which are fetched at a time
    is specified by the batchSize property.  The total number of images available can be
    accessed via the totalCount property and the number of fetched images via the count
    property.

    The model provides a single data role, the "image" role, which returns a \l PlaceImage object.
*/

QDeclarativePlaceImageModel::QDeclarativePlaceImageModel(QObject* parent)
:   QAbstractListModel(parent), m_place(0), m_batchSize(1), m_imageCount(-1), m_reply(0),
    m_complete(false)
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(ImageRole, "image");
    setRoleNames(roleNames);
}

QDeclarativePlaceImageModel::~QDeclarativePlaceImageModel()
{
    qDeleteAll(m_images);
}

/*!
    \qmlproperty Place PlaceImageModel::place

    This property holds the Place that the images are for.
*/
QDeclarativePlace *QDeclarativePlaceImageModel::place() const
{
    return m_place;
}

void QDeclarativePlaceImageModel::setPlace(QDeclarativePlace *place)
{
    if (m_place != place) {
        beginResetModel();
        if (m_reply) {
            m_reply->abort();
            m_reply->deleteLater();
            m_reply = 0;
        }
        qDeleteAll(m_images);
        m_images.clear();
        endResetModel();

        if (m_imageCount != -1) {
            m_imageCount = -1;
            emit totalCountChanged();
        }

        m_place = place;
        emit placeChanged();

        reset();

        fetchMore(QModelIndex());
    }
}

/*!
    \qmlproperty int PlaceImageModel::batchSize

    This property holds the batch size to use when fetching more image items.
*/
int QDeclarativePlaceImageModel::batchSize() const
{
    return m_batchSize;
}

void QDeclarativePlaceImageModel::setBatchSize(int batchSize)
{
    if (m_batchSize != batchSize) {
        m_batchSize = batchSize;
        emit batchSizeChanged();
    }
}

/*!
    \qmlproperty int PlaceImageModel::totalCount

    This property holds the total number of image items for the place.
*/
int QDeclarativePlaceImageModel::totalCount() const
{
    return m_imageCount;
}

static QPair<int, int> findMissingKey(const QMap<int, QDeclarativePlaceImage *> &map)
{
    int start = 0;
    while (map.value(start, 0) != 0)
        ++start;

    QMap<int, QDeclarativePlaceImage *>::const_iterator it = map.lowerBound(start);
    if (it == map.end())
        return qMakePair(start, -1);

    int end = start;
    while (map.value(end, 0) == 0)
        ++end;

    return qMakePair(start, end - 1);
}

bool QDeclarativePlaceImageModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid())
        return false;

    if (!m_place)
        return false;

    if (m_imageCount == -1)
        return true;

    return m_images.count() != m_imageCount;
}

void QDeclarativePlaceImageModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;

    if (!m_place)
        return;

    if (m_reply)
        return;

    QDeclarativeGeoServiceProvider *plugin = m_place->plugin();

    QGeoServiceProvider *serviceProvider = plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << tr("Places not supported by %1 Plugin.").arg(plugin->name());
        return;
    }

    QPlaceRequest query;

    if (m_imageCount == -1) {
        query.setOffset(0);
        query.setLimit(m_batchSize);
    } else {
        QPair<int, int> missing = findMissingKey(m_images);
        query.setOffset(missing.first);
        if (missing.second == -1)
            query.setLimit(m_batchSize);
        else
            query.setLimit(qMin(m_batchSize, missing.second - missing.first + 1));
    }

    m_reply = placeManager->getContent(QPlaceContent::ImageType, m_place->place(), query);
    connect(m_reply, SIGNAL(finished()), this, SLOT(fetchFinished()), Qt::QueuedConnection);
}

void QDeclarativePlaceImageModel::clear()
{
    beginResetModel();
    m_imageCount = -1;
    qDeleteAll(m_images);
    m_images.clear();
    delete m_reply;
    m_reply = 0;
    endResetModel();
}

void QDeclarativePlaceImageModel::classBegin()
{
}

void QDeclarativePlaceImageModel::componentComplete()
{
    m_complete = true;
    fetchMore(QModelIndex());
}

void QDeclarativePlaceImageModel::fetchFinished()
{
    QPlaceContentReply *reply = m_reply;
    m_reply = 0;

    if (m_imageCount != reply->totalCount()) {
        m_imageCount = reply->totalCount();
        emit totalCountChanged();
    }

    if (!reply->content().isEmpty()) {
        QPlaceContent::Collection images = reply->content();

        //find out which indexes are new and which ones have changed.
        QMapIterator<int, QPlaceContent> imagesIter(images);
        QList<int> changedIndexes;
        QList<int> newIndexes;
        while (imagesIter.hasNext()) {
            imagesIter.next();
            if (!m_images.contains(imagesIter.key())) {
                newIndexes.append(imagesIter.key());
            } else if (imagesIter.value() != m_images.value(imagesIter.key())->image()) {
                changedIndexes.append(imagesIter.key());
            } else {
                //image item at given index has not changed, do nothing
            }
        }

        //insert new indexes in blocks where within each
        //block, the indexes are consecutive.
        QListIterator<int> newIndexesIter(newIndexes);
        int startIndex = -1;
        while (newIndexesIter.hasNext()) {
            int currentIndex = newIndexesIter.next();
            if (startIndex == -1)
                startIndex = currentIndex;

            if (!newIndexesIter.hasNext() || (newIndexesIter.hasNext() && (newIndexesIter.peekNext() > (currentIndex + 1)))) {
                beginInsertRows(QModelIndex(),startIndex,currentIndex);
                for (int i = startIndex; i <= currentIndex; ++i)
                    m_images.insert(i, new QDeclarativePlaceImage(images.value(i), this));
                endInsertRows();
                startIndex = -1;
            }
        }

        //modify changed indexes in blocks where within each
        //block, the indexes are consecutive.
        startIndex = -1;
        QListIterator<int> changedIndexesIter(changedIndexes);
        while (changedIndexesIter.hasNext()) {
            int currentIndex = changedIndexesIter.next();
            if (startIndex == -1)
                startIndex = currentIndex;

            if (!changedIndexesIter.hasNext() || (changedIndexesIter.hasNext() && changedIndexesIter.peekNext() > (currentIndex +1))) {
                for (int i = startIndex; i <= currentIndex; ++i) {
                    m_images.remove(i);
                    m_images.insert(i, new QDeclarativePlaceImage(images.value(i), this));
                }
                emit dataChanged(index(startIndex),index(currentIndex));
                startIndex = -1;
            }
        }
    }

    reply->deleteLater();
}

int QDeclarativePlaceImageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_images.count();
}

QVariant QDeclarativePlaceImageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == ImageRole)
        return QVariant::fromValue(static_cast<QObject *>(m_images.value(index.row())));

    return QVariant();
}
