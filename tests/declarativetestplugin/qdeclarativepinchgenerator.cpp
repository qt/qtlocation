#include "qdeclarativepinchgenerator_p.h"
#include <QtWidgets/QApplication>
#include <qsgengine.h>


QDeclarativePinchGenerator::QDeclarativePinchGenerator():
    target_(0),
    state_(Invalid),
    canvas_(0),
    activeSwipe_(0),
    replayTimer_(-1),
    replayBookmark_(-1),
    masterSwipe_(-1),
    touchPointId_(0),
    replaySpeedFactor_(1.0),
    enabled_(true)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);
    swipeTimer_.invalidate();
}

QDeclarativePinchGenerator::~QDeclarativePinchGenerator()
{
    if (!swipeList1_.isEmpty()) {
        qDeleteAll(swipeList1_);
        swipeList1_.clear();
    }
    if (!swipeList2_.isEmpty()) {
        qDeleteAll(swipeList2_);
        swipeList2_.clear();
    }
}

void QDeclarativePinchGenerator::componentComplete()
{
    QSGItem::componentComplete();
}

void QDeclarativePinchGenerator::mousePressEvent(QMouseEvent *event)
{
    if (state_ != Idle || !enabled_) {
        event->ignore();
        return;
    }
    Q_ASSERT(!activeSwipe_);
    Q_ASSERT(!swipeTimer_.isValid());
    // Start recording a pinch gesture.
    QTouchEvent::TouchPoint touchPoint = mouseEventToTouchPoint(event);
    if (touchPoint.state() == Qt::TouchPointPressed) {
        activeSwipe_ = new Swipe;
        //touchPointId_ = 0;
        activeSwipe_->touchPoints.append(touchPoint);
        activeSwipe_->totalDuration = 0;
        activeSwipe_->touchPointDurations.append(0);
        swipeTimer_.start();
        setState(Recording);
    } else {
        Q_ASSERT(false); // no!
    }
}

bool QDeclarativePinchGenerator::enabled() const
{
    return enabled_;
}


void QDeclarativePinchGenerator::setEnabled(bool enabled)
{
    if (enabled == enabled_)
        return;
    enabled_ = enabled;
    if (!enabled_) {
        stop();
        clear();
    }
    emit enabledChanged();
}

QTouchEvent::TouchPoint QDeclarativePinchGenerator::mouseEventToTouchPoint(QMouseEvent *event)
{
    QTouchEvent::TouchPoint touchPoint;
    switch (event->type()) {
    //case QEvent::GraphicsSceneMousePress:
    case QEvent::MouseButtonPress:
        touchPoint.setState(Qt::TouchPointPressed);
        break;
    //case QEvent::GraphicsSceneMouseMove:
    case QEvent::MouseMove:
        touchPoint.setState(Qt::TouchPointMoved);
        break;
    //case QEvent::GraphicsSceneMouseRelease:
    case QEvent::MouseButtonRelease:
        touchPoint.setState(Qt::TouchPointReleased);
        break;
    default:
        break;
    }
    //touchPoint.setId(touchPointId_++); // running number
    touchPoint.setId(0);
    touchPoint.setPressure(0.75);
    touchPoint.setPos(event->pos());
    touchPoint.setLastPos(event->pos());
    touchPoint.setScenePos(target_->mapToScene(event->pos()));
    touchPoint.setLastScenePos(target_->mapToScene(event->pos()));
    return touchPoint;
}

// fixme this is excessive
QTouchEvent::TouchPoint QDeclarativePinchGenerator::convertToPrimary(QTouchEvent::TouchPoint original)
{
    QTouchEvent::TouchPoint touchPoint;
    touchPoint.setState(original.state() | Qt::TouchPointPrimary);
    touchPoint.setId(original.id() + 1);
    touchPoint.setPressure(original.pressure());
    touchPoint.setPos(original.pos());
    touchPoint.setLastPos(original.lastPos());
    touchPoint.setScenePos(original.scenePos());
    touchPoint.setLastScenePos(original.lastScenePos());
    return touchPoint;

}

void QDeclarativePinchGenerator::mouseMoveEvent(QMouseEvent *event)
{
    if (state_ != Recording || !enabled_) {
        event->ignore();
        return;
    }
    Q_ASSERT(activeSwipe_);
    Q_ASSERT(swipeTimer_.isValid());
    QTouchEvent::TouchPoint touchPoint = mouseEventToTouchPoint(event);
    if (touchPoint.state() == Qt::TouchPointMoved) {
        activeSwipe_->touchPoints.append(touchPoint);
        activeSwipe_->totalDuration += swipeTimer_.elapsed();
        activeSwipe_->touchPointDurations.append(swipeTimer_.elapsed());
        swipeTimer_.restart();
    } else {
        Q_ASSERT(false); // no!
    }
}

void QDeclarativePinchGenerator::mouseReleaseEvent(QMouseEvent *event)
{
    if (state_ != Recording || !enabled_) {
        event->ignore();
        return;
    }
    Q_ASSERT(activeSwipe_);
    Q_ASSERT(swipeTimer_.isValid());
    QTouchEvent::TouchPoint touchPoint = mouseEventToTouchPoint(event);
    if (touchPoint.state() == Qt::TouchPointReleased) {
        activeSwipe_->touchPoints.append(touchPoint);
        activeSwipe_->totalDuration += swipeTimer_.elapsed();
        activeSwipe_->touchPointDurations.append(swipeTimer_.elapsed());
    } else {
        Q_ASSERT(false); // no!
    }
    if (swipes_.count() == SWIPES_REQUIRED)
        delete swipes_.takeFirst();
    swipes_.append(activeSwipe_);
    activeSwipe_ = 0;
    swipeTimer_.invalidate();
    if (canvas_ && target_) setState(Idle); else setState(Invalid);
    emit swipesChanged();
}

QList<QObject*> QDeclarativePinchGenerator::swipe1()
{
    if (swipes_.count() < 1)
        return QList<QObject*>();
    if (!swipeList1_.isEmpty()) {
        qDeleteAll(swipeList1_);
        swipeList1_.clear();
    }
    for (int i = 0; i < swipes_.at(0)->touchPoints.count(); ++i) {
        TouchPoint* touchPoint = new TouchPoint(
                    swipes_.at(0)->touchPoints.at(i).scenePos().x(),
                    swipes_.at(0)->touchPoints.at(i).scenePos().y(),
                    static_cast<int>(swipes_.at(0)->touchPoints.at(i).state()));
        swipeList1_.append(touchPoint);
    }
    return swipeList1_;
}

QList<QObject*> QDeclarativePinchGenerator::swipe2()
{
    if (swipes_.count() < 2)
        return QList<QObject*>();
    if (!swipeList2_.isEmpty()) {
        qDeleteAll(swipeList2_);
        swipeList2_.clear();
    }
    for (int i = 0; i < swipes_.at(1)->touchPoints.count(); ++i) {
        TouchPoint* touchPoint = new TouchPoint(
                    static_cast<double>(swipes_.at(1)->touchPoints.at(i).scenePos().x()),
                    static_cast<double>(swipes_.at(1)->touchPoints.at(i).scenePos().y()),
                    static_cast<int>(swipes_.at(1)->touchPoints.at(i).state()));
        swipeList2_.append(touchPoint);
    }
    return swipeList2_;
}

qreal QDeclarativePinchGenerator::replaySpeedFactor() const
{
    return replaySpeedFactor_;
}

void QDeclarativePinchGenerator::setReplaySpeedFactor(qreal factor)
{
    if (factor == replaySpeedFactor_ || factor < 0.001)
        return;
    replaySpeedFactor_ = factor;
    emit replaySpeedFactorChanged();
}

void QDeclarativePinchGenerator::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (!enabled_) {
        event->ignore();
        return;
    }
    stop();
    clear();
    if (canvas_ && target_) setState(Idle); else setState(Invalid);
}

QString QDeclarativePinchGenerator::state() const
{
    switch (state_) {
    case Invalid:
        return "Invalid";
    case Idle:
        return "Idle";
        break;
    case Recording:
        return "Recording";
        break;
    case Replaying:
        return "Replaying";
        break;
    default:
        Q_ASSERT(false);
    }
    return "How emberassing";
}

int QDeclarativePinchGenerator::count() const
{
    return swipes_.count();
}

void QDeclarativePinchGenerator::keyPressEvent(QKeyEvent *e)
{
    if (!enabled_) {
        e->ignore();
    }

    if (e->key() == Qt::Key_C) {
        clear();
    } else if (e->key() == Qt::Key_R) {
        replay();
    } else if (e->key() == Qt::Key_S) {
        stop();
    } else if (e->key() == Qt::Key_Plus) {
        setReplaySpeedFactor(replaySpeedFactor() + 0.1);
    } else if (e->key() == Qt::Key_Minus) {
        setReplaySpeedFactor(replaySpeedFactor() - 0.1);
    } else {
        qDebug() << metaObject()->className() << "Unsupported key event.";
    }
}
void QDeclarativePinchGenerator::setState(GeneratorState state)
{
    if (state == state_)
        return;
    state_ = state;
    emit stateChanged();
}

void QDeclarativePinchGenerator::itemChange(ItemChange change, const ItemChangeData & data)
{
    if (change == ItemSceneChange) {
        canvas_ = data.canvas;
        if (target_)
            setState(Idle);
    }
}

void QDeclarativePinchGenerator::timerEvent(QTimerEvent *event)
{
    Q_ASSERT(replayTimer_ == event->timerId());
    Q_ASSERT(state_ == Replaying);

    // Create touchevent. May have one or two touchpoints.
    QTouchEvent* touchEvent;
    switch (swipes_.at(masterSwipe_)->touchPoints.at(replayBookmark_).state()) {
    case (Qt::TouchPointPressed):
        touchEvent = new QTouchEvent(QEvent::TouchBegin,QTouchEvent::TouchScreen,Qt::NoModifier,Qt::TouchPointReleased);
        break;
    case (Qt::TouchPointMoved):
        touchEvent = new QTouchEvent(QEvent::TouchUpdate,QTouchEvent::TouchScreen,Qt::NoModifier,Qt::TouchPointReleased);
        break;
    case (Qt::TouchPointReleased):
        touchEvent = new QTouchEvent(QEvent::TouchEnd,QTouchEvent::TouchScreen,Qt::NoModifier,Qt::TouchPointReleased);
        break;
    default:
        Q_ASSERT(false);
        break;
    }
    // Set touch points. Master swipe has touchpoints as it was chosen to have more touchpoints.
    // For the other swipe we need to check.
    QList<QTouchEvent::TouchPoint> touchPoints;
    // The primary touch point generates also mouse events. Does not work at the moment because
    // we dispatch the touch event via canvas rather than the qApp. However qApp dispatching
    // seems to miss some events arbitrarily.
    touchPoints.append(convertToPrimary(swipes_.at(masterSwipe_)->touchPoints.at(replayBookmark_)));
    //touchPoints.append(swipes_.at(masterSwipe_)->touchPoints.at(replayBookmark_));

    int slaveSwipe(1);
    if (masterSwipe_ == 1)
        slaveSwipe = 0;
    if (replayBookmark_ < swipes_.at(slaveSwipe)->touchPoints.count())
        touchPoints.append(swipes_.at(slaveSwipe)->touchPoints.at(replayBookmark_));
    touchEvent->setTouchPoints(touchPoints);
    canvas_->sendEvent(target_, touchEvent);
    delete touchEvent;

    replayBookmark_++;
    if (replayBookmark_ >= swipes_.at(masterSwipe_)->touchPoints.count()) {
        qDebug() << "Pinch replay finished!";
        stop();
    } else  {
        killTimer(replayTimer_);
        replayTimer_ = startTimer((swipes_.at(masterSwipe_)->touchPointDurations.at(replayBookmark_) + 5) / replaySpeedFactor_ );
    }
}

QSGItem* QDeclarativePinchGenerator::target() const
{
    return target_;
}

void QDeclarativePinchGenerator::setTarget(QSGItem* target)
{
    if (target == target_)
        return;
    target_ = target;
    stop();
    clear();
    if (canvas_)
        setState(Idle);
    else
        setState(Invalid);
    emit targetChanged();
}

Q_INVOKABLE void QDeclarativePinchGenerator::replay()
{
    if (state_ != Idle) {
        qDebug() << "Wrong state, will not replay pinch, state: " << state_;
        return;
    }
    if (swipes_.count() < SWIPES_REQUIRED) {
        qDebug() << "Too few swipes, cannot replay, amount: " << swipes_.count();
        return;
    }
    if ((swipes_.at(0)->touchPoints.count() < 2) || (swipes_.at(1)->touchPoints.count() < 2)) {
        qDebug() << "Too few touchpoints, won't replay, amount: " << swipes_.at(0)->touchPoints.count() << (swipes_.at(1)->touchPoints.count() < 2);
        return;
    }
    if (swipes_.at(0)->touchPoints.back().state() != Qt::TouchPointReleased) {
        qDebug() << "Swipe misses release event, won't replay.";
        return;
    }
    if (swipes_.at(1)->touchPoints.back().state() != Qt::TouchPointReleased) {
        qDebug() << "Swipe misses release event, won't replay.";
        return;
    }

    Q_ASSERT(swipes_.at(0)->touchPoints.count() == swipes_.at(0)->touchPointDurations.count());
    Q_ASSERT(swipes_.at(1)->touchPoints.count() == swipes_.at(1)->touchPointDurations.count());

    // 'nuff checkin' already, start replay. let the one with more swipes dictate
    if (swipes_.at(0)->touchPoints.count() >= swipes_.at(1)->touchPoints.count())
        masterSwipe_ = 0;
    else
        masterSwipe_ = 1;

    replayTimer_ = startTimer(swipes_.at(masterSwipe_)->touchPointDurations.at(0) / replaySpeedFactor_);
    replayBookmark_ = 0;
    setState(Replaying);
}

Q_INVOKABLE void QDeclarativePinchGenerator::clear()
{
    stop();
    delete activeSwipe_;
    activeSwipe_ = 0;
    if (!swipes_.isEmpty()) {
        qDeleteAll(swipes_);
        swipes_.clear();
        emit countChanged();
        emit swipesChanged();
    }
}

Q_INVOKABLE void QDeclarativePinchGenerator::stop()
{
    if (state_ != Replaying)
        return;
    // stop replay
    Q_ASSERT(replayTimer_ != -1);
    killTimer(replayTimer_);
    replayTimer_ = -1;
    setState(Idle);
}
