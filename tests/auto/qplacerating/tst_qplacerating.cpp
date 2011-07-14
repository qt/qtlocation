#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacerating.h>

QT_USE_NAMESPACE

class tst_QPlaceRating : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceRating();

private Q_SLOTS:
    void constructorTest();
    void valueTest();
    void countTest();
    void operatorsTest();
};

tst_QPlaceRating::tst_QPlaceRating()
{
}

void tst_QPlaceRating::constructorTest()
{
    QPlaceRating testObj;
    Q_UNUSED(testObj);

    QPlaceRating *testObjPtr = new QPlaceRating(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(testObjPtr->count() == 0, "Copy constructor - wrong count");
    QVERIFY2(testObjPtr->value() == 0, "Copy constructor - wrong value");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceRating::valueTest()
{
    QPlaceRating testObj;
    QVERIFY2(testObj.value() == 0, "Wrong default value");
    testObj.setValue(-10.23);
    QVERIFY2(testObj.value() == -10.23, "Wrong value returned");
}

void tst_QPlaceRating::countTest()
{
    QPlaceRating testObj;
    QVERIFY2(testObj.count() == 0, "Wrong default value");
    testObj.setCount(-1002);
    QVERIFY2(testObj.count() == -1002, "Wrong value returned");
}

void tst_QPlaceRating::operatorsTest()
{
    QPlaceRating testObj;
    testObj.setValue(0.123);
    QPlaceRating testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setCount(-10);
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceRating);

#include "tst_qplacerating.moc"
