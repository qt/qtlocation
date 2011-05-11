/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
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

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;

class QDeclarativeGeoMapGroupObject : public QDeclarativeGeoMapObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGeoMapObject> objects READ objects)

public:
    QDeclarativeGeoMapGroupObject(QDeclarativeItem *parent = 0);
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

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapGroupObject));

#endif
