#include "qplacereviewreply.h"
#include "qplacereply_p.h"

#include "qplace.h"

QTM_BEGIN_NAMESPACE
class QPlaceReviewReplyPrivate : public QPlaceReplyPrivate
{
public:
    QPlaceReviewReplyPrivate() {}
    QList<QPlaceReview> reviews;
    int totalCount;
};

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

/*!
    \class QPlaceReviewReply

    \brief The QPlaceReviewReply class manages a review retrieval operation started by an
    instance of QPlaceManager.

    \inmodule QtLocation

    \ingroup places-main
*/

/*!
    Constructs a review reply with a given \a parent.
*/
QPlaceReviewReply::QPlaceReviewReply(QObject *parent)
    : QPlaceReply(new QPlaceReviewReplyPrivate, parent)
{
}

/*!
    Destroys the reply.
*/
QPlaceReviewReply::~QPlaceReviewReply()
{
}

/*!
    Returns the type of reply.
*/
QPlaceReply::Type QPlaceReviewReply::type() const
{
    return QPlaceReply::ReviewReply;
}

 /*!
    Returns the reviews.
*/
QList<QPlaceReview> QPlaceReviewReply::reviews() const
{
    Q_D(const QPlaceReviewReply);
    return d->reviews;
}

/*!
    Sets the \a reviews.
*/
void QPlaceReviewReply::setReviews(const QList<QPlaceReview> &reviews)
{
    Q_D(QPlaceReviewReply);
    d->reviews = reviews;
}

/*!
    Returns the total number of reviews for a place.  If the total number of
    reviews cannot be counted a value of -1 is returned.
*/
int QPlaceReviewReply::totalCount() const
{
    Q_D(const QPlaceReviewReply);
    return d->totalCount;
}

/*!
    Sets the \a total number of media objects for a place.
*/
void QPlaceReviewReply::setTotalCount(int total)
{
    Q_D(QPlaceReviewReply);
    d->totalCount = total;
}
