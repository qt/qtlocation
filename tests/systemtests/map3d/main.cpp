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
