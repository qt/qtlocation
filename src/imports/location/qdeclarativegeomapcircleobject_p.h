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

#ifndef QDECLARATIVEGEOMAPCIRCLEOBJECT_H
#define QDECLARATIVEGEOMAPCIRCLEOBJECT_H

#include "qdeclarativegeomapobject_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomapobjectborder_p.h"
#include "qgeomapcircleobject.h"

class QColor;
class QBrush;

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

class QDeclarativeGeoMapCircleObject : public QDeclarativeGeoMapObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeCoordinate* center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QDeclarativeGeoMapObjectBorder* border READ border)

public:
    QDeclarativeGeoMapCircleObject(QSGItem *parent = 0);
    ~QDeclarativeGeoMapCircleObject();

    QDeclarativeCoordinate* center();
    void setCenter(QDeclarativeCoordinate *center);

    qreal radius() const;
    void setRadius(qreal radius);

    QColor color() const;
    void setColor(const QColor &color);

    QDeclarativeGeoMapObjectBorder* border();

Q_SIGNALS:
    void centerChanged(const QDeclarativeCoordinate *center);
    void radiusChanged(qreal radius);
    void colorChanged(const QColor &color);

private Q_SLOTS:
    void borderColorChanged(const QColor &color);
    void borderWidthChanged(int width);
    void centerLatitudeChanged(double latitude);
    void centerLongitudeChanged(double longitude);
    void centerAltitudeChanged(double altitude);

private:
    QGeoMapCircleObject* circle_;
    QDeclarativeCoordinate *center_;
    QColor color_;
    QDeclarativeGeoMapObjectBorder border_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoMapCircleObject));

#endif
