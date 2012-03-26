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
#include <QtGui/QStyleHints>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include "qdeclarativegeomapflickable_p.h"
#include "qgeomapcontroller_p.h"
#include <QTimer>
#include "qgeomap_p.h"
#include "math.h"

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
// RetainGrabVelocity is the maxmimum instantaneous velocity that
// will ensure the Flickable retains the grab on consecutive flicks.
static const int RetainGrabVelocity = 15;
// Really slow flicks can be annoying.
const qreal MinimumFlickVelocity = 75.0;

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapFlickable QDeclarativeGeoMapFlickable

    \inqmlmodule QtLocation 5

    \brief The MapFlickable element provides basic flicking of the Map.

    MapFlickable elements are used as part of a Map, to provide for the common
    flick gesture that pans the map in an intuitive fashion. This is comparable
    to the activity of the \l Flickable element.

    A MapFlickable is automatically created with a new Map and available with
    the \l{QtLocation5::Map::flick}{flick} property. This is the only way
    to create a MapFlickable, and once created this way cannot be destroyed
    without its parent Map.

    The two most commonly used properties of the MapFlickable are \l enabled and
    \l deceleration. The former must be set in order for the MapFlickable to
    have any effect upon its parent Map. The latter property customizes the
    response of the MapFlickable to input from the user, by adjusting how much
    "friction" slows down the flick after the user has raised their finger
    or released the mouse button.

    \section2 Performance

    The MapFlickable, when enabled, must process all incoming touch or cursor
    movement events in order to build up the "flick" gesture, and then complete
    an animation for the "deceleration" phase. The MapFlickable builds up a
    buffer of samples during the motion capture phase: currently a maximum of
    3 past samples are processed at each new event. As a result, the overhead
    on movement events can be considered constant time, as there are many more
    than 3 events in a typical gesture. The cost of animation is identical to
    two parallel double float \l{QPropertyAnimation}{QPropertyAnimations} with
    OutQuad easing curves.

    \section2 Example Usage

    The following example enables the flick gesture on a map, and sets the
    deceleration/friction value to be double its standard level.

    \code
    Map {
        flick.enabled: true
        flick.deceleration: 5000
    }
    \endcode

    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0
*/

/*!
    \qmlproperty bool QtLocation5::MapFlickable::enabled

    This property holds whether the flicking is enabled.
    By default flicking is enabled. Disabling flicking will terminate
    any potentially active flicks (movement or actual flick).
*/

/*!
    \qmlproperty real QtLocation5::MapFlickable::deceleration

    This property holds the rate at which a flick will decelerate. The
    larger the value, the faster the flick decelerates.
    Default value is 2500, minimum value is 500 and maximum value is 10000.
*/

/*!
    \qmlsignal QtLocation5::MapFlickable::onMovementStarted()

    This handler is called when the view begins moving due to user
    interaction. Typically this means that the user is dragging a finger -
    or a mouse with one of more mouse buttons pressed - on the map.
*/

/*!
    \qmlsignal QtLocation5::MapFlickable::onMovementEnded()

    This handler is called when the view stops moving due to user
    interaction.  If a flick was generated, this handler will
    be triggered once the flick stops.  If a flick was not
    generated, the handler will be triggered when the
    user stops dragging - that is a mouse or touch release.

*/

/*!
    \qmlsignal QtLocation5::MapFlickable::onFlickStarted()

    This handler is called when the view is flicked.  A flick
    starts from the point that the mouse or touch is released,
    while still in motion.
*/

/*!
    \qmlsignal QtLocation5::MapFlickable::onFlickEnded()

    This handler is called when the view stops moving due to a flick.
    The order of onMovementEnded() and onFlickEnded() is not specified.
*/

QDeclarativeGeoMapFlickable::QDeclarativeGeoMapFlickable(QObject *parent)
    : QObject(parent),
      pressed_(false),
      maxVelocity_(QML_MAP_FLICK_DEFAULTMAXVELOCITY),
      deceleration_(QML_MAP_FLICK_DEFAULTDECELERATION),
      velocityX_(0.0),
      velocityY_(0.0),
      flicking_(false),
      map_(0),
      animation_(0),
      enabled_(true),
      moving_(false)
{
    pressTime_.invalidate();
    lastPosTime_.invalidate();
    velocityTime_.invalidate();
}

QDeclarativeGeoMapFlickable::~QDeclarativeGeoMapFlickable()
{
}

void QDeclarativeGeoMapFlickable::setMap(QGeoMap* map)
{
    if (map_ || !map)
        return;
    map_ = map;
    animation_ = new QPropertyAnimation(map_->mapController(), "center", this);
    animation_->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    connect(animation_, SIGNAL(finished()), this, SLOT(flickAnimationFinished()));
}

qreal QDeclarativeGeoMapFlickable::deceleration() const
{
    return deceleration_;
}

void QDeclarativeGeoMapFlickable::setDeceleration(qreal deceleration)
{
    if (deceleration < QML_MAP_FLICK_MINIMUMDECELERATION)
        deceleration = QML_MAP_FLICK_MINIMUMDECELERATION;
    else if (deceleration > QML_MAP_FLICK_MAXIMUMDECELERATION)
        deceleration = QML_MAP_FLICK_MAXIMUMDECELERATION;
    if (deceleration == deceleration_)
        return;
    deceleration_ = deceleration;
    emit decelerationChanged();
}

bool QDeclarativeGeoMapFlickable::mousePressEvent(QMouseEvent *event)
{
    if (!enabled_)
        return false;
    stop();
    pressed_ = true;
    lastPos_ = QPointF();
    pressPos_ = event->pos();
    lastPosTime_.start();
    pressTime_.start();
    velocityTime_.start();
    return true;
}

void QDeclarativeGeoMapFlickable::stop()
{
    velocityBufferX_.clear();
    velocityBufferY_.clear();
    velocityX_ = 0.0;
    velocityY_ = 0.0;
    if (moving_) {
        moving_ = false;
        emit movementEnded();
    }
    if (flicking_) {
        flicking_ = false;
        if (animation_->state() == QPropertyAnimation::Running)
            animation_->stop();
        emit flickEnded();
    }
    lastPosTime_.invalidate();
    pressTime_.invalidate();
    velocityTime_.invalidate();
}

bool QDeclarativeGeoMapFlickable::mouseMoveEvent(QMouseEvent *event)
{
    if (!enabled_ || !pressed_ || !lastPosTime_.isValid())
        return false;
    // Check if thresholds for normal panning are met.
    // (normal panning vs flicking: flicking will start from mouse release event).
    int dyFromPress = int(event->pos().y() - pressPos_.y());
    int dxFromPress = int(event->pos().x() - pressPos_.x());
    int dyFromLastPos;
    int dxFromLastPos;
    if (!lastPos_.isNull()) {
        dyFromLastPos = event->pos().y() - lastPos_.y();
        dxFromLastPos = event->pos().x() - lastPos_.x();
    }

    // Simple pan (drag) while being pressed
    const int startDragDistance = qApp->styleHints()->startDragDistance();
    if ((qAbs(dyFromPress) > startDragDistance
            || qAbs(dxFromPress) > startDragDistance
            || pressTime_.elapsed() > 200) && !lastPos_.isNull()) {

        if (!moving_) {
            moving_ = true;
            emit movementStarted();
        }
        updateCamera(dxFromLastPos, dyFromLastPos, 0);
    }
    // Take velocity samples, used later to determine the flick
    // duration and speed (when mouse is released).
    if (!lastPos_.isNull()) {
        qreal elapsed = qreal(lastPosTime_.elapsed()) / 1000.;
        if (elapsed <= 0) {
            lastPos_ = event->pos();
            return false;
        }
        lastPosTime_.restart();
        addVelocitySample(velocityBufferY_, double(dyFromLastPos)/elapsed);
        addVelocitySample(velocityBufferX_, double(dxFromLastPos)/elapsed);
    }
    lastPos_ = event->pos();
    return true;
}

// FIXME:
// - not left right / up down flicking, so if map is rotated, will act unintuitively
void QDeclarativeGeoMapFlickable::updateCamera(int dx, int dy, int timeMs)
{
    if (timeMs < 0)
        return;
    AnimatableCoordinate animationStartCoordinate = map_->mapController()->center();
    QGeoCoordinate coordinate = animationStartCoordinate.coordinate();
     if (timeMs == 0) {
        // No animation, just set new values.
        QPointF p = map_->coordinateToScreenPosition(coordinate);
        p.setY(p.y() - dy);
        p.setX(p.x() - dx);
        animationStartCoordinate.setCoordinate(map_->screenPositionToCoordinate(p));
        map_->mapController()->setCenter(animationStartCoordinate);
    } else {
        //qDebug() << "Will do flick animation dx (pix), dy (pix), time (ms): " << dx << dy << timeMs;
        if (animation_->state() == QPropertyAnimation::Running)
            animation_->stop();
        AnimatableCoordinate animationEndCoordinate = map_->mapController()->center();
        animation_->setDuration(timeMs);
        coordinate.setLongitude(coordinate.longitude() - (dx / pow(2.0, map_->mapController()->zoom())));
        coordinate.setLatitude(coordinate.latitude() + (dy / pow(2.0, map_->mapController()->zoom())));
        animationEndCoordinate.setCoordinate(coordinate);
        animation_->setStartValue(QVariant::fromValue(animationStartCoordinate));
        animation_->setEndValue(QVariant::fromValue(animationEndCoordinate));
        //qDebug() << "The latitude will go from:" << animationStartCoordinate.coordinate().latitude() << "to:" << animationEndCoordinate.coordinate().latitude();
        //qDebug() << "The longitude will go from:" << animationStartCoordinate.coordinate().longitude() << "to:" << animationEndCoordinate.coordinate().longitude();
        // start animation straight away, user may disable the flick in the flickStarted() handler
        animation_->start();
        flicking_ = true;
        emit flickStarted();
    }
}

// Adds velocity sample to sample buffer. Data is later used to calculate
// flick speed. By default 3 latest samples are considered.
void QDeclarativeGeoMapFlickable::addVelocitySample(QVector<qreal>& buffer, qreal sample)
{
    if (sample > maxVelocity_)
        sample = maxVelocity_;
    else if (sample < -maxVelocity_)
        sample = -maxVelocity_;
    buffer.append(sample);
    if (buffer.count() > QML_MAP_FLICK_SAMPLEBUFFER)
        buffer.remove(0);
}

void QDeclarativeGeoMapFlickable::updateVelocity(QVector<qreal>& buffer, qreal& velocity)
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


void QDeclarativeGeoMapFlickable::setEnabled(bool enabled)
{
    if (enabled_ == enabled)
        return;
    enabled_ = enabled;
    if (!enabled_)
        stop();
    emit enabledChanged();
}

bool QDeclarativeGeoMapFlickable::enabled() const
{
    return enabled_;
}

bool QDeclarativeGeoMapFlickable::mouseReleaseEvent(QMouseEvent *event)
{
    if (!pressed_ || !enabled_)
        return false;
    pressed_ = false;

    // if we drag then pause before release we should not cause a flick.
    if (lastPosTime_.elapsed() < 100) {
        updateVelocity(velocityBufferY_, velocityY_);
        updateVelocity(velocityBufferX_, velocityX_);
    } else {
        velocityX_ = 0.0;
        velocityY_ = 0.0;
    }
    int flickTimeY = 0;
    int flickTimeX = 0;
    int flickPixelsX = 0;
    int flickPixelsY = 0;
    if (qAbs(velocityY_) > MinimumFlickVelocity && qAbs(event->pos().y() - pressPos_.y()) > FlickThreshold) {
        // calculate Y flick animation values
        qreal acceleration = deceleration_;
        if ((velocityY_ > 0.0f) == (deceleration_ > 0.0f))
            acceleration = acceleration * -1.0f;
        flickTimeY = static_cast<int>(-1000 * velocityY_ / acceleration);
        flickPixelsY = (flickTimeY * velocityY_) / (1000.0 * 2);
        //qDebug() << "---=== would flick Y, velocity (pix/sec), flick duration (msec): ===---" << velocityY_ << flickTimeY;
    } else {
        // reset
        //qDebug() << "---=== would NOT flick Y, velocity (pix/sec): ===---" <<  velocityY_;
    }
    if (qAbs(velocityX_) > MinimumFlickVelocity && qAbs(event->pos().x() - pressPos_.x()) > FlickThreshold) {
        // calculate X flick animation values
        qreal acceleration = deceleration_;
        if ((velocityX_ > 0.0f) == (deceleration_ > 0.0f))
            acceleration = acceleration * -1.0f;
        flickTimeX = static_cast<int>(-1000 * velocityX_ / acceleration);
        flickPixelsX = (flickTimeX * velocityX_) / (1000.0 * 2);
        //qDebug() << "---=== would flick X, velocity (pix/sec), flick duration (msec), pixels: ===---" << velocityX_ << flickTimeX << "pixels: " << flickPixelsX;
    } else {
        // reset
        //qDebug() << "---=== would NOT flick X, velocity (pix/sec) ===---" << velocityX_;
    }
    int flickTime = qMax(flickTimeY, flickTimeX);
    updateCamera(flickPixelsX, flickPixelsY, flickTime);

    if (flickTime == 0 && moving_) {
        emit movementEnded();
        moving_ = false;
    }
    velocityBufferX_.clear();
    velocityBufferY_.clear();
    velocityX_ = 0.0;
    velocityY_ = 0.0;
    lastPosTime_.invalidate();
    pressTime_.invalidate();
    velocityTime_.invalidate();
    return true;
}

void QDeclarativeGeoMapFlickable::flickAnimationFinished()
{
    //qDebug() << metaObject()->className() << __FUNCTION__;
    //Q_ASSERT(flicking_);
    //Q_ASSERT(moving_);
    flicking_ = false;
    moving_ = false;
    emit flickEnded();
    emit movementEnded();
}

/*
void QDeclarativeGeoMapFlickable::flickAnimationValueChanged(const QVariant& value)
{
    qDebug() << metaObject()->className() << __FUNCTION__;
}
*/

void QDeclarativeGeoMapFlickable::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event); // TODO press delay handling
    //qDebug() << metaObject()->className() << __FUNCTION__ ;
}

#include "moc_qdeclarativegeomapflickable_p.cpp"

QT_END_NAMESPACE
