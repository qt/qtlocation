#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacemediaobject.h>

QT_USE_NAMESPACE

class tst_QPlaceMediaObject : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceMediaObject();

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

tst_QPlaceMediaObject::tst_QPlaceMediaObject()
{
}

void tst_QPlaceMediaObject::constructorTest()
{
    QPlaceMediaObject testObj;
    testObj.setId("testId");
    QPlaceMediaObject *testObjPtr = new QPlaceMediaObject(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceMediaObject::supplierTest()
{
    QPlaceMediaObject testObj;
    QVERIFY2(testObj.supplier().supplierId() == QString(), "Wrong default value");
    QPlaceSupplier sup;
    sup.setName("testName1");
    sup.setSupplierId("testId");
    testObj.setSupplier(sup);
    QVERIFY2(testObj.supplier() == sup, "Wrong value returned");
}

void tst_QPlaceMediaObject::urlTest()
{
    QUrl mediaUrl("testText");

    QPlaceMediaObject testObj;
    QVERIFY2(testObj.url() == QString(), "Wrong default value");
    testObj.setUrl(mediaUrl);
    QVERIFY2(testObj.url() == mediaUrl, "Wrong value returned");
}

void tst_QPlaceMediaObject::thumbnailUrlTest()
{
    QUrl thumbnailUrl("testText");

    QPlaceMediaObject testObj;
    QVERIFY2(testObj.thumbnailUrl() == QString(), "Wrong default value");
    testObj.setThumbnailUrl(thumbnailUrl);
    QVERIFY2(testObj.thumbnailUrl() == thumbnailUrl, "Wrong value returned");
}

void tst_QPlaceMediaObject::idTest()
{
    QPlaceMediaObject testObj;
    QVERIFY2(testObj.id() == QString(), "Wrong default value");
    testObj.setId("testText");
    QVERIFY2(testObj.id() == "testText", "Wrong value returned");
}

void tst_QPlaceMediaObject::metaInfoTest()
{
    QPlaceMediaObject testObj;
    QVERIFY2(testObj.metaInfo() == QString(), "Wrong default value");
    testObj.setMetaInfo("testText");
    QVERIFY2(testObj.metaInfo() == "testText", "Wrong value returned");
}

void tst_QPlaceMediaObject::mimeTypeTest()
{
    QPlaceMediaObject testObj;
    QVERIFY2(testObj.mimeType() == QString(), "Wrong default value");
    testObj.setMimeType("testText");
    QVERIFY2(testObj.mimeType() == "testText", "Wrong value returned");
}

void tst_QPlaceMediaObject::operatorsTest()
{
    QPlaceMediaObject testObj;
    testObj.setMimeType("testValue");
    QPlaceMediaObject testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setId("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceMediaObject);

#include "tst_qplacemediaobject.moc"
