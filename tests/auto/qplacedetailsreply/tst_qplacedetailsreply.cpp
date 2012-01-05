/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtLocation/QPlaceDetailsReply>

QT_USE_NAMESPACE

class TestDetailsReply : public QPlaceDetailsReply
{
    Q_OBJECT
public:
    TestDetailsReply(QObject *parent) : QPlaceDetailsReply(parent){}
    ~TestDetailsReply() {}
    void setPlace(const QPlace &place) { QPlaceDetailsReply::setPlace(place); }
};

class tst_QPlaceDetailsReply : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceDetailsReply();

private Q_SLOTS:
    void constructorTest();
    void typeTest();
    void placeTest();
};

tst_QPlaceDetailsReply::tst_QPlaceDetailsReply()
{
}

void tst_QPlaceDetailsReply::constructorTest()
{
    TestDetailsReply *reply = new TestDetailsReply(this);
    QVERIFY(reply->parent() == this);
    delete reply;
}

void tst_QPlaceDetailsReply::typeTest()
{
    TestDetailsReply *reply = new TestDetailsReply(this);
    QCOMPARE(reply->type(), QPlaceReply::DetailsReply);
    delete reply;
}

void tst_QPlaceDetailsReply::placeTest()
{
    TestDetailsReply *reply = new TestDetailsReply(this);
    QPlace place;
    place.setName(QLatin1String("Gotham City"));
    reply->setPlace(place);

    QCOMPARE(reply->place(), place);
    delete reply;
}

QTEST_APPLESS_MAIN(tst_QPlaceDetailsReply)

#include "tst_qplacedetailsreply.moc"
