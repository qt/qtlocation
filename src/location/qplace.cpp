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

#include <QStringList>

QT_BEGIN_NAMESPACE

/*!
    \class QPlace
    \inmodule QtLocation
    \ingroup QtLocation-places
    \since QtLocation 5.0

    \brief The QPlace class represents basic information about a place.
*/

/*!
    Constructs an empty place object.
*/
QPlace::QPlace()
        : d_ptr(new QPlacePrivate())
{
}

/*!
    Constructs a copy of \a other.
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
*/
QPlace &QPlace::operator= (const QPlace & other)
{
    d_ptr = other.d_ptr;
    return *this;
}

inline QPlacePrivate* QPlace::d_func()
{
    return static_cast<QPlacePrivate *>(d_ptr.data());
}

inline const QPlacePrivate* QPlace::d_func() const
{
    return static_cast<const QPlacePrivate *>(d_ptr.constData());
}

/*!
    Returns true if \a other is equal to this place,
    otherwise returns false.
*/
bool QPlace::operator== (const QPlace &other) const
{
    Q_D(const QPlace);
    return *d == *other.d_func();
}

/*!
    Returns true if \a other is not equal to this place,
    otherwise returns false.
*/
bool QPlace::operator!= (const QPlace &other) const
{
    Q_D(const QPlace);
    return !(*d == *other.d_func());
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
    Returns the supplier of this place data.
*/
QPlaceSupplier QPlace::supplier() const
{
    Q_D(const QPlace);
    return d->supplier;
}

/*!
    Sets the supplier of this place data to \a supplier.
*/
void QPlace::setSupplier(const QPlaceSupplier &supplier)
{
    Q_D(QPlace);
    d->supplier = supplier;
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
void QPlace::insertContent(QPlaceContent::Type type, const QPlaceContent::Collection &content)
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
int QPlace::totalContentCount(QPlaceContent::Type type) const
{
    Q_D(const QPlace);
    return d->contentCounts.value(type, 0);
}

/*!
    Sets the \a totalCount of content objects of the given \a type.
*/
void QPlace::setTotalContentCount(QPlaceContent::Type type, int totalCount)
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
    Returns a rich text attribution string of the place.  Note, some providers may have a
    requirement where the attribution must be shown whenever a place is displayed to an end user.
*/
QString QPlace::attribution() const
{
    Q_D(const QPlace);
    return d->attribution;
}

/*!
    Sets the attribution string of the place.
*/
void QPlace::setAttribution(const QString &attribution)
{
    Q_D(QPlace);
    d->attribution = attribution;
}

/*!
    Returns the icon of the place.
*/
QPlaceIcon QPlace::icon() const
{
    Q_D(const QPlace);
    return d->icon;
}

/*!
    Sets the icon of the place.
*/
void QPlace::setIcon(const QPlaceIcon &icon)
{
    Q_D(QPlace);
    d->icon = icon;
}

/*!
    Returns the primary phone number for this place.
*/
QString QPlace::primaryPhone() const
{
    Q_D(const QPlace);
    QList<QPlaceContactDetail> phoneNumbers = d->contacts.value(QPlaceContactDetail::Phone);
    if (!phoneNumbers.isEmpty())
        return phoneNumbers.at(0).value();
    else
        return QString();
}

/*!
    Returns the primary fax number for this place.
*/
QString QPlace::primaryFax() const
{
    Q_D(const QPlace);
    QList<QPlaceContactDetail> faxNumbers = d->contacts.value(QPlaceContactDetail::Fax);
    if (!faxNumbers.isEmpty())
        return faxNumbers.at(0).value();
    else
        return QString();
}

/*!
    Returns the primary email address for this place.
*/
QString QPlace::primaryEmail() const
{
    Q_D(const QPlace);
    QList<QPlaceContactDetail> emailAddresses = d->contacts.value(QPlaceContactDetail::Email);
    if (!emailAddresses.isEmpty())
        return emailAddresses.at(0).value();
    else
        return QString();
}

/*!
    Returns the primary URL of this place.
*/
QUrl QPlace::primaryWebsite() const
{
    Q_D(const QPlace);
    QList<QPlaceContactDetail> websites = d->contacts.value(QPlaceContactDetail::Website);
    if (!websites.isEmpty())
        return QUrl(websites.at(0).value().toAscii());
    else
        return QString();
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
    Returns the types of extended attributes that this place has.
*/
QStringList QPlace::extendedAttributeTypes() const
{
    Q_D(const QPlace);
    return d->extendedAttributes.keys();
}

/*!
    Returns the exteded attribute corresponding to the specified \a attributeType.
    If the place does not have that particular attribute type, a default constructed
    QPlaceExtendedAttribute is returned.
*/
QPlaceAttribute QPlace::extendedAttribute(const QString &attributeType) const
{
    Q_D(const QPlace);
    return d->extendedAttributes.value(attributeType);
}

/*!
    Assigns an \a attribute of the given \a attributeType to a place.  If the given \a attributeType
    already exists in the place, then it is overwritten.

    If \a attribute is a default constructed QPlaceAttribute, then the \a attributeType
    is removed from the place i.e it will not be listed by QPlace::extendedAttributeTypes().
*/
void QPlace::setExtendedAttribute(const QString &attributeType,
                                    const QPlaceAttribute &attribute)
{
    Q_D(QPlace);
    if (attribute == QPlaceAttribute())
        d->extendedAttributes.remove(attributeType);
    else
        d->extendedAttributes.insert(attributeType, attribute);
}

/*!
    Remove the attribute of \a attributeType from the place.

    The attribute will no longer be listed by QPlace::extendedAttributeTypes()
*/
void QPlace::removeExtendedAttribute(const QString &attributeType)
{
    Q_D(QPlace);
    d->extendedAttributes.remove(attributeType);
}

/*!
    Returns the type of contact details this place has.
*/
QStringList QPlace::contactTypes() const
{
    Q_D(const QPlace);
    return d->contacts.keys();
}

/*!
    Returns a list of contact details of the specified \a contactType
*/
QList<QPlaceContactDetail> QPlace::contactDetails(const QString &contactType)
{
    Q_D(const QPlace);
    return d->contacts.value(contactType);
}

/*!
    Sets the contact \a details of a specified \a contactType.
*/
void QPlace::setContactDetails(const QString &contactType, QList<QPlaceContactDetail> details)
{
    Q_D(QPlace);
    if (details.isEmpty())
        d->contacts.remove(contactType);
    else
        d->contacts.insert(contactType, details);
}

/*!
    Appends a contact \a detail of a specified \a contactType.
*/
void QPlace::appendContactDetail(const QString &contactType, const QPlaceContactDetail &detail)
{
    Q_D(QPlace);
    QList<QPlaceContactDetail> details = d->contacts.value(contactType);
    details.append(detail);
    d->contacts.insert(contactType, details);
}

/*!
    Sets the visibility of the place to \a visibility.
*/
void QPlace::setVisibility(QtLocation::Visibility visibility)
{
    Q_D(QPlace);
    d->visibility = visibility;
}

/*!
    Returns the visibility of the place.
*/
QtLocation::Visibility QPlace::visibility() const
{
    Q_D(const QPlace);
    return d->visibility;
}

/*******************************************************************************
*******************************************************************************/

QPlacePrivate::QPlacePrivate()
:   QSharedData(), visibility(QtLocation::UnspecifiedVisibility), detailsFetched(false)
{
}

QPlacePrivate::QPlacePrivate(const QPlacePrivate &other)
        : QSharedData(other),
        categories(other.categories),
        location(other.location),
        rating(other.rating),
        supplier(other.supplier),
        name(other.name),
        placeId(other.placeId),
        attribution(other.attribution),
        contentCollections(other.contentCollections),
        contentCounts(other.contentCounts),
        contacts(other.contacts),
        extendedAttributes(other.extendedAttributes),
        visibility(other.visibility),
        detailsFetched(other.detailsFetched),
        icon(other.icon)
{
}

QPlacePrivate::~QPlacePrivate() {}

QPlacePrivate& QPlacePrivate::operator= (const QPlacePrivate & other)
{
    categories = other.categories;
    location = other.location;
    rating = other.rating;
    supplier = other.supplier;
    name = other.name;
    placeId = other.placeId;
    attribution = other.attribution;
    contentCollections = other.contentCollections;
    contentCounts = other.contentCounts;
    contacts = other.contacts;
    extendedAttributes = other.extendedAttributes;
    visibility = other.visibility;
    detailsFetched = other.detailsFetched;
    icon = other.icon;

    return *this;
}

bool QPlacePrivate::operator== (const QPlacePrivate &other) const
{
#ifdef QPLACE_DEBUG
    qDebug() << "categories: " << (categories == other.categories);
    qDebug() << "location:" << (location == other.location);
    qDebug() << "rating" << (rating == other.rating);
    qDebug() << "suppliers" << (suppliers == other.suppliers);
    qDebug() << "contentCollections " << (contentCollections == other.contentCollections);
    qDebug() << "contentCounts " << (contentCounts == other.contentCounts);
    qDebug() << "name " << (name == other.name);
    qDebug() << "placeId" << (placeId == other.placeId);
    qDebug() << "attribution" << (attribution == other.attribution);
    qDebug() << "contacts" << (contacts == other.contacts);
    qDebug() << "extendedAttributes" << (extendedAttributes == other.extendedAttributes);
    qDebug() << "visibility" << (visibility == other.visibility);
    qDebug() << "icon" << (icon == other.icon);
#endif

    return (categories == other.categories
            && location == other.location
            && rating == other.rating
            && supplier == other.supplier
            && contentCollections == other.contentCollections
            && contentCounts == other.contentCounts
            && name == other.name
            && placeId == other.placeId
            && attribution == other.attribution
            && contacts == other.contacts
            && extendedAttributes == other.extendedAttributes
            && visibility == other.visibility
            && icon == other.icon
            );
}

QT_END_NAMESPACE
