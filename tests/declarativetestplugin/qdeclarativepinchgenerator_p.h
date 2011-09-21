#ifndef QDECLARATIVEPINCHGENERATOR_H
#define QDECLARATIVEPINCHGENERATOR_H

#include <QtDeclarative/QSGItem>
#include <QMouseEvent>
#include <QElapsedTimer>
#include <QTouchEvent>
#include <QSGCanvas>
#include <QKeyEvent>
#include <QList>
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

class QDeclarativePinchGenerator : public QSGItem
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(qreal replaySpeedFactor READ replaySpeedFactor WRITE setReplaySpeedFactor NOTIFY replaySpeedFactorChanged)
    Q_PROPERTY(QSGItem* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QList<QObject*> swipe1 READ swipe1 NOTIFY swipesChanged)
    Q_PROPERTY(QList<QObject*> swipe2 READ swipe2 NOTIFY swipesChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    QDeclarativePinchGenerator();
    ~QDeclarativePinchGenerator();

    QString state() const;
    int count() const;
    QSGItem* target() const;
    void setTarget(QSGItem* target);
    qreal replaySpeedFactor() const;
    void setReplaySpeedFactor(qreal factor);
    bool enabled() const;
    void setEnabled(bool enabled);

    QList<QObject*> swipe1();
    QList<QObject*> swipe2();

    Q_INVOKABLE void replay();
    Q_INVOKABLE void clear();
    Q_INVOKABLE void stop();

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
    // from QSGItem
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
    QTouchEvent::TouchPoint convertToPrimary(QTouchEvent::TouchPoint original);

private:
    QSGItem* target_;
    GeneratorState state_;
    QSGCanvas* canvas_;
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
