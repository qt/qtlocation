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

#ifndef QDECLARATIVEGEOMAPRECTANGLEOBJECT_H
#define QDECLARATIVEGEOMAPRECTANGLEOBJECT_H

#include "qdeclarativegeomapobject_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomapobjectborder_p.h"
#include "qgeomaprectangleobject.h"

class QColor;
class QBrush;

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoMapRectangleObject : public QDeclarativeGeoMapObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeCoordinate* topLeft READ topLeft WRITE setTopLeft NOTIFY topLeftChanged)
    Q_PROPERTY(QDeclarativeCoordinate* bottomRight READ bottomRight WRITE setBottomRight NOTIFY bottomRightChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QDeclarativeGeoMapObjectBorder* border READ border)

public:
    QDeclarativeGeoMapRectangleObject(QDeclarativeItem *parent = 0);
    ~QDeclarativeGeoMapRectangleObject();

    QDeclarativeCoordinate* topLeft();
    void setTopLeft(QDeclarativeCoordinate *center);

    QDeclarativeCoordinate* bottomRight();
    void setBottomRight(QDeclarativeCoordinate *center);

    QColor color() const;
    void setColor(const QColor &color);

    QDeclarativeGeoMapObjectBorder* border();

Q_SIGNALS:
    void topLeftChanged(const QDeclarativeCoordinate *center);
    void bottomRightChanged(const QDeclarativeCoordinate *center);
    void colorChanged(const QColor &color);

private Q_SLOTS:
    void topLeftLatitudeChanged(double latitude);
    void topLeftLongitudeChanged(double longitude);
    void topLeftAltitudeChanged(double altitude);

    void bottomRightLatitudeChanged(double latitude);
    void bottomRightLongitudeChanged(double longitude);
    void bottomRightAltitudeChanged(double altitude);

    void borderColorChanged(const QColor &color);
    void borderWidthChanged(int width);

private:
    QGeoMapRectangleObject* rectangle_;
    QDeclarativeCoordinate *topLeft_;
    QDeclarativeCoordinate *bottomRight_;
    QColor color_;
    QDeclarativeGeoMapObjectBorder border_;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapRectangleObject));

#endif
