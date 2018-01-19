/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#include "qmapiconobject_p.h"
#include "qmapiconobject_p_p.h"
#include <QExplicitlySharedDataPointer>

QT_BEGIN_NAMESPACE

QMapIconObjectPrivate::~QMapIconObjectPrivate()
{

}

QMapIconObjectPrivate::QMapIconObjectPrivate(QGeoMapObject *q) : QGeoMapObjectPrivate(q)
{

}

QGeoMapObject::Type QMapIconObjectPrivate::type() const
{
    return QGeoMapObject::IconType;
}

bool QMapIconObjectPrivate::equals(const QGeoMapObjectPrivate &other) const
{
    if (other.type() != type()) // This check might be unnecessary, depending on how equals gets used
        return false;

    const QMapIconObjectPrivate &o = static_cast<const QMapIconObjectPrivate &>(other);
    return (QGeoMapObjectPrivate::equals(o)
            && content() == o.content()
            && coordinate() == o.coordinate());
}

//
// QGeoMapIconPrivate default implementation
//

QMapIconObjectPrivateDefault::QMapIconObjectPrivateDefault(QGeoMapObject *q) : QMapIconObjectPrivate(q)
{

}
QMapIconObjectPrivateDefault::QMapIconObjectPrivateDefault(const QMapIconObjectPrivate &other) : QMapIconObjectPrivate(other.q)
{
    m_coordinate = other.coordinate();
    m_content = other.content();
    m_size = other.size();
}

QMapIconObjectPrivateDefault::~QMapIconObjectPrivateDefault()
{

}

QGeoCoordinate QMapIconObjectPrivateDefault::coordinate() const
{
    return m_coordinate;
}

void QMapIconObjectPrivateDefault::setCoordinate(const QGeoCoordinate &center)
{
    m_coordinate = center;
}

QVariant QMapIconObjectPrivateDefault::content() const
{
    return m_content;
}

void QMapIconObjectPrivateDefault::setContent(const QVariant &content)
{
    m_content = content;
}

QSizeF QMapIconObjectPrivateDefault::size() const
{
    return m_size;
}

void QMapIconObjectPrivateDefault::setSize(const QSizeF &size)
{
    m_size = size;
}

QGeoMapObjectPrivate *QMapIconObjectPrivateDefault::clone()
{
    return new QMapIconObjectPrivateDefault(static_cast<QMapIconObjectPrivate &>(*this));
}


/*

    QGeoMapIconPrivate default implementation

*/


QMapIconObject::QMapIconObject(QObject *parent)
    : QGeoMapObject(QExplicitlySharedDataPointer<QGeoMapObjectPrivate>(new QMapIconObjectPrivateDefault(this)), parent)
{}

QMapIconObject::~QMapIconObject()
{

}

QVariant QMapIconObject::content() const
{
    const QMapIconObjectPrivate *d = static_cast<const QMapIconObjectPrivate *>(d_ptr.data());
    return d->content();
}

QGeoCoordinate QMapIconObject::coordinate() const
{
    const QMapIconObjectPrivate *d = static_cast<const QMapIconObjectPrivate *>(d_ptr.data());
    return d->coordinate();
}

void QMapIconObject::setContent(QVariant content)
{
    QMapIconObjectPrivate *d = static_cast<QMapIconObjectPrivate *>(d_ptr.data());
    if (d->content() == content)
        return;

    d->setContent(content);
    emit contentChanged(content);
}

void QMapIconObject::setCoordinate(const QGeoCoordinate &center)
{
    QMapIconObjectPrivate *d = static_cast<QMapIconObjectPrivate*>(d_ptr.data());
    if (d->coordinate() == center)
        return;

    d->setCoordinate(center);
    emit coordinateChanged(center);
}

QSizeF QMapIconObject::size() const
{
    const QMapIconObjectPrivate *d = static_cast<const QMapIconObjectPrivate *>(d_ptr.data());
    return d->size();
}


void QMapIconObject::setSize(const QSizeF &size)
{
    QMapIconObjectPrivate *d = static_cast<QMapIconObjectPrivate*>(d_ptr.data());
    if (d->size() == size)
        return;

    d->setSize(size);
    emit sizeChanged();
}

void QMapIconObject::setMap(QGeoMap *map)
{
    QMapIconObjectPrivate *d = static_cast<QMapIconObjectPrivate *>(d_ptr.data());
    if (d->m_map == map)
        return;

    QGeoMapObject::setMap(map); // This is where the specialized pimpl gets created and injected

    if (!map) {
        // Map was set, now it has ben re-set to NULL
        d_ptr = new QMapIconObjectPrivateDefault(*d);
        // Old pimpl deleted implicitly by QExplicitlySharedDataPointer
    }
}

QT_END_NAMESPACE
