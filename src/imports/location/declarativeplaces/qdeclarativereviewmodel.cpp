#include "qdeclarativereviewmodel_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativeplace_p.h"
#include "qdeclarativesupplier_p.h"

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

    The model returns data for the following roles:
    \table
        \header
            \o Role
            \o Type
            \o Description
        \row
            \o date
            \o string
            \o The date that the review was posted.
        \row
            \o description
            \o string
            \o The content of the review.
        \row
            \o language
            \o string
            \o The language that the review is written in.
        \row
            \o helpfulVotings
            \o int
            \o The number of votings indicating that the review was helpful.
        \row
            \o unhelpfulVotings
            \o int
            \o The number of votings indicating that the review was not helpful.
        \row
            \o rating
            \o real
            \o The rating that the reviewer gave to the place.
        \row
            \o mediaIds
            \o list
            \o The list of media ids associated with the review.
        \row
            \o reviewId
            \o string
            \o The id of the review.
        \row
            \o supplier
            \o Supplier
            \o The source of the review.
        \row
            \o title
            \o string
            \o The title of the review.
        \row
            \o userId
            \o string
            \o The id of the user who posted the review.
        \row
            \o userName
            \o string
            \o The name of the user who posted the review.
        \row
            \o originatorUrl
            \o string
            \o The URL of the review.
    \endtable
*/

QDeclarativeReviewModel::QDeclarativeReviewModel(QObject* parent)
:   QAbstractListModel(parent), m_place(0), m_batchSize(1), m_reviewCount(-1), m_reply(0),
    m_complete(false)
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(DateRole, "date");
    roleNames.insert(DescriptionRole, "description");
    roleNames.insert(LanguageRole, "language");
    roleNames.insert(HelpfulVotingsRole, "helpfulVotings");
    roleNames.insert(UnhelpfulVotingsRole, "unhelpfulVotings");
    roleNames.insert(RatingRole, "rating");
    roleNames.insert(MediaIdsRole, "mediaIds");
    roleNames.insert(ReviewIdRole, "reviewId");
    roleNames.insert(SupplierRole, "supplier");
    roleNames.insert(TitleRole, "title");
    roleNames.insert(UserIdRole, "userId");
    roleNames.insert(UserNameRole, "userName");
    roleNames.insert(OriginatorUrlRole, "originatorUrl");
    setRoleNames(roleNames);
}

QDeclarativeReviewModel::~QDeclarativeReviewModel()
{
    qDeleteAll(m_suppliers);
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

        m_reviews.clear();
        qDeleteAll(m_suppliers);
        m_suppliers.clear();
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

static QPair<int, int> findMissingKey(const QMap<int, QPlaceReview> &map)
{
    int start = 0;
    while (map.contains(start))
        ++start;

    QMap<int, QPlaceReview>::const_iterator it = map.lowerBound(start);
    if (it == map.end())
        return qMakePair(start, -1);

    int end = start;
    while (!map.contains(end))
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
    m_reviews.clear();
    qDeleteAll(m_suppliers);
    m_suppliers.clear();
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
            } else if (reviewsIter.value() != m_reviews.value(reviewsIter.key())) {
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
                for (int i = startIndex; i <= currentIndex; ++i) {
                    const QPlaceReview &review = reviews.value(i);

                    m_reviews.insert(i, review);
                    if (!m_suppliers.contains(review.supplier().supplierId())) {
                        m_suppliers.insert(review.supplier().supplierId(),
                                           new QDeclarativeSupplier(review.supplier(), this));
                    }
                }
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
                    const QPlaceReview &review = reviews.value(i);

                    m_reviews.insert(i, review);
                    if (!m_suppliers.contains(review.supplier().supplierId())) {
                        m_suppliers.insert(review.supplier().supplierId(),
                                           new QDeclarativeSupplier(review.supplier(), this));
                    }
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

    const QPlaceReview &review = m_reviews.value(index.row());

    switch (role) {
    case DateRole:
        return review.date();
    case DescriptionRole:
        return review.description();
    case LanguageRole:
        return review.language();
    case HelpfulVotingsRole:
        return review.helpfulVotings();
    case UnhelpfulVotingsRole:
        return review.unhelpfulVotings();
    case RatingRole:
        return review.rating();
    case MediaIdsRole:
        return review.mediaIds();
    case ReviewIdRole:
        return review.reviewId();
    case SupplierRole:
        return QVariant::fromValue(static_cast<QObject *>(m_suppliers.value(review.supplier().supplierId())));
    case TitleRole:
        return review.title();
    case UserIdRole:
        return review.userId();
    case UserNameRole:
        return review.userName();
    case OriginatorUrlRole:
        return review.originatorUrl();
    default:
        return QVariant();
    }
}
