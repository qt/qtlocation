#include "qplacesearchresult.h"
#include "qplacesearchresult_p.h"

QTM_USE_NAMESPACE

QPlaceSearchResultPrivate::QPlaceSearchResultPrivate() : QSharedData()
{
}

QPlaceSearchResultPrivate::QPlaceSearchResultPrivate(const QPlaceSearchResultPrivate &other)
    : QSharedData()
{
    this->additionalData = other.additionalData;
    this->relevance = other.relevance;
    this->distance = other.distance;
    this->heading = other.heading;
    this->matchType = other.matchType;
    this->type = other.type;
    this->place = other.place;
    this->dymString = other.dymString;
}

QPlaceSearchResultPrivate::~QPlaceSearchResultPrivate()
{
}

bool QPlaceSearchResultPrivate::operator==(const QPlaceSearchResultPrivate &other) const
{
    return (
            this->additionalData == other.additionalData
            && this->relevance == other.relevance
            && this->distance == other.distance
            && this->heading == other.heading
            && this->matchType == other.matchType
            && this->type == other.type
            && this->place == other.place
            && this->dymString == other.dymString
    );
}

/*!
    \class QPlaceSearchResult

    \inmodule QPlaces

    \brief The QPlaceSearchResult class represents a search result object.

    \ingroup places-main

    Each QPlaceSearchResult represents a place with a number of attributes
    such as distance, relevance, etc.
*/

/*!
    Default constructor. Constructs an new search result.
*/
QPlaceSearchResult::QPlaceSearchResult()
    : d(new QPlaceSearchResultPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceSearchResult::QPlaceSearchResult(const QPlaceSearchResult &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceSearchResult::~QPlaceSearchResult()
{
}

QPlaceSearchResult &QPlaceSearchResult::operator =(const QPlaceSearchResult &other) {
    d = other.d;
    return *this;
}

bool QPlaceSearchResult::operator==(const QPlaceSearchResult &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns relevance.
*/
qreal QPlaceSearchResult::relevance() const
{
    return d->relevance;
}

/*!
    Sets the \a relevance.
*/
void QPlaceSearchResult::setRelevance(const qreal &relevance)
{
    d->relevance = relevance;
}

/*!
    Returns the distance.
*/
qreal QPlaceSearchResult::distance() const
{
    return d->distance;
}

/*!
    Sets the \a distance.
*/
void QPlaceSearchResult::setDistance(const qreal &distance)
{
    d->distance = distance;
}

/*!
    Returns heading.
*/
qreal QPlaceSearchResult::heading() const
{
    return d->heading;
}

/*!
    Sets the \a heading.
*/
void QPlaceSearchResult::setHeading(const qreal &heading)
{
    d->heading = heading;
}

/*!
    Returns location match type.
*/
QPlaceSearchResult::LocationMatchType QPlaceSearchResult::matchType() const
{
    return d->matchType;
}

/*!
    Sets the location \a matchType.
*/
void QPlaceSearchResult::setLocationMatchType(const QPlaceSearchResult::LocationMatchType &matchType)
{
    d->matchType = matchType;
}

/*!
    Returns additional data.
*/
QVariantHash QPlaceSearchResult::additionalData() const
{
    return d->additionalData;
}

/*!
    Sets additional \a data.
*/
void QPlaceSearchResult::setAdditionalData(const QVariantHash &data)
{
    d->additionalData = data;
}

/*!
    Returns the place.
*/
QPlaceSearchResult::SearchResultType QPlaceSearchResult::type() const
{
    return d->type;
}

/*!
    Sets the \a place.
*/
void QPlaceSearchResult::setType(const QPlaceSearchResult::SearchResultType &type)
{
    d->type = type;
}

/*!
    Returns the place.
*/
QPlace QPlaceSearchResult::place() const
{
    return d->place;
}

/*!
    Sets the \a place.
*/
void QPlaceSearchResult::setPlace(const QPlace &place)
{
    d->place = place;
}

/*!
    Returns the "did you mean" string.
*/
QString QPlaceSearchResult::didYouMeanSuggestion() const
{
    return d->dymString;
}

/*!
    Sets the "did you mean" \a string.
*/
void QPlaceSearchResult::setDidYouMeanSuggestion(const QString &dymString)
{
    d->dymString = dymString;
}
