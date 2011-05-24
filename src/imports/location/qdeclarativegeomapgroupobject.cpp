/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "qdeclarativegeomapgroupobject_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapGroup

    \brief The MapGroup element aggregates a set of map objects.
    \inherits QGeoMapGroupObject

    \ingroup qml-location-maps

    Grouping is provided to make it easier to quickly add, remove, show
    and hide groups of objects.

    It also allows users to specify an ordering of objects local to the
    group via the z-values and insertion order of the objects in the group.

    An example of group having a small red circle on top of bigger green circle:
    \snippet examples/declarative-location/landmarkmap/landmarkmap.qml MapGroup

    The MapGroup element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoMapGroupObject::QDeclarativeGeoMapGroupObject(QDeclarativeItem *parent)
    : QDeclarativeGeoMapObject(parent)
{
    group_ = new QGeoMapGroupObject();
    setMapObject(group_);
}

QDeclarativeGeoMapGroupObject::~QDeclarativeGeoMapGroupObject()
{
    // Remove all objects from the group before deleting it.
    // These objects are owned by their declarative counterparts
    // and they'll delete them.
    QList<QGeoMapObject*> objects = group_->childObjects();
    for (int i = 0; i < objects.size(); ++i) {
        group_->removeChildObject(objects.at(i));
    }
    delete group_;
}

void QDeclarativeGeoMapGroupObject::componentComplete()
{
    QList<QGraphicsItem*> children = childItems();
    for (int i = 0; i < children.size(); ++i) {
        QDeclarativeGeoMapObject *mapObject
        = qobject_cast<QDeclarativeGeoMapObject*>(children.at(i));
        if (mapObject) {
            group_->addChildObject(mapObject->mapObject());
            objects_.append(mapObject);
            mapObject->setMap(map());
        }
    }

    QDeclarativeGeoMapObject::componentComplete();
}

void QDeclarativeGeoMapGroupObject::setMap(QDeclarativeGraphicsGeoMap *map)
{
    QDeclarativeGeoMapObject::setMap(map);
    for (int i = 0; i < objects_.size(); ++i)
        objects_[i]->setMap(map);
}

/*!
    \qmlproperty list<QGeoMapObject> MapGroup::objects
    \default

    This property holds the list of objects which make up the group.
*/

QDeclarativeListProperty<QDeclarativeGeoMapObject> QDeclarativeGeoMapGroupObject::objects()
{
    return QDeclarativeListProperty<QDeclarativeGeoMapObject>(this,
            0,
            child_append,
            child_count,
            child_at,
            child_clear);
}

void QDeclarativeGeoMapGroupObject::child_append(QDeclarativeListProperty<QDeclarativeGeoMapObject> *prop, QDeclarativeGeoMapObject *mapObject)
{
    QDeclarativeGeoMapGroupObject* group = static_cast<QDeclarativeGeoMapGroupObject*>(prop->object);
    group->group_->addChildObject(mapObject->mapObject());
    group->objects_.append(mapObject);
}

int QDeclarativeGeoMapGroupObject::child_count(QDeclarativeListProperty<QDeclarativeGeoMapObject> *prop)
{
    return static_cast<QDeclarativeGeoMapGroupObject*>(prop->object)->objects_.size();
}

QDeclarativeGeoMapObject* QDeclarativeGeoMapGroupObject::child_at(QDeclarativeListProperty<QDeclarativeGeoMapObject> *prop, int index)
{
    return static_cast<QDeclarativeGeoMapGroupObject*>(prop->object)->objects_.at(index);
}

void QDeclarativeGeoMapGroupObject::child_clear(QDeclarativeListProperty<QDeclarativeGeoMapObject> *prop)
{
    QDeclarativeGeoMapGroupObject* group = static_cast<QDeclarativeGeoMapGroupObject*>(prop->object);
    group->group_->clearChildObjects();
    group->objects_.clear();
}

void QDeclarativeGeoMapGroupObject::doubleClickEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (event->accepted())
        return;

    for (int i = objects_.size() - 1; i >= 0; --i) {
        objects_.at(i)->doubleClickEvent(event);
        if (event->accepted())
            return;
    }

    QDeclarativeGeoMapObject::doubleClickEvent(event);
}

void QDeclarativeGeoMapGroupObject::pressEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (event->accepted())
        return;

    for (int i = objects_.size() - 1; i >= 0; --i) {
        objects_.at(i)->pressEvent(event);
        if (event->accepted())
            return;
    }

    QDeclarativeGeoMapObject::pressEvent(event);
}

void QDeclarativeGeoMapGroupObject::releaseEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (event->accepted())
        return;

    for (int i = objects_.size() - 1; i >= 0; --i) {
        objects_.at(i)->releaseEvent(event);
        if (event->accepted())
            return;
    }

    QDeclarativeGeoMapObject::releaseEvent(event);
}

void QDeclarativeGeoMapGroupObject::enterEvent()
{
    for (int i = objects_.size() - 1; i >= 0; --i)
        objects_.at(i)->enterEvent();

    QDeclarativeGeoMapObject::enterEvent();
}

void QDeclarativeGeoMapGroupObject::exitEvent()
{
    for (int i = objects_.size() - 1; i >= 0; --i)
        objects_.at(i)->exitEvent();

    QDeclarativeGeoMapObject::exitEvent();
}

void QDeclarativeGeoMapGroupObject::moveEvent(QDeclarativeGeoMapMouseEvent *event)
{
    if (event->accepted())
        return;

    for (int i = objects_.size() - 1; i >= 0; --i) {
        objects_.at(i)->moveEvent(event);
        if (event->accepted())
            return;
    }

    QDeclarativeGeoMapObject::moveEvent(event);
}

/*!
    \qmlproperty int MapGroup::zValue

    This property holds the z-value of the group.

    Map objects are drawn in z-value order, and objects with the
    same z-value will be drawn in insertion order.

    The objects inside the group are drawn according to the z-values
    and insertion order of the other elements of the group.  This
    is indpendent of the z-value and insertion order of the group
    element itself.
*/

/*!
    \qmlproperty bool MapGroup::visible

    This property holds a boolean corresponding to whether or not the
    group is visible.
*/

#include "moc_qdeclarativegeomapgroupobject_p.cpp"

QTM_END_NAMESPACE

