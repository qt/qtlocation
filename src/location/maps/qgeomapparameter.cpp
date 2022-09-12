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


#include "qgeomapparameter_p.h"
#include <QtCore/QVariant>
#include <QDebug>
#include <QMetaProperty>

QT_BEGIN_NAMESPACE

QGeoMapParameter::QGeoMapParameter(QObject *parent) : QObject(parent)
{

}

QGeoMapParameter::~QGeoMapParameter()
{
}

bool QGeoMapParameter::operator==(const QGeoMapParameter &other) const
{
    return (other.toVariantMap() == toVariantMap());
}

QString QGeoMapParameter::type() const
{
    return m_type;
}

void QGeoMapParameter::setType(const QString &type)
{
    if (m_type.isEmpty())
        m_type = type;
}

// DO NOT USE to set "type"
void QGeoMapParameter::updateProperty(const char *propertyName, QVariant value)
{
    const QMetaProperty property = metaObject()->property(metaObject()->indexOfProperty(propertyName));
    property.write(this, value);
    // This should technically be emitted only for dynamically added properties.
    // Since this object has only type defined as Q_PROPERTY() which is a set-once
    // no check is really needed here.
    emit propertyUpdated(this, property);
}

QVariantMap QGeoMapParameter::toVariantMap() const
{
    QVariantMap res;
    const QMetaObject *metaObj = metaObject();
    // 0 is objectName, 1 is type, we want to skip both of them here.
    for (int i = m_initialPropertyCount; i < metaObj->propertyCount(); ++i) {
        const char *propName = metaObj->property(i).name();
        res[QLatin1String(propName)] = property(propName);
    }
    return res;
}

bool QGeoMapParameter::hasProperty(const char *propertyName) const
{
    return metaObject()->indexOfProperty(propertyName) != -1
            || dynamicPropertyNames().indexOf(QByteArray(propertyName)) != -1;
}

QT_END_NAMESPACE

