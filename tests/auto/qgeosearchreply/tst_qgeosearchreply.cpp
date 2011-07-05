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

#include "tst_qgeosearchreply.h"

QTM_USE_NAMESPACE

void tst_QGeoSearchReply::initTestCase()
{

    reply = new SubSearchReply();
}

void tst_QGeoSearchReply::cleanupTestCase()
{

    delete reply;
    delete qgeoplace;
}

void tst_QGeoSearchReply::init()
{
    qRegisterMetaType<QGeoSearchReply::Error>("Error");
    signalerror = new QSignalSpy(reply, SIGNAL(error(QGeoSearchReply::Error,const QString)));
    signalfinished = new QSignalSpy(reply, SIGNAL(finished()));
}

void tst_QGeoSearchReply::cleanup()
{
    delete signalerror;
    delete signalfinished;
}

void tst_QGeoSearchReply::constructor()
{
    QVERIFY(!reply->isFinished());

    QCOMPARE(reply->limit(),-1);
    QCOMPARE(reply->offset(),0);
    QCOMPARE(reply->error(),QGeoSearchReply::NoError);

    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);
}

void tst_QGeoSearchReply::constructor_error()
{
    QFETCH(QGeoSearchReply::Error,error);
    QFETCH(QString,msg);

    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QGeoSearchReply *qgeosearchreplycopy = new QGeoSearchReply (error,msg,0);

    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QCOMPARE (qgeosearchreplycopy->error(),error);
    QCOMPARE (qgeosearchreplycopy->errorString(),msg);

    delete qgeosearchreplycopy;
}

void tst_QGeoSearchReply::constructor_error_data()
{
    QTest::addColumn<QGeoSearchReply::Error>("error");
    QTest::addColumn<QString>("msg");

    QTest::newRow("error1") << QGeoSearchReply::NoError << "No error.";
    QTest::newRow("error2") << QGeoSearchReply::EngineNotSetError << "Engine Not Set Error.";
    QTest::newRow("error3") << QGeoSearchReply::CommunicationError << "Communication Error.";
    QTest::newRow("error4") << QGeoSearchReply::ParseError << "Parse Error.";
    QTest::newRow("error5") << QGeoSearchReply::UnsupportedOptionError << "Unsupported Option Error.";
    QTest::newRow("error6") << QGeoSearchReply::UnknownError << "Unknown Error.";

}

void tst_QGeoSearchReply::destructor()
{
    QGeoSearchReply *qgeosearchreplycopy;
    QFETCH(QGeoSearchReply::Error,error);
    QFETCH(QString,msg);

    QLocationTestUtils::uheap_mark();
    qgeosearchreplycopy = new QGeoSearchReply (error,msg,0);
    delete qgeosearchreplycopy;
    QLocationTestUtils::uheap_mark_end();

}

void tst_QGeoSearchReply::destructor_data()
{
    tst_QGeoSearchReply::constructor_error_data();
}

void tst_QGeoSearchReply::abort()
{
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->callSetFinished(true);
    reply->abort();

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->abort();
    reply->callSetFinished(false);
    reply->abort();

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),2);
}

void tst_QGeoSearchReply::error()
{
    QFETCH(QGeoSearchReply::Error,error);
    QFETCH(QString,msg);

    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );
    QCOMPARE(signalerror->count(),0);

    reply->callSetError(error,msg);

   QCOMPARE(signalerror->count(),1);
   QCOMPARE(signalfinished->count(),1);
   QCOMPARE(reply->errorString(),msg);
   QCOMPARE(reply->error(),error);


}

void tst_QGeoSearchReply::error_data()
{
    QTest::addColumn<QGeoSearchReply::Error>("error");
    QTest::addColumn<QString>("msg");

    QTest::newRow("error1") << QGeoSearchReply::NoError << "No error.";
    QTest::newRow("error2") << QGeoSearchReply::EngineNotSetError << "Engine Not Set Error.";
    QTest::newRow("error3") << QGeoSearchReply::CommunicationError << "Communication Error.";
    QTest::newRow("error4") << QGeoSearchReply::ParseError << "Parse Error.";
    QTest::newRow("error5") << QGeoSearchReply::UnsupportedOptionError << "Unsupported Option Error.";
    QTest::newRow("error6") << QGeoSearchReply::UnknownError << "Unknown Error.";
}

void tst_QGeoSearchReply::finished()
{
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->callSetFinished(true);
    QVERIFY(reply->isFinished());
    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->callSetFinished(false);

    QVERIFY(!reply->isFinished());
    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->callSetFinished(true);

    QVERIFY(reply->isFinished());
    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),2);
}



void tst_QGeoSearchReply::limit()
{
    int limit =30;
    reply->callSetLimit(limit);
    QCOMPARE(reply->limit(),limit);
}

void tst_QGeoSearchReply::offset()
{
    int offset = 2;
    reply->callSetOffset(offset);
    QCOMPARE(reply->offset(),offset);
}

void tst_QGeoSearchReply::places()
{
    QList <QGeoPlace> geoplaces;
    geoplaces = reply->places();

    QCOMPARE(geoplaces.size(),0);

    QGeoAddress *qgeoaddress = new QGeoAddress ();
    qgeoaddress->setCity("Berlin");

    QGeoCoordinate *qgeocoordinate = new QGeoCoordinate (12.12 , 54.43);

    qgeoplace = new QGeoPlace ();
    qgeoplace->setAddress(*qgeoaddress);
    qgeoplace->setCoordinate(*qgeocoordinate);

    reply->callAddPlace(*qgeoplace);

    geoplaces = reply->places();
    QCOMPARE(geoplaces.size(),1);
    QCOMPARE(geoplaces.at(0),*qgeoplace);

    QGeoPlace *qgeoplacecopy = new QGeoPlace (*qgeoplace);

    QList <QGeoPlace> qgeoplaces;
    qgeoplaces.append(*qgeoplace);
    qgeoplaces.append(*qgeoplacecopy);

    reply->callSetPlaces(qgeoplaces);

    geoplaces = reply->places();

    QCOMPARE(geoplaces.size(),qgeoplaces.size());
    for (int i = 0 ; i < geoplaces.size(); i++)
    {
        QCOMPARE(geoplaces.at(i),qgeoplaces.at(i));
    }

    delete qgeoaddress;
    delete qgeocoordinate;
    delete qgeoplacecopy;
}

void tst_QGeoSearchReply::viewport()
{
    QGeoCoordinate *qgeocoordinate = new QGeoCoordinate (12.12 , 54.43);

    qgeoboundingbox = new QGeoBoundingBox (*qgeocoordinate, 0.5 , 0.5);

    reply->callSetViewport(qgeoboundingbox);

    QCOMPARE (reply->viewport(), static_cast<QGeoBoundingArea*>(qgeoboundingbox));

    delete qgeocoordinate;
    delete qgeoboundingbox;
}

QTEST_MAIN(tst_QGeoSearchReply);
