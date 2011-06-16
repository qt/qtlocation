#include "qplacesearchquery.h"

QTM_BEGIN_NAMESPACE

class QPlaceSearchQueryPrivate : public QSharedData
{
public:
    QPlaceSearchQueryPrivate();
    QPlaceSearchQueryPrivate(const QPlaceSearchQueryPrivate &other);

    ~QPlaceSearchQueryPrivate();

    bool operator==(const QPlaceSearchQueryPrivate &other) const;

    QString searchTerm;
    QList<QPlaceCategory> categories;
    QPlaceGeoCoordinate searchCenter;
    QPlaceGeoBoundingBox boundingBox;
    int dymNumber;
};

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

QPlaceSearchQueryPrivate::QPlaceSearchQueryPrivate()
    : QSharedData(),
      dymNumber(0)
{
}

QPlaceSearchQueryPrivate::QPlaceSearchQueryPrivate(const QPlaceSearchQueryPrivate &other)
    : QSharedData()
{
    this->searchTerm = other.searchTerm;
    this->categories = other.categories;
    this->searchCenter = other.searchCenter;
    this->boundingBox = other.boundingBox;
    this->dymNumber = other.dymNumber;
}

QPlaceSearchQueryPrivate::~QPlaceSearchQueryPrivate()
{
}

bool QPlaceSearchQueryPrivate::operator==(const QPlaceSearchQueryPrivate &other) const
{
    return (
            this->searchTerm == other.searchTerm
            && this->categories == other.categories
            && this->searchCenter == other.searchCenter
            && this->boundingBox == other.boundingBox
            && this->dymNumber == other.dymNumber
    );
}

/*!
    \class QPlaceSearchQuery

    \inmodule Location

    \brief The QPlaceSearchQuery class represents a query parameters object.

    \ingroup places-main

    The QSearchQuery class represents a query parameters object. Each
    QSearchQuery cointans search query parameters like search term.
*/

/*!
    \enum QPlaceSearchQuery::RelevanceHint

    Defines hints to help rank place results.
    \value DistanceHint
        Distance to the user's current location is relevant.  This is only useful
        if a circular bounding area is used in the query.
    \value RatingHint
        The rating of the place is relevant to the user.
    \value AlphabetHint
        Alphabetic ordering of places is relevant to the user.
*/

/*!
    Default constructor. Constructs an new query object.
*/
QPlaceSearchQuery::QPlaceSearchQuery()
    : QPlaceQuery(),
      d(new QPlaceSearchQueryPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceSearchQuery::QPlaceSearchQuery(const QPlaceSearchQuery &other)
    : QPlaceQuery(other),
      d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceSearchQuery::~QPlaceSearchQuery()
{
}

QPlaceSearchQuery &QPlaceSearchQuery::operator =(const QPlaceSearchQuery &other) {
    this->QPlaceQuery::operator =(other);
    d = other.d;
    return *this;
}

bool QPlaceSearchQuery::operator==(const QPlaceSearchQuery &other) const
{
    return (this->QPlaceQuery::operator ==(other)
            && (*(d.constData()) == *(other.d.constData())));
}


/*!
    Returns the search term.
*/
QString QPlaceSearchQuery::searchTerm() const
{
    return d->searchTerm;
}

/*!
    Sets the search \a term.
*/
void QPlaceSearchQuery::setSearchTerm(const QString &term)
{
    d->searchTerm = term;
}

/*!
    Return the categories to be used in the search query.
    Places need only to belong to one of the categories
    to be considered a match by the query.
*/
QList<QPlaceCategory> QPlaceSearchQuery::categories() const
{
    return d->categories;
}

/*!
    Sets the search query to search by a single \a category
*/
void QPlaceSearchQuery::setCategory(const QPlaceCategory &category)
{
    d->categories.clear();
    if (!category.isEmpty())
        d->categories.append(category);
}

/*!
    Returns search center.
*/
QPlaceGeoCoordinate QPlaceSearchQuery::searchCenter() const
{
    return d->searchCenter;
}

/*!
    Sets the search query to search with search \a center.
*/
void QPlaceSearchQuery::setSearchCenter(const QPlaceGeoCoordinate &center)
{
    d->searchCenter = center;
}

/*!
    Returns search bounding box.
*/
QPlaceGeoBoundingBox QPlaceSearchQuery::boundingBox() const
{
    return d->boundingBox;
}

/*!
    Sets the search query to search with bounding box.
*/
void QPlaceSearchQuery::setBoundingBox(const QPlaceGeoBoundingBox &boundingBox)
{
    d->boundingBox = boundingBox;
}

/*!
    Returns maximum number of "did you mean" suggestions returned by search query.
*/
int QPlaceSearchQuery::didYouMeanSuggestionNumber() const
{
    return d->dymNumber;
}

/*!
    Sets maximum \a number of "did you mean" suggestions returned by search query.
*/
void QPlaceSearchQuery::setDidYouMeanSuggestionNumber(const int &number)
{
    d->dymNumber = number;
}

/*!
    Clears the parameters of the search query.
*/
void QPlaceSearchQuery::clear()
{
    QPlaceQuery::clear();
    d->searchTerm.clear();
    d->categories.clear();
    d->boundingBox = QPlaceGeoBoundingBox();
    d->searchCenter = QPlaceGeoCoordinate();
    d->dymNumber = 0;
}
