#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacerequest.h>
#include <qplacecontentrequest.h>

QT_USE_NAMESPACE

class tst_QPlaceRequest : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceRequest();

private Q_SLOTS:
    void constructorTest();
    void offsetTest();
    void limitTest();
    void operatorsTest();
};

tst_QPlaceRequest::tst_QPlaceRequest()
{
}

void tst_QPlaceRequest::constructorTest()
{
    QPlaceRequest testObj;
    Q_UNUSED(testObj);

    QPlaceRequest *testObjPtr = new QPlaceRequest(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(testObjPtr->offset() == 0, "Copy constructor - wrong count");
    QVERIFY2(testObjPtr->limit() == -1, "Copy constructor - wrong value");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceRequest::offsetTest()
{
    QPlaceRequest testObj;
    QVERIFY2(testObj.offset() == 0, "Wrong default value");
    testObj.setOffset(-10);
    QVERIFY2(testObj.offset() == -10, "Wrong negative value returned");
    testObj.setOffset(10203);
    QVERIFY2(testObj.offset() == 10203, "Wrong value returned");
    testObj.clear();
    QVERIFY2(testObj.offset() == 0, "Wrong cleared value returned");
}

void tst_QPlaceRequest::limitTest()
{
    QPlaceRequest testObj;
    QVERIFY2(testObj.limit() == -1, "Wrong default value");
    testObj.setLimit(-10);
    QVERIFY2(testObj.limit() == -10, "Wrong negative value returned");
    testObj.setLimit(10203);
    QVERIFY2(testObj.limit() == 10203, "Wrong value returned");
    testObj.clear();
    QVERIFY2(testObj.limit() == -1, "Wrong cleared value returned");
}

void tst_QPlaceRequest::operatorsTest()
{
    QPlaceRequest testObj;
    testObj.setOffset(2);
    QPlaceRequest testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setLimit(-10);
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceRequest);

#include "tst_qplacerequest.moc"
