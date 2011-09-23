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

#include <QApplication>

#include <QUrl>
#include <QSettings>
#include <QProcessEnvironment>
#include <QNetworkProxyFactory>

#include "mainwidget.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // not in the tutorial text: set up a proxy server from
    // a QSettings file if necessary (useful on Linux)

    QApplication::setOrganizationName("Nokia");
    QApplication::setApplicationName("Maps3D");

    QSettings settings;

    QVariant value = settings.value("http.proxy");
    if (value.isValid()) {
        QUrl url(value.toString(), QUrl::TolerantMode);
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(url.host());
        proxy.setPort(url.port(8080));
        QNetworkProxy::setApplicationProxy(proxy);
    }

    int bm = 0;

    if (argc == 2) {
        bool ok = false;
        bm = QString(QLatin1String(argv[1])).toInt(&ok);
        if (!ok)
            bm = 0;
    }

    MainWidget *w = new MainWidget(0, bm);
    w->show();

    return app.exec();
}
