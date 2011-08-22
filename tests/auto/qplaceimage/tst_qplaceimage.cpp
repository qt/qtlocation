#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplaceimage.h>

QT_USE_NAMESPACE

class tst_QPlaceImage : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceImage();

private Q_SLOTS:
    void constructorTest();
    void supplierTest();
    void urlTest();
    void thumbnailUrlTest();
    void idTest();
    void metaInfoTest();
    void mimeTypeTest();
    void operatorsTest();
};

tst_QPlaceImage::tst_QPlaceImage()
{
}

void tst_QPlaceImage::constructorTest()
{
    QPlaceImage testObj;
    testObj.setId("testId");
    QPlaceImage *testObjPtr = new QPlaceImage(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceImage::supplierTest()
{
    QPlaceImage testObj;
    QVERIFY2(testObj.supplier().supplierId() == QString(), "Wrong default value");
    QPlaceSupplier sup;
    sup.setName("testName1");
    sup.setSupplierId("testId");
    testObj.setSupplier(sup);
    QVERIFY2(testObj.supplier() == sup, "Wrong value returned");
}

void tst_QPlaceImage::urlTest()
{
    QUrl imageUrl("testText");

    QPlaceImage testObj;
    QVERIFY2(testObj.url() == QString(), "Wrong default value");
    testObj.setUrl(imageUrl);
    QVERIFY2(testObj.url() == imageUrl, "Wrong value returned");
}

void tst_QPlaceImage::thumbnailUrlTest()
{
    QUrl thumbnailUrl("testText");

    QPlaceImage testObj;
    QVERIFY2(testObj.thumbnailUrl() == QString(), "Wrong default value");
    testObj.setThumbnailUrl(thumbnailUrl);
    QVERIFY2(testObj.thumbnailUrl() == thumbnailUrl, "Wrong value returned");
}

void tst_QPlaceImage::idTest()
{
    QPlaceImage testObj;
    QVERIFY2(testObj.id() == QString(), "Wrong default value");
    testObj.setId("testText");
    QVERIFY2(testObj.id() == "testText", "Wrong value returned");
}

void tst_QPlaceImage::metaInfoTest()
{
    QPlaceImage testObj;
    QVERIFY2(testObj.metaInfo() == QString(), "Wrong default value");
    testObj.setMetaInfo("testText");
    QVERIFY2(testObj.metaInfo() == "testText", "Wrong value returned");
}

void tst_QPlaceImage::mimeTypeTest()
{
    QPlaceImage testObj;
    QVERIFY2(testObj.mimeType() == QString(), "Wrong default value");
    testObj.setMimeType("testText");
    QVERIFY2(testObj.mimeType() == "testText", "Wrong value returned");
}

void tst_QPlaceImage::operatorsTest()
{
    QPlaceImage testObj;
    testObj.setMimeType("testValue");
    QPlaceImage testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setId("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceImage);

#include "tst_qplaceimage.moc"
