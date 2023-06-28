// Copyright (C) 2019 Julian Sherollari <jdotsh@gmail.com>
// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QFile>
#include <QVariantMap>
#include <QQmlContext>
#include <QGeoCircle>
#include <QGeoPath>
#include <QGeoPolygon>
#include <QtCore/qobjectdefs.h>
#ifdef Q_OS_ANDROID
#    include <QtCore/private/qandroidextras_p.h>
#endif


using namespace Qt::StringLiterals;


#ifdef Q_OS_ANDROID
// Request permissions because we're using QStandardPaths::writableLocation()
bool requestStoragePermissions()
{
    const QString permission = "android.permission.WRITE_EXTERNAL_STORAGE"_L1;
    auto checkFuture = QtAndroidPrivate::checkPermission(permission);
    if (checkFuture.result() == QtAndroidPrivate::Denied) {
        auto requestFuture = QtAndroidPrivate::requestPermission(permission);
        if (requestFuture.result() != QtAndroidPrivate::Authorized) {
            qWarning() << "Couldn't get permission: " << permission;
            return false;
        }
    }

    return true;
}
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

#ifdef Q_OS_ANDROID
    if (!requestStoragePermissions())
        return -1;
#endif

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("dataPath", QUrl(QStringLiteral("file://")
                                                              + qPrintable(QT_STRINGIFY(SRC_PATH))
                                                              + QStringLiteral("/data")));

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
