#include "qdeclarativereview_p.h"

QTM_USE_NAMESPACE

/*!
    \qmlclass Review

    \brief The Review element holds review data.
    \inherits QObject

    Review cointains many properties holding data of the description like user name,
    title, description, etc.

    \ingroup qml-places
*/

QDeclarativeReview::QDeclarativeReview(QObject* parent)
        : QObject(parent)
{
}

QDeclarativeReview::QDeclarativeReview(const QPlaceReview &src,
        QObject *parent)
        : QObject(parent),
          m_declarativeSupplier(src.supplier()),
          m_src(src)
{
}

QDeclarativeReview::~QDeclarativeReview()
{
}

void QDeclarativeReview::setReview(const QPlaceReview &src)
{
    QPlaceReview previous = m_src;
    m_src = src;

    if (previous.date() != m_src.date()) {
        emit dateChanged();
    }
    if (previous.description() != m_src.description()) {
        emit descriptionChanged();
    }
    if (previous.language() != m_src.language()) {
        emit languageChanged();
    }
    if (previous.originatorURL() != m_src.originatorURL()) {
        emit originatorURLChanged();
    }
    if (previous.reviewId() != m_src.reviewId()) {
        emit reviewIdChanged();
    }
    if (previous.title() != m_src.title()) {
        emit titleChanged();
    }
    if (previous.userId() != m_src.userId()) {
        emit userIdChanged();
    }
    if (previous.userName() != m_src.userName()) {
        emit userNameChanged();
    }
    if (previous.mediaIds() != m_src.mediaIds()) {
        emit mediaIdsChanged();
    }
    if (previous.rating() != m_src.rating()) {
        emit ratingChanged();
    }
    if (previous.helpfulVotings() != m_src.helpfulVotings()) {
        emit helpfulVotingsChanged();
    }
    if (previous.unhelpfulVotings() != m_src.unhelpfulVotings()) {
        emit unhelpfulVotingsChanged();
    }
    if (previous.supplier() != m_src.supplier()) {
        m_declarativeSupplier.setSupplier(src.supplier());
        emit supplierChanged();
    }
}

QPlaceReview QDeclarativeReview::review() const
{
    return m_src;
}

/*!
    \qmlproperty string Review::date

    This property holds date of the review.
*/

void QDeclarativeReview::setDate(const QString &date)
{
    if (m_src.date() != date) {
        m_src.setDate(date);
        emit dateChanged();
    }
}

QString QDeclarativeReview::date() const
{
    return m_src.date();
}

/*!
    \qmlproperty string Review::description

    This property holds description.
*/

void QDeclarativeReview::setDescription(const QString &description)
{
    if (m_src.description() != description) {
        m_src.setDescription(description);
        emit descriptionChanged();
    }
}

QString QDeclarativeReview::description() const
{
    return m_src.description();
}

/*!
    \qmlproperty string Review::language

    This property holds language.
*/

void QDeclarativeReview::setLanguage(const QString &language)
{
    if (m_src.language() != language) {
        m_src.setLanguage(language);
        emit languageChanged();
    }
}

QString QDeclarativeReview::language() const
{
    return m_src.language();
}

/*!
    \qmlproperty string Review::originatorURL

    This property holds originator URL.
*/

void QDeclarativeReview::setOriginatorURL(const QString &originatorURL)
{
    if (m_src.originatorURL() != originatorURL) {
        m_src.setOriginatorURL(originatorURL);
        emit originatorURLChanged();
    }
}

QString QDeclarativeReview::originatorURL() const
{
    return m_src.originatorURL();
}

/*!
    \qmlproperty string Review::reviewId

    This property holds review Id.
*/
void QDeclarativeReview::setReviewId(const QString &reviewId)
{
    if (m_src.reviewId() != reviewId) {
        m_src.setReviewId(reviewId);
        emit reviewIdChanged();
    }
}

QString QDeclarativeReview::reviewId() const
{
    return m_src.reviewId();
}

/*!
    \qmlproperty string Review::title

    This property holds title.
*/
void QDeclarativeReview::setTitle(const QString &title)
{
    if (m_src.title() != title) {
        m_src.setTitle(title);
        emit titleChanged();
    }
}

QString QDeclarativeReview::title() const
{
    return m_src.title();
}

/*!
    \qmlproperty string Review::userId

    This property holds user id.
*/
void QDeclarativeReview::setUserId(const QString &userId)
{
    if (m_src.userId() != userId) {
        m_src.setUserId(userId);
        emit reviewIdChanged();
    }
}

QString QDeclarativeReview::userId() const
{
    return m_src.userId();
}

/*!
    \qmlproperty string Review::userName

    This property holds user name.
*/
void QDeclarativeReview::setUserName(const QString &userName)
{
    if (m_src.userName() != userName) {
        m_src.setUserName(userName);
        emit userNameChanged();
    }
}

QString QDeclarativeReview::userName() const
{
    return m_src.userName();
}

/*!
    \qmlproperty string Review::mediaIds

    This property holds media ids connected to that review.
*/
void QDeclarativeReview::setMediaIds(const QStringList &mediaIds)
{
    if (m_src.mediaIds() != mediaIds) {
        m_src.setMediaIds(mediaIds);
        emit mediaIdsChanged();
    }
}

QStringList QDeclarativeReview::mediaIds() const
{
    return m_src.mediaIds();
}

/*!
    \qmlproperty string Review::rating

    This property holds rating value.
*/
void QDeclarativeReview::setRating(const double &rating)
{
    if (m_src.rating() != rating) {
        m_src.setRating(rating);
        emit ratingChanged();
    }
}

double QDeclarativeReview::rating() const
{
    return m_src.rating();
}

/*!
    \qmlproperty int Review::helpfulVotings

    This property holds helpful votings count.
*/
void QDeclarativeReview::setHelpfulVotings(const int &helpfulVotings)
{
    if (m_src.helpfulVotings() != helpfulVotings) {
        m_src.setHelpfulVotings(helpfulVotings);
        emit helpfulVotingsChanged();
    }
}

int QDeclarativeReview::helpfulVotings() const
{
    return m_src.helpfulVotings();
}

/*!
    \qmlproperty int Review::unHelpfulVotings

    This property holds not helpful votings count.
*/
void QDeclarativeReview::setUnhelpfulVotings(const int &unhelpfulVotings)
{
    if (m_src.unhelpfulVotings() != unhelpfulVotings) {
        m_src.setUnhelpfulVotings(unhelpfulVotings);
        emit unhelpfulVotingsChanged();
    }
}

int QDeclarativeReview::unhelpfulVotings() const
{
    return m_src.unhelpfulVotings();
}

/*!
    \qmlproperty string Review::supplier

    This property holds supplier info.
*/
void QDeclarativeReview::setSupplier(QDeclarativeSupplier *src)
{
    if (m_declarativeSupplier.supplier() != src->supplier()) {
        m_declarativeSupplier.setSupplier(src->supplier());
        emit supplierChanged();
    }
}

QDeclarativeSupplier *QDeclarativeReview::supplier()
{
    return &m_declarativeSupplier;
}
