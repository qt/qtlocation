/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
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

#ifndef QDECLARATIVEGEOMAPMOUSEAREA_H
#define QDECLARATIVEGEOMAPMOUSEAREA_H

#include "qdeclarativecoordinate_p.h"
#include "qdeclarative3dgraphicsgeomap_p.h"
#include "qdeclarativegeomapmouseevent_p.h"

#include <QtDeclarative/QSGItem>

QT_BEGIN_NAMESPACE

#ifndef QSGMOUSEAREA_AVAILABLE

class QDeclarativeGeoMapMouseArea : public QSGItem
{
    Q_OBJECT

public:
    void setMap(QDeclarative3DGraphicsGeoMap *map) {Q_UNUSED(map);}
    bool mouseEvent(QMouseEvent* event) {Q_UNUSED(event); return false;}

    QDeclarativeGeoMapMouseArea(QSGItem *parent = 0) { Q_UNUSED(parent); qWarning("=================== using map mouse area stub ===============");}
    ~QDeclarativeGeoMapMouseArea() {}
};

#else

class QSGMouseArea;
class QSGMouseEvent;

class QDeclarativeGeoMapMouseArea : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(qreal mouseX READ mouseX NOTIFY positionChanged)
    Q_PROPERTY(qreal mouseY READ mouseY NOTIFY positionChanged)
    Q_PROPERTY(bool containsMouse READ hovered NOTIFY hoveredChanged)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(Qt::MouseButtons pressedButtons READ pressedButtons NOTIFY pressedChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons NOTIFY acceptedButtonsChanged)

public:
    QDeclarativeGeoMapMouseArea(QSGItem *parent = 0);
    ~QDeclarativeGeoMapMouseArea();

    // From QDeclarativeParserStatus
    virtual void componentComplete();

    void setMap(QDeclarative3DGraphicsGeoMap *map);

    qreal mouseX() const;
    qreal mouseY() const;
    bool hovered() const;
    bool pressed() const;

    bool isEnabled() const;
    void setEnabled(bool enabled);

    Qt::MouseButtons pressedButtons() const;
    void setAcceptedButtons(Qt::MouseButtons acceptedButtons);
    Qt::MouseButtons acceptedButtons() const;

    bool mouseEvent(QMouseEvent* event);

Q_SIGNALS:
    // publicly supported (i.e. documented) signals:
    void clicked(QDeclarativeGeoMapMouseEvent *mouse);
    void doubleClicked(QDeclarativeGeoMapMouseEvent *mouse);
    void pressed(QDeclarativeGeoMapMouseEvent *mouse);
    void released(QDeclarativeGeoMapMouseEvent *mouse);
    void positionChanged(QDeclarativeGeoMapMouseEvent *mouse);
    void pressAndHold(QDeclarativeGeoMapMouseEvent *mouse);
    void entered();
    void exited();
    // internal signals (non documented, used as property notifiers):
    void hoveredChanged(bool hovered);
    void pressedChanged(bool pressed);
    void enabledChanged(bool enabled);
    void acceptedButtonsChanged(Qt::MouseButtons acceptedButtons);

protected:
    // from QSGItem
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private slots:
    void pressedHandler(QSGMouseEvent* event);
    void releasedHandler(QSGMouseEvent* event);
    void clickedHandler(QSGMouseEvent* event);
    void doubleClickedHandler(QSGMouseEvent* event);
    void positionChangedHandler(QSGMouseEvent* event);
    void pressAndHoldHandler(QSGMouseEvent* event);
    void enteredHandler();
    void exitedHandler();
    void canceledHandler();
    void enabledChangedHandler();
    void acceptedButtonsChangedHandler();
    void pressedChangedHandler();
    void hoveredChangedHandler();

private:
    void mapMouseEvent(QSGMouseEvent* event);
    QDeclarativeGeoMapMouseEvent* mouseEvent_;
    QDeclarative3DGraphicsGeoMap* map_;
    QSGMouseArea* mouseArea_;
    bool componentCompleted_;
};

#endif // QSGMOUSEAREA_AVAILABLE
QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoMapMouseArea));

#endif
