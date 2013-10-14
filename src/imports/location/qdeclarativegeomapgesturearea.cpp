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

#include "qdeclarativegeomapgesturearea_p.h"
#include "qdeclarativegeomap_p.h"
#include "error_messages.h"

#include <QtGui/QGuiApplication>
#include <QtGui/qevent.h>
#include <QtGui/QStyleHints>
#include <QtQml/qqmlinfo.h>
#include <QPropertyAnimation>
#include <QDebug>
#include "math.h"
#include "qgeomap_p.h"

#define QML_MAP_FLICK_DEFAULTMAXVELOCITY 2500
#define QML_MAP_FLICK_MINIMUMDECELERATION 500
#define QML_MAP_FLICK_DEFAULTDECELERATION 2500
#define QML_MAP_FLICK_MAXIMUMDECELERATION 10000

#define QML_MAP_FLICK_VELOCITY_SAMPLE_PERIOD 50
// FlickThreshold determines how far the "mouse" must have moved
// before we perform a flick.
static const int FlickThreshold = 20;
// Really slow flicks can be annoying.
const qreal MinimumFlickVelocity = 75.0;

QT_BEGIN_NAMESPACE


/*!
    \qmltype MapPinchEvent
    \instantiates QDeclarativeGeoMapPinchEvent
    \inqmlmodule QtLocation

    \brief MapPinchEvent type provides basic information about pinch event.

    MapPinchEvent type provides basic information about pinch event. They are
    present in handlers of MapPinch (for example pinchStarted/pinchUpdated). Events are only
    guaranteed to be valid for the duration of the handler.

    Except for the \l accepted property, all properties are read-only.

    \section2 Example Usage

    The following example enables the pinch gesture on a map and reacts to the
    finished event.

    \code
    Map {
        id: map
        gesture.enabled: true
        gesture.onPinchFinished:{
            var coordinate1 = map.toCoordinate(gesture.point1)
            var coordinate2 = map.toCoordinate(gesture.point2)
            console.log("Pinch started at:")
            console.log("        Points (" + gesture.point1.x + ", " + gesture.point1.y + ") - (" + gesture.point2.x + ", " + gesture.point2.y + ")")
            console.log("   Coordinates (" + coordinate1.latitude + ", " + coordinate1.longitude + ") - (" + coordinate2.latitude + ", " + coordinate2.longitude + ")")
        }
    }
    \endcode

    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0
*/

/*!
    \qmlproperty QPoint QtLocation::MapPinchEvent::center

    This read-only property holds the current center point.
*/

/*!
    \qmlproperty real QtLocation::MapPinchEvent::angle

    This read-only property holds the current angle between the two points in
    the range -180 to 180. Positive values for the angles mean counter-clockwise
    while negative values mean the clockwise direction. Zero degrees is at the
    3 o'clock position.
*/

/*!
    \qmlproperty QPoint QtLocation::MapPinchEvent::point1
    \qmlproperty QPoint QtLocation::MapPinchEvent::point2

    These read-only properties hold the actual touch points generating the pinch.
    The points are not in any particular order.
*/

/*!
    \qmlproperty int QtLocation::MapPinchEvent::pointCount

    This read-only property holds the number of points currently touched.
    The MapPinch will not react until two touch points have initiated a gesture,
    but will remain active until all touch points have been released.
*/

/*!
    \qmlproperty bool QtLocation::MapPinchEvent::accepted

    Setting this property to false in the \c MapPinch::onPinchStarted handler
    will result in no further pinch events being generated, and the gesture
    ignored.
*/

/*!
    \qmltype MapGestureArea
    \instantiates QDeclarativeGeoMapGestureArea

    \inqmlmodule QtLocation

    \brief The MapGestureArea type provides Map gesture interaction.

    MapGestureArea objects are used as part of a Map, to provide for panning,
    flicking and pinch-to-zoom gesture used on touch displays.

    A MapGestureArea is automatically created with a new Map and available with
    the \l{Map::gesture}{gesture} property. This is the only way
    to create a MapGestureArea, and once created this way cannot be destroyed
    without its parent Map.

    The two most commonly used properties of the MapGestureArea are the \l enabled
    and \l activeGestures properties. Both of these must be set before a
    MapGestureArea will have any effect upon interaction with the Map.
    The \l flickDeceleration property controls how quickly the map pan slows after contact
    is released while panning the map.

    \section2 Performance

    The MapGestureArea, when enabled, must process all incoming touch events in
    order to track the shape and size of the "pinch". The overhead added on
    touch events can be considered constant time.

    \section2 Example Usage

    The following example enables the zoom and pan gestures on the map, but not flicking. So the
    map scrolling will halt immediately on releasing the mouse button / touch.

    \code
    Map {
        gesture.enabled: true
        gesture.activeGestures: MapGestureArea.ZoomGesture | MapGestureArea.PanGesture
    }
    \endcode

    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0
*/

/*!
    \qmlproperty bool QtLocation::MapGestureArea::enabled

    This property holds whether the gestures are enabled.
    Note: disabling gestures during an active gesture does not have effect on
    the potentially active current gesture.
*/


/*!
    \qmlproperty bool QtLocation::MapGestureArea::panEnabled

    This property holds whether the pan gestures are enabled.
    Note: disabling gestures during an active gesture does not have effect on
    the potentially active current gesture.
*/

/*!
    \qmlproperty bool QtLocation::MapGestureArea::pinchEnabled

    This property holds whether the pinch gestures are enabled.
    Note: disabling gestures during an active gesture does not have effect on
    the potentially active current gesture.
*/

/*!
    \qmlproperty bool QtLocation::MapGestureArea::isPinchActive

    This read-only property holds whether any pinch gesture is active.
*/

/*!
    \qmlproperty bool QtLocation::MapGestureArea::isPanActive

    This read-only property holds whether any pan gesture (panning or flicking) is active.
*/

/*!
    \qmlproperty enumeration QtLocation::MapGestureArea::activeGestures

    This property holds the gestures that will be active. By default
    the zoom, pan and flick gestures are enabled.

    \list
    \li MapGestureArea.NoGesture - Don't support any additional gestures (value: 0x0000).
    \li MapGestureArea.ZoomGesture - Support the map zoom gesture (value: 0x0001).
    \li MapGestureArea.RotationGesture - Support the map rotation gesture (value: 0x0002).
    \li MapGestureArea.TiltGesture - Support the map tilt gesture (value: 0x0004).
    \li MapGestureArea.PanGesture  - Support the map pan gesture (value: 0x0008).
    \li MapGestureArea.FlickGesture  - Support the map flick gesture (value: 0x0010).
    \endlist

    For the extremist, one may OR flag the RotationGesture or TiltGesture
    but these come with absolutely no warranty or guarantees at the moment
    (may be removed, changed, moved around)

    \note For the time being, only MapGestureArea.ZoomGesture is supported.
*/

/*!
    \qmlproperty real QtLocation::MapGestureArea::maximumZoomLevelChange

    This property holds the maximum zoom level change per pinch, essentially
    meant to be used for setting the zoom sensitivity.

    It is an indicative measure calculated from the dimensions of the
    map area, roughly corresponding how much zoom level could change with
    maximum pinch zoom. Default value is 2.0, maximum value is 10.0
*/

/*!
    \qmlproperty real MapGestureArea::rotationFactor

    This property holds the rotation factor for zoom, essentially meant to be used for setting
    the rotation sensitivity.

    It is an indicative measure; the default value 1.0 means the map roughly follows the fingers,
    whereas 2.0 means rotating twice as fast. Maximum value is 5.0.
*/

/*!
    \qmlproperty real MapGestureArea::flickDeceleration

    This property holds the rate at which a flick will decelerate.

    The default value is 2500.
*/

/*!
    \qmlsignal QtLocation::MapGestureArea::pinchStarted(PinchEvent event)

    Raised when a pinch gesture is started.

    \sa pinchUpdated, pinchFinished
*/

/*!
    \qmlsignal QtLocation::MapGestureArea::pinchUpdated(PinchEvent event)

    Once a pinch has begun this event gets raised as the user moves her fingers
    across the map.

    \sa pinchStarted, pinchFinished
*/

/*!
    \qmlsignal QtLocation::MapGestureArea::pinchFinished(PinchEvent event)

    The end of a pinch gesture is signaled by this event.

    \sa pinchStarted, pinchUpdated
*/

/*!
    \qmlsignal QtLocation::MapGestureArea::panStarted()

    This handler is called when the view begins moving due to user
    interaction. Typically this means that the user is dragging a finger -
    or a mouse with one of more mouse buttons pressed - on the map.
*/

/*!
    \qmlsignal QtLocation::MapGestureArea::panFinished()

    This handler is called when the view stops moving due to user
    interaction.  If a flick was generated, this handler will
    be triggered once the flick stops.  If a flick was not
    generated, the handler will be triggered when the
    user stops dragging - that is a mouse or touch release.

*/

/*!
    \qmlsignal QtLocation::MapGestureArea::flickStarted()

    This handler is called when the view is flicked.  A flick
    starts from the point that the mouse or touch is released,
    while still in motion.
*/

/*!
    \qmlsignal QtLocation::MapGestureArea::flickFinished()

    This handler is called when the view stops moving due to a flick.
    The order of panFinished() and flickFinished() is not specified.
*/

QDeclarativeGeoMapGestureArea::QDeclarativeGeoMapGestureArea(QDeclarativeGeoMap *map, QObject *parent)
    : QObject(parent),
      declarativeMap_(map),
      enabled_(true),
      activeGestures_(ZoomGesture | PanGesture | FlickGesture)
{
    map_ = 0;
    pan_.enabled_ = true,
    pan_.maxVelocity_ = QML_MAP_FLICK_DEFAULTMAXVELOCITY;
    pan_.deceleration_ = QML_MAP_FLICK_DEFAULTDECELERATION;
    pan_.animation_ = 0;
    touchPointState_ = touchPoints0;
    pinchState_ = pinchInactive;
    panState_ = panInactive;

}
/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMap(QGeoMap *map)
{
    if (map_ || !map)
        return;
    map_ = map;
    pan_.animation_ = new QPropertyAnimation(map_->mapController(), "center", this);
    pan_.animation_->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    connect(pan_.animation_, SIGNAL(finished()), this, SLOT(endFlick()));
    connect(this, SIGNAL(movementStopped()),
            map_, SLOT(cameraStopped()));
}

QDeclarativeGeoMapGestureArea::~QDeclarativeGeoMapGestureArea()
{
}

/*!
    \internal
*/
QDeclarativeGeoMapGestureArea::ActiveGestures QDeclarativeGeoMapGestureArea::activeGestures() const
{
    return activeGestures_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setActiveGestures(ActiveGestures activeGestures)
{
    if (activeGestures == activeGestures_)
        return;
    activeGestures_ = activeGestures;
    if (activeGestures_ & RotationGesture)
        qmlInfo(this) << QCoreApplication::translate(CONTEXT_NAME, PINCH_ROTATE_GESTURE_ACTIVATED);
    if (activeGestures_ & TiltGesture)
        qmlInfo(this) << QCoreApplication::translate(CONTEXT_NAME, PINCH_TILT_GESTURE_ACTIVATED);
    emit activeGesturesChanged();
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::isPinchActive() const
{
    return pinchState_ == pinchActive;
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::isPanActive() const
{
    return panState_ == panActive || panState_ == panFlick;
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::enabled() const
{
    return enabled_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setEnabled(bool enabled)
{
    if (enabled == enabled_)
        return;
    enabled_ = enabled;
    emit enabledChanged();
}


/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::pinchEnabled() const
{
    return pinch_.enabled;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setPinchEnabled(bool enabled)
{
    if (enabled == pinch_.enabled)
        return;
    pinch_.enabled = enabled;
    emit pinchEnabledChanged();
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::panEnabled() const
{
    return pan_.enabled_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setPanEnabled(bool enabled)
{
    if (enabled == pan_.enabled_)
        return;
    pan_.enabled_ = enabled;
    emit panEnabledChanged();

    // unlike the pinch, the pan existing functionality is to stop immediately
    if (!enabled)
        stopPan();
}

/*!
    \internal
    called internally when plugin's limits change. somewhat dodgy but
    initialization order complicates the zoom limit settings a bit (for example when is
    it possible to check against mapping plugins' limits)
*/
void QDeclarativeGeoMapGestureArea::zoomLevelLimits(qreal min, qreal max)
{
    if (pinch_.zoom.minimum == -1.0 || min > pinch_.zoom.minimum)
        pinch_.zoom.minimum = min;
    if (pinch_.zoom.maximum == -1.0 || max < pinch_.zoom.maximum)
        pinch_.zoom.maximum = max;
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::maximumZoomLevelChange() const
{
    return pinch_.zoom.maximumChange;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMaximumZoomLevelChange(qreal maxChange)
{
    if (maxChange == pinch_.zoom.maximumChange || maxChange < 0.1 || maxChange > 10.0)
        return;
    pinch_.zoom.maximumChange = maxChange;
    emit maximumZoomLevelChangeChanged();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::rotationFactor() const
{
    return pinch_.rotation.factor;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setRotationFactor(qreal factor)
{
    if (pinch_.rotation.factor == factor ||  factor < 0 || factor > 5)
        return;
    pinch_.rotation.factor = factor;
    emit rotationFactorChanged();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::flickDeceleration() const
{
    return pan_.deceleration_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setFlickDeceleration(qreal deceleration)
{
    if (deceleration < QML_MAP_FLICK_MINIMUMDECELERATION)
        deceleration = QML_MAP_FLICK_MINIMUMDECELERATION;
    else if (deceleration > QML_MAP_FLICK_MAXIMUMDECELERATION)
        deceleration = QML_MAP_FLICK_MAXIMUMDECELERATION;
    if (deceleration == pan_.deceleration_)
        return;
    pan_.deceleration_ = deceleration;
    emit flickDecelerationChanged();
}

/*!
    \internal
*/
QTouchEvent::TouchPoint makeTouchPointFromMouseEvent(QMouseEvent *event, Qt::TouchPointState state)
{
    // this is only partially filled. But since it is only partially used it works
    // more robust would be to store a list of QPointFs rather than TouchPoints
    QTouchEvent::TouchPoint newPoint;
    newPoint.setPos(event->localPos());
    newPoint.setScenePos(event->windowPos());
    newPoint.setScreenPos(event->screenPos());
    newPoint.setState(state);
    newPoint.setId(0);
    return newPoint;
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::mousePressEvent(QMouseEvent *event)
{
    if (!(enabled_ && activeGestures_))
        return false;

    touchPoints_.clear();
    touchPoints_ << makeTouchPointFromMouseEvent(event, Qt::TouchPointPressed);

    update();
    return true;
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::mouseMoveEvent(QMouseEvent *event)
{
    if (!(enabled_ && activeGestures_))
        return false;

    touchPoints_.clear();

    touchPoints_ << makeTouchPointFromMouseEvent(event, Qt::TouchPointMoved);
    update();
    return true;
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::mouseReleaseEvent(QMouseEvent *)
{
    if (!(enabled_ && activeGestures_))
        return false;

    touchPoints_.clear();
    update();
    return true;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::touchEvent(QTouchEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
        touchPoints_.clear();
        for (int i = 0; i < event->touchPoints().count(); ++i) {
            if (!(event->touchPoints().at(i).state() & Qt::TouchPointReleased)) {
                touchPoints_ << event->touchPoints().at(i);
            }
        }
        update();
        break;
    case QEvent::TouchEnd:
        touchPoints_.clear();
        update();
        break;
    default:
        // no-op
        break;
    }
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::filterMapChildMouseEvent(QMouseEvent *event)
{
    bool used = false;
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        used = mousePressEvent(event);
        break;
    case QEvent::MouseButtonRelease:
        used = mouseReleaseEvent(event);
        break;
    case QEvent::MouseMove:
        used = mouseMoveEvent(event);
        break;
    default:
        used = false;
        break;
    }
    return used && (isPanActive() || isPinchActive());
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::filterMapChildTouchEvent(QTouchEvent *event)
{
    touchEvent(event);
    return isPanActive() || isPinchActive();
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::clearTouchData()
{
    velocityX_ = 0;
    velocityY_ = 0;
    pressTime_.start();
    sceneCenter_.setX(0);
    sceneCenter_.setY(0);
    touchCenterCoord_.setLongitude(0);
    touchCenterCoord_.setLatitude(0);
    startCoord_.setLongitude(0);
    startCoord_.setLatitude(0);
}


/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::updateVelocityList(const QPointF &pos)
{
    // Take velocity samples every sufficient period of time, used later to determine the flick
    // duration and speed (when mouse is released).
    qreal elapsed = qreal(lastPosTime_.elapsed());

    if (elapsed >= QML_MAP_FLICK_VELOCITY_SAMPLE_PERIOD) {
        elapsed /= 1000.;
        int dyFromLastPos = pos.y() - lastPos_.y();
        int dxFromLastPos = pos.x() - lastPos_.x();
        lastPos_ = pos;
        lastPosTime_.restart();
        qreal velX = qreal(dxFromLastPos) / elapsed;
        qreal velY = qreal(dyFromLastPos) / elapsed;
        velocityX_ = qBound<qreal>(-pan_.maxVelocity_, velX, pan_.maxVelocity_);
        velocityY_ = qBound<qreal>(-pan_.maxVelocity_, velY, pan_.maxVelocity_);
    }
}

/*!
    \internal
*/
// simplify the gestures by using a state-machine format (easy to move to a future state machine)
void QDeclarativeGeoMapGestureArea::update()
{
    // First state machine is for the number of touch points
    touchPointStateMachine();

    // Parallel state machine for pinch
    if (isPinchActive() || (enabled_ && pinch_.enabled && (activeGestures_ & (ZoomGesture | TiltGesture | RotationGesture))))
        pinchStateMachine();

    // Parallel state machine for pan (since you can pan at the same time as pinching)
    // The stopPan function ensures that pan stops immediately when disabled,
    // but the line below allows pan continue its current gesture if you disable
    // the whole gesture (enabled_ flag), this keeps the enabled_ consistent with the pinch
    if (isPanActive() || (enabled_ && pan_.enabled_ && (activeGestures_ & (PanGesture | FlickGesture))))
        panStateMachine();
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::touchPointStateMachine()
{
    // Transitions:
    switch (touchPointState_) {
    case touchPoints0:
        if (touchPoints_.count() == 1) {
            clearTouchData();
            startOneTouchPoint();
            touchPointState_ = touchPoints1;
        } else if (touchPoints_.count() == 2) {
            clearTouchData();
            startTwoTouchPoints();
            touchPointState_ = touchPoints2;
        }
        break;
    case touchPoints1:
        if (touchPoints_.count() == 0) {
            touchPointState_ = touchPoints0;
        } else if (touchPoints_.count() == 2) {
            touchCenterCoord_ = map_->screenPositionToCoordinate(sceneCenter_, false);
            startTwoTouchPoints();
            touchPointState_ = touchPoints2;
        }
        break;
    case touchPoints2:
        if (touchPoints_.count() == 0) {
            touchPointState_ = touchPoints0;
        } else if (touchPoints_.count() == 1) {
            touchCenterCoord_ = map_->screenPositionToCoordinate(sceneCenter_, false);
            startOneTouchPoint();
            touchPointState_ = touchPoints1;
        }
        break;
    };

    // Update
    switch (touchPointState_) {
    case touchPoints0:
        break; // do nothing if no touch points down
    case touchPoints1:
        updateOneTouchPoint();
        break;
    case touchPoints2:
        updateTwoTouchPoints();
        break;
    }
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::startOneTouchPoint()
{
    sceneStartPoint1_ = touchPoints_.at(0).scenePos();
    lastPos_ = sceneStartPoint1_;
    lastPosTime_.start();
    QGeoCoordinate startCoord = map_->screenPositionToCoordinate(sceneStartPoint1_, false);
    // ensures a smooth transition for panning
    startCoord_.setLongitude(startCoord_.longitude() + startCoord.longitude() -
                             touchCenterCoord_.longitude());
    startCoord_.setLatitude(startCoord_.latitude() + startCoord.latitude() -
                            touchCenterCoord_.latitude());
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::updateOneTouchPoint()
{
    sceneCenter_ = touchPoints_.at(0).scenePos();
    updateVelocityList(sceneCenter_);
}


/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::startTwoTouchPoints()
{
    sceneStartPoint1_ = touchPoints_.at(0).scenePos();
    sceneStartPoint2_ = touchPoints_.at(1).scenePos();
    QPointF startPos = (sceneStartPoint1_ + sceneStartPoint2_) * 0.5;
    lastPos_ = startPos;
    lastPosTime_.start();
    QGeoCoordinate startCoord = map_->screenPositionToCoordinate(startPos, false);
    startCoord_.setLongitude(startCoord_.longitude() + startCoord.longitude() -
                             touchCenterCoord_.longitude());
    startCoord_.setLatitude(startCoord_.latitude() + startCoord.latitude() -
                            touchCenterCoord_.latitude());
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::updateTwoTouchPoints()
{
    QPointF p1 = touchPoints_.at(0).scenePos();
    QPointF p2 = touchPoints_.at(1).scenePos();
    qreal dx = p1.x() - p2.x();
    qreal dy = p1.y() - p2.y();
    distanceBetweenTouchPoints_ = sqrt(dx * dx + dy * dy);
    sceneCenter_ = (p1 + p2) / 2;

    updateVelocityList(sceneCenter_);

    twoTouchAngle_ = QLineF(p1, p2).angle();
    if (twoTouchAngle_ > 180)
        twoTouchAngle_ -= 360;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setPinchActive(bool active)
{
    if ((active && pinchState_ == pinchActive) || (!active && pinchState_ != pinchActive))
        return;
    pinchState_ = active ? pinchActive : pinchInactive;
    emit pinchActiveChanged();
}


/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::pinchStateMachine()
{
    PinchState lastState = pinchState_;
    // Transitions:
    switch (pinchState_) {
    case pinchInactive:
        if (canStartPinch()) {
            startPinch();
            setPinchActive(true);
        }
        break;
    case pinchActive:
        if (touchPoints_.count() <= 1) {
            endPinch();
            setPinchActive(false);
        }
        break;
    }
    // This line implements an exclusive state machine, where the transitions and updates don't
    // happen on the same frame
    if (pinchState_ != lastState)
         return;
    // Update
    switch (pinchState_) {
    case pinchInactive:
        break; // do nothing
    case pinchActive:
        updatePinch();
        break;
    }
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::canStartPinch()
{
    const int startDragDistance = qApp->styleHints()->startDragDistance();

    if (touchPoints_.count() >= 2) {
        QPointF p1 = touchPoints_.at(0).scenePos();
        QPointF p2 = touchPoints_.at(1).scenePos();
        if (qAbs(p1.x()-sceneStartPoint1_.x()) > startDragDistance
         || qAbs(p1.y()-sceneStartPoint1_.y()) > startDragDistance
         || qAbs(p2.x()-sceneStartPoint2_.x()) > startDragDistance
         || qAbs(p2.y()-sceneStartPoint2_.y()) > startDragDistance) {
            pinch_.event.setCenter(declarativeMap_->mapFromScene(sceneCenter_));
            pinch_.event.setAngle(twoTouchAngle_);
            pinch_.event.setPoint1(p1);
            pinch_.event.setPoint2(p2);
            pinch_.event.setPointCount(touchPoints_.count());
            pinch_.event.setAccepted(true);
            emit pinchStarted(&pinch_.event);
            return pinch_.event.accepted();
        }
    }
    return false;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::startPinch()
{
    pinch_.startDist = distanceBetweenTouchPoints_;
    pinch_.zoom.previous = 1.0;
    pinch_.tilt.previous = 0.0;
    pinch_.lastAngle = twoTouchAngle_;
    pinch_.rotation.angle = 0.0;

    pinch_.lastPoint1 = touchPoints_.at(0).scenePos();
    pinch_.lastPoint2 = touchPoints_.at(1).scenePos();

    pinch_.zoom.start = declarativeMap_->zoomLevel();
    pinch_.rotation.start = declarativeMap_->bearing();
    pinch_.tilt.start = declarativeMap_->tilt();
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::updatePinch()
{
    // Calculate the new zoom level if we have distance ( >= 2 touchpoints), otherwise stick with old.
    qreal newZoomLevel = pinch_.zoom.previous;
    if (distanceBetweenTouchPoints_) {
        newZoomLevel =
                // How much further/closer the current touchpoints are (in pixels) compared to pinch start
                ((distanceBetweenTouchPoints_ - pinch_.startDist)  *
                 //  How much one pixel corresponds in units of zoomlevel (and multiply by above delta)
                 (pinch_.zoom.maximumChange / ((declarativeMap_->width() + declarativeMap_->height()) / 2))) +
                // Add to starting zoom level. Sign of (dist-pinchstartdist) takes care of zoom in / out
                pinch_.zoom.start;
    }
    qreal da = pinch_.lastAngle - twoTouchAngle_;
    if (da > 180)
        da -= 360;
    else if (da < -180)
        da += 360;
    pinch_.rotation.angle -= da;
    pinch_.event.setCenter(declarativeMap_->mapFromScene(sceneCenter_));
    pinch_.event.setAngle(twoTouchAngle_);

    pinch_.lastPoint1 = touchPoints_.at(0).scenePos();
    pinch_.lastPoint2 = touchPoints_.at(1).scenePos();
    pinch_.event.setPoint1(pinch_.lastPoint1);
    pinch_.event.setPoint2(pinch_.lastPoint2);
    pinch_.event.setPointCount(touchPoints_.count());
    pinch_.event.setAccepted(true);

    pinch_.lastAngle = twoTouchAngle_;
    emit pinchUpdated(&pinch_.event);

    if (activeGestures_ & ZoomGesture) {
        // Take maximum and minimumzoomlevel into account
        qreal perPinchMinimumZoomLevel = qMax(pinch_.zoom.start - pinch_.zoom.maximumChange, pinch_.zoom.minimum);
        qreal perPinchMaximumZoomLevel = qMin(pinch_.zoom.start + pinch_.zoom.maximumChange, pinch_.zoom.maximum);
        newZoomLevel = qMin(qMax(perPinchMinimumZoomLevel, newZoomLevel), perPinchMaximumZoomLevel);
        declarativeMap_->setZoomLevel(newZoomLevel);
    }
    if (activeGestures_ & TiltGesture && pinch_.zoom.minimum >= 0 && pinch_.zoom.maximum >= 0) {
        // Note: tilt is not yet supported.
        qreal newTilt = pinch_.tilt.previous;
        if (distanceBetweenTouchPoints_) {
            newTilt =
                    // How much further/closer the current touchpoints are (in pixels) compared to pinch start
                    ((distanceBetweenTouchPoints_ - pinch_.startDist)  *
                     //  How much one pixel corresponds in units of tilt degrees (and multiply by above delta)
                     (pinch_.tilt.maximumChange / ((declarativeMap_->width() + declarativeMap_->height()) / 2))) +
                    // Add to starting tilt.
                    pinch_.tilt.start;
        }
        qreal perPinchMinimumTilt = qMax(pinch_.tilt.start - pinch_.tilt.maximumChange, pinch_.tilt.minimum);
        qreal perPinchMaximumTilt = qMin(pinch_.tilt.start + pinch_.tilt.maximumChange, pinch_.tilt.maximum);
        newTilt = qMin(qMax(perPinchMinimumTilt, newTilt), perPinchMaximumTilt);
        pinch_.tilt.previous = newTilt;
        declarativeMap_->setTilt(newTilt);
    }
    if (activeGestures_ & RotationGesture) {
        bool unlimitedRotation = (pinch_.rotation.minimum == 0.0 && pinch_.rotation.maximum == 0.0);
        if ((pinch_.rotation.start >= pinch_.rotation.minimum && pinch_.rotation.start <= pinch_.rotation.maximum) || unlimitedRotation)  {
            qreal r = pinch_.rotation.angle * pinch_.rotation.factor + pinch_.rotation.start;
            if (!unlimitedRotation)
                r = qMin(qMax(pinch_.rotation.minimum,r), pinch_.rotation.maximum);
            if (r > 360.0)
                r -= 360;
            if (r < -360.0)
                r += 360.0;
            declarativeMap_->setBearing(r);
        }
    }
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::endPinch()
{
    QPointF pinchCenter = declarativeMap_->mapFromScene(sceneCenter_);
    pinch_.event.setCenter(pinchCenter);
    pinch_.event.setAngle(pinch_.lastAngle);
    pinch_.event.setPoint1(declarativeMap_->mapFromScene(pinch_.lastPoint1));
    pinch_.event.setPoint2(declarativeMap_->mapFromScene(pinch_.lastPoint2));
    pinch_.event.setAccepted(true);
    pinch_.event.setPointCount(0);
    emit pinchFinished(&pinch_.event);
    pinch_.startDist = 0;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::panStateMachine()
{
    PanState lastState = panState_;

    // Transitions
    switch (panState_) {
    case panInactive:
        if (canStartPan())
            panState_ = panActive;
        break;
    case panActive:
        if (touchPoints_.count() == 0) {
            panState_ = panFlick;
            if (!tryStartFlick())
            {
                panState_ = panInactive;
                // mark as inactive for use by camera
                if (pinchState_ == pinchInactive)
                    emit movementStopped();
            }
        }
        break;
    case panFlick:
        if (touchPoints_.count() > 0) { // re touched before movement ended
            endFlick();
            panState_ = panActive;
        }
        break;
    }
    // Update
    switch (panState_) {
    case panInactive: // do nothing
        break;
    case panActive:
        updatePan();
        // this ensures 'panStarted' occurs after the pan has actually started
        if (lastState != panActive)
            emit panStarted();
        break;
    case panFlick:
        break;
    }
}
/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::canStartPan()
{
    if (touchPoints_.count() == 0 || (activeGestures_ & PanGesture) == 0)
        return false;

    // Check if thresholds for normal panning are met.
    // (normal panning vs flicking: flicking will start from mouse release event).
    const int startDragDistance = qApp->styleHints()->startDragDistance();
    QPointF p1 = touchPoints_.at(0).scenePos();
    int dyFromPress = int(p1.y() - sceneStartPoint1_.y());
    int dxFromPress = int(p1.x() - sceneStartPoint1_.x());
    if ((qAbs(dyFromPress) > startDragDistance || qAbs(dxFromPress) > startDragDistance
         || pressTime_.elapsed() > 200))
        return true;
    return false;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::updatePan()
{
    QPointF startPoint = map_->coordinateToScreenPosition(startCoord_, false);
    int dx = static_cast<int>(sceneCenter_.x() - startPoint.x());
    int dy = static_cast<int>(sceneCenter_.y() - startPoint.y());
    QPointF mapCenterPoint;
    mapCenterPoint.setY(map_->height() / 2.0  - dy);
    mapCenterPoint.setX(map_->width() / 2.0 - dx);
    AnimatableCoordinate animationStartCoordinate;
    animationStartCoordinate.setCoordinate(map_->screenPositionToCoordinate(mapCenterPoint, false));
    map_->mapController()->setCenter(animationStartCoordinate);

}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::tryStartFlick()
{
    if ((activeGestures_ & FlickGesture) == 0)
        return false;
    // if we drag then pause before release we should not cause a flick.
    qreal velocityX = 0.0;
    qreal velocityY = 0.0;
    if (lastPosTime_.elapsed() < QML_MAP_FLICK_VELOCITY_SAMPLE_PERIOD) {
        velocityY = velocityY_;
        velocityX = velocityX_;
    }
    int flickTimeY = 0;
    int flickTimeX = 0;
    int flickPixelsX = 0;
    int flickPixelsY = 0;
    if (qAbs(velocityY) > MinimumFlickVelocity && qAbs(sceneCenter_.y() - sceneStartPoint1_.y()) > FlickThreshold) {
        // calculate Y flick animation values
        qreal acceleration = pan_.deceleration_;
        if ((velocityY > 0.0f) == (pan_.deceleration_ > 0.0f))
            acceleration = acceleration * -1.0f;
        flickTimeY = static_cast<int>(-1000 * velocityY / acceleration);
        flickPixelsY = (flickTimeY * velocityY) / (1000.0 * 2);
    }
    if (qAbs(velocityX) > MinimumFlickVelocity && qAbs(sceneCenter_.x() - sceneStartPoint1_.x()) > FlickThreshold) {
        // calculate X flick animation values
        qreal acceleration = pan_.deceleration_;
        if ((velocityX > 0.0f) == (pan_.deceleration_ > 0.0f))
            acceleration = acceleration * -1.0f;
        flickTimeX = static_cast<int>(-1000 * velocityX / acceleration);
        flickPixelsX = (flickTimeX * velocityX) / (1000.0 * 2);
    }
    int flickTime = qMax(flickTimeY, flickTimeX);
    if (flickTime > 0) {
        startFlick(flickPixelsX, flickPixelsY, flickTime);
        return true;
    }
    return false;
}

/*!
    \internal
*/
// FIXME:
// - not left right / up down flicking, so if map is rotated, will act unintuitively
void QDeclarativeGeoMapGestureArea::startFlick(int dx, int dy, int timeMs)
{
    if (timeMs < 0)
        return;
    AnimatableCoordinate animationStartCoordinate = map_->mapController()->center();
    QGeoCoordinate coordinate = animationStartCoordinate.coordinate();

    if (pan_.animation_->state() == QPropertyAnimation::Running)
        pan_.animation_->stop();
    AnimatableCoordinate animationEndCoordinate = map_->mapController()->center();
    pan_.animation_->setDuration(timeMs);
    coordinate.setLongitude(coordinate.longitude() - (dx / pow(2.0, map_->mapController()->zoom())));
    coordinate.setLatitude(coordinate.latitude() + (dy / pow(2.0, map_->mapController()->zoom())));
    animationEndCoordinate.setCoordinate(coordinate);
    pan_.animation_->setStartValue(QVariant::fromValue(animationStartCoordinate));
    pan_.animation_->setEndValue(QVariant::fromValue(animationEndCoordinate));
    pan_.animation_->start();
    emit flickStarted();
}

void QDeclarativeGeoMapGestureArea::stopPan()
{
    velocityX_ = 0;
    velocityY_ = 0;
    if (panState_ == panFlick) {
        endFlick();
    } else if (panState_ == panActive) {
        emit panFinished();
        emit movementStopped();
    }
    panState_ = panInactive;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::endFlick()
{
    emit panFinished();
    if (pan_.animation_->state() == QPropertyAnimation::Running)
        pan_.animation_->stop();
    emit flickFinished();
    panState_ = panInactive;
    emit movementStopped();
}






#include "moc_qdeclarativegeomapgesturearea_p.cpp"

QT_END_NAMESPACE
