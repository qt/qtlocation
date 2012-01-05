/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
** $QT_END_LICENSE$
**
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacesearchreplyimpl.h"

#if defined(QT_PLACES_LOGGING)
    #include <QDebug>
#endif

QT_USE_NAMESPACE

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
        connect(parser, SIGNAL(finished(QPlaceJSonParser::Error,QString)),
                this, SLOT(resultReady(QPlaceJSonParser::Error,QString)));
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

void QPlaceSearchReplyImpl::setError(QPlaceReply::Error errorId, const QString &errorString)
{
    QPlaceReply::setError(errorId, errorString);
    emit error(this->error(), this->errorString());
    emit processingError(this, this->error(), this->errorString());
    setFinished(true);
    emit finished();
    emit processingFinished(this);
}

void QPlaceSearchReplyImpl::restError(QPlaceRestReply::Error errorId)
{
    if (errorId == QPlaceRestReply::Canceled) {
        this->setError(CancelError, "RequestCanceled");
    } else if (errorId == QPlaceRestReply::NetworkError) {
        this->setError(CommunicationError, "Network error");
    }
    emit error(this->error(), this->errorString());
    emit processingError(this, this->error(), this->errorString());
    setFinished(true);
    emit finished();
    emit processingFinished(this);
}

void QPlaceSearchReplyImpl::resultReady(const QPlaceJSonParser::Error &errorId,
                      const QString &errorMessage)
{
    if (errorId == QPlaceJSonParser::NoError) {
        setResults(filterSecondSearchCenter(parser->searchResults()));
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

QList<QPlaceSearchResult> QPlaceSearchReplyImpl::filterSecondSearchCenter(const QList<QPlaceSearchResult> &list)
{
    QList<QPlaceSearchResult> newList;
    foreach (QPlaceSearchResult res, list) {
        if (res.type() == QPlaceSearchResult::PlaceResult) {
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
        } else if (res.type() == QPlaceSearchResult::CorrectionResult) {
            newList.append(res);
        }
    }
    return newList;
}

