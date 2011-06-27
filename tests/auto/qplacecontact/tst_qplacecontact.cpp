#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacecontact.h>

QTM_USE_NAMESPACE

class tst_QPlaceContact : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceContact();

private Q_SLOTS:
    void constructorTest();
    void descriptionTest();
    void typeTest();
    void valueTest();
    void operatorsTest();
};

tst_QPlaceContact::tst_QPlaceContact()
{
}

void tst_QPlaceContact::constructorTest()
{
    QPlaceContact testObj;
    Q_UNUSED(testObj);

    QPlaceContact *testObjPtr = new QPlaceContact(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceContact::descriptionTest()
{
    QPlaceContact testObj;
    QVERIFY2(testObj.description() == QString(), "Wrong default value");
    testObj.setDescription("testText");
    QVERIFY2(testObj.description() == "testText", "Wrong value returned");
}

void tst_QPlaceContact::typeTest()
{
    QPlaceContact testObj;
    QVERIFY2(testObj.type() == QPlaceContact::Undefined, "Wrong default value");
    testObj.setType(QPlaceContact::Fax);
    QVERIFY2(testObj.type() == QPlaceContact::Fax, "Wrong value returned");
}

void tst_QPlaceContact::valueTest()
{
    QPlaceContact testObj;
    QVERIFY2(testObj.value() == QString(), "Wrong default value");
    testObj.setValue("testText");
    QVERIFY2(testObj.value() == "testText", "Wrong value returned");
}

void tst_QPlaceContact::operatorsTest()
{
    QPlaceContact testObj;
    testObj.setDescription("testValue");
    QPlaceContact testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setValue("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceContact);

#include "tst_qplacecontact.moc"
