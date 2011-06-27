#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplacecategory.h>

QTM_USE_NAMESPACE

class tst_QPlaceCategory : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceCategory();

private Q_SLOTS:
    void constructorTest();
    void alternativeNamesTest();
    void categoryIdTest();
    void descriptionTest();
    void nameTest();
    void operatorsTest();
};

tst_QPlaceCategory::tst_QPlaceCategory()
{
}

void tst_QPlaceCategory::constructorTest()
{
    QPlaceCategory testObj;
    Q_UNUSED(testObj);

    testObj.setCategoryId("testId");
    QPlaceCategory *testObjPtr = new QPlaceCategory(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QPlaceCategory::alternativeNamesTest()
{
    QPlaceCategory testObj;
    QVERIFY2(testObj.alternativeNames().count() == 0, "Wrong default value");
    QList<QPlaceAlternativeValue> list;
    QPlaceAlternativeValue altValue;
    list.append(altValue);
    altValue.setKey("testData");
    list.append(altValue);
    testObj.setAlternativeNames(list);
    QVERIFY2(testObj.alternativeNames().count() == 2, "Wrong value returned");
    QVERIFY2(testObj.alternativeNames()[0].key() == QString(), "Wrong key[0] returned");
    QVERIFY2(testObj.alternativeNames()[1].key() == "testData", "Wrong key[1] returned");
}

void tst_QPlaceCategory::categoryIdTest()
{
    QPlaceCategory testObj;
    QVERIFY2(testObj.categoryId() == QString(), "Wrong default value");
    testObj.setCategoryId("testText");
    QVERIFY2(testObj.categoryId() == "testText", "Wrong value returned");
}

void tst_QPlaceCategory::descriptionTest()
{
    QPlaceCategory testObj;
    QVERIFY2(testObj.description() == QString(), "Wrong default value");
    testObj.setDescription("testText");
    QVERIFY2(testObj.description() == "testText", "Wrong value returned");
}

void tst_QPlaceCategory::nameTest()
{
    QPlaceCategory testObj;
    QVERIFY2(testObj.name() == QString(), "Wrong default value");
    testObj.setName("testText");
    QVERIFY2(testObj.name() == "testText", "Wrong value returned");
}

void tst_QPlaceCategory::operatorsTest()
{
    QPlaceCategory testObj;
    testObj.setName("testValue");
    QPlaceCategory testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setDescription("testValue2");
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceCategory);

#include "tst_qplacecategory.moc"
