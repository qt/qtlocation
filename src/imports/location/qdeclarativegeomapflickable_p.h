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

#ifndef QDECLARATIVEGEOMAPFLICKABLE_H
#define QDECLARATIVEGEOMAPFLICKABLE_H

// Constant value from QSGFlickable
// The number of samples to use in calculating the velocity of a flick
#ifndef QML_FLICK_SAMPLEBUFFER
#define QML_FLICK_SAMPLEBUFFER 3
#endif

#include <QtDeclarative/qdeclarative.h>
#include <QElapsedTimer>
#include <QVector>
#include <QObject>
#include <QDebug>

// Internal. Calculates relevant flick parameters based on mouse
// movements, which can then be used to fire a panning animation.
QT_BEGIN_NAMESPACE

class QGraphicsSceneMouseEvent;
class QPropertyAnimation;
class CameraData;
class Map;

class QDeclarativeGeoMapFlickable: public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal deceleration READ deceleration WRITE setDeceleration NOTIFY decelerationChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged())

public:
    QDeclarativeGeoMapFlickable(Map* map, QObject *parent = 0);
    ~QDeclarativeGeoMapFlickable();

    qreal deceleration() const;
    void setDeceleration(qreal deceleration);

    bool enabled() const;
    void setEnabled(bool enabled);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void timerEvent(QTimerEvent *event);

signals:
    void decelerationChanged();
    void movementStarted();
    void enabledChanged();
    void movementEnded();
    void flickStarted();
    void flickEnded();

private:
    void addVelocitySample(QVector<qreal>& buffer, qreal sample);
    void updateVelocity(QVector<qreal>& buffer, qreal& velocity);
    void updateCamera(int dx, int dy, int timeMs = 0);
    void stop();

private slots:
    void flickAnimationFinished();
    //void flickAnimationValueChanged(const QVariant&);

private:
    bool pressed_;
    qreal maxVelocity_;
    qreal deceleration_;
    QElapsedTimer lastPosTime_;
    QElapsedTimer pressTime_;
    QElapsedTimer velocityTime_;
    QVector<qreal> velocityBufferX_;
    qreal velocityX_;
    QVector<qreal> velocityBufferY_;
    qreal velocityY_;
    QPointF lastPos_;
    QPointF pressPos_;
    bool flicking_;
    Map* map_;
    QPropertyAnimation* animation_;
    bool enabled_;
    bool moving_;
};

QT_END_NAMESPACE
QML_DECLARE_TYPE(QDeclarativeGeoMapFlickable);

#endif
