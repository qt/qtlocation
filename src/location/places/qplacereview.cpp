#include "qplacereview.h"
#include "qplacereview_p.h"

using namespace QT_PLACES_NAMESPACE;

QPlaceReviewPrivate::QPlaceReviewPrivate() : QSharedData()
{
}

QPlaceReviewPrivate::QPlaceReviewPrivate(const QPlaceReviewPrivate &other)
    : QSharedData()
{
    this->date = other.date;
    this->description = other.description;
    this->language = other.language;
    this->helpfulVotings = other.helpfulVotings;
    this->mediaIds = other.mediaIds;
    this->notHelpfulVotings = other.notHelpfulVotings;
    this->rating = other.rating;
    this->reviewId = other.reviewId;
    this->supplier = other.supplier;
    this->title = other.title;
    this->userId = other.userId;
    this->userName = other.userName;
    this->originatorURL = other.originatorURL;
}

QPlaceReviewPrivate::~QPlaceReviewPrivate()
{
}

bool QPlaceReviewPrivate::operator==(const QPlaceReviewPrivate &other) const
{
    return (
            this->date == other.date
            && this->description == other.description
            && this->language == other.language
            && this->helpfulVotings == other.helpfulVotings
            && this->mediaIds == other.mediaIds
            && this->notHelpfulVotings == other.notHelpfulVotings
            && this->rating == other.rating
            && this->reviewId == other.reviewId
            && this->supplier == other.supplier
            && this->title == other.title
            && this->userId == other.userId
            && this->userName == other.userName
            && this->originatorURL == other.originatorURL
    );
}

/*!
    \class QPlaceReview

    \inmodule QPlaces

    \brief The QPlaceReview class represents a review object.


    Each QPlaceReview represents a review object with a number of attributes
    such as rating, review id, connected media etc. Each QPlaceReview is associated
    with place.

    Review objects are read-only, e.g. user of API might get list of review objects
    associated to specific place but can not edit its content. User might also create new review.

    QPlaceReview is an in memory representation of a review object.
*/


/*!
    Constructs an new review object.
*/
QPlaceReview::QPlaceReview()
    : d(new QPlaceReviewPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceReview::QPlaceReview(const QPlaceReview &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceReview::~QPlaceReview()
{
}

QPlaceReview &QPlaceReview::operator =(const QPlaceReview &other) {
    d = other.d;
    return *this;
}

bool QPlaceReview::operator==(const QPlaceReview &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns review date.
*/
QString QPlaceReview::date() const
{
    return d->date;
}

/*!
    Sets review date.
*/
void QPlaceReview::setDate(const QString &data)
{
    d->date = data;
}

/*!
    Returns description.
*/
QString QPlaceReview::description() const
{
    return d->description;
}

/*!
    Sets description.
*/
void QPlaceReview::setDescription(const QString &data)
{
    d->description = data;
}

/*!
    Returns language.
*/
QString QPlaceReview::language() const
{
    return d->language;
}

/*!
    Sets language.
*/
void QPlaceReview::setLanguage(const QString &data)
{
    d->language = data;
}

/*!
    Returns "thumbs up".
*/
uint QPlaceReview::helpfulVotings() const
{
    return d->helpfulVotings;
}

/*!
    Sets "thumbs up".
*/
void QPlaceReview::setHelpfulVotings(const uint &data)
{
    d->helpfulVotings = data;
}

/*!
    Returns media ids associated with review.
*/
QStringList QPlaceReview::mediaIds() const
{
    return d->mediaIds;
}

/*!
    Sets media ids.
*/
void QPlaceReview::setMediaIds(const QStringList &data)
{
    d->mediaIds = data;
}

/*!
    Returns "thumbs down".
*/
uint QPlaceReview::notHelpfulVotings() const
{
    return d->notHelpfulVotings;
}

/*!
    Sets "thumbs down".
*/
void QPlaceReview::setNotHelpfulVotings(const uint &data)
{
    d->notHelpfulVotings = data;
}

/*!
    Returns rating.
*/
double QPlaceReview::rating() const
{
    return d->rating;
}

/*!
    Sets rating.
*/
void QPlaceReview::setRating(const double &data)
{
    d->rating = data;
}

/*!
    Returns review id.
*/
QString QPlaceReview::reviewId() const
{
    return d->reviewId;
}

/*!
    Sets review id.
*/
void QPlaceReview::setReviewId(const QString &data)
{
    d->reviewId = data;
}

/*!
    Returns supplier. Do not remove it.
*/
QPlaceSupplier QPlaceReview::supplier() const
{
    return d->supplier;
}

/*!
    Sets supplier.
*/
void QPlaceReview::setSupplier(const QPlaceSupplier &data)
{
    d->supplier = data;
}

/*!
    Returns review title.
*/
QString QPlaceReview::title() const
{
    return d->title;
}

/*!
    Sets title.
*/
void QPlaceReview::setTitle(const QString &data)
{
    d->title = data;
}

/*!
    Returns user id.
*/
QString QPlaceReview::userId() const
{
    return d->userId;
}

/*!
    Sets user id.
*/
void QPlaceReview::setUserId(const QString &data)
{
    d->userId = data;
}

/*!
    Returns user name.
*/
QString QPlaceReview::userName() const
{
    return d->userName;
}

/*!
    Sets user name.
*/
void QPlaceReview::setUserName(const QString &data)
{
    d->userName = data;
}

/*!
    Returns originator URL.
*/
QString QPlaceReview::originatorURL() const
{
    return d->originatorURL;
}

/*!
    Sets originator URL.
*/
void QPlaceReview::setOriginatorURL(const QString &data)
{
    d->originatorURL = data;
}
