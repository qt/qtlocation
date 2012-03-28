/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtQuickTest/quicktest.h>
#include <QFile>

#define TESTNAME "routing backend tests"

QT_USE_NAMESPACE

// Empty tests so we can pick up the test results even if backend tests don't run
class tst_routing : public QObject
{
    Q_OBJECT
};

int main(int argc, char **argv)
{
// We only want to run these tests in build configuration on Linux x86
#if (defined(Q_OS_LINUX) && defined(Q_PROCESSOR_X86_32)) || defined(QTLOCATION_BACKEND_TESTS)
    const QString configurationFile = QStringLiteral("/var/lib/qt/qtlocation_tests_configuration.js");

    if (QFile::exists(configurationFile))
        return quick_test_main(argc, argv, TESTNAME, 0, QUICK_TEST_SOURCE_DIR);

    qWarning("File '/var/lib/qt/qtlocation_tests_configuration.js' does not exist.");
    qWarning("Not running tests.");
#else
    qWarning("Not configured to run '" TESTNAME "'!");
#endif

    tst_routing tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_routing.moc"
