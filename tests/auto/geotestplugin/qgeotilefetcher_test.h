/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOTILEFETCHER_TEST_H
#define QGEOTILEFETCHER_TEST_H

#include <qgeotiledmapreply.h>

#include "qgeotilefetcher.h"
#include "qgeotilespec.h"

#include <QLocale>
#include <QPainter>
#include <QPixmap>
#include <QByteArray>
#include <QBuffer>
#include <QTimer>
#include <QDebug>
#include <QTimerEvent>

QT_USE_NAMESPACE

class TiledMapReplyTest :public QGeoTiledMapReply
{
    Q_OBJECT
public:
    TiledMapReplyTest(const QGeoTileSpec &spec, QObject *parent=0): QGeoTiledMapReply (spec, parent) {}
    void callSetError ( Error error, const QString & errorString ) {setError(error, errorString);}
    void callSetFinished ( bool finished ) { setFinished(finished);}
    void callSetCached(bool cached) { setFinished(cached);}
    void callSetMapImageData(const QByteArray &data) { setMapImageData(data); }
    void callSetMapImageFormat(const QString &format) { setMapImageFormat(format); }
    void abort() { emit aborted(); }

Q_SIGNALS:
    void aborted();
};

class QGeoTileFetcherTest: public QGeoTileFetcher
{
Q_OBJECT
public:
    QGeoTileFetcherTest(QGeoTiledMappingManagerEngine *engine, QObject *parent = 0)
        : QGeoTileFetcher(engine, parent) {}

    bool init()
    {
        if (m_parameters->contains("finishRequestImmediately"))
            finishRequestImmediately_ = qvariant_cast<bool>(m_parameters->value("finishRequestImmediately"));
        return true;
    }

    QGeoTiledMapReply* getTileImage(const QGeoTileSpec &spec)
    {
        mappingReply_ = new TiledMapReplyTest(spec, this);

        QImage im(256, 256, QImage::Format_RGB888);
        im.fill(QColor("lightgray"));
        QRectF rect;
        QString text("X: " + QString::number(spec.x()) + "\nY: " + QString::number(spec.y()) + "\nZ: " + QString::number(spec.zoom()));
        rect.setWidth(250);
        rect.setHeight(250);
        rect.setLeft(3);
        rect.setTop(3);
        QPainter painter;
        QPen pen(QColor("firebrick"));
        painter.begin(&im);
        painter.setPen(pen);
        painter.setFont( QFont("Times", 35, 10, false));
        painter.drawText(rect, text);
        // different border color for vertically and horizontally adjacent frames
        if ((spec.x() + spec.y()) % 2 == 0)
            pen.setColor(QColor("yellow"));
        pen.setWidth(5);
        painter.setPen(pen);
        painter.drawRect(0,0,255,255);
        painter.end();
        QPixmap pm = QPixmap::fromImage(im);
        QByteArray bytes;
        QBuffer buffer(&bytes);
        buffer.open(QIODevice::WriteOnly);
        pm.save(&buffer, "PNG");

        mappingReply_->callSetMapImageData(bytes);
        mappingReply_->callSetMapImageFormat("png");
        mappingReply_->callSetFinished(true);

        return static_cast<QGeoTiledMapReply*>(mappingReply_);
    }

    void setParams(const QMap<QString, QVariant> *parameters)
    {
        m_parameters = parameters;
    }

    void setTileSize(QSize tileSize)
    {
        m_tileSize = tileSize;
    }

public Q_SLOTS:
    void requestAborted()
    {
        if (timerId_) {
            killTimer(timerId_);
            timerId_ = 0;
        }
        errorString_ = "";
        errorCode_ = QGeoTiledMapReply::NoError;
    }

protected:
     void timerEvent(QTimerEvent *event)
     {
         Q_ASSERT(timerId_ == event->timerId());
         Q_ASSERT(mappingReply_);
         killTimer(timerId_);
         timerId_ = 0;
         if (errorCode_) {
             mappingReply_->callSetError(errorCode_, errorString_);
             emit tileError(mappingReply_->tileSpec(), errorString_);
        } else {
             mappingReply_->callSetError(QGeoTiledMapReply::NoError, "no error");
             mappingReply_->callSetFinished(true);
         }
         // emit finished(mappingReply_); todo tileFinished
     }

private:
    bool finishRequestImmediately_;
    TiledMapReplyTest* mappingReply_;
    int timerId_;
    QGeoTiledMapReply::Error errorCode_;
    QString errorString_;
    const QMap<QString, QVariant> *m_parameters;
    QSize m_tileSize;
};

#endif
