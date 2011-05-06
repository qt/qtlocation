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

#ifndef QGEOMAPOBJECTINFO_H
#define QGEOMAPOBJECTINFO_H

#include "qmobilityglobal.h"
#include "qgeomapobject.h"

#include <QObject>
#include <QSizeF>

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingBox;

class QGeoMapData;
class QGeoMapObject;
class QGeoMapObjectInfoPrivate;

class Q_LOCATION_EXPORT QGeoMapObjectInfo : public QObject
{
    Q_OBJECT
public:
    QGeoMapObjectInfo(QGeoMapData *mapData, QGeoMapObject *mapObject);
    virtual ~QGeoMapObjectInfo();

    virtual void init();

    virtual QGeoBoundingBox boundingBox() const;
    virtual bool contains(const QGeoCoordinate &coordinate) const;

public slots:
    virtual void windowSizeChanged(const QSizeF &windowSize);
    virtual void zoomLevelChanged(qreal zoomLevel);
    virtual void centerChanged(const QGeoCoordinate &coordinate);

    virtual void zValueChanged(int zValue);
    virtual void visibleChanged(bool visible);
    virtual void selectedChanged(bool selected);

    virtual void originChanged(const QGeoCoordinate &origin);
    virtual void unitsChanged(QGeoMapObject::CoordinateUnit units);
    virtual void transformTypeChanged(QGeoMapObject::TransformType transformType);

protected:
    QGeoMapData* mapData();
    QGeoMapObject* mapObject();
    QGeoMapObject* mapObject() const;

private:
    QGeoMapObjectInfoPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoMapObjectInfo)
};

QTM_END_NAMESPACE

#endif
