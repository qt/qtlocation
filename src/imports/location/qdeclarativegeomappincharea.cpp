/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtGui/QApplication>
#include "qdeclarativegeomappincharea_p.h"
#include "qdeclarative3dgraphicsgeomap_p.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "math.h"
#include "map.h"

QT_BEGIN_NAMESPACE

QDeclarativeGeoMapPinchArea::QDeclarativeGeoMapPinchArea(QDeclarative3DGraphicsGeoMap* map, QObject *parent)
    : QObject(parent),
      map_(map),
      enabled_(true),
      active_(false),
      minimumZoomLevel_(1.0),
      maximumZoomLevel_(1.0),
      minimumRotation_(0.0),
      maximumRotation_(0.0),
      inPinch_(false),
      pinchRejected_(false),
      pinchActivated_(false),
      pinchStartDist_(0),
      pinchStartZoomLevel_(0.0),
      pinchLastZoomLevel_(0.0),
      pinchStartRotation_(0.0),
      pinchStartAngle_(0.0),
      pinchLastAngle_(0.0),
      pinchRotation_(0.0),
      id1_(-1),
      maximumZoomLevelChange_(2.0),
      activeGestures_(ZoomGesture | RotationGesture),
      minimumTilt_(0.0),
      maximumTilt_(90.0),
      maximumTiltChange_(10.0),
      pinchLastTilt_(0.0),
      pinchStartTilt_(0.0)
{
    // this can be set as 'target' property should the need be to distinguish this element into MapPinchArea:
    Q_ASSERT(map_);
}

QDeclarativeGeoMapPinchArea::~QDeclarativeGeoMapPinchArea()
{
}

QDeclarativeGeoMapPinchArea::ActiveGestures QDeclarativeGeoMapPinchArea::activeGestures() const
{
    return activeGestures_;
}

void QDeclarativeGeoMapPinchArea::setActiveGestures(ActiveGestures activeGestures)
{
    qDebug() << metaObject()->className() <<  __FUNCTION__ << activeGestures;
    if (activeGestures == activeGestures_)
        return;
    activeGestures_ = activeGestures;
    emit activeGesturesChanged();
}

bool QDeclarativeGeoMapPinchArea::active() const
{
    return active_;
}

void QDeclarativeGeoMapPinchArea::setActive(bool active)
{
    if (active == active_)
        return;
    active = active_;
    emit activeChanged();
}

bool QDeclarativeGeoMapPinchArea::enabled() const
{
    return enabled_;
}

void QDeclarativeGeoMapPinchArea::setEnabled(bool enabled)
{
    if (enabled == enabled_)
        return;
    enabled = enabled_;
    emit enabledChanged();
}

qreal QDeclarativeGeoMapPinchArea::minimumZoomLevel() const
{
    return minimumZoomLevel_;
}

void QDeclarativeGeoMapPinchArea::setMinimumZoomLevel(qreal zoomLevel)
{
    if (zoomLevel < map_->minimumZoomLevel())
        zoomLevel = map_->minimumZoomLevel();
    if (zoomLevel == minimumZoomLevel_)
        return;
    minimumZoomLevel_ = zoomLevel;
    emit minimumZoomLevelChanged();
}

qreal QDeclarativeGeoMapPinchArea::maximumZoomLevel() const
{
    return maximumZoomLevel_;
}

void QDeclarativeGeoMapPinchArea::setMaximumZoomLevel(qreal zoomLevel)
{
    if (zoomLevel > map_->maximumZoomLevel())
        zoomLevel = map_->maximumZoomLevel();
    if (zoomLevel == maximumZoomLevel_)
        return;
    maximumZoomLevel_ = zoomLevel;
    emit maximumZoomLevelChanged();
}


qreal QDeclarativeGeoMapPinchArea::maximumZoomLevelChange() const
{
    return maximumZoomLevelChange_;
}

void QDeclarativeGeoMapPinchArea::setMaximumZoomLevelChange(qreal maxChange)
{
    if (maxChange == maximumZoomLevelChange_ || maxChange < 0.1)
        return;
    maximumZoomLevelChange_ = maxChange;
    emit maximumZoomLevelChangeChanged();
}

qreal QDeclarativeGeoMapPinchArea::minimumRotation() const
{
    return minimumRotation_;
}

void QDeclarativeGeoMapPinchArea::setMinimumRotation(qreal rotation)
{
    if (rotation == minimumRotation_ || rotation < 0.1)
        return;
    minimumRotation_ = rotation;
    emit minimumRotationChanged();
}

qreal QDeclarativeGeoMapPinchArea::maximumRotation() const
{
    return maximumRotation_;
}

void QDeclarativeGeoMapPinchArea::setMaximumRotation(qreal rotation)
{
    if (rotation == maximumRotation_ || rotation > 360)
        return;
    maximumRotation_ = rotation;
    emit maximumRotationChanged();
}

qreal QDeclarativeGeoMapPinchArea::rotationSpeed() const
{
    return rotationSpeed_;
}

void QDeclarativeGeoMapPinchArea::setRotationSpeed(qreal speed)
{
    if (rotationSpeed_ == speed || speed < 0.1)
        return;
    rotationSpeed_ = speed;
    emit rotationSpeedChanged();
}

qreal QDeclarativeGeoMapPinchArea::maximumTilt() const
{
    return maximumTilt_;
}

void QDeclarativeGeoMapPinchArea::setMaximumTilt(qreal tilt)
{
    if (maximumTilt_ == tilt)
        return;
    maximumTilt_ = tilt;
    emit maximumTiltChanged();
}

qreal QDeclarativeGeoMapPinchArea::minimumTilt() const
{
    return minimumTilt_;
}

void QDeclarativeGeoMapPinchArea::setMinimumTilt(qreal tilt)
{
    if (minimumTilt_ == tilt || tilt < 0.1)
        return;
    minimumTilt_ = tilt;
    emit minimumTiltChanged();
}

qreal QDeclarativeGeoMapPinchArea::maximumTiltChange() const
{
    return maximumTiltChange_;
}

void QDeclarativeGeoMapPinchArea::setMaximumTiltChange(qreal tilt)
{
    if (maximumTiltChange_ == tilt || tilt < 0.1)
        return;
    maximumTiltChange_ = tilt;
    emit maximumTiltChangeChanged();
}

void QDeclarativeGeoMapPinchArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event); // TODO may not be needed at all
    if (!enabled_)
        return;
}

void QDeclarativeGeoMapPinchArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event); // TODO may not be needed at all
    if (!enabled_)
        return;
}

void QDeclarativeGeoMapPinchArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event); // TODO may not be needed at all
    if (!enabled_)
        return;
}

void QDeclarativeGeoMapPinchArea::touchEvent(QTouchEvent *event)
{
    // Keep processing if gesture(s) are in progress. Otherwise we might
    // end up in lock.
    if ((!enabled_ || (activeGestures_ == NoGesture)) && !active())
        return;
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
        touchPoints_.clear();
        for (int i = 0; i < event->touchPoints().count(); ++i) {
            if (!(event->touchPoints().at(i).state() & Qt::TouchPointReleased)) {
                touchPoints_ << event->touchPoints().at(i);
            }
        }
        updatePinch();
        break;
    case QEvent::TouchEnd:
        touchPoints_.clear();
        updatePinch();
        break;
    default:
        // no-op
        break;
    }
}

void QDeclarativeGeoMapPinchArea::updatePinch()
{
    if (touchPoints_.count() == 0) {
        if (inPinch_) {
            inPinch_ = false;
            QPointF pinchCenter = map_->mapFromScene(sceneLastCenter_);
            QDeclarativeGeoMapPinchEvent pe(pinchCenter, pinchLastZoomLevel_, pinchLastAngle_, pinchRotation_);
            pe.setStartCenter(pinchStartCenter_);
            pe.setPreviousCenter(pinchCenter);
            pe.setPreviousAngle(pinchLastAngle_);
            pe.setPreviousZoomLevel(pinchLastZoomLevel_);
            pe.setStartPoint1(map_->mapFromScene(sceneStartPoint1_));
            pe.setStartPoint2(map_->mapFromScene(sceneStartPoint2_));
            pe.setPoint1(map_->mapFromScene(lastPoint1_));
            pe.setPoint2(map_->mapFromScene(lastPoint2_));
            emit pinchFinished(&pe);
            pinchStartDist_ = 0;
            pinchActivated_ = false;
            setActive(false);
        }
        return;
    }
    QTouchEvent::TouchPoint touchPoint1 = touchPoints_.at(0);
    QTouchEvent::TouchPoint touchPoint2 = touchPoints_.at(touchPoints_.count() >= 2 ? 1 : 0);
    if (touchPoints_.count() == 2
        && (touchPoint1.state() & Qt::TouchPointPressed || touchPoint2.state() & Qt::TouchPointPressed)) {
        id1_ = touchPoint1.id();
        sceneStartPoint1_ = touchPoint1.scenePos();
        sceneStartPoint2_ = touchPoint2.scenePos();
        inPinch_ = false;
        pinchRejected_ = false;
        pinchActivated_ = true;
    } else if (pinchActivated_ && !pinchRejected_){
        const int dragThreshold = QApplication::startDragDistance();
        QPointF p1 = touchPoint1.scenePos();
        QPointF p2 = touchPoint2.scenePos();
        qreal dx = p1.x() - p2.x();
        qreal dy = p1.y() - p2.y();
        qreal dist = sqrt(dx*dx + dy*dy);
        QPointF sceneCenter = (p1 + p2)/2;
        qreal angle = QLineF(p1, p2).angle();
        qDebug() << "angle between point1, point2, s: " << p1 <<  p2 << angle;
        if (touchPoints_.count() == 1) {
            // If we only have one point then just move the center. TODO do we need this anymore.
            if (id1_ == touchPoint1.id())
                sceneCenter = sceneLastCenter_ + touchPoint1.scenePos() - lastPoint1_;
            else
                sceneCenter = sceneLastCenter_ + touchPoint2.scenePos() - lastPoint2_;
            angle = pinchLastAngle_;
        }
        id1_ = touchPoint1.id();
        if (angle > 180)
            angle -= 360;
        if (!inPinch_) {
            if (touchPoints_.count() >= 2
                    && (qAbs(p1.x()-sceneStartPoint1_.x()) > dragThreshold
                    || qAbs(p1.y()-sceneStartPoint1_.y()) > dragThreshold
                    || qAbs(p2.x()-sceneStartPoint2_.x()) > dragThreshold
                    || qAbs(p2.y()-sceneStartPoint2_.y()) > dragThreshold)) {
                sceneStartCenter_ = sceneCenter;
                sceneLastCenter_ = sceneCenter;
                pinchStartCenter_ = map_->mapFromScene(sceneCenter);
                pinchStartDist_ = dist;
                pinchStartAngle_ = angle;
                pinchLastZoomLevel_ = 1.0;
                pinchLastTilt_ = 0.0;
                pinchLastAngle_ = angle;
                pinchRotation_ = 0.0;
                lastPoint1_ = p1;
                lastPoint2_ = p2;

                QDeclarativeGeoMapPinchEvent pe(pinchStartCenter_, 1.0, angle, 0.0);
                pe.setStartCenter(pinchStartCenter_);
                pe.setPreviousCenter(pinchStartCenter_);
                pe.setPreviousAngle(pinchLastAngle_);
                pe.setPreviousZoomLevel(pinchLastZoomLevel_);
                pe.setStartPoint1(map_->mapFromScene(sceneStartPoint1_));
                pe.setStartPoint2(map_->mapFromScene(sceneStartPoint2_));
                pe.setPoint1(map_->mapFromScene(lastPoint1_));
                pe.setPoint2(map_->mapFromScene(lastPoint2_));
                pe.setPointCount(touchPoints_.count());
                emit pinchStarted(&pe);
                if (pe.accepted()) {
                    inPinch_ = true;
                    // TODO is this pos needed. Analyze.
                    pinchStartPos_ = map_->pos();
                    pinchStartZoomLevel_ = map_->cameraData().zoomFactor();
                    pinchStartRotation_ = map_->cameraData().bearing();
                    pinchStartTilt_ = map_->cameraData().tilt();
                    qDebug() << "set starting tilt to : " << pinchStartTilt_;
                    setActive(true);
                } else {
                    pinchRejected_ = true;
                }
            }
        } else if (pinchStartDist_ > 0) { // TODO restructure this codeblock according to activeGestures_
            // Calculate the new zoom level if we have distance ( >= 2 touchpoints), otherwise stick with old.
            qreal newZoomLevel = pinchLastZoomLevel_;
            if (dist) {
                newZoomLevel =
                        // How much further/closer the current touchpoints are (in pixels) compared to pinch start
                        ((dist - pinchStartDist_)  *
                         //  How much one pixel corresponds in units of zoomlevel (and multiply by above delta)
                         (maximumZoomLevelChange_ / ((map_->width() + map_->height()) / 2))) +
                        // Add to starting zoom level. Sign of (dist-pinchstartdist) takes care of zoom in / out
                        pinchStartZoomLevel_;
            }
            qreal da = pinchLastAngle_ - angle;
            if (da > 180)
                da -= 360;
            else if (da < -180)
                da += 360;
            qDebug() << "pinch rotaiton will be update from, to, da is: " << pinchRotation_ << da - pinchRotation_ << da;
            pinchRotation_ -= da;
            // TODO check how this impacts is this needed
            QPointF pinchCenter = map_->mapFromScene(sceneCenter);
            QDeclarativeGeoMapPinchEvent pe(pinchCenter, newZoomLevel, angle, pinchRotation_);
            // TODO these events need to accommodate tilt.
            pe.setStartCenter(pinchStartCenter_);
            pe.setPreviousCenter(map_->mapFromScene(sceneLastCenter_));
            pe.setPreviousAngle(pinchLastAngle_);
            pe.setPreviousZoomLevel(pinchLastZoomLevel_);
            pe.setStartPoint1(map_->mapFromScene(sceneStartPoint1_));
            pe.setStartPoint2(map_->mapFromScene(sceneStartPoint2_));
            pe.setPoint1(touchPoint1.pos());
            pe.setPoint2(touchPoint2.pos());
            pe.setPointCount(touchPoints_.count());

            sceneLastCenter_ = sceneCenter;
            pinchLastAngle_ = angle;
            lastPoint1_ = touchPoint1.scenePos();
            lastPoint2_ = touchPoint2.scenePos();
            emit pinchUpdated(&pe);

            if (activeGestures_ & ZoomGesture) {
                // Take maximum and minimumzoomlevel into account
                qreal perPinchMinimumZoomLevel = qMax(pinchStartZoomLevel_ - maximumZoomLevelChange_, minimumZoomLevel_);
                qreal perPinchMaximumZoomLevel = qMin(pinchStartZoomLevel_ + maximumZoomLevelChange_, maximumZoomLevel_);
                newZoomLevel = qMin(qMax(perPinchMinimumZoomLevel, newZoomLevel), perPinchMaximumZoomLevel);
                pinchLastZoomLevel_ = newZoomLevel;
                CameraData cam = map_->map()->cameraData();
                cam.setZoomFactor(newZoomLevel);
                map_->map()->setCameraData(cam);
            }
            if (activeGestures_ & TiltGesture) {
                // TODO zzz
                qreal newTilt = pinchLastTilt_;
                if (dist) {
                    newTilt =
                            // How much further/closer the current touchpoints are (in pixels) compared to pinch start
                            ((dist - pinchStartDist_)  *
                             //  How much one pixel corresponds in units of tilt degrees (and multiply by above delta)
                             (maximumTiltChange_ / ((map_->width() + map_->height()) / 2))) +
                            // Add to starting tilt.
                            pinchStartTilt_;
                }
                qreal perPinchMinimumTilt = qMax(pinchStartTilt_ - maximumTiltChange_, minimumTilt_);
                qreal perPinchMaximumTilt = qMin(pinchStartTilt_ + maximumTiltChange_, maximumTilt_);
                newTilt = qMin(qMax(perPinchMinimumTilt, newTilt), perPinchMaximumTilt);
                pinchLastTilt_ = newTilt;

                qDebug() << "tilting pinchStartTilt, pinchLastTilt, resulting newTilt: " << pinchStartTilt_ << pinchLastTilt_ << newTilt;
                CameraData cam = map_->map()->cameraData();
                cam.setTilt(newTilt);
                map_->map()->setCameraData(cam);
            }

            //QPointF pos = sceneCenter - sceneStartCenter_ + pinchStartPos_;
            // TODO we probably don't want drag - leave that to flickable
            //if (pinch()->axis() & QSGPinch::XAxis) {
            //    qreal x = pos.x();
            //    if (x < pinch()->xmin())
            //        x = pinch()->xmin();
            //    else if (x > pinch()->xmax())
            //        x = pinch()->xmax();
            //    pinch()->target()->setX(x);
            //}
            //if (pinch()->axis() & QSGPinch::YAxis) {
            //    qreal y = pos.y();
            //    if (y < pinch()->ymin())
            //        y = pinch()->ymin();
            //    else if (y > pinch()->ymax())
            //        y = pinch()->ymax();
            //    pinch()->target()->setY(y);
            //}
            if (activeGestures_ & RotationGesture) {
                bool unlimitedRotation = (minimumRotation_ == 0.0 && maximumRotation_ == 0.0);
                if ((pinchStartRotation_ >= minimumRotation_ && pinchStartRotation_ <= maximumRotation_) || unlimitedRotation)  {
                    qreal r = pinchRotation_ * rotationSpeed_ + pinchStartRotation_;
                    if (!unlimitedRotation)
                        r = qMin(qMax(minimumRotation_,r), maximumRotation_);
                    if (r > 360.0)
                        r -= 360;
                    if (r < -360.0)
                        r += 360.0;
                    CameraData cam = map_->map()->cameraData();
                    cam.setBearing(r);
                    map_->map()->setCameraData(cam);
                }
            }
            // }
        }
    }
}


#include "moc_qdeclarativegeomappincharea_p.cpp"

QT_END_NAMESPACE
