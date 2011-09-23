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

#include "mainwidget.h"

#include "tilecache.h"
#include "tileview.h"
#include "camerawidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QComboBox>

#include <QApplication>

#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

MainWidget::MainWidget(QWidget *parent, int bm)
        : QWidget(parent),
          exitWhenDone_(false)
{
    TileCache *cache = new TileCache();

    tileView_ = new TileView(cache);
    tileView_->setMinimumSize(450, 450);

    camera1_ = new CameraWidget();

    connect(camera1_,
            SIGNAL(updated()),
            this,
            SLOT(camera1Update()));
    connect(camera1_,
            SIGNAL(capture()),
            this,
            SLOT(camera1Capture()));

    camera2_ = new CameraWidget();

    animate1_ = new QPushButton("Go");
    connect(animate1_,
            SIGNAL(clicked()),
            this,
            SLOT(animate1Clicked()));

    animate2_ = new QPushButton("Go");
    connect(animate2_,
            SIGNAL(clicked()),
            this,
            SLOT(animate2Clicked()));

    QLabel *durationLabel = new QLabel("Duration");
    duration_ = new QLineEdit();
    duration_->setText("2000");

    QLabel *easingLabel = new QLabel("Easing Curve");
    easing_ = new QComboBox();
    easing_->setEditable(false);

    easing_->addItem("Linear", 0);

    easing_->addItem("InQuad", 1);
    easing_->addItem("OutQuad", 2);
    easing_->addItem("InOutQuad", 3);
    easing_->addItem("OutInQuad", 4);

    easing_->addItem("InCubic", 5);
    easing_->addItem("OutCubic", 6);
    easing_->addItem("InOutCubic", 7);
    easing_->addItem("OutInCubic", 8);

    easing_->addItem("InQuart", 9);
    easing_->addItem("OutQuart", 10);
    easing_->addItem("InOutQuart", 11);
    easing_->addItem("OutInQuart", 12);

    easing_->addItem("InQunit", 13);
    easing_->addItem("OutQunit", 14);
    easing_->addItem("InOutQunit", 15);
    easing_->addItem("OutInQunit", 16);

    easing_->addItem("InSine", 17);
    easing_->addItem("OutSine", 18);
    easing_->addItem("InOutSine", 19);
    easing_->addItem("OutInSine", 20);

    easing_->addItem("InExpo", 21);
    easing_->addItem("OutExpo", 22);
    easing_->addItem("InOutExpo", 23);
    easing_->addItem("OutInExpo", 24);

    easing_->addItem("InCirc", 25);
    easing_->addItem("OutCirc", 26);
    easing_->addItem("InOutCirc", 27);
    easing_->addItem("OutInCirc", 28);

    easing_->addItem("InElastic", 29);
    easing_->addItem("OutElastic", 30);
    easing_->addItem("InOutElastic", 31);
    easing_->addItem("OutInElastic", 32);

    easing_->addItem("InBounce", 33);
    easing_->addItem("OutBounce", 34);
    easing_->addItem("InOutBounce", 35);
    easing_->addItem("OutInBounce", 36);

    easing_->addItem("InBack", 37);
    easing_->addItem("OutBack", 38);
    easing_->addItem("InOutBack", 39);
    easing_->addItem("OutInBack", 40);

    connect(camera2_,
            SIGNAL(updated()),
            this,
            SLOT(camera2Update()));
    connect(camera2_,
            SIGNAL(capture()),
            this,
            SLOT(camera2Capture()));

    bm1Button_ = new QPushButton("Benchmark 1");
    connect(bm1Button_,
            SIGNAL(clicked()),
            this,
            SLOT(runBenchmark1()));

    QGridLayout *animateLayout = new QGridLayout;
    animateLayout->addWidget(durationLabel, 0, 0);
    animateLayout->addWidget(duration_, 0, 1);
    animateLayout->addWidget(easingLabel, 1, 0);
    animateLayout->addWidget(easing_, 1, 1);

    QGridLayout *cameraLayout = new QGridLayout;
    cameraLayout->addWidget(camera1_, 0, 0);
    cameraLayout->addWidget(camera2_, 0, 1);
    cameraLayout->addLayout(animateLayout, 1, 0, 1, 2);
    cameraLayout->addWidget(animate1_, 2, 0);
    cameraLayout->addWidget(animate2_, 2, 1);
    cameraLayout->addWidget(bm1Button_, 3, 0);
    cameraLayout->setRowStretch(4, 1);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(cameraLayout, 0);
    layout->addWidget(tileView_, 1);

    CameraData c1;
    camera1_->setCamera(c1);
    CameraData c2;
    c2.setCenter(QGeoCoordinate(-19.2, 146.75));
    c2.setZoomFactor(8.0);
    camera2_->setCamera(c2);

    setLayout(layout);

    if (bm == 1) {
        exitWhenDone_ = true;
        QTimer::singleShot(0, this, SLOT(runBenchmark1()));
    }
}

void MainWidget::runBenchmark1()
{
    oldAutoUpdate_ = tileView_->map()->autoUpdate();
    tileView_->map()->setAutoUpdate(true);

    CameraData newCameraData;
    newCameraData.setCenter(QGeoCoordinate(-19.2, 146.75));
    newCameraData.setZoomFactor(8.0);
    // hack to get the internal projection set in the cameradata object
    tileView_->map()->setCameraData(newCameraData);
    CameraData newCamera = tileView_->map()->cameraData();

    CameraData oldCameraData;
    oldCameraData.setCenter(QGeoCoordinate(-27.5, 153));
    oldCameraData.setZoomFactor(4.0);
    // hack to get the internal projection set in the cameradata object
    tileView_->map()->setCameraData(oldCameraData);
    CameraData oldCamera = tileView_->map()->cameraData();

    QPropertyAnimation *a20f = new QPropertyAnimation(tileView_->map(), "camera", this);
    a20f->setStartValue(QVariant::fromValue(oldCamera));
    a20f->setEndValue(QVariant::fromValue(newCamera));
    a20f->setDuration(20000);

    QPropertyAnimation *a20b = new QPropertyAnimation(tileView_->map(), "camera", this);
    a20b->setStartValue(QVariant::fromValue(newCamera));
    a20b->setEndValue(QVariant::fromValue(oldCamera));
    a20b->setDuration(20000);

    QPropertyAnimation *a2f = new QPropertyAnimation(tileView_->map(), "camera", this);
    a2f->setStartValue(QVariant::fromValue(oldCamera));
    a2f->setEndValue(QVariant::fromValue(newCamera));
    a2f->setDuration(2000);

    QPropertyAnimation *a2b = new QPropertyAnimation(tileView_->map(), "camera", this);
    a2b->setStartValue(QVariant::fromValue(newCamera));
    a2b->setEndValue(QVariant::fromValue(oldCamera));
    a2b->setDuration(2000);

    QSequentialAnimationGroup *s2 = new QSequentialAnimationGroup(this);
    s2->addAnimation(a2f);
    s2->addAnimation(a2b);
    s2->setLoopCount(10);

    QSequentialAnimationGroup *s = new QSequentialAnimationGroup(this);
    s->addAnimation(a20f);
    s->addAnimation(a20b);
    s->addAnimation(s2);

    if (exitWhenDone_) {
        connect(s,
                SIGNAL(finished()),
                qApp,
                SLOT(quit()));
    }

    QTimer::singleShot(0, s, SLOT(start()));

}

void MainWidget::camera1Update()
{
    tileView_->map()->setCameraData(camera1_->camera());
    if (!tileView_->map()->autoUpdate())
        tileView_->map()->update();
}

void MainWidget::camera2Update()
{
    tileView_->map()->setCameraData(camera2_->camera());
    if (!tileView_->map()->autoUpdate())
        tileView_->map()->update();
}

void MainWidget::camera1Capture()
{
    camera1_->setCamera(tileView_->map()->cameraData());
}

void MainWidget::camera2Capture()
{
    camera2_->setCamera(tileView_->map()->cameraData());
}

void MainWidget::animate1Clicked()
{
    oldAutoUpdate_ = tileView_->map()->autoUpdate();
    tileView_->map()->setAutoUpdate(true);
    CameraData oldCamera = tileView_->map()->cameraData();
    CameraData newCamera = camera1_->camera();

    QPropertyAnimation *a = new QPropertyAnimation(tileView_->map(), "camera", this);

    connect(a,
            SIGNAL(finished()),
            this,
            SLOT(animationFinished()));

    a->setStartValue(QVariant::fromValue(oldCamera));
    a->setEndValue(QVariant::fromValue(newCamera));
    a->setDuration(duration_->text().toInt());
    a->setEasingCurve(QEasingCurve::Type(easing_->itemData(easing_->currentIndex()).toInt()));

    QTimer::singleShot(0, a, SLOT(start()));
}

void MainWidget::animate2Clicked()
{
    oldAutoUpdate_ = tileView_->map()->autoUpdate();
    tileView_->map()->setAutoUpdate(true);
    CameraData oldCamera = tileView_->map()->cameraData();
    CameraData newCamera = camera2_->camera();

    QPropertyAnimation *a = new QPropertyAnimation(tileView_->map(), "camera", this);

    connect(a,
            SIGNAL(finished()),
            this,
            SLOT(animationFinished()));

    a->setStartValue(QVariant::fromValue(oldCamera));
    a->setEndValue(QVariant::fromValue(newCamera));
    a->setDuration(duration_->text().toInt());
    a->setEasingCurve(QEasingCurve::Type(easing_->itemData(easing_->currentIndex()).toInt()));

//    TilePlan *tp = new TilePlan(a);
//    tileView_->map()->mapCamera()->prepareAnimation(tp);
//    delete tp;

    QTimer::singleShot(0, a, SLOT(start()));
}

void MainWidget::animationFinished()
{
    tileView_->map()->setAutoUpdate(oldAutoUpdate_);
}
