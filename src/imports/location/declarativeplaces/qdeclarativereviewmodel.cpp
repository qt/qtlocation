#include "qdeclarativereviewmodel_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativeplace_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>

QT_USE_NAMESPACE

/*!
    \qmlclass ReviewModel QDeclarativeReviewModel

    \brief The ReviewModel element provides access to reviews of a Place.
    \ingroup qml-places
    \since 5.0

    The ReviewModel is a read-only model used to fetch reviews about a Place.  The model
    incrementally fetches reviews.  The number of reviews which are fetched at a time is specified
    by the batchSize property.  The total number of reviews available can be accessed via the
    totalCount property and the number of fetched reviews via the count property.

    The model provides a single data role, the "review" role, which returns a Review object.
*/

QDeclarativeReviewModel::QDeclarativeReviewModel(QObject* parent)
:   QAbstractListModel(parent), m_place(0), m_batchSize(1), m_reviewCount(-1), m_reply(0),
    m_complete(false)
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(ReviewRole, "review");
    setRoleNames(roleNames);
}

QDeclarativeReviewModel::~QDeclarativeReviewModel()
{
    qDeleteAll(m_reviews);
}

/*!
    \qmlproperty Place ReviewModel::place

    This property holds the Place that the reviews are for.
*/
QDeclarativePlace *QDeclarativeReviewModel::place() const
{
    return m_place;
}

void QDeclarativeReviewModel::setPlace(QDeclarativePlace *place)
{
    if (m_place != place) {
        beginResetModel();
        if (m_reply) {
            m_reply->abort();
            m_reply->deleteLater();
            m_reply = 0;
        }
        qDeleteAll(m_reviews);
        m_reviews.clear();
        endResetModel();

        if (m_reviewCount != -1) {
            m_reviewCount = -1;
            emit totalCountChanged();
        }

        m_place = place;
        emit placeChanged();

        reset();

        fetchMore(QModelIndex());
    }
}

/*!
    \qmlproperty int ReviewModel::batchSize

    This property holds the batch size to use when fetching more reviews.
*/
int QDeclarativeReviewModel::batchSize() const
{
    return m_batchSize;
}

void QDeclarativeReviewModel::setBatchSize(int batchSize)
{
    if (m_batchSize != batchSize) {
        m_batchSize = batchSize;
        emit batchSizeChanged();
    }
}

/*!
    \qmlproperty int ReviewModel::totalCount

    This property holds the total number of reviews for the place.
*/
int QDeclarativeReviewModel::totalCount() const
{
    return m_reviewCount;
}

static QPair<int, int> findMissingKey(const QMap<int, QDeclarativeReview *> &map)
{
    int start = 0;
    while (map.value(start, 0) != 0)
        ++start;

    QMap<int, QDeclarativeReview *>::const_iterator it = map.lowerBound(start);
    if (it == map.end())
        return qMakePair(start, -1);

    int end = start;
    while (map.value(end, 0) == 0)
        ++end;

    return qMakePair(start, end - 1);
}

bool QDeclarativeReviewModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid())
        return false;

    if (!m_place)
        return false;

    if (m_reviewCount == -1)
        return true;

    return m_reviews.count() != m_reviewCount;
}

void QDeclarativeReviewModel::fetchMore(const QModelIndex &parent)
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

    QPlaceContentRequest request;
    request.setContentType(QPlaceContent::ReviewType);

    if (m_reviewCount == -1) {
        request.setOffset(0);
        request.setLimit(m_batchSize);
    } else {
        QPair<int, int> missing = findMissingKey(m_reviews);
        request.setOffset(missing.first);
        if (missing.second == -1)
            request.setLimit(m_batchSize);
        else
            request.setLimit(qMin(m_batchSize, missing.second - missing.first + 1));
    }

    m_reply = placeManager->getContent(m_place->place(), request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(fetchFinished()), Qt::QueuedConnection);
}

void QDeclarativeReviewModel::clear()
{
    beginResetModel();
    m_reviewCount = -1;
    qDeleteAll(m_reviews);
    m_reviews.clear();
    delete m_reply;
    m_reply = 0;
    endResetModel();
}

void QDeclarativeReviewModel::classBegin()
{
}

void QDeclarativeReviewModel::componentComplete()
{
    m_complete = true;
    fetchMore(QModelIndex());
}

void QDeclarativeReviewModel::fetchFinished()
{
    QPlaceContentReply *reply = m_reply;
    m_reply = 0;

    if (m_reviewCount != reply->totalCount()) {
        m_reviewCount = reply->totalCount();
        emit totalCountChanged();
    }

    if (!reply->content().isEmpty()) {
        QPlaceContent::Collection reviews = reply->content();

        //find out which indexes are new and which ones have changed.
        QMapIterator<int, QPlaceContent> reviewsIter(reviews);
        QList<int> changedIndexes;
        QList<int> newIndexes;
        while (reviewsIter.hasNext()) {
            reviewsIter.next();
            if (!m_reviews.contains(reviewsIter.key())) {
                newIndexes.append(reviewsIter.key());
            } else if (reviewsIter.value() != m_reviews.value(reviewsIter.key())->review()) {
                changedIndexes.append(reviewsIter.key());
            } else {
                //review item at given index has not changed, do nothing
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
                    m_reviews.insert(i, new QDeclarativeReview(reviews.value(i), this));
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
                    m_reviews.remove(i);
                    m_reviews.insert(i, new QDeclarativeReview(reviews.value(i), this));
                }
                emit dataChanged(index(startIndex),index(currentIndex));
                startIndex = -1;
            }
        }
    }

    reply->deleteLater();
}

int QDeclarativeReviewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_reviews.count();
}

QVariant QDeclarativeReviewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount(index.parent()) || index.row() < 0)
        return QVariant();

    if (role == ReviewRole)
        return QVariant::fromValue(static_cast<QObject *>(m_reviews.value(index.row())));

    return QVariant();
}
