/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMAPPINCHAREA_H
#define QDECLARATIVEGEOMAPPINCHAREA_H

#include <QtQml/qqml.h>
#include <QTouchEvent>
#include <QObject>
#include <QDebug>
#include <QElapsedTimer>
#include "qgeocoordinate.h"
#include "qdeclarativegeomapgesturearea_p.h"

QT_BEGIN_NAMESPACE

class QGraphicsSceneMouseEvent;
class QDeclarativeGeoMap;
class QTouchEvent;
class QGeoMap;
class QPropertyAnimation;

// Note: this class id being deprecated, please use the gestureArea instead
class QDeclarativeGeoMapPinchArea: public QObject
{
    Q_OBJECT
    Q_ENUMS(ActiveGesture)
    Q_FLAGS(ActiveGestures)

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(ActiveGestures activeGestures READ activeGestures WRITE setActiveGestures NOTIFY activeGesturesChanged)
    Q_PROPERTY(qreal maximumZoomLevelChange READ maximumZoomLevelChange WRITE setMaximumZoomLevelChange NOTIFY maximumZoomLevelChangeChanged)
    Q_PROPERTY(qreal rotationFactor READ rotationFactor WRITE setRotationFactor NOTIFY rotationFactorChanged)
    Q_PROPERTY(qreal flickDeceleration READ flickDeceleration WRITE setFlickDeceleration NOTIFY flickDecelerationChanged)

public:
    QDeclarativeGeoMapPinchArea(QObject *parent, QDeclarativeGeoMapGestureArea *gestureArea);
    ~QDeclarativeGeoMapPinchArea();

    enum ActiveGesture {
        NoGesture = 0x0000,
        ZoomGesture = 0x0001,
        RotationGesture = 0x0002,
        TiltGesture = 0x0004
    };
    Q_DECLARE_FLAGS(ActiveGestures, ActiveGesture)
    ActiveGestures activeGestures()
    {
        QDeclarativeGeoMapGestureArea::ActiveGestures gestures = gestureArea_->activeGestures();
        activeGestures_ &= 0; // reset;
        if (gestures & QDeclarativeGeoMapGestureArea::ZoomGesture)
            activeGestures_ |= ZoomGesture;
        if (gestures & QDeclarativeGeoMapGestureArea::RotationGesture)
            activeGestures_ |= RotationGesture;
        if (gestures & QDeclarativeGeoMapGestureArea::TiltGesture)
            activeGestures_ |= TiltGesture;
        return activeGestures_;
    }
    void setActiveGestures(ActiveGestures activeGestures)
    {
        if (activeGestures == activeGestures_)
            return;
        activeGestures_ = activeGestures;
        QDeclarativeGeoMapGestureArea::ActiveGestures &gestures = gestureArea_->activeGestures_;
        gestures &= ~7; // reset the pinch component;
        if (activeGestures & ZoomGesture)
            gestures |= QDeclarativeGeoMapGestureArea::ZoomGesture;
        if (activeGestures & RotationGesture)
            gestures |= QDeclarativeGeoMapGestureArea::RotationGesture;
        if (activeGestures & TiltGesture)
            gestures |= QDeclarativeGeoMapGestureArea::TiltGesture;
        emit gestureArea_->activeGesturesChanged();
        emit activeGesturesChanged();
    }

    bool active() const { return gestureArea_->isPinchActive(); }
    void setActive(bool active) { gestureArea_->setPinchActive(active); }

    bool enabled() const { return gestureArea_->pinchEnabled(); }
    void setEnabled(bool enabled){ gestureArea_->setPinchEnabled(enabled); }

    qreal minimumZoomLevel() const { return gestureArea_->minimumZoomLevel(); }
    void setMinimumZoomLevel(qreal zoomLevel){ gestureArea_->setMinimumZoomLevel(zoomLevel); }

    qreal maximumZoomLevel() const { return gestureArea_->maximumZoomLevel(); }
    void setMaximumZoomLevel(qreal zoomLevel){ gestureArea_->setMaximumZoomLevel(zoomLevel); }

    qreal maximumZoomLevelChange() const { return gestureArea_->maximumZoomLevelChange(); }
    void setMaximumZoomLevelChange(qreal maxChange){ gestureArea_->setMaximumZoomLevelChange(maxChange); }

    qreal minimumRotation() const { return gestureArea_->minimumRotation(); }
    void setMinimumRotation(qreal zoomLevel){ gestureArea_->setMinimumRotation(zoomLevel); }

    qreal maximumRotation() const { return gestureArea_->maximumRotation(); }
    void setMaximumRotation(qreal zoomLevel){ gestureArea_->setMaximumRotation(zoomLevel); }

    qreal rotationFactor() const { return gestureArea_->rotationFactor(); }
    void setRotationFactor(qreal factor){ gestureArea_->setRotationFactor(factor); }

    qreal maximumTilt() const { return gestureArea_->maximumTilt(); }
    void setMaximumTilt(qreal tilt){ gestureArea_->setMaximumTilt(tilt); }

    qreal minimumTilt() const { return gestureArea_->minimumTilt(); }
    void setMinimumTilt(qreal tilt){ gestureArea_->setMinimumTilt(tilt); }

    qreal maximumTiltChange() const { return gestureArea_->maximumTiltChange(); }
    void setMaximumTiltChange(qreal tilt){ gestureArea_->setMaximumTiltChange(tilt); }

    qreal flickDeceleration() const { return gestureArea_->flickDeceleration(); }
    void setFlickDeceleration(qreal deceleration){ gestureArea_->setFlickDeceleration(deceleration); }

    void zoomLevelLimits(qreal min, qreal max){ gestureArea_->zoomLevelLimits(min, max); }
    void setMap(QGeoMap* map){ gestureArea_->setMap(map); }

signals:
    void activeChanged();
    void enabledChanged();
    void minimumZoomLevelChanged();
    void maximumZoomLevelChanged();
    void maximumZoomLevelChangeChanged();
    void minimumRotationChanged();
    void maximumRotationChanged();
    void rotationFactorChanged();
    void activeGesturesChanged();
    void minimumTiltChanged();
    void maximumTiltChanged();
    void maximumTiltChangeChanged();
    void flickDecelerationChanged();

    void pinchStarted(QDeclarativeGeoMapPinchEvent* pinch);
    void pinchUpdated(QDeclarativeGeoMapPinchEvent* pinch);
    void pinchFinished(QDeclarativeGeoMapPinchEvent* pinch);

private:
    QDeclarativeGeoMapGestureArea *gestureArea_; // the destination for this wrapper class
    ActiveGestures activeGestures_;
};

QT_END_NAMESPACE
QML_DECLARE_TYPE(QDeclarativeGeoMapPinchArea);

#endif
