/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
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
    Q_UNUSED(engine);
    Q_UNUSED(jsEngine);

    return new LocationLabsSingleton;
}

class QtLocationLabsDeclarativeModule: public QQmlExtensionPlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    QtLocationLabsDeclarativeModule(QObject *parent = nullptr) : QQmlExtensionPlugin(parent) { }
    void registerTypes(const char *uri) override
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
