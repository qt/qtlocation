#include "camerawidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>

CameraWidget::CameraWidget(QWidget *parent)
        : QWidget(parent)
{
    setupUi();
}

void CameraWidget::setupUi()
{
    QLabel *latLabel = new QLabel("Latitude");
    lat_ = new QLineEdit();

    QLabel *lonLabel = new QLabel("Longitude");
    lon_ = new QLineEdit();

    QLabel *distanceLabel = new QLabel("Distance");
    distance_ = new QLineEdit();

    QLabel *zoomLevelLabel = new QLabel("Zoom Level");
    zoomLevel_ = new QLineEdit();

    QLabel *zoomFactorLabel = new QLabel("Zoom Factor");
    zoomFactor_ = new QLineEdit();

    QLabel *bearingLabel = new QLabel("Bearing");
    bearing_ = new QLineEdit();

    QLabel *tiltLabel = new QLabel("Tilt");
    tilt_ = new QLineEdit();

    QLabel *rollLabel = new QLabel("Roll");
    roll_ = new QLineEdit();

    QGridLayout *grid = new QGridLayout();
    grid->addWidget(latLabel, 0, 0);
    grid->addWidget(lat_, 0, 1);
    grid->addWidget(lonLabel, 1, 0);
    grid->addWidget(lon_, 1, 1);
    grid->addWidget(distanceLabel, 2, 0);
    grid->addWidget(distance_, 2, 1);
    grid->addWidget(zoomLevelLabel, 3, 0);
    grid->addWidget(zoomLevel_, 3, 1);
    grid->addWidget(zoomFactorLabel, 4, 0);
    grid->addWidget(zoomFactor_, 4, 1);
    grid->addWidget(bearingLabel, 5, 0);
    grid->addWidget(bearing_, 5, 1);
    grid->addWidget(tiltLabel, 6, 0);
    grid->addWidget(tilt_, 6, 1);
    grid->addWidget(rollLabel, 7, 0);
    grid->addWidget(roll_, 7, 1);

    update_ = new QPushButton("Update");
    connect(update_,
            SIGNAL(clicked()),
            this,
            SLOT(updateCamera()));

    capture_ = new QPushButton("Capture");
    connect(capture_,
            SIGNAL(clicked()),
            this,
            SIGNAL(capture()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(grid);
    layout->addWidget(update_);
    layout->addWidget(capture_);

    setLayout(layout);
}

CameraData CameraWidget::camera() const
{
    return cam_;
}

void CameraWidget::setCamera(const CameraData& camera)
{
    cam_ = camera;

    lat_->setText(QString::number(cam_.center().latitude(), 'g', 10));
    lon_->setText(QString::number(cam_.center().longitude(), 'g', 10));
    distance_->setText(QString::number(cam_.distance(), 'g', 4));
    zoomLevel_->setText(QString::number(cam_.zoomLevel()));
    zoomFactor_->setText(QString::number(cam_.zoomFactor()));
    bearing_->setText(QString::number(cam_.bearing(), 'g', 10));
    tilt_->setText(QString::number(cam_.tilt(), 'g', 10));
    roll_->setText(QString::number(cam_.roll(), 'g', 10));
}

void CameraWidget::updateCamera()
{
    double lat = lat_->text().toDouble();
    double lon = lon_->text().toDouble();
    cam_.setCenter(QGeoCoordinate(lat, lon));
    if (!zoomFactor_->text().isEmpty()) {
        cam_.setZoomFactor(zoomFactor_->text().toDouble());
    } else {
        cam_.setZoomLevel(zoomLevel_->text().toInt());
        cam_.setDistance(distance_->text().toDouble());
    }
    cam_.setBearing(bearing_->text().toDouble());
    cam_.setTilt(tilt_->text().toDouble());
    cam_.setRoll(roll_->text().toDouble());

    emit updated();
}
