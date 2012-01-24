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

#include <QtGui/QGuiApplication>
#include "qdeclarativegeomappincharea_p.h"
#include "qdeclarativegeomap_p.h"
#include <QtGui/qevent.h>
#include <QtGui/QStyleHints>
#include <QtDeclarative/qdeclarativeinfo.h>
#include <QDebug>
#include "math.h"
#include "qgeomap_p.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapPinchEvent
    \inqmlmodule QtLocation 5

    \brief MapPinchEvent element provides basic information about pinch event.

    MapPinchEvent element provides basic information about pinch event. They are
    present in handlers of MapPinch (e.g. pinchStarted/pinchUpdated). Events are only
    guaranteed to be valid for the duration of the handler.

    Except for the \l accepted property, all properties are read-only.

    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0
*/

/*!
    \qmlproperty QPoint QtLocation5::MapPinchEvent::center

    This read-only property holds the current center point.
*/

/*!
    \qmlproperty real QtLocation5::MapPinchEvent::angle

    This read-only property holds the current angle between the two points in
    the range -180 to 180. Positive values for the angles mean counter-clockwise
    while negative values mean the clockwise direction. Zero degrees is at the
    3 o'clock position.
*/

/*!
    \qmlproperty QPoint QtLocation5::MapPinchEvent::point1
    \qmlproperty QPoint QtLocation5::MapPinchEvent::point2

    These read-only properties hold the actual touch points generating the pinch.
    The points are not in any particular order.
*/

/*!
    \qmlproperty int QtLocation5::MapPinchEvent::pointCount

    This read-only property holds the number of points currently touched.
    The MapPinch will not react until two touch points have initited a gesture,
    but will remain active until all touch points have been released.
*/

/*!
    \qmlproperty bool QtLocation5::MapPinchEvent::accepted

    Setting this property to false in the \c MapPinch::onPinchStarted handler
    will result in no further pinch events being generated, and the gesture
    ignored.
*/

/*!
    \qmlclass MapPinchArea QDeclarativeGeoMapPinchArea

    \inqmlmodule QtLocation 5

    \brief The MapPinchArea element provides basic Map pinch interaction.

    MapPinchArea elements are used as part of a Map, to provide for the
    pinch-to-zoom gesture used on touch displays. This is comparable to the
    activity of the \l{PinchArea} element.

    A MapPinchArea is automatically created with a new Map and available with
    the \l{QtLocation5::Map::pinch}{pinch} property. This is the only way
    to create a MapPinchArea, and once created this way cannot be destroyed
    without its parent Map.

    The two most commonly used properties of the MapPinchArea are the \l enabled
    and \l activeGestures properties. Both of these must be set before a
    MapPinchArea will have any effect upon interaction with the Map.

    \section2 Performance

    The MapPinchArea, when enabled, must process all incoming touch events in
    order to track the shape and size of the "pinch". The overhead added on
    touch events can be considered constant time.

    \section2 Example Usage

    The following example enables the pinch gesture on a map.

    \code
    Map {
        pinch.enabled: true
        pinch.activeGestures: MapPinchArea.ZoomGesture
    }
    \endcode

    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0
*/

/*!
    \qmlproperty bool QtLocation5::MapPinchArea::enabled

    This property holds whether the pinch gestures are enabled.
    Note: disabling pinch during active pinch does not have effect on
    the potentially active current pinch.
*/

/*!
    \qmlproperty bool QtLocation5::MapPinchArea::active

    This read-only property holds whether a pinch gesture is active.
*/

/*!
    \qmlproperty enumeration QtLocation5::MapPinchArea::activeGestures

    This property holds the gestures that the pinch should control.
    For the time being, only ZoomGesture is supported.

    For the extremist, one may OR flag the RotationGesture or TiltGesture
    but these come with absolutely no warranty or guarantees at the moment
    (may be removed, changed, moved around)
*/

/*!
    \qmlproperty real QtLocation5::MapPinchArea::maximumZoomLevelChange

    This property holds the maximum zoom level change per pinch, essentially
    meant to be used for setting the zoom sensitivity.

    It is an indicative measure calculated from the dimensions of the
    map area, roughly corresponding how much zoom level could change with
    maximum pinch zoom. Default value is 2.0, maximum value is 10.0
*/

/* todo uncomment this when rotation is supported
    \qmlproperty real MapPinchArea::rotationFactor

    This property holds the rotation factor for zoom, essentially meant to be used for setting
    the rotation sensitivity.

    It is an indicative measure; the default value 1.0 means the map roughly follows the fingers,
    whereas 2.0 means rotating twice as fast. Maximum value is 5.0.
*/

QDeclarativeGeoMapPinchArea::QDeclarativeGeoMapPinchArea(QDeclarativeGeoMap* map, QObject *parent)
    : QObject(parent),
      map_(map),
      enabled_(true),
      active_(false),
      minimumZoomLevel_(-1.0),
      maximumZoomLevel_(-1.0),
      minimumRotation_(0.0),
      maximumRotation_(0.0),
      inPinch_(false),
      pinchRejected_(false),
      pinchActivated_(false),
      pinchStartDist_(0),
      pinchStartZoomLevel_(0.0),
      pinchLastZoomLevel_(0.0),
      pinchStartRotation_(0.0),
      pinchLastAngle_(0.0),
      pinchRotation_(0.0),
      id1_(-1),
      maximumZoomLevelChange_(2.0),
      rotationFactor_(1.0),
      activeGestures_(ZoomGesture),
      minimumTilt_(0.0),
      maximumTilt_(90.0),
      maximumTiltChange_(20.0),
      pinchLastTilt_(0.0),
      pinchStartTilt_(0.0)
{
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
    if (activeGestures == activeGestures_)
        return;
    activeGestures_ = activeGestures;
    if (activeGestures_ & RotationGesture)
        qmlInfo(this) << tr("Pinchrotation gesture activated. Note that it is experimental feature.");
    if (activeGestures_ & TiltGesture)
        qmlInfo(this) << tr("Pinchtilt gesture activated. Note that it is experimental feature.");
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
    active_ = active;
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
    enabled_ = enabled;
    emit enabledChanged();
}

qreal QDeclarativeGeoMapPinchArea::minimumZoomLevel() const
{
    return minimumZoomLevel_;
}

void QDeclarativeGeoMapPinchArea::setMinimumZoomLevel(qreal zoomLevel)
{
    if (zoomLevel == minimumZoomLevel_ ||
            zoomLevel < map_->minimumZoomLevel() ||
            (maximumZoomLevel_ != -1.0 && zoomLevel > maximumZoomLevel_) )
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
    if (zoomLevel == maximumZoomLevel_ ||
            zoomLevel > map_->maximumZoomLevel() ||
            (minimumZoomLevel_ != - 1.0 && zoomLevel < minimumZoomLevel_))
        return;
    maximumZoomLevel_ = zoomLevel;
    emit maximumZoomLevelChanged();
}

// called internally when plugin's limits change. somewhat dodgy but
// initialization order complicates the zoom limit settings a bit (e.g. when is
// it possible to check against mapping plugins' limits)
void QDeclarativeGeoMapPinchArea::zoomLevelLimits(qreal min, qreal max)
{
    if (minimumZoomLevel_ == -1.0 || min > minimumZoomLevel_)
        setMinimumZoomLevel(min);
    if (maximumZoomLevel_ == -1.0 || max < maximumZoomLevel_)
        setMaximumZoomLevel(max);
}

qreal QDeclarativeGeoMapPinchArea::maximumZoomLevelChange() const
{
    return maximumZoomLevelChange_;
}

void QDeclarativeGeoMapPinchArea::setMaximumZoomLevelChange(qreal maxChange)
{
    if (maxChange == maximumZoomLevelChange_ || maxChange < 0.1 || maxChange > 10.0)
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
    if (rotation == minimumRotation_ ||
            rotation < 0 ||
            rotation > maximumRotation_)
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
    if (rotation == maximumRotation_ ||
            rotation > 360 ||
            rotation < minimumRotation_)
        return;
    maximumRotation_ = rotation;
    emit maximumRotationChanged();
}

qreal QDeclarativeGeoMapPinchArea::rotationFactor() const
{
    return rotationFactor_;
}

void QDeclarativeGeoMapPinchArea::setRotationFactor(qreal factor)
{
    if (rotationFactor_ == factor ||
            factor < 0 ||
            factor > 5)
        return;
    rotationFactor_ = factor;
    emit rotationFactorChanged();
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
            pinchEvent_.setCenter(pinchCenter);
            pinchEvent_.setAngle(pinchLastAngle_);
            pinchEvent_.setPoint1(map_->mapFromScene(lastPoint1_));
            pinchEvent_.setPoint2(map_->mapFromScene(lastPoint2_));
            pinchEvent_.setAccepted(true);
            pinchEvent_.setPointCount(0);
            emit pinchFinished(&pinchEvent_);
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
        const int dragThreshold = qApp->styleHints()->startDragDistance();
        QPointF p1 = touchPoint1.scenePos();
        QPointF p2 = touchPoint2.scenePos();
        qreal dx = p1.x() - p2.x();
        qreal dy = p1.y() - p2.y();
        qreal dist = sqrt(dx*dx + dy*dy);
        QPointF sceneCenter = (p1 + p2)/2;
        qreal angle = QLineF(p1, p2).angle();
        if (touchPoints_.count() == 1) {
            // If we only have one point then just move the center
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
                sceneLastCenter_ = sceneCenter;
                pinchStartDist_ = dist;
                pinchLastZoomLevel_ = 1.0;
                pinchLastTilt_ = 0.0;
                pinchLastAngle_ = angle;
                pinchRotation_ = 0.0;
                lastPoint1_ = p1;
                lastPoint2_ = p2;

                pinchEvent_.setCenter(map_->mapFromScene(sceneCenter));
                pinchEvent_.setAngle(angle);
                pinchEvent_.setPoint1(map_->mapFromScene(lastPoint1_));
                pinchEvent_.setPoint2(map_->mapFromScene(lastPoint2_));
                pinchEvent_.setPointCount(touchPoints_.count());
                pinchEvent_.setAccepted(true);
                emit pinchStarted(&pinchEvent_);

                if (pinchEvent_.accepted()) {
                    inPinch_ = true;
                    pinchStartZoomLevel_ = map_->zoomLevel();
                    pinchStartRotation_ = map_->bearing();
                    pinchStartTilt_ = map_->tilt();
                    setActive(true);
                } else {
                    pinchRejected_ = true;
                }
            }
        } else if (pinchStartDist_ > 0) {
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
            pinchRotation_ -= da;
            pinchEvent_.setCenter(map_->mapFromScene(sceneCenter));
            pinchEvent_.setAngle(angle);
            pinchEvent_.setPoint1(touchPoint1.pos());
            pinchEvent_.setPoint2(touchPoint2.pos());
            pinchEvent_.setPointCount(touchPoints_.count());
            pinchEvent_.setAccepted(true);

            sceneLastCenter_ = sceneCenter;
            pinchLastAngle_ = angle;
            lastPoint1_ = touchPoint1.scenePos();
            lastPoint2_ = touchPoint2.scenePos();
            emit pinchUpdated(&pinchEvent_);

            if (activeGestures_ & ZoomGesture) {
                // Take maximum and minimumzoomlevel into account
                qreal perPinchMinimumZoomLevel = qMax(pinchStartZoomLevel_ - maximumZoomLevelChange_, minimumZoomLevel_);
                qreal perPinchMaximumZoomLevel = qMin(pinchStartZoomLevel_ + maximumZoomLevelChange_, maximumZoomLevel_);
                newZoomLevel = qMin(qMax(perPinchMinimumZoomLevel, newZoomLevel), perPinchMaximumZoomLevel);
                pinchLastZoomLevel_ = newZoomLevel;
                map_->setZoomLevel(newZoomLevel);
            }
            if (activeGestures_ & TiltGesture && minimumZoomLevel_ >= 0 && maximumZoomLevel_ >= 0) {
                // Note: tilt is not yet supported.
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
                map_->setTilt(newTilt);
            }
            if (activeGestures_ & RotationGesture) {
                bool unlimitedRotation = (minimumRotation_ == 0.0 && maximumRotation_ == 0.0);
                if ((pinchStartRotation_ >= minimumRotation_ && pinchStartRotation_ <= maximumRotation_) || unlimitedRotation)  {
                    qreal r = pinchRotation_ * rotationFactor_ + pinchStartRotation_;
                    if (!unlimitedRotation)
                        r = qMin(qMax(minimumRotation_,r), maximumRotation_);
                    if (r > 360.0)
                        r -= 360;
                    if (r < -360.0)
                        r += 360.0;
                    map_->setBearing(r);
                }
            }
        }
    }
}


#include "moc_qdeclarativegeomappincharea_p.cpp"

QT_END_NAMESPACE
