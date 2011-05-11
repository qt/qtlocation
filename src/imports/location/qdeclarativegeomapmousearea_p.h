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

#ifndef QDECLARATIVEGEOMAPMOUSEAREA_H
#define QDECLARATIVEGEOMAPMOUSEAREA_H

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegraphicsgeomap_p.h"
#include "qdeclarativegeomapmouseevent_p.h"

#include <QtDeclarative/qdeclarativeitem.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoMapMouseArea : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(qreal mouseX READ mouseX NOTIFY mousePositionChanged)
    Q_PROPERTY(qreal mouseY READ mouseY NOTIFY mousePositionChanged)
    Q_PROPERTY(bool containsMouse READ hovered NOTIFY hoveredChanged)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(Qt::MouseButton pressedButton READ pressedButton NOTIFY pressedButtonChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)

public:
    QDeclarativeGeoMapMouseArea(QDeclarativeItem *parent = 0);
    ~QDeclarativeGeoMapMouseArea();

    void setMap(QDeclarativeGraphicsGeoMap *map);
    QDeclarativeGraphicsGeoMap* map() const;

    qreal mouseX() const;
    qreal mouseY() const;
    bool hovered() const;
    bool pressed() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    Qt::MouseButton pressedButton() const;

    bool hoverEnabled() const;
    void setHoverEnabled(bool hoverEnabled);

    void setAcceptedButtons(Qt::MouseButtons acceptedButtons);
    Qt::MouseButtons acceptedButtons() const;

    void doubleClickEvent(QDeclarativeGeoMapMouseEvent *event);
    void pressEvent(QDeclarativeGeoMapMouseEvent *event);
    void releaseEvent(QDeclarativeGeoMapMouseEvent *event);
    void enterEvent();
    void exitEvent();
    void moveEvent(QDeclarativeGeoMapMouseEvent *event);

Q_SIGNALS:
    void mousePositionChanged();
    void hoveredChanged(bool hovered);
    void pressedChanged(bool pressed);
    void enabledChanged(bool enabled);
    void pressedButtonChanged(Qt::MouseButtons pressedButton);
    void acceptedButtonsChanged(Qt::MouseButtons acceptedButtons);
    void hoverEnabledChanged(bool hoverEnabled);

    void positionChanged(QDeclarativeGeoMapMouseEvent *mouse);
    void pressed(QDeclarativeGeoMapMouseEvent *mouse);
//    void pressAndHold(QDeclarativeGeoMapMouseEvent *mouse);
    void released(QDeclarativeGeoMapMouseEvent *mouse);
    void clicked(QDeclarativeGeoMapMouseEvent *mouse);
    void doubleClicked(QDeclarativeGeoMapMouseEvent *mouse);
    void entered();
    void exited();
//    void cancelled();

private:
    bool setPressed(bool pressed, QDeclarativeGeoMapMouseEvent *event);
    void setHovered(bool hovered);

    bool hovered_;
    bool enabled_;
    bool hoverEnabled_;
    qreal mouseX_;
    qreal mouseY_;
    bool pressed_;
    bool longPress_;
    bool doubleClick_;
    Qt::MouseButtons acceptedButtons_;
    Qt::MouseButton pressedButton_;
    Qt::KeyboardModifiers modifiers_;

//    qreal startX_;
//    qreal startY_;
//    QPointF lastPos_;
//    QPointF lastScenePos_;
//    Qt::MouseButton lastButton_;
//    Qt::MouseButtons lastButtons_;
//    Qt::KeyboardModifiers lastModifiers_;

    QDeclarativeGraphicsGeoMap* map_;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapMouseArea));

#endif
