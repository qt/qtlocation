#include <QGuiApplication>
#include <QtDeclarative/QSGView>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtNetwork>

#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    // setting proxy for application. It will be used in all clients.
    QString urlEnv = getenv("HTTP_PROXY");
    if (urlEnv.isEmpty()) {
        urlEnv = getenv("http_proxy");
    }
    if (!urlEnv.isEmpty()) {
        QNetworkProxy proxy;
        QUrl url = QUrl(urlEnv, QUrl::TolerantMode);
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(url.host());
        proxy.setPort(url.port(8080));
        QNetworkProxy::setApplicationProxy(proxy);
    } else {
        QNetworkProxyFactory::setUseSystemConfiguration(true);
    }

    QSGView view;
    view.setSource(QString("qrc:/qmlplaces.qml"));
    view.show();
    return a.exec();
}
