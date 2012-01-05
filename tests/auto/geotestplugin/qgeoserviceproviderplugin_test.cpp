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

#include "qgeoserviceproviderplugin_test.h"
#include "qgeocodingmanagerengine_test.h"
#include "qgeoroutingmanagerengine_test.h"
#include "qgeomappingmanagerengine_test.h"
#include "qplacemanagerengine_test.h"

#include <QtPlugin>

QGeoServiceProviderFactoryTest::QGeoServiceProviderFactoryTest()
{
}

QGeoServiceProviderFactoryTest::~QGeoServiceProviderFactoryTest()
{
}

QString QGeoServiceProviderFactoryTest::providerName() const
{
    return "qmlgeo.test.plugin";
}

QStringList QGeoServiceProviderFactoryTest::keys() const
{
    return QStringList() << QLatin1String("qmlgeo.test.plugin");
}

int QGeoServiceProviderFactoryTest::providerVersion() const
{
    return 3;
}

QGeoRoutingManagerEngine* QGeoServiceProviderFactoryTest::createRoutingManagerEngine (
            const QMap<QString, QVariant> & parameters,
            QGeoServiceProvider::Error * error, QString * errorString ) const
{
    return new QGeoRoutingManagerEngineTest(parameters, error, errorString);
}


QGeocodingManagerEngine* QGeoServiceProviderFactoryTest::createGeocodingManagerEngine(const QMap<
    QString, QVariant> &parameters, QGeoServiceProvider::Error *error, QString *errorString) const
{
    return new QGeocodingManagerEngineTest(parameters, error, errorString);
}


QGeoMappingManagerEngine* QGeoServiceProviderFactoryTest::createMappingManagerEngine (
            const QMap<QString, QVariant> & parameters,
            QGeoServiceProvider::Error * error, QString * errorString ) const
{
    return new QGeoMappingManagerEngineTest(parameters, error, errorString);
}

QPlaceManagerEngine* QGeoServiceProviderFactoryTest::createPlaceManagerEngine(
        const QMap<QString, QVariant> &parameters,
        QGeoServiceProvider::Error *error, QString *errorString) const
{
    Q_UNUSED(error);
    Q_UNUSED(errorString);
    return new QPlaceManagerEngineTest(parameters);
}

Q_EXPORT_PLUGIN2(qtgeoservices_qmltestplugin, QGeoServiceProviderFactoryTest)
