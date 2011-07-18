/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeoplace.h"
#include "qgeoplace_p.h"
#include "qlandmark.h"

#ifdef QGEOPLACE_DEBUG
#include <QDebug>
#endif

#if !defined(Q_CC_MWERKS)
template<> QT_PREPEND_NAMESPACE(QGeoPlacePrivate) *QSharedDataPointer<QT_PREPEND_NAMESPACE(QGeoPlacePrivate)>::clone()
{
    return d->clone();
}
#endif

QT_BEGIN_NAMESPACE

/*!
    \class QGeoPlace
    \brief The QGeoPlace class represents basic information about a place.

    \inmodule QtLocation
    \since 1.1

    \ingroup location

    A QGeoPlace contains a coordinate and the corresponding address, along
    with an optional bounding box describing the minimum viewport necessary
    to display the entirety of the place.

    A QGeoPlace may contain an QLandmark instance.  The isLandmark() function
    can be used to determine if this is the case, and the
    QLandmark(const QGeoPlace &place) constructor can be used to restore
    access to the landmark data.

    For example:
    \code
    QGeoPlace p;
    QLandmark l;
    ...
    if (p.isLandmark())
        l = QLandmark(p)
    \endcode
*/

/*!
    Constructs an empty place object.
*/
QGeoPlace::QGeoPlace()
        : d_ptr(new QGeoPlacePrivate())
{
}

/*!
    \internal
*/
QGeoPlace::QGeoPlace(QGeoPlacePrivate *dd)
        : d_ptr(dd) {}

/*!
    Constructs a copy of \a other.
    \since 1.1
*/
QGeoPlace::QGeoPlace(const QGeoPlace &other)
        : d_ptr(other.d_ptr)
{
}

/*!
    Destroys this place.
*/
QGeoPlace::~QGeoPlace()
{
}

/*!
    Assigns \a other to this place and returns a reference
    to this place.
    \since 1.1
*/
QGeoPlace &QGeoPlace::operator= (const QGeoPlace & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

inline QGeoPlacePrivate* QGeoPlace::d_func()
{
    return reinterpret_cast<QGeoPlacePrivate*>(d_ptr.data());
}

inline const QGeoPlacePrivate* QGeoPlace::d_func() const
{
    return reinterpret_cast<const QGeoPlacePrivate*>(d_ptr.constData());
}

/*!
    Returns true if \a other is equal to this place,
    otherwise returns false.
    \since 1.1
*/
bool QGeoPlace::operator== (const QGeoPlace &other) const
{
    return ( *(d_ptr.constData()) == *(other.d_ptr.constData()));
}

/*!
    Returns true if \a other is not equal to this place,
    otherwise returns false.
    \since 1.1
*/
bool QGeoPlace::operator!= (const QGeoPlace &other) const
{
    return (!this->operator ==(other));
}

/*!
    This function returns whether this QGeoPlace instance contain all of the
    information required to construct a QLandmark instance.

    If so, the QLandmark(const QGeoPlace &place) constructor can be used to
    restore access to the landmark data.
    \since 1.1
*/
bool QGeoPlace::isLandmark() const
{
    return (d_ptr->type == QGeoPlacePrivate::LandmarkType);
}

/*!
    Returns the viewport associated with this place.

    The viewport is a suggestion for a size and position of a map window
    which aims to view this palce.
    \since 1.1
*/
QGeoBoundingBox QGeoPlace::viewport() const
{
    Q_D(const QGeoPlace);
    return d->viewport;
}

/*!
    Sets the viewport associated with this place to \a viewport.

    The viewport is a suggestion for a size and position of a map window
    which aims to view this palce.
    \since 1.1
*/
void QGeoPlace::setViewport(const QGeoBoundingBox &viewport)
{
    Q_D(QGeoPlace);
    d->viewport = viewport;
}

/*!
    Returns the coordinate that this place is located at.
    \since 1.1
*/
QGeoCoordinate QGeoPlace::coordinate() const
{
    Q_D(const QGeoPlace);
    return d->coordinate;
}

/*!
    Sets the \a coordinate that this place is located at.
    \since 1.1
*/
void QGeoPlace::setCoordinate(const QGeoCoordinate &coordinate)
{
    Q_D(QGeoPlace);
    d->coordinate = coordinate;
}

/*!
    Returns the address of this place.
    \since 1.1
*/
QGeoAddress QGeoPlace::address() const
{
    Q_D(const QGeoPlace);
    return d->address;
}

/*!
    Sets the \a address of this place.
    \since 1.1
*/
void QGeoPlace::setAddress(const QGeoAddress &address)
{
    Q_D(QGeoPlace);
    d->address = address;
}

/*!
    Returns additional data.
*/
QVariantHash QGeoPlace::additionalData() const
{
    Q_D(const QGeoPlace);
    return d->additionalData;
}

/*!
    Sets additional data.
*/
void QGeoPlace::setAdditionalData(const QVariantHash &data)
{
    Q_D(QGeoPlace);
    d->additionalData = data;
}

/*!
    Returns alternative names.
*/
QList<QPlaceAlternativeValue> QGeoPlace::alternativeNames() const
{
    Q_D(const QGeoPlace);
    return d->alternativeNames;
}

/*!
    Sets alternative names.
*/
void QGeoPlace::setAlternativeNames(const QList<QPlaceAlternativeValue> &alternativeNames)
{
    Q_D(QGeoPlace);
    d->alternativeNames = alternativeNames;
}

/*!
    Returns place score.
*/
qreal QGeoPlace::placeScore() const
{
    Q_D(const QGeoPlace);
    return d->placeScore;
}

/*!
    Sets place score.
*/
void QGeoPlace::setPlaceScore(const qreal &score)
{
    Q_D(QGeoPlace);
    d->placeScore = score;
}

/*!
    Returns business info.
*/
QPlaceBusinessInformation QGeoPlace::businessInformation() const
{
    Q_D(const QGeoPlace);
    return d->businessInfo;
}

/*!
    Sets business info.
*/
void QGeoPlace::setBusinessInformation(const QPlaceBusinessInformation &business)
{
    Q_D(QGeoPlace);
    d->businessInfo = business;
}

/*!
    Returns categories.
*/
QList<QPlaceCategory> QGeoPlace::categories() const
{
    Q_D(const QGeoPlace);
    return d->categories;
}

/*!
    Sets categories.
*/
void QGeoPlace::setCategories(const QList<QPlaceCategory> &categories)
{
    Q_D(QGeoPlace);
    d->categories = categories;
}

/*!
    Returns contacts.
*/
QList<QPlaceContact> QGeoPlace::contacts() const
{
    Q_D(const QGeoPlace);
    return d->contacts;
}

/*!
    Sets contacts.
*/
void QGeoPlace::setContacts(const QList<QPlaceContact> &contacts)
{
    Q_D(QGeoPlace);
    d->contacts = contacts;
}

/*!
    Returns descriptions.
*/
QList<QPlaceDescription> QGeoPlace::descriptions() const
{
    Q_D(const QGeoPlace);
    return d->descriptions;
}

/*!
    Sets descriptions.
*/
void QGeoPlace::setDescriptions(const QList<QPlaceDescription> &descriptions)
{
    Q_D(QGeoPlace);
    d->descriptions = descriptions;
}

/*!
    Returns location.
*/
QGeoLocation QGeoPlace::location() const
{
    Q_D(const QGeoPlace);
    return d->location;
}

/*!
    Sets location.
*/
void QGeoPlace::setLocation(const QGeoLocation &location)
{
    Q_D(QGeoPlace);
    d->location = location;
}

/*!
    Returns alternative locations.
*/
QList<QGeoLocation> QGeoPlace::alternativeLocations() const
{
    Q_D(const QGeoPlace);
    return d->alternativeLocations;
}

/*!
    Sets alternative locations.
*/
void QGeoPlace::setAlternativeLocations(const QList<QGeoLocation> &locations)
{
    Q_D(QGeoPlace);
    d->alternativeLocations = locations;
}

/*!
    Returns rating.
*/
QPlaceRating QGeoPlace::rating() const
{
    Q_D(const QGeoPlace);
    return d->rating;
}

/*!
    Sets rating.
*/
void QGeoPlace::setRating(const QPlaceRating &rating)
{
    Q_D(QGeoPlace);
    d->rating = rating;
}

/*!
    Returns suppliers.
*/
QList<QPlaceSupplier> QGeoPlace::suppliers() const
{
    Q_D(const QGeoPlace);
    return d->suppliers;
}

/*!
    Sets suppliers.
*/
void QGeoPlace::setSuppliers(const QList<QPlaceSupplier> &data)
{
    Q_D(QGeoPlace);
    d->suppliers = data;
}

/*!
    Returns feeds list.
*/
QStringList QGeoPlace::feeds() const
{
    Q_D(const QGeoPlace);
    return d->feeds;
}

/*!
    Sets feeds list.
*/
void QGeoPlace::setFeeds(const QStringList &feeds)
{
    Q_D(QGeoPlace);
    d->feeds = feeds;
}

/*!
    Returns list with media objects connected to this place.
*/
QPlacePaginationList<QPlaceMediaObject> QGeoPlace::media() const
{
    Q_D(const QGeoPlace);
    return d->media;
}

/*!
    Sets list with media objects connected to this place.
*/
void QGeoPlace::setMedia(const QPlacePaginationList<QPlaceMediaObject> &media)
{
    Q_D(QGeoPlace);
    d->media = media;
}

/*!
    Returns media count.
*/
int QGeoPlace::mediaCount() const
{
    Q_D(const QGeoPlace);
    return d->mediaCount;
}

/*!
    Sets media count.
*/
void QGeoPlace::setMediaCount(const int &count)
{
    Q_D(QGeoPlace);
    d->mediaCount = count;
}

/*!
    Returns name.
*/
QString QGeoPlace::name() const
{
    Q_D(const QGeoPlace);
    return d->name;
}

/*!
    Sets name.
*/
void QGeoPlace::setName(const QString &name)
{
    Q_D(QGeoPlace);
    d->name = name;
}

/*!
    Returns placeId.
*/
QString QGeoPlace::placeId() const
{
    Q_D(const QGeoPlace);
    return d->placeId;
}

/*!
    Sets placeId.
*/
void QGeoPlace::setPlaceId(const QString &placeId)
{
    Q_D(QGeoPlace);
    d->placeId = placeId;
}

/*!
    Returns reviews.
*/
QPlacePaginationList<QPlaceReview> QGeoPlace::reviews() const
{
    Q_D(const QGeoPlace);
    return d->reviews;
}

/*!
    Sets reviews.
*/
void QGeoPlace::setReviews(const QPlacePaginationList<QPlaceReview> &reviews)
{
    Q_D(QGeoPlace);
    d->reviews = reviews;
}

/*!
    Returns review count.
*/
int QGeoPlace::reviewCount() const
{
    Q_D(const QGeoPlace);
    return d->reviewCount;
}

/*!
    Sets review count.
*/
void QGeoPlace::setReviewCount(const int &count)
{
    Q_D(QGeoPlace);
    d->reviewCount = count;
}

/*!
    Returns short description.
*/
QString QGeoPlace::shortDescription() const
{
    Q_D(const QGeoPlace);
    return d->shortDescription;
}

/*!
    Sets short description.
*/
void QGeoPlace::setShortDescription(const QString &description)
{
    Q_D(QGeoPlace);
    d->shortDescription = description;
}

/*!
    Returns list of tags.
*/
QStringList QGeoPlace::tags() const
{
    Q_D(const QGeoPlace);
    return d->tags;
}

/*!
    Sets tags.
*/
void QGeoPlace::setTags(const QStringList &tags)
{
    Q_D(QGeoPlace);
    d->tags = tags;
}

/*!
    Returns whether the details of this place have been fetched or not.
*/
bool QGeoPlace::detailsFetched() const
{
    Q_D(const QGeoPlace);
    return d->detailsFetched;
}

/*!
    Sets whether the details of this place have been \a fetched or not.
*/
void QGeoPlace::setDetailsFetched(bool fetched)
{
    Q_D(QGeoPlace);
    d->detailsFetched = fetched;
}

/*******************************************************************************
*******************************************************************************/

QGeoPlacePrivate::QGeoPlacePrivate()
        : QSharedData(),
        type(QGeoPlacePrivate::GeoPlaceType),
        placeScore(0.0),
        mediaCount(0),
        reviewCount(0),
        detailsFetched(false)
{
}

QGeoPlacePrivate::QGeoPlacePrivate(const QGeoPlacePrivate &other)
        : QSharedData(other),
        type(other.type),
        viewport(other.viewport),
        coordinate(other.coordinate),
        address(other.address),
        additionalData(other.additionalData),
        placeScore(other.placeScore),
        businessInfo(other.businessInfo),
        categories(other.categories),
        contacts(other.contacts),
        descriptions(other.descriptions),
        location(other.location),
        alternativeLocations(other.alternativeLocations),
        rating(other.rating),
        suppliers(other.suppliers),
        feeds(other.feeds),
        media(other.media),
        name(other.name),
        placeId(other.placeId),
        reviews(other.reviews),
        reviewCount(other.reviewCount),
        shortDescription(other.shortDescription),
        tags(other.tags),
        detailsFetched(other.detailsFetched)
{
}

QGeoPlacePrivate::~QGeoPlacePrivate() {}

QGeoPlacePrivate& QGeoPlacePrivate::operator= (const QGeoPlacePrivate & other)
{
    type = other.type;
    viewport = other.viewport;
    coordinate = other.coordinate;
    address = other.address;

    return *this;
}

bool QGeoPlacePrivate::operator== (const QGeoPlacePrivate &other) const
{

#ifdef QGEOPLACE_DEBUG
    qDebug() << "type" << (type == other.type);
    qDebug() << "viewport" << (viewport == other.viewport);
    qDebug() << "coordinate:" <<  (coordinate == other.coordinate);
    qDebug() << "address:" << (address == other.address);
    qDebug() << "additionalData: " << (additionalData == other.additionalData);
    qDebug() << "alternativeNames: " << (alternativeNames == other.alternativeNames);
    qDebug() << "placeScore: " << (placeScore == other.placeScore);
    qDebug() << "businessInfo: " << (businessInfo == other.businessInfo);
    qDebug() << "categories: " << (categories == other.categories);
    qDebug() << "descriptions: " << (descriptions == other.descriptions);
    qDebug() << "location:" << (location == other.location);
    qDebug() << "alternativeLocations:" << (alternativeLocations == other.alternativeLocations);
    qDebug() << "rating" << (rating == other.rating);
    qDebug() << "suppliers" << (suppliers == other.suppliers);
    qDebug() << "feeds " << (feeds == other.feeds);
    qDebug() << "media " << (media == other.media);
    qDebug() << "mediaCount " << (mediaCount == other.mediaCount);
    qDebug() << "name " << (name == other.name);
    qDebug() << "placeId" << (placeId == other.placeId);
    qDebug() << "reviews" << (reviews == other.reviews);
    qDebug() << "review count" << (reviewCount == other.reviewCount);
    qDebug() << "shortDescription" << (shortDescription == other.shortDescription);
    qDebug() << "tags" << (tags == other.tags);
#endif

    return (type == other.type
            && viewport == other.viewport
            && coordinate == other.coordinate
            && address == other.address
            && additionalData == other.additionalData
            && alternativeNames == other.alternativeNames
            && placeScore == other.placeScore
            && businessInfo == other.businessInfo
            && categories == other.categories
            && contacts == other.contacts
            && descriptions == other.descriptions
            && location == other.location
            && alternativeLocations == other.alternativeLocations
            && rating == other.rating
            && suppliers == other.suppliers
            && feeds == other.feeds
            && media == other.media
            && mediaCount == other.mediaCount
            && name == other.name
            && placeId == other.placeId
            && reviews == other.reviews
            && reviewCount == other.reviewCount
            && shortDescription == other.shortDescription
            && tags == other.tags
            );
}

QT_END_NAMESPACE

