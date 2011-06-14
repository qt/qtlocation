#ifndef TILEPROVIDER_H
#define TILEPROVIDER_H

#include <QObject>
#include <QHash>

#include "tilespec.h"

class Tile;
class QNetworkAccessManager;
class QNetworkReply;

class TileProvider : public QObject
{
    Q_OBJECT
public:
    TileProvider(QObject *parent = 0);

public slots:
    void tileRequest(const TileSpec &spec);

private slots:
    void finished(QNetworkReply *reply);

signals:
    void tileReady(const Tile &tile);
    void tileError(const TileSpec &spec);

private:
    QNetworkAccessManager *nam_;
    QHash<QNetworkReply*, TileSpec> map_;
};

#endif // TILEPROVIDER_H
