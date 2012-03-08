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

QT_BEGIN_NAMESPACE

class QGraphicsSceneMouseEvent;
class QDeclarativeGeoMap;
class QTouchEvent;
class QGeoMap;

class QDeclarativeGeoMapPinchEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPointF center READ center)
    Q_PROPERTY(qreal angle READ angle)
    Q_PROPERTY(QPointF point1 READ point1)
    Q_PROPERTY(QPointF point2 READ point2)
    Q_PROPERTY(int pointCount READ pointCount)
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted)

public:
    QDeclarativeGeoMapPinchEvent(QPointF center, qreal angle,
                                 QPointF point1, QPointF point2,
                                 int pointCount = 0, bool accepted = true)
        : QObject(), center_(center), angle_(angle),
          point1_(point1), point2_(point2),
        pointCount_(pointCount), accepted_(accepted) {}
    QDeclarativeGeoMapPinchEvent() {}

    QPointF center() const { return center_; }
    void setCenter(QPointF center) { center_ = center; }
    qreal angle() const { return angle_; }
    void setAngle(qreal angle) { angle_ = angle; }
    QPointF point1() const { return point1_; }
    void setPoint1(QPointF p) { point1_ = p; }
    QPointF point2() const { return point2_; }
    void setPoint2(QPointF p) { point2_ = p; }
    int pointCount() const { return pointCount_; }
    void setPointCount(int count) { pointCount_ = count; }
    bool accepted() const { return accepted_; }
    void setAccepted(bool a) { accepted_ = a; }

private:
    QPointF center_;
    qreal angle_;
    QPointF point1_;
    QPointF point2_;
    int pointCount_;
    bool accepted_;
};

// tbd: should we have a 'active' / 'moving' boolean attribute when pinch is active?

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
    // need for these is not clear, use-case(s) not yet identified:
    //Q_PROPERTY(qreal minimumRotation READ minimumRotation WRITE setMinimumRotation NOTIFY minimumRotationChanged)
    //Q_PROPERTY(qreal maximumRotation READ maximumRotation WRITE setMaximumRotation NOTIFY maximumRotationChanged)
    //Q_PROPERTY(qreal minimumZoomLevel READ minimumZoomLevel WRITE setMinimumZoomLevel NOTIFY minimumZoomLevelChanged)
    //Q_PROPERTY(qreal maximumZoomLevel READ maximumZoomLevel WRITE setMaximumZoomLevel NOTIFY maximumZoomLevelChanged)
    // when tilt is supported, these are needed:
    //Q_PROPERTY(qreal maximumTilt READ maximumTilt WRITE setMaximumTilt NOTIFY maximumTiltChanged)
    //Q_PROPERTY(qreal minimumTilt READ minimumTilt WRITE setMinimumTilt NOTIFY minimumTiltChanged)
    //Q_PROPERTY(qreal maximumTiltChange READ maximumTiltChange WRITE setMaximumTiltChange NOTIFY maximumTiltChangeChanged)

public:
    QDeclarativeGeoMapPinchArea(QDeclarativeGeoMap* map, QObject *parent = 0);
    ~QDeclarativeGeoMapPinchArea();

    enum ActiveGesture {
        NoGesture = 0x0000,
        ZoomGesture = 0x0001,
        RotationGesture = 0x0002,
        TiltGesture = 0x0004
    };
    Q_DECLARE_FLAGS(ActiveGestures, ActiveGesture);

    ActiveGestures activeGestures() const;
    void setActiveGestures(ActiveGestures activeGestures);

    bool active() const;
    void setActive(bool active);

    bool enabled() const;
    void setEnabled(bool enabled);

    qreal minimumZoomLevel() const;
    void setMinimumZoomLevel(qreal zoomLevel);

    qreal maximumZoomLevel() const;
    void setMaximumZoomLevel(qreal zoomLevel);

    qreal maximumZoomLevelChange() const;
    void setMaximumZoomLevelChange(qreal maxChange);

    qreal minimumRotation() const;
    void setMinimumRotation(qreal zoomLevel);

    qreal maximumRotation() const;
    void setMaximumRotation(qreal zoomLevel);

    qreal rotationFactor() const;
    void setRotationFactor(qreal factor);

    qreal maximumTilt() const;
    void setMaximumTilt(qreal tilt);

    qreal minimumTilt() const;
    void setMinimumTilt(qreal tilt);

    qreal maximumTiltChange() const;
    void setMaximumTiltChange(qreal tilt);

    void touchEvent(QTouchEvent *event);

    void zoomLevelLimits(qreal min, qreal max);

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

    void pinchStarted(QDeclarativeGeoMapPinchEvent* pinch);
    void pinchUpdated(QDeclarativeGeoMapPinchEvent* pinch);
    void pinchFinished(QDeclarativeGeoMapPinchEvent* pinch);

private:
    void updatePinch();

private:
    QDeclarativeGeoMap* map_;
    QDeclarativeGeoMapPinchEvent pinchEvent_;
    bool enabled_;
    bool active_;
    qreal minimumZoomLevel_;
    qreal maximumZoomLevel_;
    qreal minimumRotation_;
    qreal maximumRotation_;
    QList<QTouchEvent::TouchPoint> touchPoints_;
    bool inPinch_;
    bool pinchRejected_;
    bool pinchActivated_;
    QPointF sceneStartPoint1_;
    QPointF sceneStartPoint2_;
    QPointF lastPoint1_;
    QPointF lastPoint2_;
    qreal pinchStartDist_;
    qreal pinchStartZoomLevel_;
    qreal pinchLastZoomLevel_;
    qreal pinchStartRotation_;
    qreal pinchLastAngle_;
    qreal pinchRotation_;
    QPointF sceneLastCenter_;
    int id1_;
    qreal maximumZoomLevelChange_;
    qreal rotationFactor_;
    ActiveGestures activeGestures_;
    qreal minimumTilt_;
    qreal maximumTilt_;
    qreal maximumTiltChange_;

    qreal pinchLastTilt_;
    qreal pinchStartTilt_;
};

QT_END_NAMESPACE
QML_DECLARE_TYPE(QDeclarativeGeoMapPinchArea);

#endif
