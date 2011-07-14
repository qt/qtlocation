#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacebusinessfeature.h>

QT_USE_NAMESPACE

class tst_QPlaceBusinessFeature : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceBusinessFeature();

private Q_SLOTS:
    void constructorTest();
    void keyTest();
    void labelTest();
    void valueTest();
    void operatorsTest();
};

tst_QPlaceBusinessFeature::tst_QPlaceBusinessFeature()
{
}

void tst_QPlaceBusinessFeature::constructorTest()
{
    QPlaceBusinessFeature testObj;
    Q_UNUSED(testObj);

    QPlaceBusinessFeature *testObjPtr = new QPlaceBusinessFeature(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceBusinessFeature::keyTest()
{
    QPlaceBusinessFeature testObj;
    QVERIFY2(testObj.key() == QString(), "Wrong default value");
    testObj.setKey("testText");
    QVERIFY2(testObj.key() == "testText", "Wrong value returned");
}

void tst_QPlaceBusinessFeature::labelTest()
{
    QPlaceBusinessFeature testObj;
    QVERIFY2(testObj.label() == QString(), "Wrong default value");
    testObj.setLabel("testText");
    QVERIFY2(testObj.label() == "testText", "Wrong value returned");
}

void tst_QPlaceBusinessFeature::valueTest()
{
    QPlaceBusinessFeature testObj;
    QVERIFY2(testObj.value() == QString(), "Wrong default value");
    testObj.setValue("testText");
    QVERIFY2(testObj.value() == "testText", "Wrong value returned");
}

void tst_QPlaceBusinessFeature::operatorsTest()
{
    QPlaceBusinessFeature testObj;
    testObj.setKey("testValue");
    QPlaceBusinessFeature testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setLabel("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceBusinessFeature);

#include "tst_qplacebusinessfeature.moc"
