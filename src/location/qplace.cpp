/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
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

#include "qplace.h"
#include "qplace_p.h"

#ifdef QPLACE_DEBUG
#include <QDebug>
#endif

#if !defined(Q_CC_MWERKS)
template<> QT_PREPEND_NAMESPACE(QPlacePrivate) *QSharedDataPointer<QT_PREPEND_NAMESPACE(QPlacePrivate)>::clone()
{
    return d->clone();
}
#endif

QT_BEGIN_NAMESPACE

/*!
    \class QPlace
    \brief The QPlace class represents basic information about a place.

    \inmodule QtLocation
    \since 1.1

    \ingroup location
*/

/*!
    Constructs an empty place object.
*/
QPlace::QPlace()
        : d_ptr(new QPlacePrivate())
{
}

/*!
    \internal
*/
QPlace::QPlace(QPlacePrivate *dd)
        : d_ptr(dd) {}

/*!
    Constructs a copy of \a other.
    \since 1.1
*/
QPlace::QPlace(const QPlace &other)
        : d_ptr(other.d_ptr)
{
}

/*!
    Destroys this place.
*/
QPlace::~QPlace()
{
}

/*!
    Assigns \a other to this place and returns a reference
    to this place.
    \since 1.1
*/
QPlace &QPlace::operator= (const QPlace & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

inline QPlacePrivate* QPlace::d_func()
{
    return reinterpret_cast<QPlacePrivate*>(d_ptr.data());
}

inline const QPlacePrivate* QPlace::d_func() const
{
    return reinterpret_cast<const QPlacePrivate*>(d_ptr.constData());
}

/*!
    Returns true if \a other is equal to this place,
    otherwise returns false.
    \since 1.1
*/
bool QPlace::operator== (const QPlace &other) const
{
    return ( *(d_ptr.constData()) == *(other.d_ptr.constData()));
}

/*!
    Returns true if \a other is not equal to this place,
    otherwise returns false.
    \since 1.1
*/
bool QPlace::operator!= (const QPlace &other) const
{
    return (!this->operator ==(other));
}

/*!
    Returns additional data.
*/
QVariantHash QPlace::additionalData() const
{
    Q_D(const QPlace);
    return d->additionalData;
}

/*!
    Sets additional data.
*/
void QPlace::setAdditionalData(const QVariantHash &data)
{
    Q_D(QPlace);
    d->additionalData = data;
}


/*!
    Returns categories.
*/
QList<QPlaceCategory> QPlace::categories() const
{
    Q_D(const QPlace);
    return d->categories;
}

/*!
    Sets categories.
*/
void QPlace::setCategories(const QList<QPlaceCategory> &categories)
{
    Q_D(QPlace);
    d->categories = categories;
}

/*!
    Returns descriptions.
*/
QList<QPlaceDescription> QPlace::descriptions() const
{
    Q_D(const QPlace);
    return d->descriptions;
}

/*!
    Sets descriptions.
*/
void QPlace::setDescriptions(const QList<QPlaceDescription> &descriptions)
{
    Q_D(QPlace);
    d->descriptions = descriptions;
}

/*!
    Returns location.
*/
QGeoLocation QPlace::location() const
{
    Q_D(const QPlace);
    return d->location;
}

/*!
    Sets location.
*/
void QPlace::setLocation(const QGeoLocation &location)
{
    Q_D(QPlace);
    d->location = location;
}

/*!
    Returns rating.
*/
QPlaceRating QPlace::rating() const
{
    Q_D(const QPlace);
    return d->rating;
}

/*!
    Sets rating.
*/
void QPlace::setRating(const QPlaceRating &rating)
{
    Q_D(QPlace);
    d->rating = rating;
}

/*!
    Returns suppliers.
*/
QList<QPlaceSupplier> QPlace::suppliers() const
{
    Q_D(const QPlace);
    return d->suppliers;
}

/*!
    Sets suppliers.
*/
void QPlace::setSuppliers(const QList<QPlaceSupplier> &data)
{
    Q_D(QPlace);
    d->suppliers = data;
}

/*!
    Returns feeds list.
*/
QStringList QPlace::feeds() const
{
    Q_D(const QPlace);
    return d->feeds;
}

/*!
    Sets feeds list.
*/
void QPlace::setFeeds(const QStringList &feeds)
{
    Q_D(QPlace);
    d->feeds = feeds;
}

/*!
    Returns a collection of content associated with a place.
    This collection is a map with the key being the index of the content object
    and value being the content object itself.

    The \a type specifies which kind of content is to be retrieved.
*/
QPlaceContent::Collection QPlace::content(QPlaceContent::Type type) const
{
    Q_D(const QPlace);
    return d->contentCollections.value(type);
}

/*!
    Sets a collection of \a content for the given \a type.
*/
void QPlace::setContent(QPlaceContent::Type type, const QPlaceContent::Collection &content)
{
    Q_D(QPlace);
    d->contentCollections.insert(type, content);
}

/*!
    Adds a collection of \a content of the given \a type to the place.  Any index in \a content
    that already exists is overwritten.
*/
void QPlace::addContent(QPlaceContent::Type type, const QPlaceContent::Collection &content)
{
    Q_D(QPlace);
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
int QPlace::contentCount(QPlaceContent::Type type) const
{
    Q_D(const QPlace);
    return d->contentCounts.value(type, 0);
}

/*!
    Sets the \a totalCount of content objects of the given \a type.
*/
void QPlace::setContentCount(QPlaceContent::Type type, int totalCount)
{
    Q_D(QPlace);
    d->contentCounts.insert(type, totalCount);
}

/*!
    Returns name.
*/
QString QPlace::name() const
{
    Q_D(const QPlace);
    return d->name;
}

/*!
    Sets name.
*/
void QPlace::setName(const QString &name)
{
    Q_D(QPlace);
    d->name = name;
}

/*!
    Returns placeId.
*/
QString QPlace::placeId() const
{
    Q_D(const QPlace);
    return d->placeId;
}

/*!
    Sets placeId.
*/
void QPlace::setPlaceId(const QString &placeId)
{
    Q_D(QPlace);
    d->placeId = placeId;
}

/*!
    Returns list of tags.
*/
QStringList QPlace::tags() const
{
    Q_D(const QPlace);
    return d->tags;
}

/*!
    Sets tags.
*/
void QPlace::setTags(const QStringList &tags)
{
    Q_D(QPlace);
    d->tags = tags;
}

/*!
    Returns the primary phone number for this place.
*/
QString QPlace::primaryPhone() const
{
    Q_D(const QPlace);
    return d->primaryPhone;
}

/*!
    Sets the primary \a phone number of this place.
*/
void QPlace::setPrimaryPhone(const QString &phone)
{
    Q_D(QPlace);
    d->primaryPhone = phone;
}

/*!
    Returns the primary fax number for this place.
*/
QString QPlace::primaryFax() const
{
    Q_D(const QPlace);
    return d->primaryFax;
}

/*!
    Sets the primary \a fax number for this place
*/
void QPlace::setPrimaryFax(const QString fax)
{
    Q_D(QPlace);
    d->primaryFax = fax;
}

/*!
    Returns the primary email address for this place.
*/
QString QPlace::primaryEmail() const
{
    Q_D(const QPlace);
    return d->primaryEmail;
}

/*!
    Sets the primary \a email address for this place.
*/
void QPlace::setPrimaryEmail(const QString &email)
{
    Q_D(QPlace);
    d->primaryEmail = email;
}

/*!
    Returns the primary URL of this place.
*/
QUrl QPlace::primaryUrl() const
{
    Q_D(const QPlace);
    return d->primaryUrl;
}

/*!
    Sets the primary \a url of this place.
*/
void QPlace::setPrimaryUrl(const QUrl &url)
{
    Q_D(QPlace);
    d->primaryUrl = url;
}

/*!
    Returns whether the details of this place have been fetched or not.
*/
bool QPlace::detailsFetched() const
{
    Q_D(const QPlace);
    return d->detailsFetched;
}

/*!
    Sets whether the details of this place have been \a fetched or not.
*/
void QPlace::setDetailsFetched(bool fetched)
{
    Q_D(QPlace);
    d->detailsFetched = fetched;
}

/*!
    Returns the extended attributes of the place
*/
QPlace::ExtendedAttributes QPlace::extendedAttributes() const
{
    Q_D(const QPlace);
    return d->extendedAttributes;
}

/*!
    Sets the extended attributes of the place.
*/
void QPlace::setExtendedAttributes(const ExtendedAttributes &attributes)
{
    Q_D(QPlace);
    d->extendedAttributes = attributes;
}

/*!
    Adds a single attribute to the place.  If the attribute already
    exists then the old value is overwritten.
*/
void QPlace::insertExtendedAttribute(const QString &key, const QPlaceAttribute &value)
{
    Q_D(QPlace);
    d->extendedAttributes.insert(key, value);
}

/*******************************************************************************
*******************************************************************************/

QPlacePrivate::QPlacePrivate()
        : QSharedData(),
        detailsFetched(false)
{
}

QPlacePrivate::QPlacePrivate(const QPlacePrivate &other)
        : QSharedData(other),
        additionalData(other.additionalData),
        categories(other.categories),
        descriptions(other.descriptions),
        location(other.location),
        rating(other.rating),
        suppliers(other.suppliers),
        feeds(other.feeds),
        name(other.name),
        placeId(other.placeId),
        tags(other.tags),
        contentCollections(other.contentCollections),
        contentCounts(other.contentCounts),
        primaryPhone(other.primaryPhone),
        primaryFax(other.primaryFax),
        primaryEmail(other.primaryEmail),
        primaryUrl(other.primaryUrl),
        extendedAttributes(other.extendedAttributes),
        detailsFetched(other.detailsFetched)
{
}

QPlacePrivate::~QPlacePrivate() {}

QPlacePrivate& QPlacePrivate::operator= (const QPlacePrivate & other)
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
    tags = other.tags;
    primaryPhone = other.primaryPhone;
    primaryFax = other.primaryFax;
    primaryEmail = other.primaryEmail;
    primaryUrl = other.primaryUrl;

    return *this;
}

bool QPlacePrivate::operator== (const QPlacePrivate &other) const
{
#ifdef QPLACE_DEBUG
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
            && tags == other.tags
            && primaryPhone == other.primaryPhone
            && primaryFax == other.primaryFax
            && primaryEmail == other.primaryEmail
            && primaryUrl == other.primaryUrl
            && extendedAttributes == other.extendedAttributes
            );
}

QT_END_NAMESPACE

