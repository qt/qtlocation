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

#ifndef QGEOMAPCIRCLEOBJECT_H
#define QGEOMAPCIRCLEOBJECT_H

#include "qgeomapobject.h"

#include <QMetaType>
#include <QPen>
#include <QBrush>

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoBoundingCircle;
class QGeoMapCircleObjectPrivate;

class Q_LOCATION_EXPORT QGeoMapCircleObject : public QGeoMapObject
{
    Q_OBJECT
public:
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QPen pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush NOTIFY brushChanged)
    Q_PROPERTY(quint32 pointCount READ pointCount WRITE setPointCount)

    QGeoMapCircleObject();
    QGeoMapCircleObject(const QGeoBoundingCircle &circle);
    QGeoMapCircleObject(const QGeoCoordinate &center, qreal radius);
    ~QGeoMapCircleObject();

    QGeoMapObject::Type type() const;

    QGeoBoundingCircle circle() const;
    void setCircle(const QGeoBoundingCircle &circle);

    bool contains(const QGeoCoordinate &coordinate) const;

    QGeoCoordinate center() const;
    void setCenter(const QGeoCoordinate &center);

    qreal radius() const;
    void setRadius(qreal radius);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

    quint32 pointCount() const;
    void setPointCount(quint32 pointCount);

Q_SIGNALS:
    void centerChanged(const QGeoCoordinate &center);
    void radiusChanged(qreal radius);
    void penChanged(const QPen &pen);
    void brushChanged(const QBrush &brush);

private:
    QGeoMapCircleObjectPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoMapCircleObject)
    Q_DISABLE_COPY(QGeoMapCircleObject)
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(qreal)

#endif
