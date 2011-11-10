/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

/* Part of the Maps Demo tutorial */
/* You probably want to read it first, look under Tutorials in
   the Mobility documentation */

// TODO this app needs to be modified to work with qt5
// when new c++ headers are available
//#include "mainwindow.h"

#include <QtWidgets/QApplication>
#include <QList>
#include <QString>
#include <QUrl>
#include <QSettings>
#include <QProcessEnvironment>
#include <QNetworkProxyFactory>

#include "qgeoserviceprovider.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // not in the tutorial text: set up a proxy server from
    // a QSettings file if necessary (useful on Linux)

    QApplication::setOrganizationName("Nokia");
    QApplication::setApplicationName("MapsDemo");

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

    // launch the main window
    qWarning("Warning: MapsDemo has not yet been migrated to Qt5. Nothing will be shown.");
//MainWindow mw;
//#if defined(Q_WS_SIMULATOR)
//    mw.showMaximized();
//#else
//    mw.resize(360,640);
//    mw.show();
//#endif
    return a.exec();
}
