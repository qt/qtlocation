/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
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

    int i;
public:
    explicit SignalMapper(int i, QObject *parent = nullptr)
        : QObject(parent), i(i) {}

public Q_SLOTS:
    void map() { emit mapped(i); }

Q_SIGNALS:
    void mapped(int);
};
} // unnamed namespace

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
:   QGeoMapParameter(parent), m_initialPropertyCount(metaObject()->propertyCount()), m_complete(false)
{

}

QDeclarativeGeoMapParameter::~QDeclarativeGeoMapParameter()
{
}

bool QDeclarativeGeoMapParameter::isComponentComplete() const
{
    return m_complete;
}

int QDeclarativeGeoMapParameter::initialPropertyCount() const
{
    return m_initialPropertyCount;
}

void QDeclarativeGeoMapParameter::classBegin()
{
}

void QDeclarativeGeoMapParameter::componentComplete()
{
    for (int i = m_initialPropertyCount; i < metaObject()->propertyCount(); ++i) {
        QMetaProperty property = metaObject()->property(i);

        if (!property.hasNotifySignal()) {
            return;
        }

        SignalMapper *mapper = new SignalMapper(i, this);

        const QByteArray signalName = '2' + property.notifySignal().methodSignature(); // TODO: explain why '2'
        QObject::connect(this, signalName, mapper, SLOT(map()));
        QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(onPropertyUpdated(int)));
    }
    m_complete = true;
    emit completed(this);
}

void QDeclarativeGeoMapParameter::onPropertyUpdated(int index)
{
    emit propertyUpdated(this, metaObject()->property(index).name());
}

QT_END_NAMESPACE

#include "qdeclarativegeomapparameter.moc"
