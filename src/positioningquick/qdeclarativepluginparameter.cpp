/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativepluginparameter_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype PluginParameter
    \inqmlmodule QtPositioning
    \ingroup qml-QtPositioning5-common
    \since QtPositioning 5.14

    \brief The PluginParameter type describes a parameter for a
    \omit
    plugin, either geoservice \l Plugin, or
    \endomit
    position plugin.

    The PluginParameter object is used to provide a parameter of some kind
    to a plugin. Typically, these parameters contain details like an application
    token for access to a service, or a proxy server to use for network access,
    or the serial port to which a serial GPS receiver is connected.

    To set such a parameter, declare a PluginParameter inside an element that
    accepts plugin parameters as configuration objects, such as a
    \omit
    \l Plugin object, or a
    \endomit
    \l PositionSource object, and set values for its \l{name} and \l{value}
    properties. A list of valid parameter names for each plugin is available
    from the
    \omit
    \l {Qt Location#Plugin References and Parameters}{plugin reference pages}
    for geoservice plugins, and
    \endomit
    \l {Qt Positioning plugins#Default plugins}{default plugins page} for
    position plugins.

    \section2 Example Usage

    The following example shows the instantiation of the
    \l {Qt Positioning NMEA plugin}{NMEA} plugin with the \e nmea.source
    parameter that specifies the data source.

    \code
    PositionSource {
        name: "nmea"
        PluginParameter { name: "nmea.source"; value: "serial:/dev/ttyACM0" }
    }
    \endcode
*/

/*!
    \qmlproperty string PluginParameter::name

    This property holds the name of the plugin parameter as a single formatted string.
    This property is a write-once property.
*/

/*!
    \qmlproperty QVariant PluginParameter::value

    This property holds the value of the plugin parameter which support different types of values (variant).
    This property is a write-once property.
*/

QDeclarativePluginParameter::QDeclarativePluginParameter(QObject *parent)
    : QObject(parent) {}

QDeclarativePluginParameter::~QDeclarativePluginParameter() {}

void QDeclarativePluginParameter::setName(const QString &name)
{
    if (!name_.isEmpty() || name.isEmpty())
        return;

    name_ = name;

    emit nameChanged(name_);
    if (value_.isValid())
        emit initialized();
}

QString QDeclarativePluginParameter::name() const
{
    return name_;
}

void QDeclarativePluginParameter::setValue(const QVariant &value)
{
    if (value_.isValid() || !value.isValid() || value.isNull())
        return;

    value_ = value;

    emit valueChanged(value_);
    if (!name_.isEmpty())
        emit initialized();
}

QVariant QDeclarativePluginParameter::value() const
{
    return value_;
}

bool QDeclarativePluginParameter::isInitialized() const
{
    return !name_.isEmpty() && value_.isValid();
}

QT_END_NAMESPACE
