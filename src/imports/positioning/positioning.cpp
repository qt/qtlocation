/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
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

#include "locationvaluetypeprovider.h"

#include <QtPositioning/private/qdeclarativegeoaddress_p.h>
#include <QtPositioning/private/qdeclarativegeolocation_p.h>

#include "qdeclarativepositionsource_p.h"
#include "qdeclarativeposition_p.h"

#include "qdeclarativegeoshape.h"
#include "qdeclarativegeorectangle.h"
#include "qdeclarativegeocircle.h"
#include "qdeclarativecoordinate_p.h"

#include "locationsingleton.h"

#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>
#include <QtQml/private/qqmlvaluetype_p.h>
#include <QtQml/private/qqmlglobal_p.h>
#include <QtQml/private/qqmlmetatype_p.h>

#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoCircle>
#include <QtPositioning/QGeoLocation>

#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

static QObject *singleton_type_factory(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(jsEngine)

    return new LocationSingleton;
}

static LocationValueTypeProvider *getValueTypeProvider()
{
    static LocationValueTypeProvider provider;
    return &provider;
}

class QLocationDeclarativeModule: public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface/1.0"
                      FILE "plugin.json")

public:
    virtual void registerTypes(const char *uri)
    {
        if (QLatin1String(uri) == QLatin1String("QtPositioning")) {

            // @uri QtPositioning 5.0

            qmlRegisterSingletonType<LocationSingleton>(uri, 5, 0, "QtPositioning", singleton_type_factory);

            QQml_addValueTypeProvider(getValueTypeProvider());
            qmlRegisterValueTypeEnums<GeoShapeValueType>(uri, 5, 0, "GeoShape");

            qmlRegisterType<QDeclarativePosition>(uri, 5, 0, "Position");
            qmlRegisterType<QDeclarativePositionSource>(uri, 5, 0, "PositionSource");
            qmlRegisterType<QDeclarativeGeoAddress>(uri, 5, 0, "Address");

            qmlRegisterType<QDeclarativeGeoLocation>(uri, 5, 0, "Location");

            qRegisterMetaType<QGeoCoordinate>("QGeoCoordinate");
            qRegisterMetaType<QGeoAddress>("QGeoAddress");
            qRegisterMetaType<QGeoRectangle>("QGeoRectangle");
            qRegisterMetaType<QGeoCircle>("QGeoCircle");
            qRegisterMetaType<QGeoLocation>("QGeoLocation");
        } else {
            qDebug() << "Unsupported URI given to load positioning QML plugin: " << QLatin1String(uri);
        }
    }
};

#include "positioning.moc"

QT_END_NAMESPACE
