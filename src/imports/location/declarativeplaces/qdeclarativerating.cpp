#include "qdeclarativerating_p.h"

QT_USE_NAMESPACE

/*!
    \qmlclass Rating

    \brief The Rating element holds place rating information.
    \inherits QObject

    \ingroup qml-places
*/

QDeclarativeRating::QDeclarativeRating(QObject* parent)
        : QObject(parent) {}

QDeclarativeRating::QDeclarativeRating(const QPlaceRating &rating,
        QObject *parent)
        : QObject(parent),
        m_rating(rating) {}

QDeclarativeRating::~QDeclarativeRating() {}

void QDeclarativeRating::setRating(const QPlaceRating &rating)
{
    QPlaceRating previous = m_rating;
    m_rating = rating;

    if (rating.value() != previous.value()) {
        emit valueChanged();
    }
    if (rating.count() != previous.count()) {
        emit countChanged();
    }
}

QPlaceRating QDeclarativeRating::rating() const
{
    return m_rating;
}

/*!
    \qmlproperty qreal Rating::value

    This property holds the value of rating
*/

void QDeclarativeRating::setValue(const qreal &value)
{
    if (m_rating.value() != value) {
        m_rating.setValue(value);
        emit valueChanged();
    }
}

qreal QDeclarativeRating::value() const
{
    return m_rating.value();
}

/*!
    \qmlproperty int Rating::count

    This property holds number of votes with rate.
*/

void QDeclarativeRating::setCount(const int &count)
{
    if (m_rating.count() != count) {
        m_rating.setCount(count);
        emit countChanged();
    }
}

int QDeclarativeRating::count() const
{
    return m_rating.count();
}

