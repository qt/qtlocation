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
    Returns a collection of content associated with a place.
    This collection is a map with the key being the index of the content object
    and value being the content object itself.

    The \a type specifies which kind of content is to be retrieved.
*/
QPlaceContent::Collection QGeoPlace::content(QPlaceContent::Type type) const
{
    Q_D(const QGeoPlace);
    return d->contentCollections.value(type);
}

/*!
    Sets a collection of \a content for the given \a type.
*/
void QGeoPlace::setContent(QPlaceContent::Type type, const QPlaceContent::Collection &content)
{
    Q_D(QGeoPlace);
    d->contentCollections.insert(type, content);
}

/*!
    Adds a collection of \a content of the given \a type to the place.  Any index in \a content
    that already exists is overwritten.
*/
void QGeoPlace::addContent(QPlaceContent::Type type, const QPlaceContent::Collection &content)
{
    Q_D(QGeoPlace);
    QMapIterator<int, QPlaceContent> iter(content);
    while (iter.hasNext()) {
        iter.next();
        d->contentCollections[type].insert(iter.key(), iter.value());
    }
}

/*!
    Returns the total count of content objects of the given \a type.
    This total count indicates how many the manager should have available.
    (As opposed to how many objects this place instance is currently assigned).
    A negative count indicates that the total number of items is unknown.
*/
int QGeoPlace::contentCount(QPlaceContent::Type type) const
{
    Q_D(const QGeoPlace);
    return d->contentCounts.value(type, 0);
}

/*!
    Sets the \a totalCount of content objects of the given \a type.
*/
void QGeoPlace::setContentCount(QPlaceContent::Type type, int totalCount)
{
    Q_D(QGeoPlace);
    d->contentCounts.insert(type, totalCount);
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
    Returns the primary phone number for this place.
*/
QString QGeoPlace::primaryPhone() const
{
    Q_D(const QGeoPlace);
    return d->primaryPhone;
}

/*!
    Sets the primary \a phone number of this place.
*/
void QGeoPlace::setPrimaryPhone(const QString &phone)
{
    Q_D(QGeoPlace);
    d->primaryPhone = phone;
}

/*!
    Returns the primary fax number for this place.
*/
QString QGeoPlace::primaryFax() const
{
    Q_D(const QGeoPlace);
    return d->primaryFax;
}

/*!
    Sets the primary \a fax number for this place
*/
void QGeoPlace::setPrimaryFax(const QString fax)
{
    Q_D(QGeoPlace);
    d->primaryFax = fax;
}

/*!
    Returns the primary email address for this place.
*/
QString QGeoPlace::primaryEmail() const
{
    Q_D(const QGeoPlace);
    return d->primaryEmail;
}

/*!
    Sets the primary \a email address for this place.
*/
void QGeoPlace::setPrimaryEmail(const QString &email)
{
    Q_D(QGeoPlace);
    d->primaryEmail = email;
}

/*!
    Returns the primary URL of this place.
*/
QUrl QGeoPlace::primaryUrl() const
{
    Q_D(const QGeoPlace);
    return d->primaryUrl;
}

/*!
    Sets the primary \a url of this place.
*/
void QGeoPlace::setPrimaryUrl(const QUrl &url)
{
    Q_D(QGeoPlace);
    d->primaryUrl = url;
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

/*!
    Returns the extended attributes of the place
*/
QGeoPlace::ExtendedAttributes QGeoPlace::extendedAttributes() const
{
    Q_D(const QGeoPlace);
    return d->extendedAttributes;
}

/*!
    Sets the extended attributes of the place.
*/
void QGeoPlace::setExtendedAttributes(const ExtendedAttributes &attributes)
{
    Q_D(QGeoPlace);
    d->extendedAttributes = attributes;
}

/*!
    Adds a single attribute to the place.  If the attribute already
    exists then the old value is overwritten.
*/
void QGeoPlace::insertExtendedAttribute(const QString &key, const QPlaceAttribute &value)
{
    Q_D(QGeoPlace);
    d->extendedAttributes.insert(key, value);
}

/*******************************************************************************
*******************************************************************************/

QGeoPlacePrivate::QGeoPlacePrivate()
        : QSharedData(),
        reviewCount(0),
        detailsFetched(false)
{
}

QGeoPlacePrivate::QGeoPlacePrivate(const QGeoPlacePrivate &other)
        : QSharedData(other),
        additionalData(other.additionalData),
        categories(other.categories),
        descriptions(other.descriptions),
        location(other.location),
        rating(other.rating),
        suppliers(other.suppliers),
        feeds(other.feeds),
        contentCollections(other.contentCollections),
        contentCounts(other.contentCounts),
        name(other.name),
        placeId(other.placeId),
        reviews(other.reviews),
        reviewCount(other.reviewCount),
        tags(other.tags),
        detailsFetched(other.detailsFetched),
        primaryPhone(other.primaryPhone),
        primaryFax(other.primaryFax),
        primaryEmail(other.primaryEmail),
        primaryUrl(other.primaryUrl),
        extendedAttributes(other.extendedAttributes)
{
}

QGeoPlacePrivate::~QGeoPlacePrivate() {}

QGeoPlacePrivate& QGeoPlacePrivate::operator= (const QGeoPlacePrivate & other)
{
    additionalData = other.additionalData;
    categories = other.categories;
    descriptions = other.descriptions;
    location = other.location;
    rating = other.rating;
    suppliers = other.suppliers;
    feeds = other.feeds;
    name = other.name;
    placeId = other.placeId;
    reviews = other.reviews;
    reviewCount = other.reviewCount;
    shortDescription = other.shortDescription;
    tags = other.tags;
    primaryPhone = other.primaryPhone;
    primaryFax = other.primaryFax;
    primaryEmail = other.primaryEmail;
    primaryUrl = other.primaryUrl;

    return *this;
}

bool QGeoPlacePrivate::operator== (const QGeoPlacePrivate &other) const
{

#ifdef QGEOPLACE_DEBUG
    qDebug() << "additionalData: " << (additionalData == other.additionalData);
    qDebug() << "categories: " << (categories == other.categories);
    qDebug() << "descriptions: " << (descriptions == other.descriptions);
    qDebug() << "location:" << (location == other.location);
    qDebug() << "rating" << (rating == other.rating);
    qDebug() << "suppliers" << (suppliers == other.suppliers);
    qDebug() << "feeds " << (feeds == other.feeds);
    qDebug() << "contentCollections " << (contentCollections == other.contentCollections);
    qDebug() << "contentCounts " << (contentCounts == other.contentCounts);
    qDebug() << "name " << (name == other.name);
    qDebug() << "placeId" << (placeId == other.placeId);
    qDebug() << "reviews" << (reviews == other.reviews);
    qDebug() << "review count" << (reviewCount == other.reviewCount);
    qDebug() << "tags" << (tags == other.tags);
    qDebug() << "phone" << (primaryPhone == other.primaryPhone);
    qDebug() << "fax" << (primaryFax == other.primaryFax);
    qDebug() << "email" << (primaryEmail == other.primaryEmail);
    qDebug() << "url" << (primaryUrl == other.primaryUrl);
    qDebug() << "extendedAttributes" << (extendedAttributes == other.extendedAttributes);
#endif

    return (additionalData == other.additionalData
            && categories == other.categories
            && descriptions == other.descriptions
            && location == other.location
            && rating == other.rating
            && suppliers == other.suppliers
            && feeds == other.feeds
            && contentCollections == other.contentCollections
            && contentCounts == other.contentCounts
            && name == other.name
            && placeId == other.placeId
            && reviews == other.reviews
            && reviewCount == other.reviewCount
            && tags == other.tags
            && primaryPhone == other.primaryPhone
            && primaryFax == other.primaryFax
            && primaryEmail == other.primaryEmail
            && primaryUrl == other.primaryUrl
            && extendedAttributes == other.extendedAttributes
            );
}

QT_END_NAMESPACE

