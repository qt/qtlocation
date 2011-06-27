#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacealternativevalue.h>

QTM_USE_NAMESPACE

class tst_QPlaceAlternativeValue : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceAlternativeValue();

private Q_SLOTS:
    void constructorTest();
    void keyTest();
    void languageTest();
    void semanticsTest();
    void typeTest();
    void valueTest();
    void operatorsTest();
};

tst_QPlaceAlternativeValue::tst_QPlaceAlternativeValue()
{
}

void tst_QPlaceAlternativeValue::constructorTest()
{
    QPlaceAlternativeValue testObj;
    Q_UNUSED(testObj);

    QPlaceAlternativeValue *testObjPtr = new QPlaceAlternativeValue(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceAlternativeValue::keyTest()
{
    QPlaceAlternativeValue testObj;
    QVERIFY2(testObj.key() == QString(), "Wrong default value");
    testObj.setKey("testText");
    QVERIFY2(testObj.key() == "testText", "Wrong value returned");
}

void tst_QPlaceAlternativeValue::languageTest()
{
    QPlaceAlternativeValue testObj;
    QVERIFY2(testObj.language() == QString(), "Wrong default value");
    testObj.setLanguage("testText");
    QVERIFY2(testObj.language() == "testText", "Wrong value returned");
}

void tst_QPlaceAlternativeValue::semanticsTest()
{
    QPlaceAlternativeValue testObj;
    QVERIFY2(testObj.semantics() == QPlaceAlternativeValue::Unclassified, "Wrong default value");
    testObj.setSemantics(QPlaceAlternativeValue::Exonym);
    QVERIFY2(testObj.semantics() == QPlaceAlternativeValue::Exonym, "Wrong value returned");
}

void tst_QPlaceAlternativeValue::typeTest()
{
    QPlaceAlternativeValue testObj;
    QVERIFY2(testObj.type() == QPlaceAlternativeValue::Unknown, "Wrong default value");
    testObj.setType(QPlaceAlternativeValue::BaseName);
    QVERIFY2(testObj.type() == QPlaceAlternativeValue::BaseName, "Wrong value returned");
}

void tst_QPlaceAlternativeValue::valueTest()
{
    QPlaceAlternativeValue testObj;
    QVERIFY2(testObj.value() == QString(), "Wrong default value");
    testObj.setValue("testText");
    QVERIFY2(testObj.value() == "testText", "Wrong value returned");
}

void tst_QPlaceAlternativeValue::operatorsTest()
{
    QPlaceAlternativeValue testObj;
    testObj.setKey("testValue");
    QPlaceAlternativeValue testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setLanguage("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceAlternativeValue);

#include "tst_qplacealternativevalue.moc"
