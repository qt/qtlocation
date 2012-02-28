/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
    \ingroup qml-QtLocation5-places-data
    \since QtLocation 5.0

    \brief The Icon element represents an icon image source which can have multiple sizes.

    The Icon element can be used in conjunction with an \l Image element to display an icon.
    The \l url() function is used to construct an icon URL of a requested size,
    the icon which most closely matches the requested size is returned.

    The Icon element also has a parameters map which is a set of key value pairs.  The precise
    keys to use depend on the \l {Information about plugins} {plugin backend} being used.
    The parameters map is used by the \l Plugin to determine which URL to return.

    In the case where an icon can only possibly have one image URL, the
    parameter key of \c "singleUrl" can be used with a QUrl value.  Any Icon with this
    parameter will always return the specified URL regardless of the requested icon
    size and not defer to any Plugin.

    The following code shows how to display a 64x64 pixel icon:

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml Icon

    Alternatively, a default sized icon can be specified like so:
    \snippet snippets/declarative/places.qml Icon default
*/

QDeclarativePlaceIcon::QDeclarativePlaceIcon(QObject *parent)
    : QObject(parent), m_plugin(0), m_parameters(new QDeclarativePropertyMap(this))
{
}

QDeclarativePlaceIcon::QDeclarativePlaceIcon(const QPlaceIcon &icon, QDeclarativeGeoServiceProvider *plugin, QObject *parent)
    : QObject(parent), m_parameters(new QDeclarativePropertyMap(this))
{
    if (icon.isEmpty())
        m_plugin = 0;
    else
        m_plugin = plugin;

    initParameters(icon.parameters());
}

QDeclarativePlaceIcon::~QDeclarativePlaceIcon()
{
}

QPlaceIcon QDeclarativePlaceIcon::icon() const
{
    QPlaceIcon result;

    if (m_plugin)
        result.setManager(manager());
    else
        result.setManager(0);

    QVariantMap params;
    foreach (const QString &key, m_parameters->keys()) {
        QVariant value = m_parameters->value(key);
        if (value.type() == QVariant::Url && !value.toUrl().isEmpty())
            params.insert(key, value);
        else if (value.type() == QVariant::String && !value.toString().isEmpty())
            params.insert(key, QUrl::fromUserInput(value.toString()));
    }

    result.setParameters(params);

    return result;
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
    initParameters(src.parameters());
}

/*!
    \qmlmethod url Icon::url(size size)

    Returns a URL for the icon image that most closely matches the given \a size.

    If no plugin has been assigned to the icon, and the parameters do not contain the 'singleUrl' key, a default constructed URL
    is returned.

*/
QUrl QDeclarativePlaceIcon::url(const QSize &size) const
{
    return icon().url(size);
}

/*!
    \qmlproperty Object Icon::parameters

    This property holds the parameters of the icon and is a map.  These parameters
    are used by the plugin to return the appropriate URL when url() is called and to
    specify locations to save to when saving icons.

    Consult the \l {Information about plugins} {plugin documentation}
    for what parameters are supported and how they should be used.

    Note, due to limitations of the QDeclarativePropertyMap, it is not possible
    to declaratively specify the parameters in QML, assignment of parameters keys
    and values can only be accomplished by javascript.

*/
QDeclarativePropertyMap *QDeclarativePlaceIcon::parameters() const
{
    return m_parameters;
}

/*!
    \qmlproperty Plugin Icon::plugin

    The property holds the plugin that is responsible for managing this icon.
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

    if (m_plugin->isAttached()) {
        pluginReady();
    } else {
        connect(m_plugin, SIGNAL(attached()),
                this, SLOT(pluginReady()));
    }
}

void QDeclarativePlaceIcon::pluginReady()
{
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

void QDeclarativePlaceIcon::initParameters(const QVariantMap &parameterMap)
{
    //clear out old parameters
    foreach (const QString &key, m_parameters->keys())
        m_parameters->clear(key);

    foreach (const QString &key, parameterMap.keys()) {
        QVariant value = parameterMap.value(key);
        if (value.type() == QVariant::Url)
            m_parameters->insert(key, value);
        else if (value.type() == QVariant::String)
            m_parameters->insert(key, QUrl::fromUserInput(value.toString()));
    }
}
