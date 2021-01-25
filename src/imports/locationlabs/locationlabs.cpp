/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include <QtLocation/private/qmapiconobject_p.h>
#include <QtLocation/private/qmapobjectview_p.h>
#include <QtLocation/private/qmaprouteobject_p.h>
#include <QtLocation/private/qmapcircleobject_p.h>
#include <QtLocation/private/qmappolygonobject_p.h>
#include <QtLocation/private/qmappolylineobject_p.h>
#include <QtLocation/private/qdeclarativenavigator_p.h>
#include <QtLocation/private/qdeclarativenavigator_p_p.h>
#include <QtLocation/private/qnavigationmanagerengine_p.h>
#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>
#include <QtCore/QDebug>
#include "locationlabssingleton_p.h"

QT_BEGIN_NAMESPACE

static QObject *singleton_type_factory(QQmlEngine *engine, QJSEngine *jsEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(jsEngine)

    return new LocationLabsSingleton;
}

class QtLocationLabsDeclarativeModule: public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid
                      FILE "plugin.json")

public:
    QtLocationLabsDeclarativeModule(QObject *parent = nullptr) : QQmlExtensionPlugin(parent) { }
    virtual void registerTypes(const char *uri)
    {
        if (QLatin1String(uri) == QLatin1String("Qt.labs.location")) {

            // @uri QtLocationLabs
            int major = 1;
            int minor = 0;

            // Register the 1.0 labs types
            qmlRegisterType<QMapIconObject>(uri, major, minor, "MapIconObject");
            qmlRegisterType<QMapObjectView>(uri, major, minor, "MapObjectView");
            qmlRegisterType<QMapRouteObject>(uri, major, minor, "MapRouteObject");
            qmlRegisterType<QMapCircleObject>(uri, major, minor, "MapCircleObject");
            qmlRegisterType<QMapPolygonObject>(uri, major, minor, "MapPolygonObject");
            qmlRegisterType<QMapPolylineObject>(uri, major, minor, "MapPolylineObject");
            qmlRegisterAnonymousType<QDeclarativeNavigationBasicDirections>(uri, major);
            qmlRegisterType<QDeclarativeNavigator>(uri, major, minor, "Navigator");
            qmlRegisterAnonymousType<QAbstractNavigator>(uri, major);
            qmlRegisterSingletonType<LocationLabsSingleton>(uri, major, minor, "QtLocationLabs", singleton_type_factory);
        } else {
            qDebug() << "Unsupported URI given to load location QML plugin: " << QLatin1String(uri);
        }
    }
};

QT_END_NAMESPACE

#include "locationlabs.moc"
