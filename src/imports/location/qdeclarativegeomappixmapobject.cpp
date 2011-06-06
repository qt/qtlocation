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

#include "qdeclarativegeomappixmapobject_p.h"

#include <QBrush>
#include <QUrl>
#include <QFile>
#include <QIODevice>
#include <QImage>
#include <QImageReader>

#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapImage

    \brief The MapImage element displays an image on a map.
    \inherits QGeoMapPixmapObject

    \ingroup qml-location-maps

    The image loaded from \l source will be drawn \l offset.x and
    \l offset.y pixels away from the on-screen position of \l coordinate.

    If \l source does not point to an image or \l coordinate is
    invalid nothing will be displayed.

    The status of the image loading can be monitored via \l status.

    An example of map image:
    \snippet examples/declarative-location/landmarkmap/landmarkmap.qml MapImage

    The MapImage element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoMapPixmapObject::QDeclarativeGeoMapPixmapObject(QDeclarativeItem *parent)
    : QDeclarativeGeoMapObject(parent),
      pixmap_(0),
      reply_(0),
      status_(QDeclarativeGeoMapPixmapObject::Null)
{
    pixmap_ = new QGeoMapPixmapObject();
    setMapObject(pixmap_);

    connect(pixmap_,
            SIGNAL(offsetChanged(QPoint)),
            this,
            SIGNAL(offsetChanged(QPoint)));
}

QDeclarativeGeoMapPixmapObject::~QDeclarativeGeoMapPixmapObject()
{
    delete pixmap_;
}

/*!
    \qmlproperty Coordinate MapImage::coordinate

    This property holds the coordinate at which to anchor the image.
*/

void QDeclarativeGeoMapPixmapObject::setCoordinate(QDeclarativeCoordinate *coordinate)
{
    if (!coordinate || coordinate == coordinate_)
        return;
    coordinate_ = coordinate;

    connect(coordinate_,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(coordinateLatitudeChanged(double)));
    connect(coordinate_,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(coordinateLongitudeChanged(double)));
    connect(coordinate_,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(coordinateAltitudeChanged(double)));

    pixmap_->setCoordinate(coordinate->coordinate());
    emit coordinateChanged(coordinate_);
}

QDeclarativeCoordinate* QDeclarativeGeoMapPixmapObject::coordinate()
{
    return coordinate_;
}

void QDeclarativeGeoMapPixmapObject::coordinateLatitudeChanged(double /*latitude*/)
{
    pixmap_->setCoordinate(coordinate_->coordinate());
}

void QDeclarativeGeoMapPixmapObject::coordinateLongitudeChanged(double /*longitude*/)
{
    pixmap_->setCoordinate(coordinate_->coordinate());
}

void QDeclarativeGeoMapPixmapObject::coordinateAltitudeChanged(double /*altitude*/)
{
    pixmap_->setCoordinate(coordinate_->coordinate());
}

/*!
    \qmlproperty int MapImage::offset.x
    \qmlproperty int MapImage::offset.y

    These properties hold the offset from the on-screen position of
    \l coordinate at which the image should be displayed.

    They both default to 0.
*/

QPoint QDeclarativeGeoMapPixmapObject::offset() const
{
    return pixmap_->offset();
}

void QDeclarativeGeoMapPixmapObject::setOffset(const QPoint &offset)
{
    pixmap_->setOffset(offset);
}

/*!
    \qmlproperty url MapImage::source

    This property holds the URL describing the location of the image to
    display.

    The URL can be absolute or relative to where the QML file
    was loaded from, and can be a local file, a file embedded within
    a Qt Resource bundle, or a file retrieved from the network.
*/

void QDeclarativeGeoMapPixmapObject::setSource(const QUrl &source)
{
    if (source_ == source)
        return;

    source_ = source;

    load();

    emit sourceChanged(source);
}

QUrl QDeclarativeGeoMapPixmapObject::source() const
{
    return source_;
}

/*!
    \qmlproperty enumeration MapImage::status

    This property holds the status of image loading.  It can be one of:
    \list
    \o MapImage.Null - no image has been set
    \o MapImage.Ready - the image has been loaded
    \o MapImage.Loading - the image is currently being loaded
    \o MapImage.Error - an error occurred while loading the image
    \endlist
*/

QDeclarativeGeoMapPixmapObject::Status QDeclarativeGeoMapPixmapObject::status() const
{
    return status_;
}


void QDeclarativeGeoMapPixmapObject::setStatus(const QDeclarativeGeoMapPixmapObject::Status status)
{
    if (status_ == status)
        return;

    status_ = status;

    emit statusChanged(status_);
}

void QDeclarativeGeoMapPixmapObject::load()
{
    // need to deal with absolute / relative local / remote files

    QUrl url = QDeclarativeEngine::contextForObject(this)->resolvedUrl(source_);

    QString path;

    if (url.scheme().compare(QLatin1String("qrc"), Qt::CaseInsensitive) == 0) {
        if (url.authority().isEmpty())
            path = QLatin1Char(':') + url.path();
    } else if (url.scheme().compare(QLatin1String("file"), Qt::CaseInsensitive) == 0) {
        path = url.toLocalFile();
    }

    if (!path.isEmpty()) {
        QFile f(path);
        if (f.open(QIODevice::ReadOnly)) {
            QImage image;
            QImageReader imageReader(&f);
            if (imageReader.read(&image)) {
                pixmap_->setPixmap(QPixmap::fromImage(image));
                setStatus(QDeclarativeGeoMapPixmapObject::Ready);
            } else {
                pixmap_->setPixmap(QPixmap());
                setStatus(QDeclarativeGeoMapPixmapObject::Error);
                //qWarning() << "image read fail";
            }
        } else {
            pixmap_->setPixmap(QPixmap());
            setStatus(QDeclarativeGeoMapPixmapObject::Error);
            //qWarning() << "file open fail";
        }
    } else {
        if (reply_) {
            reply_->abort();
            reply_->deleteLater();
            reply_ = 0;
        }

        QDeclarativeEngine *engine = QDeclarativeEngine::contextForObject(this)->engine();
        if (engine) {
            QNetworkAccessManager *nam = engine->networkAccessManager();
            reply_ = nam->get(QNetworkRequest(url));

            if (reply_->isFinished()) {
                if (reply_->error() == QNetworkReply::NoError) {
                    finished();
                } else {
                    error(reply_->error());
                }
                delete reply_;
                reply_ = 0;
                return;
            }

            setStatus(QDeclarativeGeoMapPixmapObject::Loading);

            connect(reply_,
                    SIGNAL(finished()),
                    this,
                    SLOT(finished()));
            connect(reply_,
                    SIGNAL(error(QNetworkReply::NetworkError)),
                    this,
                    SLOT(error(QNetworkReply::NetworkError)));

        } else {
            pixmap_->setPixmap(QPixmap());
            setStatus(QDeclarativeGeoMapPixmapObject::Error);
            //qWarning() << "null engine fail";
        }
    }
}

void QDeclarativeGeoMapPixmapObject::finished()
{
    if (reply_->error() != QNetworkReply::NoError) {
        reply_->deleteLater();
        reply_ = 0;
        return;
    }

    QImage image;
    QImageReader imageReader(reply_);
    if (imageReader.read(&image)) {
        pixmap_->setPixmap(QPixmap::fromImage(image));
        setStatus(QDeclarativeGeoMapPixmapObject::Ready);
    } else {
        pixmap_->setPixmap(QPixmap());
        setStatus(QDeclarativeGeoMapPixmapObject::Error);
        //qWarning() << "image read fail";
    }

    reply_->deleteLater();
    reply_ = 0;
}

void QDeclarativeGeoMapPixmapObject::error(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    reply_->deleteLater();
    reply_ = 0;

    pixmap_->setPixmap(QPixmap());
    setStatus(QDeclarativeGeoMapPixmapObject::Error);
    //qWarning() << "network error fail";
}

/*!
    \qmlproperty int MapImage::z

    This property holds the z-value of the image.

    Map objects are drawn in z-value order, and objects with the
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapImage::visible

    This property holds a boolean corresponding to whether or not the
    image is visible.
*/

#include "moc_qdeclarativegeomappixmapobject_p.cpp"

QTM_END_NAMESPACE

