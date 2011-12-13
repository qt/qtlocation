/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtCore/QString>
#include <QtTest/QtTest>

#include <QtLocation/QPlaceReply>

QT_USE_NAMESPACE

class TestReply : public QPlaceReply
{
public:
    TestReply(QObject *parent) : QPlaceReply(parent) {}
    void setFinished(bool finished) { QPlaceReply::setFinished(finished); }
    void setError(QPlaceReply::Error error, const QString &errorString) {
        QPlaceReply::setError(error,errorString);
    }
};

class tst_QPlaceReply : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceReply();

private Q_SLOTS:
    void constructorTest();
    void typeTest();
    void finishedTest();
    void errorTest();
};

tst_QPlaceReply::tst_QPlaceReply()
{

}

void tst_QPlaceReply::constructorTest()
{
    TestReply *reply = new TestReply(this);
    QCOMPARE(reply->parent(), this);
    delete reply;
}

void tst_QPlaceReply::typeTest()
{
    TestReply *reply = new TestReply(this);
    QCOMPARE(reply->type(), QPlaceReply::Reply);

    delete reply;
}

void tst_QPlaceReply::finishedTest()
{
    TestReply *reply = new TestReply(this);
    QCOMPARE(reply->isFinished(), false);
    reply->setFinished(true);
    QCOMPARE(reply->isFinished(), true);

    delete reply;
}

void tst_QPlaceReply::errorTest()
{
    TestReply *reply = new TestReply(this);
    QCOMPARE(reply->error(), QPlaceReply::NoError);
    QCOMPARE(reply->errorString(), QString());

    reply->setError(QPlaceReply::CommunicationError, QLatin1String("Could not connect to server"));
    QCOMPARE(reply->error(), QPlaceReply::CommunicationError);
    QCOMPARE(reply->errorString(), QLatin1String("Could not connect to server"));

    reply->setError(QPlaceReply::NoError, QString());
    QCOMPARE(reply->error(), QPlaceReply::NoError);
    QCOMPARE(reply->errorString(), QString());

    delete reply;
}

QTEST_APPLESS_MAIN(tst_QPlaceReply)

#include "tst_qplacereply.moc"
