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

int QParameterizableObject::count(QQmlListProperty<QObject> *p)
{
    return static_cast<QParameterizableObject*>(p->object)->m_children.count();
}

QObject *QParameterizableObject::at(QQmlListProperty<QObject> *p, int idx)
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
            QAbstractDeclarativeData::parentChanged = parentChanged;
        }
    }

    static void parentChanged(QAbstractDeclarativeData *d, QObject *o, QObject *p);
};

Q_GLOBAL_STATIC(QParameterizableObjectData, parametrizableObjectData)

QParameterizableObject::QParameterizableObject(QObject *parent)
    : QObject(parent)
{
    QObjectPrivate::get(this)->declarativeData = parametrizableObjectData;
}

void QParameterizableObjectData::parentChanged(QAbstractDeclarativeData *d, QObject *o, QObject *p)
{
    Q_UNUSED(p)
    Q_UNUSED(d)
    QParameterizableObject *po = qobject_cast<QParameterizableObject *>(o);
    if (po)
        po->parentChanged();
}

QT_END_NAMESPACE
