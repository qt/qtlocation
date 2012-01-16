/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#ifndef QDECLARATIVEPINCHGENERATOR_H
#define QDECLARATIVEPINCHGENERATOR_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickCanvas>
#include <QtGui/QMouseEvent>
#include <QtGui/QTouchEvent>
#include <QtGui/QKeyEvent>
#include <QtCore/QElapsedTimer>
#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QDebug>

// how many concurrent "swipes" should we have
// bit overkill here first I thought support random about of swipes (1..x)
// but thats for later
#define SWIPES_REQUIRED 2

typedef struct {
    int totalDuration; // not sure if needed
    QList<QTouchEvent::TouchPoint> touchPoints;
    QList<int> touchPointDurations;
} Swipe;

// total overkill fixme
class TouchPoint: public QObject
{
    Q_OBJECT
    Q_PROPERTY(double targetX READ targetX CONSTANT)
    Q_PROPERTY(double targetY READ targetY CONSTANT)
    Q_PROPERTY(int touchState READ touchState CONSTANT)
public:
    TouchPoint(double x, double y, int state): targetX_(x), targetY_(y),touchState_(state) {}
    double targetX() {return targetX_;}
    double targetY() {return targetY_;}
    double touchState() {return touchState_;}
    double targetX_;
    double targetY_;
    int touchState_;
};

class QDeclarativePinchGenerator : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(qreal replaySpeedFactor READ replaySpeedFactor WRITE setReplaySpeedFactor NOTIFY replaySpeedFactorChanged)
    Q_PROPERTY(QQuickItem* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QList<QObject*> swipe1 READ swipe1 NOTIFY swipesChanged)
    Q_PROPERTY(QList<QObject*> swipe2 READ swipe2 NOTIFY swipesChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    QDeclarativePinchGenerator();
    ~QDeclarativePinchGenerator();

    QString state() const;
    int count() const;
    QQuickItem* target() const;
    void setTarget(QQuickItem* target);
    qreal replaySpeedFactor() const;
    void setReplaySpeedFactor(qreal factor);
    bool enabled() const;
    void setEnabled(bool enabled);

    QList<QObject*> swipe1();
    QList<QObject*> swipe2();

    Q_INVOKABLE void replay();
    Q_INVOKABLE void clear();
    Q_INVOKABLE void stop();

    // programmatic interface, useful for autotests
    Q_INVOKABLE void pinch(QPoint point1From,
                           QPoint point1To,
                           QPoint point2From,
                           QPoint point2To,
                           int interval1 = 20,
                           int interval2 = 20,
                           int samples1 = 10,
                           int samples2 = 10);
signals:
    void stateChanged();
    void countChanged();
    void targetChanged();
    void swipesChanged();
    void replaySpeedFactorChanged();
    void enabledChanged();

public:
    enum GeneratorState {
        Invalid    = 0,
        Idle       = 1,
        Recording  = 2,
        Replaying  = 3
    };

    // from QDeclarativeParserStatus
    virtual void componentComplete();
    // from QQuickItem
    void itemChange(ItemChange change, const ItemChangeData & data);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    void setState(GeneratorState state);
    QTouchEvent::TouchPoint mouseEventToTouchPoint(QMouseEvent* event);
    QTouchEvent::TouchPoint createTouchPoint(QEvent::Type type, QPoint pos);
    void generateSwipe(QPoint from, QPoint to, int duration, int samples);

private:
    QQuickItem* target_;
    GeneratorState state_;
    QQuickCanvas* canvas_;
    QList<Swipe*> swipes_;
    Swipe* activeSwipe_;
    QElapsedTimer swipeTimer_;
    int replayTimer_;
    int replayBookmark_;
    int masterSwipe_;
    int touchPointId_;
    qreal replaySpeedFactor_;
    QList<QObject*> swipeList1_;
    QList<QObject*> swipeList2_;
    bool enabled_;
};

#endif
