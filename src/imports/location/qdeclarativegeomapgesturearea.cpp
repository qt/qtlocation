
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
#include "qdeclarativegeomapgesturearea_p.h"
#include "qdeclarativegeomap_p.h"
#include "qdeclarativecoordinate_p.h"
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
// The number of samples to use in calculating the velocity of a flick
#define QML_MAP_FLICK_SAMPLEBUFFER 3
// The number of samples to discard when calculating the flick velocity.
// Touch panels often produce inaccurate results as the finger is lifted.
#define QML_MAP_FLICK_DISCARDSAMPLES 1

// FlickThreshold determines how far the "mouse" must have moved
// before we perform a flick.
static const int FlickThreshold = 20;
// Really slow flicks can be annoying.
const qreal MinimumFlickVelocity = 75.0;

QT_BEGIN_NAMESPACE


/*!
    \qmlclass MapPinchEvent
    \inqmlmodule QtLocation 5

    \brief MapPinchEvent element provides basic information about pinch event.

    MapPinchEvent element provides basic information about pinch event. They are
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
    The MapPinch will not react until two touch points have initiated a gesture,
    but will remain active until all touch points have been released.
*/

/*!
    \qmlproperty bool QtLocation5::MapPinchEvent::accepted

    Setting this property to false in the \c MapPinch::onPinchStarted handler
    will result in no further pinch events being generated, and the gesture
    ignored.
*/

/*!
    \qmlclass MapGestureArea QDeclarativeGeoMapGestureArea

    \inqmlmodule QtLocation 5

    \brief The MapGestureArea element provides Map gesture interaction.

    MapGestureArea elements are used as part of a Map, to provide for panning,
    flicking and pinch-to-zoom gesture used on touch displays.

    A MapGestureArea is automatically created with a new Map and available with
    the \l{QtLocation5::Map::pinch}{pinch} property. This is the only way
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
    \qmlproperty bool QtLocation5::MapGestureArea::enabled

    This property holds whether the gestures are enabled.
    Note: disabling gestures during an active gesture does not have effect on
    the potentially active current gesture.
*/


/*!
    \qmlproperty bool QtLocation5::MapGestureArea::panEnabled

    This property holds whether the pan gestures are enabled.
    Note: disabling gestures during an active gesture does not have effect on
    the potentially active current gesture.
*/

/*!
    \qmlproperty bool QtLocation5::MapGestureArea::pinchEnabled

    This property holds whether the pinch gestures are enabled.
    Note: disabling gestures during an active gesture does not have effect on
    the potentially active current gesture.
*/

/*!
    \qmlproperty bool QtLocation5::MapGestureArea::isPinchActive

    This read-only property holds whether any pinch gesture is active.
*/

/*!
    \qmlproperty bool QtLocation5::MapGestureArea::isPanActive

    This read-only property holds whether any pan gesture (panning or flicking) is active.
*/

/*!
    \qmlproperty enumeration QtLocation5::MapGestureArea::activeGestures

    This property holds the gestures that will be active. By default
    the zoom, pan and flick gestures are enabled.

    \list
    \li GestureArea.NoGesture - Don't support any additional gestures (value: 0x0000).
    \li GestureArea.ZoomGesture - Support the map zoom gesture (value: 0x0001).
    \li GestureArea.RotationGesture - Support the map rotation gesture (value: 0x0002).
    \li GestureArea.TiltGesture - Support the map tilt gesture (value: 0x0004).
    \li GestureArea.PanGesture  - Support the map pan gesture (value: 0x0008).
    \li GestureArea.FlickGesture  - Support the map flick gesture (value: 0x0010).
    \endlist

    For the extremist, one may OR flag the RotationGesture or TiltGesture
    but these come with absolutely no warranty or guarantees at the moment
    (may be removed, changed, moved around)

    \note For the time being, only \l GestureArea.ZoomGesture is supported.
*/

/*!
    \qmlproperty real QtLocation5::MapGestureArea::maximumZoomLevelChange

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
    \qmlsignal void QtLocation5::MapGestureArea::pinchStarted(PinchEvent event)

    Raised when a pinch gesture is started.

    \sa pinchUpdated pinchFinished
*/

/*!
    \qmlsignal void QtLocation5::MapGestureArea::pinchUpdated(PinchEvent event)

    Once a pinch has begun this event gets raised as the user moves her fingers
    across the map.

    \sa pinchStarted pinchFinished
*/

/*!
    \qmlsignal void QtLocation5::MapGestureArea::pinchUpdated(PinchEvent event)

    The end of a pinch gesture is signaled by this event.

    \sa pinchUpdated pinchFinished
*/

/*!
    \qmlsignal QtLocation5::MapFlickable::panStarted()

    This handler is called when the view begins moving due to user
    interaction. Typically this means that the user is dragging a finger -
    or a mouse with one of more mouse buttons pressed - on the map.
*/

/*!
    \qmlsignal QtLocation5::MapFlickable::panFinished()

    This handler is called when the view stops moving due to user
    interaction.  If a flick was generated, this handler will
    be triggered once the flick stops.  If a flick was not
    generated, the handler will be triggered when the
    user stops dragging - that is a mouse or touch release.

*/

/*!
    \qmlsignal QtLocation5::MapFlickable::flickStarted()

    This handler is called when the view is flicked.  A flick
    starts from the point that the mouse or touch is released,
    while still in motion.
*/

/*!
    \qmlsignal QtLocation5::MapFlickable::flickFinished()

    This handler is called when the view stops moving due to a flick.
    The order of panFinished() and flickFinished() is not specified.
*/


QDeclarativeGeoMapGestureArea::QDeclarativeGeoMapGestureArea(QDeclarativeGeoMap* map, QObject *parent)
    : QObject(parent),
      declarativeMap_(map),
      enabled_(true),
      pinchEnabled_(true),
      minimumZoomLevel_(-1.0),
      maximumZoomLevel_(-1.0),
      minimumRotation_(0.0),
      maximumRotation_(0.0),
      pinchStartDist_(0),
      pinchStartZoomLevel_(0.0),
      pinchLastZoomLevel_(0.0),
      pinchStartRotation_(0.0),
      pinchLastAngle_(0.0),
      pinchRotation_(0.0),
      maximumZoomLevelChange_(2.0),
      rotationFactor_(1.0),
      minimumTilt_(0.0),
      maximumTilt_(90.0),
      maximumTiltChange_(20.0),
      pinchLastTilt_(0.0),
      pinchStartTilt_(0.0),
      activeGestures_(ZoomGesture | PanGesture | FlickGesture)
{
    map_ = 0;
    pan_.enabled_ = true,
    pan_.maxVelocity_ = QML_MAP_FLICK_DEFAULTMAXVELOCITY;
    pan_.deceleration_ = QML_MAP_FLICK_DEFAULTDECELERATION;
    pan_.animation_ = 0;
#if defined(TOUCH_EVENT_WORKAROUND)
    mouseBeingUsed_ = true;
#endif
    touchPointState_ = touchPoints0;
    pinchState_ = pinchInactive;
    panState_ = panInactive;
}
/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMap(QGeoMap* map)
{
    if (map_ || !map)
        return;
    map_ = map;
    pan_.animation_ = new QPropertyAnimation(map_->mapController(), "center", this);
    pan_.animation_->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    connect(pan_.animation_, SIGNAL(finished()), this, SLOT(endFlick()));
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
        qmlInfo(this) << tr("Pinchrotation gesture activated. Note that it is experimental feature.");
    if (activeGestures_ & TiltGesture)
        qmlInfo(this) << tr("Pinchtilt gesture activated. Note that it is experimental feature.");
    emit activeGesturesChanged();
    emit pinchDep_->activeGesturesChanged();
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
    return pinchEnabled_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setPinchEnabled(bool enabled)
{
    if (enabled == pinchEnabled_)
        return;
    pinchEnabled_ = enabled;
    emit pinchEnabledChanged();
    emit pinchDep_->enabledChanged();
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
    emit flickableDep_->enabledChanged();

    // unlike the pinch, the pan existing functionality is to stop immediately
    if (!enabled)
        stopPan();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::minimumZoomLevel() const
{
    return minimumZoomLevel_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMinimumZoomLevel(qreal zoomLevel)
{
    if (zoomLevel == minimumZoomLevel_ ||
            zoomLevel < declarativeMap_->minimumZoomLevel() ||
            (maximumZoomLevel_ != -1.0 && zoomLevel > maximumZoomLevel_) )
        return;
    minimumZoomLevel_ = zoomLevel;
    emit minimumZoomLevelChanged();
    emit pinchDep_->minimumZoomLevelChanged();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::maximumZoomLevel() const
{
    return maximumZoomLevel_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMaximumZoomLevel(qreal zoomLevel)
{
    if (zoomLevel == maximumZoomLevel_ ||
            zoomLevel > declarativeMap_->maximumZoomLevel() ||
            (minimumZoomLevel_ != - 1.0 && zoomLevel < minimumZoomLevel_))
        return;
    maximumZoomLevel_ = zoomLevel;
    emit maximumZoomLevelChanged();
    emit pinchDep_->maximumZoomLevelChanged();
}

/*!
    \internal
    called internally when plugin's limits change. somewhat dodgy but
    initialization order complicates the zoom limit settings a bit (for example when is
    it possible to check against mapping plugins' limits)
*/
void QDeclarativeGeoMapGestureArea::zoomLevelLimits(qreal min, qreal max)
{
    if (minimumZoomLevel_ == -1.0 || min > minimumZoomLevel_)
        setMinimumZoomLevel(min);
    if (maximumZoomLevel_ == -1.0 || max < maximumZoomLevel_)
        setMaximumZoomLevel(max);
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::maximumZoomLevelChange() const
{
    return maximumZoomLevelChange_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMaximumZoomLevelChange(qreal maxChange)
{
    if (maxChange == maximumZoomLevelChange_ || maxChange < 0.1 || maxChange > 10.0)
        return;
    maximumZoomLevelChange_ = maxChange;
    emit maximumZoomLevelChangeChanged();
    emit pinchDep_->maximumZoomLevelChangeChanged();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::minimumRotation() const
{
    return minimumRotation_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMinimumRotation(qreal rotation)
{
    if (rotation == minimumRotation_ ||
            rotation < 0 ||
            rotation > maximumRotation_)
        return;
    minimumRotation_ = rotation;
    emit minimumRotationChanged();
    emit pinchDep_->minimumRotationChanged();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::maximumRotation() const
{
    return maximumRotation_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMaximumRotation(qreal rotation)
{
    if (rotation == maximumRotation_ ||
            rotation > 360 ||
            rotation < minimumRotation_)
        return;
    maximumRotation_ = rotation;
    emit maximumRotationChanged();
    emit pinchDep_->maximumRotationChanged();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::rotationFactor() const
{
    return rotationFactor_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setRotationFactor(qreal factor)
{
    if (rotationFactor_ == factor ||
            factor < 0 ||
            factor > 5)
        return;
    rotationFactor_ = factor;
    emit rotationFactorChanged();
    emit pinchDep_->rotationFactorChanged();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::maximumTilt() const
{
    return maximumTilt_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMaximumTilt(qreal tilt)
{
    if (maximumTilt_ == tilt)
        return;
    maximumTilt_ = tilt;
    emit maximumTiltChanged();
    emit pinchDep_->maximumTiltChanged();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::minimumTilt() const
{
    return minimumTilt_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMinimumTilt(qreal tilt)
{
    if (minimumTilt_ == tilt || tilt < 0.1)
        return;
    minimumTilt_ = tilt;
    emit minimumTiltChanged();
    emit pinchDep_->minimumTiltChanged();
}

/*!
    \internal
*/
qreal QDeclarativeGeoMapGestureArea::maximumTiltChange() const
{
    return maximumTiltChange_;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::setMaximumTiltChange(qreal tilt)
{
    if (maximumTiltChange_ == tilt || tilt < 0.1)
        return;
    maximumTiltChange_ = tilt;
    emit maximumTiltChangeChanged();
    emit pinchDep_->maximumTiltChangeChanged();
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
    emit flickableDep_->decelerationChanged();
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
#if defined(TOUCH_EVENT_WORKAROUND)
    if (!mouseBeingUsed_)
        return true;
#endif
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
#if defined(TOUCH_EVENT_WORKAROUND)
    if (!mouseBeingUsed_)
        return true;
#endif
    touchPoints_.clear();

    touchPoints_ << makeTouchPointFromMouseEvent(event, Qt::TouchPointMoved);
    update();
    return true;
}


/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::mouseReleaseEvent(QMouseEvent */*event*/)
{
#if defined(TOUCH_EVENT_WORKAROUND)
    if (!mouseBeingUsed_)
        return true;
#endif
    touchPoints_.clear();
    update();
    return true;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::touchEvent(QTouchEvent *event)
{
#if defined(TOUCH_EVENT_WORKAROUND)
    mouseBeingUsed_ = false;
#endif
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
void QDeclarativeGeoMapGestureArea::clearTouchData()
{
    velocityBufferX_.clear();
    velocityBufferY_.clear();
    pressTime_.start();
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
    // Take velocity samples, used later to determine the flick
    // duration and speed (when mouse is released).
    if (!lastPos_.isNull()) {
        int dyFromLastPos = pos.y() - lastPos_.y();
        int dxFromLastPos = pos.x() - lastPos_.x();

        qreal elapsed = qreal(lastPosTime_.elapsed()) / 1000.;
        if (elapsed <= 0) {
            return;
        }
        lastPosTime_.restart();
        addVelocitySample(velocityBufferY_, double(dyFromLastPos)/elapsed);
        addVelocitySample(velocityBufferX_, double(dxFromLastPos)/elapsed);
    }
}


/*!
    \internal
    Adds velocity sample to sample buffer. Data is later used to calculate
    flick speed. By default 3 latest samples are considered.
*/
void QDeclarativeGeoMapGestureArea::addVelocitySample(QVector<qreal>& buffer, qreal sample)
{
    if (sample > pan_.maxVelocity_)
        sample = pan_.maxVelocity_;
    else if (sample < -pan_.maxVelocity_)
        sample = -pan_.maxVelocity_;
    buffer.append(sample);
    if (buffer.count() > QML_MAP_FLICK_SAMPLEBUFFER)
        buffer.remove(0);
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::updateVelocity(QVector<qreal>& buffer, qreal& velocity)
{
    if (buffer.count() > QML_MAP_FLICK_DISCARDSAMPLES) {
        velocity = 0;
        int count = buffer.count() - QML_MAP_FLICK_DISCARDSAMPLES;
        for (int i = 0; i < count; ++i) {
            qreal v = buffer.at(i);
            velocity += v;
        }
        velocity /= count;
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
    if (isPinchActive() || (enabled_ && pinchEnabled_ && (activeGestures_ & (ZoomGesture | TiltGesture | RotationGesture))))
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
    switch (touchPointState_)
    {
    case touchPoints0:{
        if (touchPoints_.count()==1){
            clearTouchData();
            startOneTouchPoint();
            touchPointState_ = touchPoints1;
        }
        else if (touchPoints_.count()==2){
            clearTouchData();
            startTwoTouchPoints();
            touchPointState_ = touchPoints2;
        }
        break;
    }
    case touchPoints1:{
        if (touchPoints_.count()==0){
            touchPointState_ = touchPoints0;
        }
        else if (touchPoints_.count()==2){
            startTwoTouchPoints();
            touchPointState_ = touchPoints2;
        }
        break;
    }
    case touchPoints2:{
        if (touchPoints_.count()==0){
            touchPointState_ = touchPoints0;
        }
        else if (touchPoints_.count()==1){
            startOneTouchPoint();
            touchPointState_ = touchPoints1;
        }
        break;
    }
    };

    // Update
    switch (touchPointState_)
    {
    case touchPoints0:{
        break; // do nothing if no touch points down
    }
    case touchPoints1:{
        updateOneTouchPoint();
        break;
    }
    case touchPoints2:{
        updateTwoTouchPoints();
        break;
    }
    }
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::startOneTouchPoint()
{
    sceneCenter_ = QPointF();
    lastPosTime_.start();

    sceneStartPoint1_ = touchPoints_.at(0).scenePos();
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
    lastPos_ = sceneCenter_;
    sceneCenter_ = touchPoints_.at(0).scenePos();
    touchCenterCoord_ = map_->screenPositionToCoordinate(sceneCenter_, false);

    updateVelocityList(sceneCenter_);
}


/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::startTwoTouchPoints()
{
    sceneCenter_ = QPointF();
    lastPosTime_.start();

    sceneStartPoint1_ = touchPoints_.at(0).scenePos();
    sceneStartPoint2_ = touchPoints_.at(1).scenePos();
    QPointF startPos = (sceneStartPoint1_ + sceneStartPoint2_) * 0.5;
    QGeoCoordinate startCoord =  map_->screenPositionToCoordinate(startPos, false);
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
    distanceBetweenTouchPoints_ = sqrt(dx*dx + dy*dy);
    lastPos_ = sceneCenter_;
    sceneCenter_ = (p1 + p2)/2;
    touchCenterCoord_ = map_->screenPositionToCoordinate(sceneCenter_, false);

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
    if ((active && pinchState_==pinchActive) || (!active && pinchState_!=pinchActive))
        return;
    pinchState_ = active ? pinchActive : pinchInactive;
    emit pinchDep_->activeChanged();
    emit pinchActiveChanged();
}


/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::pinchStateMachine()
{
    PinchState lastState = pinchState_;
    // Transitions:
    switch (pinchState_)
    {
    case pinchInactive:{
        if (canStartPinch()){
            startPinch();
            setPinchActive(true);
        }
        break;
    }
    case pinchActive:{
        if (touchPoints_.count() <= 1){
            endPinch();
            setPinchActive(false);
        }
        break;
    }
    }
    // This line implements an exclusive state machine, where the transitions and updates don't
    // happen on the same frame
    if (pinchState_ != lastState)
         return;
    // Update
    switch (pinchState_)
    {
    case pinchInactive:{
        break; // do nothing
    }
    case pinchActive:{
        updatePinch();
        break;
    }
    }
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::canStartPinch()
{
    const int startDragDistance = qApp->styleHints()->startDragDistance();

    if (touchPoints_.count() >= 2){
        QPointF p1 = touchPoints_.at(0).scenePos();
        QPointF p2 = touchPoints_.at(1).scenePos();
        if (qAbs(p1.x()-sceneStartPoint1_.x()) > startDragDistance
         || qAbs(p1.y()-sceneStartPoint1_.y()) > startDragDistance
         || qAbs(p2.x()-sceneStartPoint2_.x()) > startDragDistance
         || qAbs(p2.y()-sceneStartPoint2_.y()) > startDragDistance) {
            pinchEvent_.setCenter(declarativeMap_->mapFromScene(sceneCenter_));
            pinchEvent_.setAngle(twoTouchAngle_);
            pinchEvent_.setPoint1(p1);
            pinchEvent_.setPoint2(p2);
            pinchEvent_.setPointCount(touchPoints_.count());
            pinchEvent_.setAccepted(true);
            emit pinchStarted(&pinchEvent_);
            emit pinchDep_->pinchStarted(&pinchEvent_);
            return pinchEvent_.accepted();
        }
    }
    return false;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::startPinch()
{
    pinchStartDist_ = distanceBetweenTouchPoints_;
    pinchLastZoomLevel_ = 1.0;
    pinchLastTilt_ = 0.0;
    pinchLastAngle_ = twoTouchAngle_;
    pinchRotation_ = 0.0;

    lastPoint1_ = touchPoints_.at(0).scenePos();
    lastPoint2_ = touchPoints_.at(1).scenePos();

    pinchStartZoomLevel_ = declarativeMap_->zoomLevel();
    pinchStartRotation_ = declarativeMap_->bearing();
    pinchStartTilt_ = declarativeMap_->tilt();
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::updatePinch()
{
    // Calculate the new zoom level if we have distance ( >= 2 touchpoints), otherwise stick with old.
    qreal newZoomLevel = pinchLastZoomLevel_;
    if (distanceBetweenTouchPoints_) {
        newZoomLevel =
                // How much further/closer the current touchpoints are (in pixels) compared to pinch start
                ((distanceBetweenTouchPoints_ - pinchStartDist_)  *
                 //  How much one pixel corresponds in units of zoomlevel (and multiply by above delta)
                 (maximumZoomLevelChange_ / ((declarativeMap_->width() + declarativeMap_->height()) / 2))) +
                // Add to starting zoom level. Sign of (dist-pinchstartdist) takes care of zoom in / out
                pinchStartZoomLevel_;
    }
    qreal da = pinchLastAngle_ - twoTouchAngle_;
    if (da > 180)
        da -= 360;
    else if (da < -180)
        da += 360;
    pinchRotation_ -= da;
    pinchEvent_.setCenter(declarativeMap_->mapFromScene(sceneCenter_));
    pinchEvent_.setAngle(twoTouchAngle_);

    lastPoint1_ = touchPoints_.at(0).scenePos();
    lastPoint2_ = touchPoints_.at(1).scenePos();
    pinchEvent_.setPoint1(lastPoint1_);
    pinchEvent_.setPoint2(lastPoint2_);
    pinchEvent_.setPointCount(touchPoints_.count());
    pinchEvent_.setAccepted(true);

    pinchLastAngle_ = twoTouchAngle_;
    emit pinchUpdated(&pinchEvent_);
    emit pinchDep_->pinchUpdated(&pinchEvent_);

    if (activeGestures_ & ZoomGesture) {
        // Take maximum and minimumzoomlevel into account
        qreal perPinchMinimumZoomLevel = qMax(pinchStartZoomLevel_ - maximumZoomLevelChange_, minimumZoomLevel_);
        qreal perPinchMaximumZoomLevel = qMin(pinchStartZoomLevel_ + maximumZoomLevelChange_, maximumZoomLevel_);
        newZoomLevel = qMin(qMax(perPinchMinimumZoomLevel, newZoomLevel), perPinchMaximumZoomLevel);
        declarativeMap_->setZoomLevel(newZoomLevel);
    }
    if (activeGestures_ & TiltGesture && minimumZoomLevel_ >= 0 && maximumZoomLevel_ >= 0) {
        // Note: tilt is not yet supported.
        qreal newTilt = pinchLastTilt_;
        if (distanceBetweenTouchPoints_) {
            newTilt =
                    // How much further/closer the current touchpoints are (in pixels) compared to pinch start
                    ((distanceBetweenTouchPoints_ - pinchStartDist_)  *
                     //  How much one pixel corresponds in units of tilt degrees (and multiply by above delta)
                     (maximumTiltChange_ / ((declarativeMap_->width() + declarativeMap_->height()) / 2))) +
                    // Add to starting tilt.
                    pinchStartTilt_;
        }
        qreal perPinchMinimumTilt = qMax(pinchStartTilt_ - maximumTiltChange_, minimumTilt_);
        qreal perPinchMaximumTilt = qMin(pinchStartTilt_ + maximumTiltChange_, maximumTilt_);
        newTilt = qMin(qMax(perPinchMinimumTilt, newTilt), perPinchMaximumTilt);
        pinchLastTilt_ = newTilt;
        declarativeMap_->setTilt(newTilt);
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
    pinchEvent_.setCenter(pinchCenter);
    pinchEvent_.setAngle(pinchLastAngle_);
    pinchEvent_.setPoint1(declarativeMap_->mapFromScene(lastPoint1_));
    pinchEvent_.setPoint2(declarativeMap_->mapFromScene(lastPoint2_));
    pinchEvent_.setAccepted(true);
    pinchEvent_.setPointCount(0);
    emit pinchFinished(&pinchEvent_);
    emit pinchDep_->pinchFinished(&pinchEvent_);
    pinchStartDist_ = 0;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::panStateMachine()
{
    PanState lastState = panState_;

    // Transitions
    switch (panState_)
    {
    case panInactive:{
        if (canStartPan())
            panState_ = panActive;
        break;
    }
    case panActive:{
        if (touchPoints_.count()==0){
            panState_ = panFlick;
            if (!tryStartFlick())
                panState_ = panInactive;
        }
        break;
    }
    case panFlick:{
        if (touchPoints_.count()>0){ // re touched before movement ended
            endFlick();
            panState_ = panActive;
        }
        break;
    }
    }
    // Update
    switch (panState_)
    {
    case panInactive:{ // do nothing
        break;
    }
    case panActive:{
        updatePan();
        // this ensures 'panStarted' occurs after the pan has actually started
        if (lastState != panActive){
            emit panStarted();
            emit flickableDep_->movementStarted();
        }
        break;
    }
    case panFlick:{
        break;
    }
    }
}
/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::canStartPan()
{
    if (touchPoints_.count() == 0 || (activeGestures_ & PanGesture)==0)
        return false;

    // Check if thresholds for normal panning are met.
    // (normal panning vs flicking: flicking will start from mouse release event).
    const int startDragDistance = qApp->styleHints()->startDragDistance();
    QPointF p1 = touchPoints_.at(0).scenePos();
    int dyFromPress = int(p1.y() - sceneStartPoint1_.y());
    int dxFromPress = int(p1.x() - sceneStartPoint1_.x());
    if ((qAbs(dyFromPress) > startDragDistance
      || qAbs(dxFromPress) > startDragDistance
            || pressTime_.elapsed() > 200) && !lastPos_.isNull())
        return true;
    return false;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::updatePan()
{
    QDeclarativeCoordinate *center = declarativeMap_->center();
    qreal newLat = center->latitude() + startCoord_.latitude() - touchCenterCoord_.latitude();
    qreal newLong = center->longitude() + startCoord_.longitude() - touchCenterCoord_.longitude();
    center->setLatitude(newLat);
    center->setLongitude(newLong);
}

/*!
    \internal
*/
bool QDeclarativeGeoMapGestureArea::tryStartFlick()
{
    if ((activeGestures_ & FlickGesture)==0)
        return false;
    // if we drag then pause before release we should not cause a flick.
    qreal velocityX = 0.0;
    qreal velocityY = 0.0;
    if (lastPosTime_.elapsed() < 100) {
        updateVelocity(velocityBufferY_, velocityY);
        updateVelocity(velocityBufferX_, velocityX);
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
    if (flickTime > 0){
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
    emit flickableDep_->flickStarted();
}

void QDeclarativeGeoMapGestureArea::stopPan()
{
    velocityBufferX_.clear();
    velocityBufferY_.clear();
    if (panState_ == panFlick)
        endFlick();
    else if (panState_ == panActive){
        emit panFinished();
        flickableDep_->movementEnded();
    }
    panState_ = panInactive;
}

/*!
    \internal
*/
void QDeclarativeGeoMapGestureArea::endFlick()
{
    emit panFinished();
    emit flickableDep_->movementEnded();
    if (pan_.animation_->state() == QPropertyAnimation::Running)
        pan_.animation_->stop();
    emit flickFinished();
    emit flickableDep_->flickEnded();
    panState_ = panInactive;
}






#include "moc_qdeclarativegeomapgesturearea_p.cpp"

QT_END_NAMESPACE
