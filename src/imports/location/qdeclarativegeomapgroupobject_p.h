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

#ifndef QDECLARATIVEGEOMAPGROUPOBJECT_H
#define QDECLARATIVEGEOMAPGROUPOBJECT_H

#include "qgeomapgroupobject.h"

#include "qdeclarativegeomapobject_p.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeListProperty>

// !!! IMPORTANT !!!
//
// Inheriting from QSGItem here
// is just a workaround to have non-gui related (ie where visualization is not
// the main thing) autotests to pass in QML2 environment.
// Real QML2 Map support (and related map object is a work in progress elsewhere.
// This Map element instantiates but does not do anything meaningful from app dev
// perspective.
//
// !!! IMPORTANT !!!

QT_BEGIN_NAMESPACE

class QGeoCoordinate;

class QDeclarativeGeoMapGroupObject : public QDeclarativeGeoMapObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGeoMapObject> objects READ objects)

public:
    QDeclarativeGeoMapGroupObject(QSGItem *parent = 0);
    ~QDeclarativeGeoMapGroupObject();

    virtual void componentComplete();

    virtual void setMap(QDeclarativeGraphicsGeoMap *map);

    QDeclarativeListProperty<QDeclarativeGeoMapObject> objects();

    virtual void doubleClickEvent(QDeclarativeGeoMapMouseEvent *event);
    virtual void pressEvent(QDeclarativeGeoMapMouseEvent *event);
    virtual void releaseEvent(QDeclarativeGeoMapMouseEvent *event);
    virtual void enterEvent();
    virtual void exitEvent();
    virtual void moveEvent(QDeclarativeGeoMapMouseEvent *event);

private:
    static void child_append(QDeclarativeListProperty<QDeclarativeGeoMapObject> *prop, QDeclarativeGeoMapObject *mapObject);
    static int child_count(QDeclarativeListProperty<QDeclarativeGeoMapObject> *prop);
    static QDeclarativeGeoMapObject* child_at(QDeclarativeListProperty<QDeclarativeGeoMapObject> *prop, int index);
    static void child_clear(QDeclarativeListProperty<QDeclarativeGeoMapObject> *prop);

    QGeoMapGroupObject* group_;
    QList<QDeclarativeGeoMapObject*> objects_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoMapGroupObject));

#endif
