/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

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
