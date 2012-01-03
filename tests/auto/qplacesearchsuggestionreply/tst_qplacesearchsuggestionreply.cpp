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

#include <QtLocation/QPlaceSearchSuggestionReply>

QT_USE_NAMESPACE

class SuggestionReply : public QPlaceSearchSuggestionReply
{
    Q_OBJECT
public:
    SuggestionReply(QObject *parent) : QPlaceSearchSuggestionReply(parent){}

    void setSuggestions(const QStringList &suggestions) {
        QPlaceSearchSuggestionReply::setSuggestions(suggestions);
    }
};

class tst_QPlaceSearchSuggestionReply : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceSearchSuggestionReply();

private Q_SLOTS:
    void constructorTest();
    void typeTest();
    void suggestionsTest();
};

tst_QPlaceSearchSuggestionReply::tst_QPlaceSearchSuggestionReply()
{
}

void tst_QPlaceSearchSuggestionReply::constructorTest()
{
    SuggestionReply *reply = new SuggestionReply(this);
    QCOMPARE(reply->parent(), this);

    delete reply;
}

void tst_QPlaceSearchSuggestionReply::typeTest()
{
    SuggestionReply *reply = new SuggestionReply(this);
    QCOMPARE(reply->type(), QPlaceReply::SearchSuggestionReply);

    delete reply;
}

void tst_QPlaceSearchSuggestionReply::suggestionsTest()
{
    QStringList suggestions;
    suggestions << QLatin1String("one") << QLatin1String("two")
                << QLatin1String("three");

    SuggestionReply *reply = new SuggestionReply(this);
    reply->setSuggestions(suggestions);
    QCOMPARE(reply->suggestions(), suggestions);

    delete reply;
}

QTEST_APPLESS_MAIN(tst_QPlaceSearchSuggestionReply)

#include "tst_qplacesearchsuggestionreply.moc"
