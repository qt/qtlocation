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
    \qmlclass Category

    \brief The Category element holds various positional data, such as \l
    latitude and \l longitude and.
    \inherits QObject

    \ingroup qml-places
*/

QDeclarativeCategory::QDeclarativeCategory(QObject* parent)
    : QObject(parent), m_icon(0), m_reply(0)
{
}

QDeclarativeCategory::QDeclarativeCategory(const QPlaceCategory &category,
                                           QDeclarativeGeoServiceProvider *plugin,
                                           QObject *parent)
        : QObject(parent),
          m_category(category),
          m_icon(0), m_plugin(plugin), m_reply(0)
{
    Q_ASSERT(plugin);
    setCategory(category);
}

QDeclarativeCategory::~QDeclarativeCategory() {}

// From QDeclarativeParserStatus
void QDeclarativeCategory::componentComplete()
{
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
    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support places.");
        return;
    }
}

QDeclarativeGeoServiceProvider* QDeclarativeCategory::plugin() const
{
    return m_plugin;
}

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

    This property holds the id of the category
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

    This property holds name of the category
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

    This property holds the icon of the category.
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
    \list
    \o Category.Ready - No Error occurred during the last operation,
                     further operations may be performed on the category.
    \o Category.Saving - The category is currently being saved, no other operations
                      may be perfomed until complete.
    \o Category.Removing - The category is currently being removed, no other
                        operations can be performed until complete.
    \o Category.Error - An error occurred during the last operation,
                     further operations can still be performed on the category.
    \endlist
*/
void QDeclarativeCategory::setStatus(Status status)
{
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
    \qmlmethod void Category::savePlace()

    This method performs a remove operation on the category.
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
    \qmlmethod void Category::removePlace()

    This method performs a remove operation on the category.
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
    Helper function to return the manager, this manager is intended to be used
    to perform the next operation.
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
           qmlInfo(this) << tr("Plugin not assigned to category");
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
