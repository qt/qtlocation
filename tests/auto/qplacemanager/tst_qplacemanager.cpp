#include <QString>
#include <QtTest/QtTest>

#include <qgeoserviceprovider.h>
#include <qplacemanager.h>

QT_USE_NAMESPACE

class tst_QPlaceManager : public QObject
{
    Q_OBJECT
public:
    tst_QPlaceManager();

private Q_SLOTS:
    void initTestCase();

private:
    QGeoServiceProvider *provider;
    QPlaceManager *placeManager;
};

tst_QPlaceManager::tst_QPlaceManager()
{
}

void tst_QPlaceManager::initTestCase()
{
    QStringList providers = QGeoServiceProvider::availableServiceProviders();
    providers.contains("qmlgeo.test.plugin");

    provider = new QGeoServiceProvider("qmlgeo.test.plugin");
    placeManager = provider->placeManager();
    QVERIFY(placeManager);

    QVERIFY(placeManager->connectivityMode() == QPlaceManager::OnlineMode);
}

QTEST_MAIN(tst_QPlaceManager)

#include "tst_qplacemanager.moc"
