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
    \qmlclass Icon QDeclarativePlaceIcon
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The Icon element represents an icon image source which can have multiple states and
           sizes.

    The Icon element can be used in conjunction with an \l Image element to display an icon in
    multiple states and sizes.  The \l url() function is used to construct an icon url from the
    \l baseUrl property and the desired icon size and state.  For example the following code will
    display a 64x64 pixel icon for a list in the selected state.

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml Icon
*/

QDeclarativePlaceIcon::QDeclarativePlaceIcon(QObject *parent)
    : QObject(parent), m_plugin(0)
{
}

QDeclarativePlaceIcon::QDeclarativePlaceIcon(const QPlaceIcon &icon, QDeclarativeGeoServiceProvider *plugin, QObject *parent)
    :QObject(parent)
{
    m_baseUrl = icon.baseUrl();
    m_fullUrl = icon.fullUrl();
    if (icon.isEmpty())
        m_plugin = 0;
    else
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
    if (!m_baseUrl.isEmpty())
        icon.setBaseUrl(m_baseUrl);
    else if (!m_fullUrl.isEmpty())
        icon.setFullUrl(m_fullUrl);

    return icon;
}

/*!
    \qmlproperty QPlaceIcon Icon::icon

    This property is used to provide an interface between C++ and QML code.  First a pointer to a
    Icon object must be obtained from C++, then use the \l {QObject::property()}{property()} and
    \l {QObject::setProperty()}{setProperty()} functions to get and set the \c icon property.

    The following gets the QPlaceIcon representing this object from C++:

    \snippet snippets/cpp/cppqml.cpp Icon get

    The following sets the properties of this object based on a QPlaceIcon object from C++:

    \snippet snippets/cpp/cppqml.cpp Icon set
*/
void QDeclarativePlaceIcon::setIcon(const QPlaceIcon &src)
{
    bool baseChanged = m_baseUrl != src.baseUrl();
    bool fullChanged = m_fullUrl != src.fullUrl();

    if (baseChanged)
        m_baseUrl = src.baseUrl();
    if (fullChanged)
        m_fullUrl = src.fullUrl();

    if (baseChanged)
        emit baseUrlChanged();
    if (fullChanged)
        emit fullUrlChanged();
}

/*!
    \qmlmethod url Icon::url(size size, IconFlags flags)

    Returns a url for the icon best suited to the given \a size and \a flags.

    The \a flags parameter is a bitwise or combination of

    \table
        \row
            \o Icon.Normal
            \o An icon with no state modifications.  This flag indicates that the user is not
               interacting with the icon, but the functionality represented by the icon is
               available.
        \row
            \o Icon.Disabled
            \o An icon with a disabled appearance.  This flag indicates that the functionality
               represented by the icon is not available.
        \row
            \o Icon.Active
            \o An icon with an active appearance.  This flag indicates that the functionality
               represented by the icon is available and the user is interacting with the icon,
               for example, touching it.
        \row
            \o Icon.Selected
            \o An icon with a selected appearance.  This flag indicates that the item represented
               by the icon is selected.
        \row
            \o Icon.Map
            \o An icon intended for display on a \l {QtLocation5::Map}{Map}.
        \row
            \o Icon.List
            \o An icon intended for display in a list.
    \endtable

    If the \l fullUrl property is set, this method will return \l fullUrl, otherwise it will
    construct an url from the \l baseUrl and the given \a size and \a flags.  If an explicit icon
    for the given set of flags does not exist an url for the closest matched icon will be returned.
*/
QUrl QDeclarativePlaceIcon::url(const QSize &size, QDeclarativePlaceIcon::IconFlags flags) const
{
    return icon().url(size, QPlaceIcon::IconFlags(int(flags)));
}

/*!
    \qmlproperty url Icon::fullUrl

    This property holds the full url of the icon of the place.  Setting this property implies that
    the \l baseUrl property is cleared. If this property is set \l url() will always return the
    full url regardless of the of the parameters passed to it.
*/
QUrl QDeclarativePlaceIcon::fullUrl() const
{
    return m_fullUrl;
}

void QDeclarativePlaceIcon::setFullUrl(const QUrl &url)
{
    if (m_fullUrl == url)
        return;

    m_fullUrl = url;

    if (!m_baseUrl.isEmpty()) {
        m_baseUrl.clear();
        emit baseUrlChanged();
    }

    emit fullUrlChanged();
}

/*!
    \qmlproperty url Icon::baseUrl

    This property holds the base url which is used to construct a complete icon url by the
    \l url() method.  Settings this property implies that the \l fullUrl property is cleared.

    An example base url might be \c {http://example.com/icon}.  Depending on the parameters passed
    to \l url() the returned complete url may be something like
    \c {http://example.com/icon_32x32_selected.png}.  The format of the url returned by \l url() is
    dependent on the \l plugin.
*/
QUrl QDeclarativePlaceIcon::baseUrl() const
{
    return m_baseUrl;
}

void QDeclarativePlaceIcon::setBaseUrl(const QUrl &url)
{
    if (m_baseUrl == url)
        return;

    m_baseUrl = url;

    if (!m_fullUrl.isEmpty()) {
        m_fullUrl.clear();
        emit fullUrlChanged();
    }

    emit baseUrlChanged();
}

/*!
    \qmlproperty Plugin Icon::plugin

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

    if (!m_plugin)
        return;

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


