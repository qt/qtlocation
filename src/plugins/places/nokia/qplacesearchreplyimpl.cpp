#include "qplacesearchreplyimpl.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QTM_USE_NAMESPACE

/*!
    Constructor.
*/
QPlaceSearchReplyImpl::QPlaceSearchReplyImpl(QPlaceRestReply *reply, QObject *parent) :
    QPlaceSearchReply(parent),
    restReply(reply)
{
    parser = new QPlaceJSonSearchParser(this);

    if (restReply) {
        restReply->setParent(this);
        connect(restReply, SIGNAL(finished(const QString &)),
                parser, SLOT(processData(const QString &)));
        connect(restReply, SIGNAL(error(QPlaceRestReply::Error)),
                this, SLOT(restError(QPlaceRestReply::Error)));
        connect(parser, SIGNAL(finished(QPlaceJSonSearchParser::Error,QString)),
                this, SLOT(predictionsReady(QPlaceJSonSearchParser::Error,QString)));
    }
}

/*!
    Destructor.
*/
QPlaceSearchReplyImpl::~QPlaceSearchReplyImpl()
{
}

void QPlaceSearchReplyImpl::abort()
{
    if (restReply)
        restReply->cancelProcessing();
}

void QPlaceSearchReplyImpl::restError(QPlaceRestReply::Error errorId)
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

void QPlaceSearchReplyImpl::predictionsReady(const QPlaceJSonSearchParser::Error &errorId,
                      const QString &errorMessage)
{
    if (errorId == QPlaceJSonSearchParser::NoError) {
        setResults(filterSecondSearchCenter(parser->searchResults()));
    } else if (errorId == QPlaceJSonSearchParser::ParsingError) {
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

QList<QPlaceSearchResult> QPlaceSearchReplyImpl::filterSecondSearchCenter(const QList<QPlaceSearchResult> &list)
{
    QList<QPlaceSearchResult> newList;
    foreach (QPlaceSearchResult res, list) {
        if (res.type() == QPlaceSearchResult::Place) {
            bool isNotSeconSearchCenter = true;
            foreach (QPlaceCategory cat, res.place().categories()) {
                if (cat.categoryId() == "second-search-center") {
                    isNotSeconSearchCenter = false;
                    break;
                }
            }
            if (isNotSeconSearchCenter) {
                newList.append(res);
            }
        }
    }
    return newList;
}

