#ifndef QPLACERESTREPLY_H
#define QPLACERESTREPLY_H

#include <QObject>
#include <QHash>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include "../qplaceglobal.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceRestReply : public QObject
{
    Q_OBJECT
public:
    explicit QPlaceRestReply(QNetworkReply *reply, QObject *parent = 0);
    enum Error {
        NoError,
        Canceled,
        NetworkError
    };

    ~QPlaceRestReply();

    void cancelProcessing();

signals:
    void finished(const QString &reply);
    void error(QPlaceRestReply::Error error);

private slots:
    void replyFinished();
    void replyError(QNetworkReply::NetworkError error);

private:
    QNetworkReply *mReply;
    bool mCanceled;

friend class QRestManager;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACERESTREPLY_H
