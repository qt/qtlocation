#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacequery.h>

QT_USE_NAMESPACE

class tst_QPlaceQuery : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceQuery();

private Q_SLOTS:
    void constructorTest();
    void offsetTest();
    void limitTest();
    void operatorsTest();
};

tst_QPlaceQuery::tst_QPlaceQuery()
{
}

void tst_QPlaceQuery::constructorTest()
{
    QPlaceQuery testObj;
    Q_UNUSED(testObj);

    QPlaceQuery *testObjPtr = new QPlaceQuery(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(testObjPtr->offset() == 0, "Copy constructor - wrong count");
    QVERIFY2(testObjPtr->limit() == -1, "Copy constructor - wrong value");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceQuery::offsetTest()
{
    QPlaceQuery testObj;
    QVERIFY2(testObj.offset() == 0, "Wrong default value");
    testObj.setOffset(-10);
    QVERIFY2(testObj.offset() == -10, "Wrong negative value returned");
    testObj.setOffset(10203);
    QVERIFY2(testObj.offset() == 10203, "Wrong value returned");
    testObj.clear();
    QVERIFY2(testObj.offset() == 0, "Wrong cleared value returned");
}

void tst_QPlaceQuery::limitTest()
{
    QPlaceQuery testObj;
    QVERIFY2(testObj.limit() == -1, "Wrong default value");
    testObj.setLimit(-10);
    QVERIFY2(testObj.limit() == -10, "Wrong negative value returned");
    testObj.setLimit(10203);
    QVERIFY2(testObj.limit() == 10203, "Wrong value returned");
    testObj.clear();
    QVERIFY2(testObj.limit() == -1, "Wrong cleared value returned");
}

void tst_QPlaceQuery::operatorsTest()
{
    QPlaceQuery testObj;
    testObj.setOffset(2);
    QPlaceQuery testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setLimit(-10);
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceQuery);

#include "tst_qplacequery.moc"
