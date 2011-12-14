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

#include "qdeclarativecategory_p.h"
#include "qdeclarativeplaceicon_p.h"
#include "qdeclarativegeoserviceprovider_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceManager>

QT_USE_NAMESPACE

/*!
    \qmlclass Category QDeclarativeCategory
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The Category element represents a category that a \l Place can be associated with.

    Categories are used to search for places based on the categories they are associated with.  The
    list of available categories can be obtained from the \l CategoryModel.  The
    \l PlaceSearchModel has a \l {PlaceSearchModel::categories}{categories} property that is used
    to limit the search results to places with the specified categories.

    If the \l Plugin supports it, categories can be created or removed.  To create a new category
    construct a new Category object and set its properties, then invoke the \l save() method.

    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml Category
    \dots 0
    \snippet snippets/declarative/places.qml Category save

    To remove a category ensure that the \l plugin and categoryId properties are set and call the
    \l remove() method.

    \sa CategoryModel
*/

QDeclarativeCategory::QDeclarativeCategory(QObject* parent)
:   QObject(parent), m_icon(0), m_plugin(0), m_reply(0), m_complete(false), m_status(Ready)
{
}

QDeclarativeCategory::QDeclarativeCategory(const QPlaceCategory &category,
                                           QDeclarativeGeoServiceProvider *plugin,
                                           QObject *parent)
:   QObject(parent), m_category(category), m_icon(0), m_plugin(plugin), m_reply(0),
    m_complete(false), m_status(Ready)
{
    Q_ASSERT(plugin);
    setCategory(category);
}

QDeclarativeCategory::~QDeclarativeCategory() {}

// From QDeclarativeParserStatus
void QDeclarativeCategory::componentComplete()
{
    // delayed instantiation of QObject based properties.
    if (!m_icon) {
        m_icon = new QDeclarativePlaceIcon(this);
        m_icon->setPlugin(m_plugin);
    }

    m_complete = true;
}

/*!
    \qmlproperty Plugin Category::plugin

    This property holds the plugin to which the category belongs.
*/
void QDeclarativeCategory::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    m_plugin = plugin;
    if (m_complete)
        emit pluginChanged();

    if (m_icon && m_icon->parent() == this && !m_icon->plugin())
        m_icon->setPlugin(m_plugin);

    if (!m_plugin)
        return;

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        m_errorString = tr("Places not supported by %1 plugin.").arg(m_plugin->name());
        setStatus(Error);
        return;
    }
}

QDeclarativeGeoServiceProvider* QDeclarativeCategory::plugin() const
{
    return m_plugin;
}

/*!
    \qmlproperty QPlaceCategory Category::category

    This property is used to provide an interface between C++ and QML code.  First a pointer to a
    Category object must be obtained from C++, then use the \l {QObject::property()}{property()} and
    \l {QObject::setProperty()}{setProperty()} functions to get and set the \c category property.

    The following gets the QPlaceCategory representing this object from C++:

    \snippet snippets/cpp/cppqml.cpp Category get

    The following sets the properties of this object based on a QPlaceCategory object from C++:

    \snippet snippets/cpp/cppqml.cpp Category set
*/
void QDeclarativeCategory::setCategory(const QPlaceCategory &category)
{
    QPlaceCategory previous = m_category;
    m_category = category;

    if (category.name() != previous.name())
        emit nameChanged();

    if (category.categoryId() != previous.categoryId())
        emit categoryIdChanged();

    if (m_icon && m_icon->parent() == this) {
        m_icon->setPlugin(m_plugin);
        m_icon->setBaseUrl(m_category.icon().baseUrl());
        m_icon->setFullUrl(m_category.icon().fullUrl());
    } else if (!m_icon || m_icon->parent() != this){
        m_icon = new QDeclarativePlaceIcon(m_category.icon(), m_plugin, this);
        emit iconChanged();
    }
}

QPlaceCategory QDeclarativeCategory::category()
{
    m_category.setIcon(m_icon ? m_icon->icon() : QPlaceIcon());
    return m_category;
}

/*!
    \qmlproperty string Category::categoryId

    This property holds the id of the category.  The categoryId is a string which uniquely
    identifies this category within the categories \l plugin.
*/
void QDeclarativeCategory::setCategoryId(const QString &id)
{
    if (m_category.categoryId() != id) {
        m_category.setCategoryId(id);
        emit categoryIdChanged();
    }
}

QString QDeclarativeCategory::categoryId() const
{
    return m_category.categoryId();
}

/*!
    \qmlproperty string Category::name

    This property holds name of the category.
*/
void QDeclarativeCategory::setName(const QString &name)
{
    if (m_category.name() != name) {
        m_category.setName(name);
        emit nameChanged();
    }
}

QString QDeclarativeCategory::name() const
{
    return m_category.name();
}

/*!
    \qmlproperty enumeration Category::visibility

    This property holds the visibility of the category.  It can be one of:

    \table
        \row
            \o Category.UnspecifiedVisibility
            \o The visibility of the category is unspecified, the default visibility of the plugin
               will be used.
        \row
            \o Category.DeviceVisibility
            \o The category is limited to the current device.  The category will not be transferred
               off of the device.
        \row
            \o Category.PrivateVisibility
            \o The category is private to the current user.  The category may be transferred to an
               online service but is only ever visible to the current user.
        \row
            \o Category.PublicVisibility
            \o The category is public.
    \endtable
*/
QDeclarativeCategory::Visibility QDeclarativeCategory::visibility() const
{
    return static_cast<QDeclarativeCategory::Visibility>(m_category.visibility());
}

void QDeclarativeCategory::setVisibility(Visibility visibility)
{
    if (static_cast<QDeclarativeCategory::Visibility>(m_category.visibility()) == visibility)
        return;

    m_category.setVisibility(static_cast<QtLocation::Visibility>(visibility));
    emit visibilityChanged();
}

/*!
    \qmlproperty PlaceIcon Category::icon

    This property holds the icon associated with the category.
*/
QDeclarativePlaceIcon *QDeclarativeCategory::icon() const
{
    return m_icon;
}

void QDeclarativeCategory::setIcon(QDeclarativePlaceIcon *icon)
{
    if (m_icon == icon)
        return;

    if (m_icon && m_icon->parent() == this)
        delete m_icon;

    m_icon = icon;
    emit iconChanged();
}

/*!
    \qmlmethod string Category::errorString()

    Returns a string description of the error of the last operation.
    If the last operation completed successfully then the string is empty.
*/
QString QDeclarativeCategory::errorString() const
{
    return m_errorString;
}

/*!
    \qmlproperty enumeration Category::status

    This property holds the status of the category.  It can be one of:

    \table
        \row
            \o Category.Ready
            \o No Error occurred during the last operation, further operations may be performed on
               the category.
        \row
            \o Category.Saving
            \o The category is currently being saved, no other operations may be perfomed until the
               current operation completes.
        \row
            \o Category.Removing
            \o The category is currently being removed, no other operations can be performed until
               the current operation completes.
        \row
            \o Category.Error
            \o An error occurred during the last operation, further operations can still be
               performed on the category.
    \endtable
*/
void QDeclarativeCategory::setStatus(Status status)
{
    if (status != Error)
        m_errorString.clear();

    if (m_status != status) {
        m_status = status;
        emit statusChanged();
    }
}

QDeclarativeCategory::Status QDeclarativeCategory::status() const
{
    return m_status;
}

/*!
    \qmlmethod void Category::save()

    This method saves the category to the backend service.
*/
void QDeclarativeCategory::save(const QString &parentId)
{
    QPlaceManager *placeManager = manager();
    if (!placeManager)
        return;

    m_reply = placeManager->saveCategory(category(), parentId);
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    setStatus(QDeclarativeCategory::Saving);
}

/*!
    \qmlmethod void Category::remove()

    This method permanently removes the category from the backend service.
*/
void QDeclarativeCategory::remove()
{
    QPlaceManager *placeManager = manager();
    if (!placeManager)
        return;

    m_reply = placeManager->removeCategory(m_category.categoryId());
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    setStatus(QDeclarativeCategory::Removing);
}

void QDeclarativeCategory::replyFinished()
{
    if (!m_reply)
        return;

    if (m_reply->error() == QPlaceReply::NoError) {
        switch (m_reply->type()) {
        case (QPlaceReply::IdReply) : {
            QPlaceIdReply *idReply = qobject_cast<QPlaceIdReply *>(m_reply);

            switch (idReply->operationType()) {
            case QPlaceIdReply::SaveCategory:
                setCategoryId(idReply->id());
                break;
            case QPlaceIdReply::RemoveCategory:
                setCategoryId(QString());
                break;
            default:
                //Other operation types shouldn't ever be received.
                break;
            }
            break;
        }
        default:
            //other types of replies shouldn't ever be received.
            break;
        }

        m_errorString.clear();

        m_reply->deleteLater();
        m_reply = 0;

        setStatus(QDeclarativeCategory::Ready);
    } else {
        m_errorString = m_reply->errorString();

        m_reply->deleteLater();
        m_reply = 0;

        setStatus(QDeclarativeCategory::Error);
    }
}

/*
    Helper function to return the manager, this manager is intended to be used to perform the next
    operation.  Sets status to Error and an appropriate m_errorString if the manager cannot be
    obtained.
*/
QPlaceManager *QDeclarativeCategory::manager()
{
    if (m_status != QDeclarativeCategory::Ready && m_status != QDeclarativeCategory::Error)
        return 0;

    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = 0;
    }

    if (!m_plugin) {
        m_errorString = tr("Plugin property not set.");
        setStatus(Error);
        return 0;
    }

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        m_errorString = tr("Places not supported by %1 plugin.").arg(m_plugin->name());
        setStatus(Error);
        return 0;
    }

    return placeManager;
}
