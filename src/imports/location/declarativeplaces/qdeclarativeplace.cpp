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

#include "qdeclarativeplace_p.h"
#include "qdeclarativecontactdetail_p.h"
#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativeplaceattribute_p.h"
#include "qdeclarativeplaceicon_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceDetailsReply>
#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlaceIdReply>
#include <QtLocation/QPlaceContactDetail>

QT_USE_NAMESPACE

/*!
    \qmlclass Place QDeclarativePlace
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The Place element represents a place.

    The Place element represents a physical location with additional metadata describing that
    location.  Constrasted with \l Location, \l Address, and \l Coordinate elements which are used
    to describe where a location is.  The basic properties of a Place are its \l name and
    \l location.

    Place objects are typically obtained from a search model and will generally only have their
    basic properties set.  The \l detailsFetched property can be used to test if further property
    values need to be fetched from the \l Plugin.  This can be done by invoking the \l getDetails()
    method.  Progress of the fetching operation can be monitored with the \l status property, which
    will be set to Place.Fetching when the details are being fetched.

    The Place element has many properties holding information about the place.  Details on how to
    contact the place are available from the \l contactDetails property.  Convienence properties
    for obtaining the primary \l {primaryPhone}{phone}, \l {primaryFax}{fax},
    \l {primaryEmail}{email} and \l {primaryWebsite}{website} are also available.

    Each place is is assigned zero or more \l categories.  Categories are typically used when
    searching for a particular type of place, such as a restaurant or hotel.  Some places have a
    \l rating, which gives an indication of the quality of the place.

    Place metadata is provided by a \l supplier who may require that an \l attribution message be
    displayed to the user when the place details are viewed.

    Places have an associated \l icon which can be used to represent a place on a map or to
    decorate a delegate in a view.

    Places may have additional rich content associated with them.  The currently supported rich
    content include editorial descriptions, reviews and images.  These are exposed as a set of
    models for retrieving the content.  Editorial descriptions of the place are available from the
    \l editorialModel property.  Reviews of the place are available from the \l reviewModel
    property.  A gallery of pictures of the place can be accessed using the \l imageModel property.

    Places may have additional attributes which are not covered in the formal API.  The
    \l extendedAttributes property provides access to these.  The type of extended attributes
    available is specific to each \l Plugin.

    A Place is almost always tied to a \l plugin.  The \l plugin property must be set before it is
    possible to call \l save(), \l remove() or \l getDetails().  The \l reviewModel, \l imageModel
    and \l editorialModel are only valid then the \l plugin property is set.

    \section2 Saving a place

    If the \l Plugin supports it the Place element can be used to save a place.  First create a new
    Place and set its properties.

    \snippet snippets/declarative/places.qml Place savePlace def

    Then invoke the \l save() method.

    \snippet snippets/declarative/places.qml Place savePlace

    The \l status property will change to Place.Saving and then to Place.Ready if the save was
    successful or to Place.Error if an error occurs.

    If the \l placeId property is set, the backend will update an existing place otherwise it will
    create a new place.  On success the \l placeId property will be updated with the id of the newly
    saved place.

    \section3 Caveats
    \input place-caveats.qdocinc

    \section3 Saving between plugins
    When saving places between plugins, there are a few things to be aware of.
    Some fields of a place such as the id, categories and icons are plugin specific entities e.g.
    the categories in one manager may not be recognised in another.
    Therefore trying to save a place directly from one plugin to another is not possible.
    The typical approach is to either clear the id, categories and icon
    or alternatively assign them with appropriate values of the new manager.  The plugin
    property naturally be needs to be assigned to the new plugin and the visibility scope
    may need to be adjusted.

    \snippet snippets/declarative/places.qml Place save to different plugin

    \section2 Removing a place

    To remove a place, ensure that a Place object with a valid \l placeId property exists and call
    its \l remove() method.  The \l status property will change to Place.Removing and then to
    Place.Ready if the save was successful or to Place.Error if an error occurs.

    \sa PlaceSearchModel, PlaceRecommendationModel
*/

QDeclarativePlace::QDeclarativePlace(QObject* parent)
:   QObject(parent), m_location(0), m_ratings(0), m_supplier(0), m_icon(0),
    m_reviewModel(0), m_imageModel(0), m_editorialModel(0),
    m_extendedAttributes(new QDeclarativePropertyMap(this)),
    m_contactDetails(0), m_reply(0), m_plugin(0), m_complete(false),
    m_status(QDeclarativePlace::Ready), m_errorString(QString())
{
    setPlace(QPlace());
}

QDeclarativePlace::QDeclarativePlace(const QPlace &src, QDeclarativeGeoServiceProvider *plugin, QObject *parent)
:   QObject(parent), m_location(0), m_ratings(0), m_supplier(0), m_icon(0),
    m_reviewModel(0), m_imageModel(0), m_editorialModel(0),
    m_extendedAttributes(new QDeclarativePropertyMap(this)), m_contactDetails(0),
    m_src(src), m_reply(0), m_plugin(plugin), m_complete(false),
    m_status(QDeclarativePlace::Ready)
{
    Q_ASSERT(plugin);
    setPlace(src);
}

QDeclarativePlace::~QDeclarativePlace()
{
}

// From QDeclarativeParserStatus
void QDeclarativePlace::componentComplete()
{
    m_complete = true;
}

/*!
    \qmlproperty Plugin Place::plugin

    This property holds the \l Plugin that provided this place.
*/
void QDeclarativePlace::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    m_plugin = plugin;
    if (m_complete)
        emit pluginChanged();
    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support places.");
        return;
    }
}

QDeclarativeGeoServiceProvider* QDeclarativePlace::plugin() const
{
    return m_plugin;
}

/*!
    \qmlproperty ReviewModel Place::reviewModel

    This property holds a model which can be used to retrieve reviews about the place.
*/
QDeclarativeReviewModel *QDeclarativePlace::reviewModel()
{
    if (!m_reviewModel) {
        m_reviewModel = new QDeclarativeReviewModel(this);
        m_reviewModel->setPlace(this);
    }

    return m_reviewModel;
}

/*!
    \qmlproperty ImageModel Place::imageModel

    This property holds a model which can be used to retrieve images of the place.
*/
QDeclarativePlaceImageModel *QDeclarativePlace::imageModel()
{
    if (!m_imageModel) {
        m_imageModel = new QDeclarativePlaceImageModel(this);
        m_imageModel->setPlace(this);
    }

    return m_imageModel;
}

/*!
    \qmlproperty EditorialModel Place::editorialModel

    This property holds a model which can be used to retrieve editorial descriptions of the place.
*/
QDeclarativePlaceEditorialModel *QDeclarativePlace::editorialModel()
{
    if (!m_editorialModel) {
        m_editorialModel = new QDeclarativePlaceEditorialModel(this);
        m_editorialModel->setPlace(this);
    }

    return m_editorialModel;
}

/*!
    \qmlproperty QPlace Place::place

    This property is used to provide an interface between C++ and QML code.  First a pointer to a
    Place object must be obtained from C++, then use the \l {QObject::property()}{property()} and
    \l {QObject::setProperty()}{setProperty()} functions to get and set the \c place property.

    The following gets the QPlace representing this object from C++:

    \snippet snippets/cpp/cppqml.cpp Place get

    The following sets the properties of this object based on a QPlace object from C++:

    \snippet snippets/cpp/cppqml.cpp Place set
*/
void QDeclarativePlace::setPlace(const QPlace &src)
{
    QPlace previous = m_src;
    m_src = src;

    if (previous.categories() != m_src.categories()) {
        synchronizeCategories();
        emit categoriesChanged();
    }

    if (m_location && m_location->parent() == this) {
        m_location->setLocation(m_src.location());
    } else if (!m_location || m_location->parent() != this) {
        m_location = new QDeclarativeGeoLocation(m_src.location(), this);
        emit locationChanged();
    }

    if (m_ratings && m_ratings->parent() == this) {
        m_ratings->setRatings(m_src.ratings());
    } else if (!m_ratings || m_ratings->parent() != this) {
        m_ratings = new QDeclarativeRatings(m_src.ratings(), this);
        emit ratingsChanged();
    }

    if (m_supplier && m_supplier->parent() == this) {
        m_supplier->setSupplier(m_src.supplier(), m_plugin);
    } else if (!m_supplier || m_supplier->parent() != this) {
        m_supplier = new QDeclarativeSupplier(m_src.supplier(), m_plugin, this);
        emit supplierChanged();
    }

    if (m_icon && m_icon->parent() == this) {
        m_icon->setPlugin(m_plugin);
        m_icon->setIcon(m_src.icon());
    } else if (!m_icon || m_icon->parent() != this) {
        m_icon = new QDeclarativePlaceIcon(m_src.icon(), m_plugin, this);
        emit iconChanged();
    }

    if (previous.name() != m_src.name()) {
        emit nameChanged();
    }
    if (previous.placeId() != m_src.placeId()) {
        emit placeIdChanged();
    }
    if (previous.attribution() != m_src.attribution()) {
        emit attributionChanged();
    }
    if (previous.detailsFetched() != m_src.detailsFetched()) {
        emit detailsFetchedChanged();
    }
    if (previous.primaryPhone() != m_src.primaryPhone()) {
        emit primaryPhoneChanged();
    }
    if (previous.primaryFax() != m_src.primaryFax()) {
        emit primaryFaxChanged();
    }
    if (previous.primaryEmail() != m_src.primaryEmail()) {
        emit primaryEmailChanged();
    }
    if (previous.primaryWebsite() != m_src.primaryWebsite()) {
        emit primaryWebsiteChanged();
    }

    if (previous.placeId() != m_src.placeId()) {
        if (m_reviewModel)
            m_reviewModel->clear();
        if (m_imageModel)
            m_imageModel->clear();
        if (m_editorialModel)
            m_editorialModel->clear();
    }

    if (m_extendedAttributes && m_extendedAttributes->parent() == this) {
        synchronizeExtendedAttributes();
    } else if (!m_extendedAttributes || m_extendedAttributes->parent() != this) {
        m_extendedAttributes = new QDeclarativePropertyMap(this);
        synchronizeExtendedAttributes();
        emit extendedAttributesChanged();
    }

    if (m_contactDetails && m_contactDetails->parent() == this) {
        synchronizeContacts();
    } else if (!m_contactDetails || m_contactDetails->parent() != this) {
        m_contactDetails = new QDeclarativePropertyMap(this);
        connect(m_contactDetails, SIGNAL(valueChanged(QString,QVariant)), this, SLOT(contactsModified(QString,QVariant)));
        synchronizeContacts();

        emit contactDetailsChanged();
    }
}

QPlace QDeclarativePlace::place()
{
    // The following properties are not stored in m_src but instead stored in QDeclarative* objects

    QPlace result = m_src;

    // Categories
    QList<QPlaceCategory> categories;
    foreach (QDeclarativeCategory *value, m_categories)
        categories.append(value->category());

    result.setCategories(categories);

    // Location
    result.setLocation(m_location ? m_location->location() : QGeoLocation());

    // Rating
    result.setRatings(m_ratings ? m_ratings->ratings() : QPlaceRatings());

    // Supplier
    result.setSupplier(m_supplier ? m_supplier->supplier() : QPlaceSupplier());

    // Icon
    result.setIcon(m_icon ? m_icon->icon() : QPlaceIcon());

    // Extended Attributes
    return result;
}

/*!
    \qmlproperty QtLocation5::Location Place::location

    This property holds the location of the place.
*/
void QDeclarativePlace::setLocation(QDeclarativeGeoLocation *location)
{
    if (m_location == location)
        return;

    if (m_location && m_location->parent() == this)
        delete m_location;

    m_location = location;
    emit locationChanged();
}

QDeclarativeGeoLocation *QDeclarativePlace::location()
{
    return m_location;
}

/*!
    \qmlproperty Ratings Place::ratings

    This property holds ratings of the place.  The ratings provides an indication of the quality of a
    place.
*/
void QDeclarativePlace::setRatings(QDeclarativeRatings *rating)
{
    if (m_ratings == rating)
        return;

    if (m_ratings && m_ratings->parent() == this)
        delete m_ratings;

    m_ratings = rating;
    emit ratingsChanged();
}

QDeclarativeRatings *QDeclarativePlace::ratings()
{

    return m_ratings;
}

/*!
    \qmlproperty Supplier Place::supplier

    This property holds the supplier of the place data.
*/
void QDeclarativePlace::setSupplier(QDeclarativeSupplier *supplier)
{
    if (m_supplier == supplier)
        return;

    if (m_supplier && m_supplier->parent() == this)
        delete m_supplier;

    m_supplier = supplier;
    emit supplierChanged();
}

QDeclarativeSupplier *QDeclarativePlace::supplier() const
{
    return m_supplier;
}

/*!
    \qmlproperty Icon Place::icon

    This property holds a graphical icon which can be used to represent the place.
*/
QDeclarativePlaceIcon *QDeclarativePlace::icon() const
{
    return m_icon;
}

void QDeclarativePlace::setIcon(QDeclarativePlaceIcon *icon)
{
    if (m_icon == icon)
        return;

    if (m_icon && m_icon->parent() == this)
        delete m_icon;

    m_icon = icon;
    emit iconChanged();
}

/*!
    \qmlproperty string Place::name

    This property holds the name of the place.
*/
void QDeclarativePlace::setName(const QString &name)
{
    if (m_src.name() != name) {
        m_src.setName(name);
        emit nameChanged();
    }
}

QString QDeclarativePlace::name() const
{
    return m_src.name();
}

/*!
    \qmlproperty string Place::placeId

    This property holds the unique identifier of the place.  The place id is only meaningful to the
    \l Plugin that generated it and is not transferable between \l {Plugin}{Plugins}.  The place id
    is not guarinteed to be universally unique, but unique within the \l Plugin that generated it.

    If only the place id is known, all other place data can fetched from the \l Plugin.

    \snippet snippets/declarative/places.qml Place placeId
*/
void QDeclarativePlace::setPlaceId(const QString &placeId)
{
    if (m_src.placeId() != placeId) {
        m_src.setPlaceId(placeId);
        emit placeIdChanged();
    }
}

QString QDeclarativePlace::placeId() const
{
    return m_src.placeId();
}

/*!
    \qmlproperty string Place::attribution

    This property holds a rich text attribution string for the place.  Some providers may require
    that the attribution be shown whenever a place is displayed to an end user.  The contents of
    this property should be shown to the user if it is not empty.
*/
void QDeclarativePlace::setAttribution(const QString &attribution)
{
    if (m_src.attribution() != attribution) {
        m_src.setAttribution(attribution);
        emit attributionChanged();
    }
}

QString QDeclarativePlace::attribution() const
{
    return m_src.attribution();
}

/*!
    \qmlproperty bool Place::detailsFetched

    This property indicates whether the details of the place have been fetched.  If this property
    is false then the place details have not yet been fetch, which can be done by invoking the
    \l getDetails() method.

    \sa getDetails()
*/
void QDeclarativePlace::setDetailsFetched(bool fetched)
{
    if (m_src.detailsFetched() != fetched) {
        m_src.setDetailsFetched(fetched);
        emit detailsFetchedChanged();
    }
}

bool QDeclarativePlace::detailsFetched() const
{
    return m_src.detailsFetched();
}

/*!
    \qmlproperty enumeration Place::status

    This property holds the status of the place.  It can be one of:

    \table
        \row
            \o Place.Ready
            \o No Error occurred during the last operation, further operations may be performed on
               the place.
        \row
            \o Place.Saving
            \o The place is currently being saved, no other operations may be perfomed until
               complete.
        \row
            \o Place.Fetching
            \o The place details are currently being fetched, no other operations may be performed
               until complete.
        \row
            \o Place.Removing
            \o The place is currently being removed, no other operations can be performed until
               complete.
        \row
            \o Place.Error
            \o An error occurred during the last operation, further operations can still be
               performed on the place.
    \endtable
*/
void QDeclarativePlace::setStatus(Status status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

QDeclarativePlace::Status QDeclarativePlace::status() const
{
    return m_status;
}

void QDeclarativePlace::finished()
{
    if (!m_reply)
        return;

    if (m_reply->error() == QPlaceReply::NoError) {
        switch (m_reply->type()) {
        case (QPlaceReply::IdReply) : {
            QPlaceIdReply *idReply = qobject_cast<QPlaceIdReply *>(m_reply);

            switch (idReply->operationType()) {
            case QPlaceIdReply::SavePlace:
                setPlaceId(idReply->id());
                break;
            case QPlaceIdReply::RemovePlace:
                setPlaceId(QString());
                break;
            default:
                //Other operation types shouldn't ever be received.
                break;
            }
            break;
        }
        case (QPlaceReply::DetailsReply): {
            QPlaceDetailsReply *detailsReply = qobject_cast<QPlaceDetailsReply *>(m_reply);
            setPlace(detailsReply->place());
            break;
        }
        default:
            //other types of replies shouldn't ever be received.
            break;
        }

        m_errorString.clear();

        m_reply->deleteLater();
        m_reply = 0;

        setStatus(QDeclarativePlace::Ready);
    } else {
        m_errorString = m_reply->errorString();

        m_reply->deleteLater();
        m_reply = 0;

        setStatus(QDeclarativePlace::Error);
    }
}

void QDeclarativePlace::contactsModified(const QString &key, const QVariant &value)
{
    primarySignalsEmission(key);
}

/*!
    \qmlmethod void Place::getDetails()

    This methods starts fetching place details.

    The \l status property will change to Place.Fetching while the fetch is in progress.  On
    success the element properties will be updated, \l status will be set to Place.Ready and
    \l detailsFetched will be set to true.  On error \l status will be set to Place.Error.  The
    \l errorString() method can be used to get the details of the error.
*/
void QDeclarativePlace::getDetails()
{
    QPlaceManager *placeManager = manager();
    if (!placeManager)
        return;

    m_reply = placeManager->getPlaceDetails(placeId());
    connect(m_reply, SIGNAL(finished()), this, SLOT(finished()));
    setStatus(QDeclarativePlace::Fetching);
}

/*!
    \qmlmethod void Place::save()

    This method performs a save operation on the place.

    The \l status property will change to Place.Saving while the save operation is in progress.  On
    success the \l placeId property will be updated and \l status will be set to Place.Ready.  On
    error \l status will be set to Place.Error.  The \l errorString() method can be used to get the
    details of the error.
*/
void QDeclarativePlace::save()
{
    QPlaceManager *placeManager = manager();
    if (!placeManager)
        return;

    m_reply = placeManager->savePlace(place());
    connect(m_reply, SIGNAL(finished()), this, SLOT(finished()));
    setStatus(QDeclarativePlace::Saving);
}

/*!
    \qmlmethod void Place::remove()

    This method performs a remove operation on the place.

    The \l status property will change to Place.Removing while the save operation is in progress.
    On success \l status will be set to Place.Ready.  On error \l status will be set to
    Place.Error.  The \l errorString() method can be used to get the details of the error.
*/
void QDeclarativePlace::remove()
{
    QPlaceManager *placeManager = manager();
    if (!placeManager)
        return;

    m_reply = placeManager->removePlace(place().placeId());
    connect(m_reply, SIGNAL(finished()), this, SLOT(finished()));
    setStatus(QDeclarativePlace::Removing);
}

/*!
    \qmlmethod string Place::errorString()

    Returns a string description of the error of the last operation.  If the last operation
    completed successfully then the string is empty.
*/
QString QDeclarativePlace::errorString() const
{
    return m_errorString;
}

/*!
    \qmlproperty string Place::primaryPhone

    This property holds the primary phone number of the place.  If no "Phone" contact detail is
    defined for this place this property will be an empty string.  It is equivalent to

    \snippet snippets/declarative/places.qml Place primaryPhone
*/
QString QDeclarativePlace::primaryPhone() const
{
    QVariantList phoneNumbers = m_contactDetails->value(QPlaceContactDetail::Phone).toList();
    if (!phoneNumbers.isEmpty()) {
        QDeclarativeContactDetail *primaryPhone = qobject_cast<QDeclarativeContactDetail*>(phoneNumbers.at(0).value<QObject *>());
        return primaryPhone->value();
    } else {
        return QString();
    }
}

/*!
    \qmlproperty string Place::primaryFax

    This property holds the primary fax number of the place.  If no "Fax" contact detail is
    defined for this place this property will be an empty string.  It is equivalent to

    \snippet snippets/declarative/places.qml Place primaryFax
*/
QString QDeclarativePlace::primaryFax() const
{
    QVariantList faxNumbers = m_contactDetails->value(QPlaceContactDetail::Fax).toList();
    if (!faxNumbers.isEmpty()) {
        QDeclarativeContactDetail *primaryFax = qobject_cast<QDeclarativeContactDetail*>(faxNumbers.at(0).value<QObject *>());
        return primaryFax->value();
    } else {
        return QString();
    }
}

/*!
    \qmlproperty string Place::primaryEmail

    This property holds the primary email address of the place.  If no "Email" contact detail is
    defined for this place this property will be an empty string.  It is equivalent to

    \snippet snippets/declarative/places.qml Place primaryEmail
*/
QString QDeclarativePlace::primaryEmail() const
{
    QVariantList emailAddresses = m_contactDetails->value(QPlaceContactDetail::Email).toList();
    if (!emailAddresses.isEmpty()) {
        QDeclarativeContactDetail *primaryEmail = qobject_cast<QDeclarativeContactDetail*>(emailAddresses.at(0).value<QObject *>());
        return primaryEmail->value();
    } else {
        return QString();
    }
}

/*!
    \qmlproperty string Place::primaryWebsite

    This property holds the primary website url of the place.  If no "Website" contact detail is
    defined for this place this property will be an empty string.  It is equivalent to

    \snippet snippets/declarative/places.qml Place primaryWebsite
*/

QUrl QDeclarativePlace::primaryWebsite() const
{
    QVariantList websites = m_contactDetails->value(QPlaceContactDetail::Website).toList();
    if (!websites.isEmpty()) {
        QDeclarativeContactDetail *primaryWebsite = qobject_cast<QDeclarativeContactDetail*>(websites.at(0).value<QObject *>());
        return primaryWebsite->value();
    } else {
        return QUrl();
    }
}

/*!
    \qmlproperty ExtendedAttributes Place::extendedAttributes

    This property holds the extended attributes of a place.  Extended attributes are additional
    information about a place not covered by the place's properties.
*/
void QDeclarativePlace::setExtendedAttributes(QDeclarativePropertyMap *attribs)
{
    if (m_extendedAttributes == attribs)
        return;

    if (m_extendedAttributes && m_extendedAttributes->parent() == this)
        delete m_extendedAttributes;

    m_extendedAttributes = attribs;
    emit extendedAttributesChanged();
}

QDeclarativePropertyMap *QDeclarativePlace::extendedAttributes() const
{
    return m_extendedAttributes;
}

/*!
    \qmlproperty ContactDetails Place::contactDetails

    This property holds the contact information for this place, for example a phone number or
    a website URL.  This property is a map of \l ContactDetail objects.
*/
void QDeclarativePlace::setContactDetails(QDeclarativePropertyMap *contactDetails)
{
    if (m_contactDetails == contactDetails)
        return;

    if (m_contactDetails && m_contactDetails->parent() == this)
        delete m_contactDetails;

    m_contactDetails = contactDetails;
    emit contactDetailsChanged();
}

QDeclarativePropertyMap *QDeclarativePlace::contactDetails() const
{
    return m_contactDetails;
}

/*!
    \qmlproperty list<Category> Place::categories

    This property holds the list of categories this place is a member of.  The categories that can
    be assigned to a place are specific to each \l plugin.
*/
QDeclarativeListProperty<QDeclarativeCategory> QDeclarativePlace::categories()
{
    return QDeclarativeListProperty<QDeclarativeCategory>(this,
                                                          0, // opaque data parameter
                                                          category_append,
                                                          category_count,
                                                          category_at,
                                                          category_clear);
}

void QDeclarativePlace::category_append(QDeclarativeListProperty<QDeclarativeCategory> *prop,
                                                  QDeclarativeCategory *value)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeCategory *altValue = new QDeclarativeCategory(object);
    altValue->setCategory(value->category());
    object->m_categories.append(altValue);
    QList<QPlaceCategory> list = object->m_src.categories();
    list.append(value->category());
    object->m_src.setCategories(list);
    emit object->categoriesChanged();
}

int QDeclarativePlace::category_count(QDeclarativeListProperty<QDeclarativeCategory> *prop)
{
    return static_cast<QDeclarativePlace*>(prop->object)->m_categories.count();
}

QDeclarativeCategory* QDeclarativePlace::category_at(QDeclarativeListProperty<QDeclarativeCategory> *prop,
                                                                          int index)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    QDeclarativeCategory *res = NULL;
    if (object->m_categories.count() > index && index > -1) {
        res = object->m_categories[index];
    }
    return res;
}

void QDeclarativePlace::category_clear(QDeclarativeListProperty<QDeclarativeCategory> *prop)
{
    QDeclarativePlace* object = static_cast<QDeclarativePlace*>(prop->object);
    if (object->m_categories.isEmpty())
        return;

    qDeleteAll(object->m_categories);
    object->m_categories.clear();
    object->m_src.setCategories(QList<QPlaceCategory>());
    emit object->categoriesChanged();
}

void QDeclarativePlace::synchronizeCategories()
{
    qDeleteAll(m_categories);
    m_categories.clear();
    foreach (QPlaceCategory value, m_src.categories()) {
        QDeclarativeCategory* declarativeValue = new QDeclarativeCategory(value, m_plugin, this);
        m_categories.append(declarativeValue);
    }
}

/*!
    \qmlproperty enumeration Place::visibility

    This property holds the visibility of the place.  It can be one of:

    \table
        \row
            \o Place.UnspecifiedVisibility
            \o The visibility of the place is unspecified, the default visibility of the \l plugin
               will be used.
        \row
            \o Place.DeviceVisibility
            \o The place is limited to the current device.  The place will not be transferred off
               of the device.
        \row
            \o Place.PrivateVisibility
            \o The place is private to the current user.  The place may be transferred to an online
               service but is only ever visible to the current user.
        \row
            \o Place.PublicVisibility
            \o The place is public.
    \endtable
*/
QDeclarativePlace::Visibility QDeclarativePlace::visibility() const
{
    return static_cast<QDeclarativePlace::Visibility>(m_src.visibility());
}

void QDeclarativePlace::setVisibility(Visibility visibility)
{
    if (static_cast<QDeclarativePlace::Visibility>(m_src.visibility()) == visibility)
        return;

    m_src.setVisibility(static_cast<QtLocation::Visibility>(visibility));
    emit visibilityChanged();
}

void QDeclarativePlace::synchronizeExtendedAttributes()
{
    QStringList keys = m_extendedAttributes->keys();
    foreach (const QString &key, keys)
        m_extendedAttributes->clear(key);

    QStringList attributeTypes = m_src.extendedAttributeTypes();
    foreach (const QString &attributeType, attributeTypes) {
        m_extendedAttributes->insert(attributeType,
            qVariantFromValue(new QDeclarativePlaceAttribute(m_src.extendedAttribute(attributeType))));
    }

    emit extendedAttributesChanged();
}

void QDeclarativePlace::synchronizeContacts()
{
    //clear out contact data
    foreach (const QString &contactType, m_contactDetails->keys()) {
        QList<QVariant> contacts = m_contactDetails->value(contactType).toList();
        foreach (const QVariant &var, contacts) {
            QObject *obj = var.value<QObject*>();
            if (obj->parent() == this)
                delete obj;
        }
        m_contactDetails->insert(contactType, QVariantList());
    }

    //insert new contact data from source place
    foreach (const QString &contactType, m_src.contactTypes()) {
        QList<QPlaceContactDetail> sourceContacts = m_src.contactDetails(contactType);
        QVariantList declContacts;
        foreach (const QPlaceContactDetail &sourceContact, sourceContacts) {
            QDeclarativeContactDetail *declContact = new QDeclarativeContactDetail(this);
            declContact->setContactDetail(sourceContact);
            declContacts.append(QVariant::fromValue(qobject_cast<QObject *>(declContact)));
        }
        m_contactDetails->insert(contactType, declContacts);
    }
    primarySignalsEmission();
}

/*
    Helper function to emit the signals for the primary___()
    fields.  It is expected that the values of the primary___()
    functions have alread been modified to new values.
*/
void QDeclarativePlace::primarySignalsEmission(const QString &type)
{
    if (type.isEmpty() || type == QPlaceContactDetail::Phone) {
        if (m_prevPrimaryPhone != primaryPhone()) {
            m_prevPrimaryPhone = primaryPhone();
            emit primaryPhoneChanged();
        }
        if (!type.isEmpty())
            return;
    }

    if (type.isEmpty() || type == QPlaceContactDetail::Email) {
        if (m_prevPrimaryEmail != primaryEmail()) {
            m_prevPrimaryEmail = primaryEmail();
            emit primaryEmailChanged();
        }
        if (!type.isEmpty())
            return;
    }

    if (type.isEmpty() || type == QPlaceContactDetail::Website) {
        if (m_prevPrimaryWebsite != primaryWebsite()) {
            m_prevPrimaryWebsite = primaryWebsite();
            emit primaryWebsiteChanged();
        }
        if (!type.isEmpty())
            return;
    }

    if (type.isEmpty() || type == QPlaceContactDetail::Fax) {
        if (m_prevPrimaryFax != primaryFax()) {
            m_prevPrimaryFax = primaryFax();
            emit primaryFaxChanged();
        }
    }
}

/*
    Helper function to return the manager, this manager is intended to be used
    to perform the next operation.  If a an operation is currently underway
    then return a null pointer.
*/
QPlaceManager *QDeclarativePlace::manager()
{
    if (m_status != QDeclarativePlace::Ready && m_status != QDeclarativePlace::Error)
        return 0;

    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = 0;
    }

    if (!m_plugin) {
           qmlInfo(this) << tr("Plugin not assigned to place");
           return 0;
    }

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return 0;

    QPlaceManager *placeManager = serviceProvider->placeManager();

    if (!placeManager) {
        qmlInfo(this) << tr("Places not supported by %1 Plugin.").arg(m_plugin->name());
        return 0;
    }

    return placeManager;
}
