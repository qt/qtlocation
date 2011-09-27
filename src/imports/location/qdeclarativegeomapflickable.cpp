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

#include <QtWidgets/QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include "qdeclarativegeomapflickable_p.h"
#include <QTimer>
#include "map.h"
#include "math.h"

// Constant values from QSGFlickable
// The default maximum velocity of a flick.
#ifndef QML_FLICK_DEFAULTMAXVELOCITY
#define QML_FLICK_DEFAULTMAXVELOCITY 2500
#endif
// The default deceleration of a flick.
#ifndef QML_FLICK_DEFAULTDECELERATION
#define QML_FLICK_DEFAULTDECELERATION 1500
#endif
// Minimum
#ifndef QML_FLICK_MINIMUMECELERATION
#define QML_FLICK_MINIMUMDECELERATION 10
#endif


// The number of samples to discard when calculating the flick velocity.
// Touch panels often produce inaccurate results as the finger is lifted.
#ifndef QML_FLICK_DISCARDSAMPLES
#define QML_FLICK_DISCARDSAMPLES 1
#endif

// FlickThreshold determines how far the "mouse" must have moved
// before we perform a flick.
static const int FlickThreshold = 20;
// RetainGrabVelocity is the maxmimum instantaneous velocity that
// will ensure the Flickable retains the grab on consecutive flicks.
static const int RetainGrabVelocity = 15;
// Really slow flicks can be annoying.
const qreal MinimumFlickVelocity = 75.0;

QT_BEGIN_NAMESPACE

QDeclarativeGeoMapFlickable::QDeclarativeGeoMapFlickable(Map* map, QObject *parent)
    : QObject(parent),
      pressed_(false),
      maxVelocity_(QML_FLICK_DEFAULTMAXVELOCITY),
      deceleration_(QML_FLICK_DEFAULTDECELERATION),
      flicking_(false),
      map_(map),
      enabled_(false),
      moving_(false)
{
    Q_ASSERT(map_);
    pressTime_.invalidate();
    lastPosTime_.invalidate();
    velocityTime_.invalidate();
    //animation_ = new QPropertyAnimation(map_->mapCamera(), "cameraData", this);
    //animation_ = new QPropertyAnimation(map_->mapCamera(), "center", this);
    animation_ = new QPropertyAnimation(map_, "camera", this);
    animation_->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    connect(animation_, SIGNAL(finished()), this, SLOT(flickAnimationFinished()));
    //connect(animation_, SIGNAL(valueChanged(const QVariant&)), this, SLOT(flickAnimationValueChanged(const QVariant&)));
}

QDeclarativeGeoMapFlickable::~QDeclarativeGeoMapFlickable()
{
}

qreal QDeclarativeGeoMapFlickable::deceleration() const
{
    return deceleration_;
}

void QDeclarativeGeoMapFlickable::setDeceleration(qreal deceleration)
{
    if (deceleration < QML_FLICK_MINIMUMDECELERATION)
        deceleration = QML_FLICK_MINIMUMDECELERATION;
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
    if ((qAbs(dyFromPress) > QApplication::startDragDistance()
            || qAbs(dxFromPress) > QApplication::startDragDistance()
            || pressTime_.elapsed() > 200) && !lastPos_.isNull()) {

        if (!moving_) {
            emit movementStarted();
            moving_ = true;
        }
        updateCamera(dxFromLastPos, dyFromLastPos, 0);
    }
    // Take velocity samples, used later to determine the flick
    // duration and speed (when mouse is released).
    if (!lastPos_.isNull()) {
        qreal elapsed = qreal(lastPosTime_.elapsed()) / 1000.;
        if (elapsed <= 0)
            return false;
        lastPosTime_.restart();
        addVelocitySample(velocityBufferY_, double(dyFromLastPos)/elapsed);
        addVelocitySample(velocityBufferX_, double(dxFromLastPos)/elapsed);
    }
    lastPos_ = event->pos();
    return true;
}

// FIXME coordinate pan with a sleeve-constant (zoom level is not considered appropriately)
void QDeclarativeGeoMapFlickable::updateCamera(int dx, int dy, int timeMs)
{
    if (timeMs < 0)
        return;
    CameraData cameraStart = map_->cameraData();
    QGeoCoordinate coordinate = cameraStart.center();
     if (timeMs == 0) {
        // No animation, just set new values.
        coordinate.setLatitude(coordinate.latitude() + (dy / pow(2.0, cameraStart.zoomFactor())));
        coordinate.setLongitude(coordinate.longitude() - (dx / pow(2.0, cameraStart.zoomFactor())));
        cameraStart.setCenter(coordinate);
        map_->setCameraData(cameraStart);
    } else {
        qDebug() << "Will do flick animation dx (pix), dy (pix), time (ms): " << dx << dy << timeMs;
        if (animation_->state() == QPropertyAnimation::Running)
            animation_->stop();
        CameraData cameraEnd = cameraStart;
        animation_->setDuration(timeMs);
        coordinate.setLongitude(coordinate.longitude() - (dx / pow(2.0, cameraStart.zoomFactor())));
        coordinate.setLatitude(coordinate.latitude() + (dy / pow(2.0, cameraStart.zoomFactor())));
        cameraEnd.setCenter(coordinate);

        animation_->setStartValue(QVariant::fromValue(cameraStart));
        animation_->setEndValue(QVariant::fromValue(cameraEnd));

        //animation_->setStartValue(QVariant::fromValue(cameraStart.center()));
        //animation_->setEndValue(QVariant::fromValue(cameraEnd.center()));

        qDebug() << "The latitude will go from:" << cameraStart.center().latitude() << "to:" << cameraEnd.center().latitude();
        qDebug() << "The longitude will go from:" << cameraStart.center().longitude() << "to:" << cameraEnd.center().longitude();
        QTimer::singleShot(0, animation_, SLOT(start()));
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
    if (buffer.count() > QML_FLICK_SAMPLEBUFFER)
        buffer.remove(0);
}

void QDeclarativeGeoMapFlickable::updateVelocity(QVector<qreal>& buffer, qreal& velocity)
{
    if (buffer.count() > QML_FLICK_DISCARDSAMPLES) {
        velocity = 0;
        int count = buffer.count() - QML_FLICK_DISCARDSAMPLES;
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
