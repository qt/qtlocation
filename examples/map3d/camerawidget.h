#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>

#include "cameradata.h"

class QLineEdit;
class QPushButton;

class CameraWidget : public QWidget
{
    Q_OBJECT
public:
    CameraWidget(QWidget *parent = 0);

    CameraData camera() const;

public slots:
    void setCamera(const CameraData& camera);

private slots:
    void updateCamera();

signals:
    void updated();
    void capture();

private:
    void setupUi();

    CameraData cam_;

    QLineEdit *lat_;
    QLineEdit *lon_;
    QLineEdit *distance_;
    QLineEdit *zoomLevel_;
    QLineEdit *zoomFactor_;
    QLineEdit *bearing_;
    QLineEdit *tilt_;
    QLineEdit *roll_;
    QPushButton *update_;
    QPushButton *capture_;
};

#endif // CAMERAWIDGET_H
