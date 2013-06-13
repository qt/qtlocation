/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtLocation/QPlaceSearchResult>
#include <QtLocation/QPlaceIcon>

QT_USE_NAMESPACE

class tst_QPlaceSearchResult : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void constructorTest();
    void title();
    void icon();
    void operators();
};

void tst_QPlaceSearchResult::constructorTest()
{
    QPlaceSearchResult result;

    QCOMPARE(result.type(), QPlaceSearchResult::UnknownSearchResult);
    QVERIFY(result.title().isEmpty());
    QVERIFY(result.icon().isEmpty());

    result.setTitle(QLatin1String("title"));
    QPlaceIcon icon;
    QVariantMap parameters;
    parameters.insert(QLatin1String("paramKey"), QLatin1String("paramValue"));
    icon.setParameters(parameters);
    result.setIcon(icon);

    QPlaceSearchResult result2(result);
    QCOMPARE(result2.title(), QLatin1String("title"));
    QCOMPARE(result2.icon().parameters().value(QLatin1String("paramKey")).toString(),
             QLatin1String("paramValue"));

    QCOMPARE(result2, result);
}

void tst_QPlaceSearchResult::title()
{
    QPlaceSearchResult result;
    QVERIFY(result.title().isEmpty());
    result.setTitle(QLatin1String("title"));
    QCOMPARE(result.title(), QLatin1String("title"));
    result.setTitle(QString());
    QVERIFY(result.title().isEmpty());
}

void tst_QPlaceSearchResult::icon()
{
    QPlaceSearchResult result;
    QVERIFY(result.icon().isEmpty());
    QPlaceIcon icon;
    QVariantMap iconParams;
    iconParams.insert(QLatin1String("paramKey"), QLatin1String("paramValue"));
    result.setIcon(icon);
    QCOMPARE(result.icon(), icon);
    result.setIcon(QPlaceIcon());
    QVERIFY(result.icon().isEmpty());
}

void tst_QPlaceSearchResult::operators()
{
    QPlaceSearchResult result1;
    QPlaceSearchResult result2;

    QVERIFY(result1 == result2);
    QVERIFY(!(result1 != result2));

    result1.setTitle(QLatin1String("title"));
    QVERIFY(!(result1 == result2));
    QVERIFY(result1 != result2);

    result2.setTitle(QLatin1String("title"));
    QVERIFY(result1 == result2);
    QVERIFY(!(result1 != result2));
}

QTEST_APPLESS_MAIN(tst_QPlaceSearchResult)

#include "tst_qplacesearchresult.moc"
