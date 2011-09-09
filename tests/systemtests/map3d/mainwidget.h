#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class TileView;
class CameraWidget;

class QTextEdit;
class QPushButton;
class QLineEdit;
class QComboBox;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget(QWidget *parent = 0, int bm = 0);

public slots:
    void camera1Update();
    void camera1Capture();
    void camera2Update();
    void camera2Capture();
    void animate1Clicked();
    void animate2Clicked();
    void animationFinished();

    void runBenchmark1();

private:
    TileView *tileView_;

    CameraWidget *camera1_;
    CameraWidget *camera2_;
    QLineEdit *duration_;
    QComboBox *easing_;
    QPushButton *animate1_;
    QPushButton *animate2_;

    QPushButton *bm1Button_;

    bool exitWhenDone_;

    bool oldAutoUpdate_;
};

#endif // MAINWIDGET_H
