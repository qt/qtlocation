/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMAPMOUSEEVENT_H
#define QDECLARATIVEGEOMAPMOUSEEVENT_H

#include <QtPositioning/QGeoCoordinate>

#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoMapMouseEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted)
    Q_PROPERTY(int button READ button)
    Q_PROPERTY(int buttons READ buttons)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool wasHeld READ wasHeld)
    Q_PROPERTY(int x READ x)
    Q_PROPERTY(int y READ y)
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate)

public:
    explicit QDeclarativeGeoMapMouseEvent(QObject *parent = 0);
    ~QDeclarativeGeoMapMouseEvent();

    void setAccepted(bool accepted);
    bool accepted() const;

    void setButton(int button);
    int button() const;

    void setButtons(int buttons);
    int buttons() const;

    void setModifiers(int modifiers);
    int modifiers() const;

    void setWasHeld(bool wasHeld);
    bool wasHeld() const;

    void setX(int x);
    int x() const;

    void setY(int y);
    int y() const;

    void setCoordinate(const QGeoCoordinate &coordinate);
    QGeoCoordinate coordinate();

private:
    bool accepted_;
    int button_;
    int modifiers_;
    bool wasHeld_;
    int x_;
    int y_;
    int buttons_;
    QGeoCoordinate coordinate_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeGeoMapMouseEvent)

#endif
