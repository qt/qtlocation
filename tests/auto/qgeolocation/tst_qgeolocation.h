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

#ifndef TST_QGEOLOCATION_H
#define TST_QGEOLOCATION_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>

#include "../qgeocoordinate/qlocationtestutils_p.h"
#include <qgeoaddress.h>
#include <qgeocoordinate.h>
#include <qgeolocation.h>
#include <qgeoboundingbox.h>

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
    void additionalData();
    void navigationPositions();
    void locationId();
    void operators();
    void comparison();
    void comparison_data();
    //End Unit Tests for qgeolocation.h

private:
    QGeoLocation m_location;

    QGeoAddress m_address;
    QGeoCoordinate m_coordinate;
    QGeoBoundingBox m_viewport;

    QVariantHash m_additionalData;
    QList<QGeoCoordinate> m_navigationPositions;
    QString m_locationId;
};

Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateFormat);
Q_DECLARE_METATYPE( QGeoCoordinate::CoordinateType);
Q_DECLARE_METATYPE( QList<double>);

#endif // TST_QGEOPLACE_H

