#include "qplace.h"
#include "qplace_p.h"

QTM_USE_NAMESPACE

QPlacePrivate::QPlacePrivate() : QSharedData()
{
}

QPlacePrivate::QPlacePrivate(const QPlacePrivate &other)
    : QSharedData(),
      placeScore(0),
      mediaCount(0),
      reviewCount(0)
{
    this->additionalData = other.additionalData;
    this->alternativeNames = other.alternativeNames;
    this->placeScore = other.placeScore;
    this->businessInfo = other.businessInfo;
    this->categories = other.categories;
    this->contacts = other.contacts;
    this->descriptions = other.descriptions;
    this->location = other.location;
    this->alternativeLocations = other.alternativeLocations;
    this->rating = other.rating;
    this->suppliers = other.suppliers;
    this->feeds = other.feeds;
    this->media = other.media;
    this->mediaCount = other.mediaCount;
    this->name = other.name;
    this->placeId = other.placeId;
    this->reviews = other.reviews;
    this->reviewCount = other.reviewCount;
    this->shortDescription = other.shortDescription;
    this->tags = other.tags;
}

QPlacePrivate::~QPlacePrivate()
{
}

bool QPlacePrivate::operator==(const QPlacePrivate &other) const
{
    return (
            this->additionalData == other.additionalData
            && this->alternativeNames == other.alternativeNames
            && this->placeScore == other.placeScore
            && this->businessInfo == other.businessInfo
            && this->categories == other.categories
            && this->contacts == other.contacts
            && this->descriptions == other.descriptions
            && this->location == other.location
            && this->alternativeLocations == other.alternativeLocations
            && this->rating == other.rating
            && this->suppliers == other.suppliers
            && this->feeds == other.feeds
            && this->media == other.media
            && this->mediaCount == other.mediaCount
            && this->name == other.name
            && this->placeId == other.placeId
            && this->reviews == other.reviews
            && this->reviewCount == other.reviewCount
            && this->shortDescription == other.shortDescription
            && this->tags == other.tags
    );
}

/*!
    \class QPlaces::QPlace

    \inmodule QPlaces

    \brief The QPlace class represents a place object.

    Each QPlace represents a place object with a number of attributes
    such as location, media, categories etc.

    Place objects are read-only, e.g. user of API might get list of media objects
    associated to specific place but can not edit its content. Not all data might be
    avaliable in the begining. Search response is not providing all data, they need
    to be loaded later from place request.

    QPlace is an in memory representation of a place object.
*/


/*!
    Default constructor. Constructs an new place object.
*/
QPlace::QPlace()
    : d(new QPlacePrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlace::QPlace(const QPlace &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlace::~QPlace()
{
}

QPlace &QPlace::operator =(const QPlace &other) {
    d = other.d;
    return *this;
}

bool QPlace::operator==(const QPlace &other) const
{
    return (*(d.constData()) == *(other.d.constData()));
}

/*!
    Returns additional data.
*/
QVariantHash QPlace::additionalData() const
{
    return d->additionalData;
}

/*!
    Sets additional data.
*/
void QPlace::setAdditionalData(const QVariantHash &data)
{
    d->additionalData = data;
}

/*!
    Returns alternative names.
*/
QList<QPlaceAlternativeValue> QPlace::alternativeNames() const
{
    return d->alternativeNames;
}

/*!
    Sets alternative names.
*/
void QPlace::setAlternativeNames(const QList<QPlaceAlternativeValue> &alternativeNames)
{
    d->alternativeNames = alternativeNames;
}

/*!
    Returns place score.
*/
double QPlace::placeScore() const
{
    return d->placeScore;
}

/*!
    Sets place score.
*/
void QPlace::setPlaceScore(const double &data)
{
    d->placeScore = data;
}

/*!
    Returns business info.
*/
QPlaceBusinessInformation QPlace::businessInformation() const
{
    return d->businessInfo;
}

/*!
    Sets business info.
*/
void QPlace::setBusinessInformation(const QPlaceBusinessInformation &business)
{
    d->businessInfo = business;
}

/*!
    Returns categories.
*/
QList<QPlaceCategory> QPlace::categories() const
{
    return d->categories;
}

/*!
    Sets categories.
*/
void QPlace::setCategories(const QList<QPlaceCategory> &categories)
{
    d->categories = categories;
}

/*!
    Returns contacts.
*/
QList<QPlaceContact> QPlace::contacts() const
{
    return d->contacts;
}

/*!
    Sets contacts.
*/
void QPlace::setContacts(const QList<QPlaceContact> &contacts)
{
    d->contacts = contacts;
}

/*!
    Returns descriptions.
*/
QList<QPlaceDescription> QPlace::descriptions() const
{
    return d->descriptions;
}

/*!
    Sets descriptions.
*/
void QPlace::setDescriptions(const QList<QPlaceDescription> &descriptions)
{
    d->descriptions = descriptions;
}

/*!
    Returns location.
*/
QPlaceLocation QPlace::location() const
{
    return d->location;
}

/*!
    Sets location.
*/
void QPlace::setLocation(const QPlaceLocation &location)
{
    d->location = location;
}

/*!
    Returns alternative locations.
*/
QList<QPlaceLocation> QPlace::alternativeLocations() const
{
    return d->alternativeLocations;
}

/*!
    Sets alternative locations.
*/
void QPlace::setAlternativeLocations(const QList<QPlaceLocation> &locations)
{
    d->alternativeLocations = locations;
}

/*!
    Returns rating.
*/
QPlaceRating QPlace::rating() const
{
    return d->rating;
}

/*!
    Sets rating.
*/
void QPlace::setRating(const QPlaceRating &rating)
{
    d->rating = rating;
}

/*!
    Returns suppliers.
*/
QList<QPlaceSupplier> QPlace::suppliers() const
{
    return d->suppliers;
}

/*!
    Sets suppliers.
*/
void QPlace::setSuppliers(const QList<QPlaceSupplier> &data)
{
    d->suppliers = data;
}

/*!
    Returns feeds list.
*/
QStringList QPlace::feeds() const
{
    return d->feeds;
}

/*!
    Sets feeds list.
*/
void QPlace::setFeeds(const QStringList &feeds)
{
    d->feeds = feeds;
}

/*!
    Returns list with media objects connected to this place.
*/
QPlacePaginationList<QPlaceMediaObject> QPlace::media() const
{
    return d->media;
}

/*!
    Sets list with media objects connected to this place.
*/
void QPlace::setMedia(const QPlacePaginationList<QPlaceMediaObject> &media)
{
    d->media = media;
}

/*!
    Returns media count.
*/
uint QPlace::mediaCount() const
{
    return d->mediaCount;
}

/*!
    Sets media count.
*/
void QPlace::setMediaCount(const uint &data)
{
    d->mediaCount = data;
}

/*!
    Returns name.
*/
QString QPlace::name() const
{
    return d->name;
}

/*!
    Sets name.
*/
void QPlace::setName(const QString &name)
{
    d->name = name;
}

/*!
    Returns placeId.
*/
QString QPlace::placeId() const
{
    return d->placeId;
}

/*!
    Sets placeId.
*/
void QPlace::setPlaceId(const QString &placeId)
{
    d->placeId = placeId;
}

/*!
    Returns reviews.
*/
QPlacePaginationList<QPlaceReview> QPlace::reviews() const
{
    return d->reviews;
}

/*!
    Sets reviews.
*/
void QPlace::setReviews(const QPlacePaginationList<QPlaceReview> &reviews)
{
    d->reviews = reviews;
}

/*!
    Returns review count.
*/
uint QPlace::reviewCount() const
{
    return d->reviewCount;
}

/*!
    Sets review count.
*/
void QPlace::setReviewCount(const uint &data)
{
    d->reviewCount = data;
}

/*!
    Returns short description.
*/
QString QPlace::shortDescription() const
{
    return d->shortDescription;
}

/*!
    Sets short description.
*/
void QPlace::setShortDescription(const QString &description)
{
    d->shortDescription = description;
}

/*!
    Returns list of tags.
*/
QStringList QPlace::tags() const
{
    return d->tags;
}

/*!
    Sets tags.
*/
void QPlace::setTags(const QStringList &tags)
{
    d->tags = tags;
}
