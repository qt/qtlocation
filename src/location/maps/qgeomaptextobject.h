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

#ifndef QGEOMAPTEXTOBJECT_H
#define QGEOMAPTEXTOBJECT_H

#include "qgeomapobject.h"
#include "qgeocoordinate.h"

#include <QFont>
#include <QPoint>

QTM_BEGIN_NAMESPACE

class QGeoMapTextObjectPrivate;

class Q_LOCATION_EXPORT QGeoMapTextObject : public QGeoMapObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QPen pen READ pen WRITE setPen NOTIFY penChanged)
    Q_PROPERTY(QBrush brush READ brush WRITE setBrush NOTIFY brushChanged)
    Q_PROPERTY(QPoint offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)

public:
    QGeoMapTextObject();
    QGeoMapTextObject(const QGeoCoordinate &coordinate,
                      const QString &text = QString(),
                      const QFont &font = QFont(),
                      const QPoint &offset = QPoint(),
                      Qt::Alignment alignment = Qt::AlignCenter);
    ~QGeoMapTextObject();

    QGeoMapObject::Type type() const;

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);

    QString text() const;
    void setText(const QString &text);

    QFont font() const;
    void setFont(const QFont &font);

    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

    QPoint offset() const;
    void setOffset(const QPoint &offset);

    Qt::Alignment alignment() const;
    void setAlignment(Qt::Alignment alignment);

Q_SIGNALS:
    void coordinateChanged(const QGeoCoordinate &coordinate);
    void textChanged(const QString &text);
    void fontChanged(const QFont &font);
    void penChanged(const QPen &pen);
    void brushChanged(const QBrush &brush);
    void offsetChanged(const QPoint &offset);
    void alignmentChanged(Qt::Alignment alignment);

private:
    QGeoMapTextObjectPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QGeoMapTextObject)
    Q_DISABLE_COPY(QGeoMapTextObject)
};

QTM_END_NAMESPACE

#endif
