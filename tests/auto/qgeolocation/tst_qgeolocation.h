/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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

#ifndef TST_QGEOLOCATION_H
#define TST_QGEOLOCATION_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>

#include "../utils/qlocationtestutils_p.h"
#include <QtPositioning/qgeoaddress.h>
#include <QtPositioning/qgeocoordinate.h>
#include <QtPositioning/qgeolocation.h>
#include <QtPositioning/QGeoRectangle>

QT_USE_NAMESPACE

class tst_QGeoLocation : public QObject
{
    Q_OBJECT

public:
    tst_QGeoLocation();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

   //Start Unit Tests for qgeolocation.h
    void constructor();
    void copy_constructor();
    void destructor();
    void address();
    void coordinate();
    void viewport();
    void operators();
    void comparison();
    void comparison_data();
    void isEmpty();
    //End Unit Tests for qgeolocation.h

private:
    QGeoLocation m_location;

    QGeoAddress m_address;
    QGeoCoordinate m_coordinate;
    QGeoRectangle m_viewport;
};

Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateFormat);
Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateType);
Q_DECLARE_METATYPE( QList<double>);

#endif

