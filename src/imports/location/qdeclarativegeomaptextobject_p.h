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

#ifndef QDECLARATIVEGEOMAPTEXTOBJECT_H
#define QDECLARATIVEGEOMAPTEXTOBJECT_H

#include "qdeclarativegeomapobject_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qgeomaptextobject.h"

#include <QColor>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoMapTextObject : public QDeclarativeGeoMapObject
{
    Q_OBJECT
    Q_ENUMS(HorizontalAlignment)
    Q_ENUMS(VerticalAlignment)

    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QPoint offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(HorizontalAlignment horizontalAlignment READ horizontalAlignment WRITE setHorizontalAlignment NOTIFY horizontalAlignmentChanged)
    Q_PROPERTY(VerticalAlignment verticalAlignment READ verticalAlignment WRITE setVerticalAlignment NOTIFY verticalAlignmentChanged)

public:
    enum HorizontalAlignment {
        AlignHCenter = Qt::AlignHCenter,
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight
    };

    enum VerticalAlignment {
        AlignVCenter = Qt::AlignVCenter,
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom
    };

    QDeclarativeGeoMapTextObject(QDeclarativeItem *parent = 0);
    ~QDeclarativeGeoMapTextObject();

    QDeclarativeCoordinate* coordinate();
    void setCoordinate(QDeclarativeCoordinate *coordinate);

    QString text() const;
    void setText(const QString &text);

    QFont font() const;
    void setFont(const QFont &font);

    QColor color() const;
    void setColor(const QColor &color);

    QPoint offset() const;
    void setOffset(const QPoint &offset);

    HorizontalAlignment horizontalAlignment() const;
    void setHorizontalAlignment(HorizontalAlignment alignment);

    VerticalAlignment verticalAlignment() const;
    void setVerticalAlignment(VerticalAlignment alignment);

Q_SIGNALS:
    void coordinateChanged(const QDeclarativeCoordinate *coordinate);
    void textChanged(const QString &text);
    void fontChanged(const QFont &font);
    void colorChanged(const QColor &color);
    void offsetChanged(const QPoint &offset);
    void horizontalAlignmentChanged(HorizontalAlignment alignment);
    void verticalAlignmentChanged(VerticalAlignment alignment);

private Q_SLOTS:
    void coordinateLatitudeChanged(double latitude);
    void coordinateLongitudeChanged(double longitude);
    void coordinateAltitudeChanged(double altitude);

private:
    QGeoMapTextObject* text_;
    QDeclarativeCoordinate *coordinate_;
    QColor color_;
    HorizontalAlignment hAlignment_;
    VerticalAlignment vAlignment_;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapTextObject));

#endif
