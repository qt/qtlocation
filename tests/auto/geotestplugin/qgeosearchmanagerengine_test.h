/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
****************************************************************************/

#ifndef QGEOSEARCHMANAGERENGINE_TEST_H
#define QGEOSEARCHMANAGERENGINE_TEST_H

#include <qgeoserviceprovider.h>
#include <qgeosearchmanagerengine.h>
#include <QLocale>
#include <qlandmarkmanager.h>
#include <qgeoaddress.h>
#include <qgeolocation.h>
#include <qgeosearchreply.h>

#include <QTimer>
#include <QTimerEvent>

QTM_USE_NAMESPACE


class SearchReplyTest :public QGeoSearchReply
{
    Q_OBJECT
public:
    SearchReplyTest(QObject *parent=0):QGeoSearchReply (parent) {}

    void  callAddLocation ( const QGeoLocation & location ) {addLocation(location);}
    void  callSetError ( Error error, const QString & errorString ) {setError(error, errorString);}
    void  callSetFinished ( bool finished ) {setFinished(finished);}
    void  callSetLimit ( int limit ) {setLimit(limit);}
    void  callSetOffset ( int offset ) {setOffset(offset);}
    void  callSetLocations ( const QList<QGeoLocation> & locations ) {setLocations(locations);}
    void  callSetViewport ( QGeoBoundingArea * viewport ) {setViewport(viewport);}
    void abort() {
        emit aborted();
    }
Q_SIGNALS:
    void aborted();
};

class QGeoSearchManagerEngineTest: public QGeoSearchManagerEngine

{
Q_OBJECT
public:
    QGeoSearchManagerEngineTest(const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error, QString *errorString) :
        QGeoSearchManagerEngine(parameters),
        validateWellKnownValues_(false),
        finishRequestImmediately_(true),
        supported_(true),
        searchReply_(0),
        timerId_(0),
        errorCode_(QGeoSearchReply::NoError)
    {
        Q_UNUSED(error)
        Q_UNUSED(errorString)
        if (parameters.contains("supported"))
            supported_ = qvariant_cast<bool>(parameters.value("supported"));
        if (parameters.contains("finishRequestImmediately"))
            finishRequestImmediately_ = qvariant_cast<bool>(parameters.value("finishRequestImmediately"));
        if (parameters.contains("validateWellKnownValues"))
            validateWellKnownValues_ = qvariant_cast<bool>(parameters.value("validateWellKnownValues"));

        setSupportsGeocoding(supported_);
        setSupportsReverseGeocoding(supported_);
        setLocale(*(new QLocale (QLocale::German, QLocale::Germany)));
    }

    QGeoSearchReply*  geocode ( const QGeoAddress & address, QGeoBoundingArea * bounds )
    {
        searchReply_ = new SearchReplyTest();
        connect(searchReply_, SIGNAL(aborted()), this, SLOT(requestAborted()));
        searchReply_->callSetViewport(bounds);

        if (address.street().startsWith("error")) {
            errorString_ = address.street();
            errorCode_ = (QGeoSearchReply::Error)address.county().toInt();
        } else {
            errorString_ = "";
            errorCode_ = QGeoSearchReply::NoError;
        }
        // 1. Check if we are to validate values
        if (validateWellKnownValues_) {
            if (address.street() != "wellknown street") {
                 searchReply_->callSetError(QGeoSearchReply::EngineNotSetError, address.street());
            } else {
                searchReply_->callSetError(QGeoSearchReply::NoError,address.street());
            }
        }

        // 2. Set the locations into the reply
        setLocations(searchReply_, address);

        // 3. Finish the request
        if (finishRequestImmediately_) {
            // check if we should finish with error
            if (errorCode_) {
                searchReply_->callSetError(errorCode_, errorString_);
            } else {
                searchReply_->callSetFinished(true);
            }
        } else {
            // we only allow serialized requests in QML - previous must have been aborted
            Q_ASSERT(timerId_ == 0);
            timerId_ = startTimer(200);
        }
        return static_cast<QGeoSearchReply*>(searchReply_);
    }

public Q_SLOTS:
    void requestAborted()
    {
        if (timerId_) {
            killTimer(timerId_);
            timerId_ = 0;
        }
        errorString_ = "";
        errorCode_ = QGeoSearchReply::NoError;
    }

public:
    void setLocations(SearchReplyTest* reply, const QString searchString, int limit )
    {
        for (int i = 0; i < limit; ++i) {
            QGeoLocation location;
            QGeoAddress address;
            address.setStreet(searchString);
            location.setAddress(address);
            reply->callAddLocation(location);
        }
    }

    void setLocations(SearchReplyTest* reply, const QGeoAddress& address)
    {
        int count = address.county().toInt();

        for (int i = 0; i < count; ++i) {
            QGeoLocation location;
            location.setAddress(address);
            reply->callAddLocation(location);
        }
    }

    void setLocations(SearchReplyTest* reply, const QGeoCoordinate & coordinate)
    {
        for (int i = 0; i < coordinate.longitude(); ++i) {
            QGeoLocation location;
            location.setCoordinate(coordinate);
            reply->callAddLocation(location);
        }
    }

    QGeoSearchReply*  reverseGeocode ( const QGeoCoordinate & coordinate, QGeoBoundingArea * bounds )
    {
        searchReply_ = new SearchReplyTest();
        connect(searchReply_, SIGNAL(aborted()), this, SLOT(requestAborted()));

        setLocations(searchReply_, coordinate);
        searchReply_->callSetViewport(bounds);

        if (coordinate.latitude() > 70) {
            errorString_ = "error";
            errorCode_ = (QGeoSearchReply::Error) (coordinate.latitude() - 70);
        } else {
            errorString_ = "";
            errorCode_ = QGeoSearchReply::NoError;
        }
        if (finishRequestImmediately_) {
            if (errorCode_) {
                searchReply_->callSetError(errorCode_, errorString_);
            } else {
                searchReply_->callSetError(QGeoSearchReply::NoError,coordinate.toString());
                searchReply_->callSetFinished(true);
            }
        } else {
            // we only allow serialized requests in QML - previous must have been aborted or finished
            Q_ASSERT(timerId_ == 0);
            timerId_ = startTimer(200);
        }
        return static_cast<QGeoSearchReply*>(searchReply_);
    }

protected:
     void timerEvent(QTimerEvent *event)
     {
         Q_ASSERT(timerId_ == event->timerId());
         Q_ASSERT(searchReply_);
         killTimer(timerId_);
         timerId_ = 0;
         if (errorCode_) {
             searchReply_->callSetError(errorCode_, errorString_);
             emit error(searchReply_, errorCode_, errorString_);
        } else {
             searchReply_->callSetError(QGeoSearchReply::NoError, "no error");
             searchReply_->callSetFinished(true);
         }
         emit finished(searchReply_);
     }

private:
    bool validateWellKnownValues_;
    bool finishRequestImmediately_;
    bool supported_;
    SearchReplyTest* searchReply_;
    int timerId_;
    QGeoSearchReply::Error errorCode_;
    QString errorString_;
};

#endif
