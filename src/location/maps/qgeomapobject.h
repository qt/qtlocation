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

#ifndef QGEOMAPOBJECT_H
#define QGEOMAPOBJECT_H

#include "qmobilityglobal.h"

#include <QList>
#include <QObject>

class QPainter;
class QRectF;
class QGraphicsItem;

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingBox;
class QGeoMapObjectPrivate;
class QGeoMapContainer;
class QGeoMapObjectInfo;

class QGeoMapData;

class Q_LOCATION_EXPORT QGeoMapObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int zValue READ zValue WRITE setZValue NOTIFY zValueChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged)
    Q_PROPERTY(QGeoCoordinate origin READ origin NOTIFY originChanged)
    Q_PROPERTY(CoordinateUnit units READ units NOTIFY unitsChanged)
    Q_PROPERTY(TransformType transformType READ transformType NOTIFY transformTypeChanged)

public:
    enum Type {
        NullType,
        GroupType,
        RectangleType,
        CircleType,
        PolylineType,
        PolygonType,
        PixmapType,
        TextType,
        RouteType,
        CustomType
    };

    enum CoordinateUnit {
        PixelUnit,
        MeterUnit,
        RelativeArcSecondUnit,
        AbsoluteArcSecondUnit
    };

    enum TransformType {
        BilinearTransform,
        ExactTransform
    };

    QGeoMapObject(QGeoMapData *mapData = 0);
    virtual ~QGeoMapObject();

    virtual Type type() const;

    void setZValue(int zValue);
    int zValue() const;

    void setVisible(bool visible);
    bool isVisible() const;

    void setSelected(bool selected);
    bool isSelected() const;

    virtual QGeoBoundingBox boundingBox() const;
    virtual bool contains(const QGeoCoordinate &coordinate) const;

    bool operator<(const QGeoMapObject &other) const;
    bool operator>(const QGeoMapObject &other) const;

    virtual void setMapData(QGeoMapData *mapData);
    virtual QGeoMapData* mapData() const;

    QGeoMapObjectInfo *info() const;

    QGeoCoordinate origin() const;
    CoordinateUnit units() const;
    TransformType transformType() const;

protected:
    void setOrigin(const QGeoCoordinate &origin);
    void setUnits(const CoordinateUnit &unit);
    void setTransformType(const TransformType &type);

Q_SIGNALS:
    void zValueChanged(int zValue);
    void visibleChanged(bool visible);
    void selectedChanged(bool selected);

    void originChanged(QGeoCoordinate origin);
    void unitsChanged(QGeoMapObject::CoordinateUnit units);
    void transformTypeChanged(QGeoMapObject::TransformType transformType);

private:
    QGeoMapObjectPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoMapObject)
    Q_DISABLE_COPY(QGeoMapObject)

    friend class QGeoMapDataPrivate;
    friend class QGeoMapGroupObject;
};

QTM_END_NAMESPACE

#endif
