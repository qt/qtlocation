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
#include "qdeclarativegeoserviceprovider_p.h"
#include "qdeclarativeplaceattribute_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceDetailsReply>
#include <QtLocation/QPlaceReply>
#include <QtLocation/QPlaceIdReply>

QT_USE_NAMESPACE

/*!
    \qmlclass Place

    \brief The Place element holds place data.
    \inherits QObject

    Place cointains many properties holding data of the place like location,
    id, name etc.

    \ingroup qml-places
*/

QDeclarativePlace::QDeclarativePlace(QObject* parent)
:   QObject(parent), m_location(0), m_rating(0), m_supplier(0),
    m_reviewModel(0), m_imageModel(0), m_editorialModel(0),
    m_extendedAttributes(new QDeclarativePropertyMap(this)),
    m_reply(0), m_plugin(0),m_complete(false), m_status(QDeclarativePlace::Ready)
{
}

QDeclarativePlace::QDeclarativePlace(const QPlace &src, QObject *parent)
:   QObject(parent), m_location(0), m_rating(0), m_supplier(0),
    m_reviewModel(0), m_imageModel(0), m_editorialModel(0),
    m_extendedAttributes(new QDeclarativePropertyMap(this)),
    m_src(src), m_reply(0), m_plugin(0), m_complete(false),
    m_status(QDeclarativePlace::Ready)
{
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

QDeclarativeReviewModel *QDeclarativePlace::reviewModel()
{
    if (!m_reviewModel) {
        m_reviewModel = new QDeclarativeReviewModel(this);
        m_reviewModel->setPlace(this);
    }

    return m_reviewModel;
}

QDeclarativePlaceImageModel *QDeclarativePlace::imageModel()
{
    if (!m_imageModel) {
        m_imageModel = new QDeclarativePlaceImageModel(this);
        m_imageModel->setPlace(this);
    }

    return m_imageModel;
}

QDeclarativePlaceEditorialModel *QDeclarativePlace::editorialModel()
{
    if (!m_editorialModel) {
        m_editorialModel = new QDeclarativePlaceEditorialModel(this);
        m_editorialModel->setPlace(this);
    }

    return m_editorialModel;
}

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

    if (m_rating && m_rating->parent() == this) {
        m_rating->setRating(m_src.rating());
    } else if (!m_rating || m_rating->parent() != this) {
        m_rating = new QDeclarativeRating(m_src.rating(), this);
        emit ratingChanged();
    }

    if (m_supplier && m_supplier->parent() == this) {
        m_supplier->setSupplier(m_src.supplier());
    } else if (!m_supplier || m_supplier->parent() != this) {
        m_supplier = new QDeclarativeSupplier(m_src.supplier(), this);
        emit supplierChanged();
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
    if (previous.primaryUrl() != m_src.primaryUrl()) {
        emit primaryUrlChanged();
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
    result.setRating(m_rating ? m_rating->rating() : QPlaceRating());

    // Supplier
    result.setSupplier(m_supplier ? m_supplier->supplier() : QPlaceSupplier());

    // Extended Attributes
    return result;
}

/*!
    \qmlproperty string Place::location

    This property holds location of the place.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
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
    \qmlproperty Rating Place::rating

    This property holds rating of the place.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
*/
void QDeclarativePlace::setRating(QDeclarativeRating *rating)
{
    if (m_rating == rating)
        return;

    if (m_rating && m_rating->parent() == this)
        delete m_rating;

    m_rating = rating;
    emit ratingChanged();
}

QDeclarativeRating *QDeclarativePlace::rating()
{

    return m_rating;
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
    \qmlproperty string Place::name

    This property holds name.
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

    This property holds place id.
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

    This property holds a rich text attribution string
    for the place.  Some providers may require that the
    attribution be shown whenever a place is displayed
    to an end user.
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

    This property holds a boolean indicating
    whether the details of the place have been fetched
    or not.
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
    \list
    \o Place.Ready - No Error occurred during the last operation,
                     further operations may be performed on the place.
    \o Place.Saving - The place is currently being saved, no other operations
                      may be perfomed until complete.
    \o Place.Fetching - The place details are currently being fetched, no
                        other operations may be performed until complete.
    \o Place.Removing - The place is currently being removed, no other
                        operations can be performed until complete.
    \o Place.Error - An error occurred during the last operation,
                     further operations can still be performed on the place.
    \endlist
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

/*!
    \qmlmethod void Place::getDetails()

    This methods starts fetching place details.
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
*/
void QDeclarativePlace::remove()
{
    QPlaceManager *placeManager = manager();
    if (!placeManager)
        return;

    m_reply = placeManager->removePlace(place());
    connect(m_reply, SIGNAL(finished()), this, SLOT(finished()));
    setStatus(QDeclarativePlace::Removing);
}

/*!
    \qmlmethod string Place::errorString()

    Returns a string description of the error of the last operation.
    If the last operation completed successfully then the string is empty.
*/
QString QDeclarativePlace::errorString() const
{
    return m_errorString;
}

/*!
    \qmlproperty string Place::primaryPhone

    This property holds the primary phone number of the place.
*/
void QDeclarativePlace::setPrimaryPhone(const QString &phone)
{
    if (m_src.primaryPhone() != phone) {
        m_src.setPrimaryPhone(phone);
        emit primaryPhoneChanged();
    }
}

QString QDeclarativePlace::primaryPhone() const
{
    return m_src.primaryPhone();
}

/*!
    \qmlproperty string Place::primaryFax

    This property holds the primary fax number of the place.
*/
void QDeclarativePlace::setPrimaryFax(const QString &fax)
{
    if (m_src.primaryFax() != fax) {
        m_src.setPrimaryFax(fax);
        emit primaryFaxChanged();
    }
}

QString QDeclarativePlace::primaryFax() const
{
    return m_src.primaryFax();
}

/*!
    \qmlproperty string Place::primaryEmail

    This property holds the primary email address of the place.
*/
void QDeclarativePlace::setPrimaryEmail(const QString &email)
{
    if (m_src.primaryEmail() != email) {
        m_src.setPrimaryEmail(email);
        emit primaryEmailChanged();
    }
}

QString QDeclarativePlace::primaryEmail() const
{
    return m_src.primaryEmail();
}

/*!
    \qmlproperty string Place::primaryUrl

    This property holds the primary website address of the place.
*/
void QDeclarativePlace::setPrimaryUrl(const QUrl &url)
{
    if (m_src.primaryUrl() != url) {
        m_src.setPrimaryUrl(url);
        emit primaryUrlChanged();
    }
}

QUrl QDeclarativePlace::primaryUrl() const
{
    return m_src.primaryUrl();
}

/*!
    \qmlproperty ExtendedAttributes extendedAttributes

    This property holds the extended attributes of a place.
    Note: this property's changed() signal is currently only emitted
    if the whole element changes, not if only the contents of
    the element changes.
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
    \qmlproperty QDeclarativeListProperty<QDeclarativeCategory> Place::categories

    This property categories list.

    Note: this property's changed() signal is currently emitted only if the
    whole element changes, not if only the contents of the element change.
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
        QDeclarativeCategory* declarativeValue = new QDeclarativeCategory(value, this);
        m_categories.append(declarativeValue);
    }
}

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

    QMapIterator<QString, QPlaceAttribute> attribIter(m_src.extendedAttributes());
    while (attribIter.hasNext()) {
        attribIter.next();
        m_extendedAttributes->insert(attribIter.key(),
            qVariantFromValue(new QDeclarativePlaceAttribute(attribIter.value())));
    }

    emit extendedAttributesChanged();
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
