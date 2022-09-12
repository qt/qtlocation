/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomapparameter_p.h"

#include <QByteArray>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>

QT_BEGIN_NAMESPACE

namespace {
class SignalMapper : public QObject
{
    Q_OBJECT
public:
    explicit SignalMapper(const QMetaProperty &p, QDeclarativeGeoMapParameter *parent)
        : QObject(parent), property(p)
    {}

    inline static QMetaMethod forwarder =
        staticMetaObject.method(staticMetaObject.indexOfSlot("forward()"));

public slots:
    void forward()
    {
        QDeclarativeGeoMapParameter *that = static_cast<QDeclarativeGeoMapParameter *>(parent());
        that->propertyUpdated(that, property);
    }

private:
    const QMetaProperty property;
};
}

/*!
    \qmltype MapParameter
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.9
    \deprecated

    This type is deprecated and has been remamed into \l DynamicParameter.
*/

/*!
    \qmltype DynamicParameter
    \instantiates QDeclarativeGeoMapParameter
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.11

    \brief The DynamicParameter (previously \l MapParameter ) type represents a parameter for a Map element,
    or other elements used in a Map (such as map items, etc.).
    This type provides a mean to specify plugin-dependent optional dynamic parameters that allow a plugin to
    extend the runtime API of the module.


    DynamicParameters by default contain only the \l type property, and
    are highly plugin-dependent.
    For this reason, additional properties have to be defined inside a
    DynamicParameter at declaration time, using the QML syntax "property var foo".

    What properties have to be put inside a particular DynamicParameter type for
    a particular plugin can be found in the documentation of the plugin.
    \note DynamicParameters are \b optional.
    By not specifying any of them, the Map, or other container elements, will have the default behavior.
*/

/*!
    \qmlproperty string QtLocation::DynamicParameter::type

    Set-once property which holds a string defining the type of the DynamicParameter
*/

QDeclarativeGeoMapParameter::QDeclarativeGeoMapParameter(QObject *parent)
    : QGeoMapParameter(parent)
{
}

bool QDeclarativeGeoMapParameter::isComponentComplete() const
{
    return m_complete;
}

void QDeclarativeGeoMapParameter::classBegin()
{
}

void QDeclarativeGeoMapParameter::componentComplete()
{
    for (int i = m_initialPropertyCount; i < metaObject()->propertyCount(); ++i) {
        const QMetaProperty property = metaObject()->property(i);

        if (!property.hasNotifySignal())
            return;

        SignalMapper *mapper = new SignalMapper(property, this);
        QObject::connect(this, property.notifySignal(), mapper, SignalMapper::forwarder);
    }
    m_complete = true;
    emit completed(this);
}

QT_END_NAMESPACE

#include "qdeclarativegeomapparameter.moc"
