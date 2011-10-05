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

#include "qdeclarativeplaceicon_p.h"

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceManager>
#include <QtDeclarative/QDeclarativeInfo>


QT_USE_NAMESPACE

/*!
    \qmlclass PlaceIcon

    \brief The PlaceIcon element holds icon related information, most prominently the icon's urls.
    \inherits QObject

    \ingroup qml-places
*/
QDeclarativePlaceIcon::QDeclarativePlaceIcon(QObject *parent)
    : QObject(parent)
{
}

QDeclarativePlaceIcon::QDeclarativePlaceIcon(const QPlaceIcon &icon, QDeclarativeGeoServiceProvider *plugin, QObject *parent)
    :QObject(parent)
{
    m_baseUrl = icon.baseUrl();
    m_fullUrl = icon.fullUrl();
    m_plugin = plugin;
}

QDeclarativePlaceIcon::~QDeclarativePlaceIcon()
{
}

QPlaceIcon QDeclarativePlaceIcon::icon() const
{
    QPlaceIcon icon;
    if (m_plugin)
        icon.setManager(manager());
    icon.setBaseUrl(m_baseUrl);
    icon.setFullUrl(m_fullUrl);
    return icon;
}

void QDeclarativePlaceIcon::setIcon(const QPlaceIcon &src)
{
    if (m_baseUrl != src.baseUrl()) {
        m_baseUrl = src.baseUrl();
        emit baseUrlChanged();
    }

    if (m_fullUrl != src.fullUrl()) {
        m_fullUrl = src.fullUrl();
        emit fullUrlChanged();
    }
}

/*!
    \qmlmethod string PlaceIcon::url(size, PlaceIcon.IconFlags flags)

    Returns a url for the icon best suited to the given size and flags.
    Note that a manager may not explicitly support a given set of sizes and
    flags, in thise case a 'closest match' url is returned.
*/
QUrl QDeclarativePlaceIcon::url(const QSize &size, QDeclarativePlaceIcon::IconFlags flags) const
{
    QPlaceManager *placeManager = manager();
    if (placeManager) {
        QPlaceIcon icon;
        icon.setBaseUrl(m_baseUrl);
        icon.setFullUrl(m_fullUrl);
        icon.setManager(placeManager);
        return placeManager->constructIconUrl(icon, size, QPlaceIcon::IconFlags(int(flags)));
    }

    return QUrl();
}

/*!
    \qmlproperty url PlaceIcon::fullUrl

    This property holds the full url of the icon of the place.  Setting
    the full url implies that the base url is cleared.  If the full url
    is set, the url() method will always return the full url regardless
    of the specified size and flags.
*/
QUrl QDeclarativePlaceIcon::fullUrl() const
{
    return m_fullUrl;
}

void QDeclarativePlaceIcon::setFullUrl(const QUrl &url)
{
    if (m_fullUrl != url) {
        m_fullUrl = url;
        emit fullUrlChanged();
        setBaseUrl(QUrl());
    }
}

/*!
    \qmlproperty url PlaceIcon::baseUrl

    This property holds a base url which the complete url returned by
    url() will be based off.  Setting the base url implies that the full
    url is cleared.

    An example base url is http://example/icon.  Depending on the
    parameters specified, when url() is invoked the returned complete url
    may be something like http://example/icon_32x32_selected.jpg.
*/
QUrl QDeclarativePlaceIcon::baseUrl() const
{
    return m_baseUrl;
}

void QDeclarativePlaceIcon::setBaseUrl(const QUrl &url)
{
    if (m_baseUrl != url) {
        m_baseUrl = url;
        emit baseUrlChanged();
        setFullUrl(QUrl());
    }
}

/*!
    \qmlproperty Plugin PlaceIcon::plugin

    The property holds plugin the that is reponsible for managing this icon.
*/
QDeclarativeGeoServiceProvider* QDeclarativePlaceIcon::plugin() const
{
    return m_plugin;
}

void QDeclarativePlaceIcon::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    m_plugin = plugin;
    emit pluginChanged();

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support places.");
        return;
    }
}

/*
    Helper function to return the manager from the plugin
*/
QPlaceManager *QDeclarativePlaceIcon::manager() const
{
    if (!m_plugin) {
           qmlInfo(this) << tr("Plugin not assigned to place icon");
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


