#include "qplacerestreply.h"

#include <QtNetwork>
#include <QHash>

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

using namespace QT_PLACES_NAMESPACE;

/*!
    Constructor.
*/
QPlaceRestReply::QPlaceRestReply(QNetworkReply *reply, QObject *parent) :
    QObject(parent),
    mReply(reply),
    mCanceled(false)
{
    if (mReply) {
        connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(replyError(QNetworkReply::NetworkError)));
        connect(mReply, SIGNAL(finished()), this, SLOT(replyFinished()));
    }
}

/*!
    Destructor.
*/
QPlaceRestReply::~QPlaceRestReply()
{
#if defined(QT_PLACES_LOGGING)
    qDebug() << "QRestReply::~QRestReply";
#endif
    if (mReply) {
        if (!mReply->isFinished()) {
            mReply->disconnect(this);
            mReply->abort();
        }
        mReply->deleteLater();
    }
}

/*!
    Emits signal 'replyReady' if request is done.
*/
void QPlaceRestReply::replyFinished()
{
#if defined(QT_PLACES_LOGGING)
    qDebug() << "QRestReply::replyFinished";
#endif
    if (!mCanceled && (mReply->error() == QNetworkReply::NoError)) {
        QByteArray response = mReply->readAll();
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");
        QString string = codec->toUnicode(response);
#if defined(QT_PLACES_LOGGING)
        qDebug() << "Data received: " + string;
#endif
        emit finished(string);
    } else if (mCanceled) {
#if defined(QT_PLACES_LOGGING)
        qDebug() << "Canceled";
#endif
        emit error(QPlaceRestReply::Canceled);
    } else {
#if defined(QT_PLACES_LOGGING)
        qDebug() << "Network Error - " + QString::number(error);
#endif
        emit error(QPlaceRestReply::NetworkError);
    }

    mReply->deleteLater();
    mReply = NULL;
}

void QPlaceRestReply::replyError(QNetworkReply::NetworkError error)
{
#if defined(QT_PLACES_LOGGING)
    qDebug() << "QRestReply::replyError: Network Error - " + QString::number(error);
#else
    Q_UNUSED(error);
#endif
}

void QPlaceRestReply::cancelProcessing()
{
#if defined(QT_PLACES_LOGGING)
    qDebug() << "QRestReply::cancelProcessing";
#endif
    mCanceled = true;
    if (mReply && mReply->isRunning()) {
        mReply->abort();
    }
}
