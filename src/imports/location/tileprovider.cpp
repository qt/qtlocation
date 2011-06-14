#include "tileprovider.h"

#include "tile.h"
#include "tilespec.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QPixmap>

TileProvider::TileProvider(QObject *parent)
    : QObject(parent)
{
    nam_ = new QNetworkAccessManager(this);
    connect(nam_,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(finished(QNetworkReply*)));
}

void TileProvider::tileRequest(const TileSpec &spec)
{
    QString url = "http://tile.openstreetmap.org/";
    url += QString::number(spec.zoom());
    url += "/";
    url += QString::number(spec.x());
    url += "/";
    url += QString::number(spec.y());
    url += ".png";

    QNetworkRequest request = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = nam_->get(request);

    map_.insert(reply, spec);
}

void TileProvider::finished(QNetworkReply *reply)
{
    if (!map_.contains(reply)) {
        reply->deleteLater();
        return;
    }

    TileSpec spec = map_.value(reply);

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray bytes = reply->readAll();

        QPixmap pixmap;
        bool result = pixmap.loadFromData(bytes, "png");

        if (result) {
            Tile tile(spec, pixmap);
            emit tileReady(tile);
        }
    } else {
        emit tileError(spec);
    }

    reply->deleteLater();
}
