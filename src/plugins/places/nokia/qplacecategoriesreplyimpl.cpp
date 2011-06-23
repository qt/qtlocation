#include "qplacecategoriesreplyimpl.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

/*!
    Constructor.
*/
QPlaceCategoriesReplyImpl::QPlaceCategoriesReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceReply(parent),
    restReply(reply)
{
    parser = new QPlaceJSonCategoriesParser(this);

    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(QString)),
                parser, SLOT(processData(QString)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
        connect(parser, SIGNAL(finished(QPlaceJSonParser::Error,QString)),
                this, SLOT(resultReady(QPlaceJSonParser::Error,QString)));
    }
}

/*!
    Destructor.
*/
QPlaceCategoriesReplyImpl::~QPlaceCategoriesReplyImpl()
{
}

QList<QPlaceCategory> QPlaceCategoriesReplyImpl::categories()
{
    return m_categories;
}

void QPlaceCategoriesReplyImpl::abort()
{
    if (restReply) {
        restReply->cancelProcessing();
    }
}

void QPlaceCategoriesReplyImpl::restError(QPlaceRestReply::Error errorId)
{
    if (errorId == QPlaceRestReply::Canceled) {
        this->setError(CancelError, "ReauestCanceled");
    } else if (errorId == QPlaceRestReply::NetworkError) {
        this->setError(CommunicationError, "Network error");
    }
    emit error(this->error(), this->errorString());
    emit processingError(this, this->error(), this->errorString());
    setFinished(true);
    emit finished();
    emit processingFinished(this);
}

void QPlaceCategoriesReplyImpl::resultReady(const QPlaceJSonParser::Error &errorId,
                      const QString &errorMessage)
{
    if (errorId == QPlaceJSonParser::NoError) {
        m_categories = parser->resultCategories();
    } else if (errorId == QPlaceJSonParser::ParsingError) {
        setError(ParseError, errorMessage);
        emit error(this->error(), this->errorString());
        emit processingError(this, ParseError, errorMessage);
    }
    setFinished(true);
    emit finished();
    emit processingFinished(this);
    parser->deleteLater();
    parser = NULL;
    restReply->deleteLater();
    restReply = NULL;
}
