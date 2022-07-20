/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include "qparameterizableobject_p.h"
#include "qdeclarativegeomapparameter_p.h"
#include <QtLocation/private/qgeomapparameter_p.h>
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

void QParameterizableObject::appendChild(QObject *v)
{
    m_children.append(v);
}

void QParameterizableObject::clearChildren()
{
    m_children.clear();
}

QQmlListProperty<QObject> QParameterizableObject::declarativeChildren()
{
    return QQmlListProperty<QObject>(this, nullptr,
                                                   &QParameterizableObject::append,
                                                   &QParameterizableObject::count,
                                                   &QParameterizableObject::at,
                                                   &QParameterizableObject::clear);
}

void QParameterizableObject::append(QQmlListProperty<QObject> *p, QObject *v)
{
    QParameterizableObject *object = static_cast<QParameterizableObject*>(p->object);
    object->appendChild(v);
}

qsizetype QParameterizableObject::count(QQmlListProperty<QObject> *p)
{
    return static_cast<QParameterizableObject*>(p->object)->m_children.count();
}

QObject *QParameterizableObject::at(QQmlListProperty<QObject> *p, qsizetype idx)
{
    return static_cast<QParameterizableObject*>(p->object)->m_children.at(idx);
}

void QParameterizableObject::clear(QQmlListProperty<QObject> *p)
{
    QParameterizableObject *object = static_cast<QParameterizableObject*>(p->object);
    object->clearChildren();
}

class QParameterizableObjectData: public QAbstractDeclarativeData
{
public:
    QParameterizableObjectData()
    {
        init();
    }

    static inline void init() {
        static bool initialized = false;
        if (!initialized) {
            initialized = true;
        }
    }

};

Q_GLOBAL_STATIC(QParameterizableObjectData, parametrizableObjectData)

QParameterizableObject::QParameterizableObject(QObject *parent)
    : QObject(parent)
{
    QObjectPrivate::get(this)->declarativeData = parametrizableObjectData;
}

void QParameterizableObject::setParentAndNotify(QObject *parent)
{
    QObject *oldParent = this->parent();
    QObject::setParent(parent);
    if (parent != oldParent)
        emit parentChanged();
}

QT_END_NAMESPACE
