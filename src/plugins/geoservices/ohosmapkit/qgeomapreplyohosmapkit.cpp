// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/qmetaobject.h>
#include <qgeomapreplyohosmapkit.h>
#include <qohosmapkitcommon.h>

QT_BEGIN_NAMESPACE

namespace {

const char pngSignature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00};

class QGeoMapReplyOhosMapKit : public QGeoTiledMapReply
{
public:
    explicit QGeoMapReplyOhosMapKit(
        QNetworkReply *reply, const QGeoTileSpec &spec, QObject *parent);

private:
    void onNetworkReplyFinished();
    void onNetworkReplyError(QNetworkReply::NetworkError error);
};

QGeoMapReplyOhosMapKit::QGeoMapReplyOhosMapKit(
    QNetworkReply *reply, const QGeoTileSpec &spec, QObject *parent)
    : QGeoTiledMapReply(spec, parent)
{
    if (reply == nullptr) {
        setError(QGeoTiledMapReply::UnknownError, OhosMapKit::msgNullReply());
        return;
    }

    connect(reply, &QNetworkReply::finished, this, &QGeoMapReplyOhosMapKit::onNetworkReplyFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, &QGeoMapReplyOhosMapKit::onNetworkReplyError);
    connect(this, &QGeoTiledMapReply::aborted, reply, &QNetworkReply::abort);
    connect(this, &QObject::destroyed, reply, &QObject::deleteLater);
}

void QGeoMapReplyOhosMapKit::onNetworkReplyFinished()
{
    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        setError(
            QGeoTiledMapReply::CommunicationError,
            OhosMapKit::msgReplyError(reply->errorString()));
        return;
    }

    const auto imageData = reply->readAll();

    if (imageData.startsWith(pngSignature)) {
        setMapImageFormat(QStringLiteral("png"));
        setMapImageData(imageData);
        setFinished(true);
    } else {
        setError(QGeoTiledMapReply::ParseError, tr("Invalid format of tile data"));
    }
}

void QGeoMapReplyOhosMapKit::onNetworkReplyError(QNetworkReply::NetworkError error)
{
    auto *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
    if (error == QNetworkReply::OperationCanceledError)
        setFinished(true);
    else
        setError(QGeoTiledMapReply::CommunicationError, reply->errorString());
}

}

QGeoTiledMapReply *makeGeoTiledMapReplyOhosMapKit(
    QNetworkReply *reply, const QGeoTileSpec &spec, QObject *parent)
{
    return new QGeoMapReplyOhosMapKit(reply, spec, parent);
}

QT_END_NAMESPACE
