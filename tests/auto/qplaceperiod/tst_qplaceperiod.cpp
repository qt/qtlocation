#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplaceperiod.h>

QTM_USE_NAMESPACE

class tst_QPlacePeriod : public QObject
{
    Q_OBJECT

public:
    tst_QPlacePeriod();

private Q_SLOTS:
    void constructorTest();
    void startDateTest();
    void startTimeTest();
    void endDateTest();
    void endTimeTest();
    void operatorsTest();
};

tst_QPlacePeriod::tst_QPlacePeriod()
{
}

void tst_QPlacePeriod::constructorTest()
{
    QPlacePeriod testObj;
    testObj.setStartTime(QTime::currentTime());
    QPlacePeriod *testObjPtr = new QPlacePeriod(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(testObjPtr->startTime() == testObj.startTime(), "Copy constructor - start time");
    delete testObjPtr;
}

void tst_QPlacePeriod::startDateTest()
{
    QPlacePeriod testObj;
    QVERIFY2(testObj.startDate().isNull() == true, "Wrong default value");
    QDate date = QDate::currentDate();
    testObj.setStartDate(date);
    QVERIFY2(testObj.startDate() == date, "Wrong value returned");
}

void tst_QPlacePeriod::startTimeTest()
{
    QPlacePeriod testObj;
    QVERIFY2(testObj.startTime().isNull() == true, "Wrong default value");
    QTime time = QTime::currentTime();
    testObj.setStartTime(time);
    QVERIFY2(testObj.startTime() == time, "Wrong value returned");
}

void tst_QPlacePeriod::endDateTest()
{
    QPlacePeriod testObj;
    QVERIFY2(testObj.endDate().isNull() == true, "Wrong default value");
    QDate date = QDate::currentDate();
    testObj.setEndDate(date);
    QVERIFY2(testObj.endDate() == date, "Wrong value returned");
}

void tst_QPlacePeriod::endTimeTest()
{
    QPlacePeriod testObj;
    QVERIFY2(testObj.endTime().isNull() == true, "Wrong default value");
    QTime time = QTime::currentTime();
    testObj.setEndTime(time);
    QVERIFY2(testObj.endTime() == time, "Wrong value returned");
}

void tst_QPlacePeriod::operatorsTest()
{
    QPlacePeriod testObj;
    QTime time = QTime::currentTime();
    testObj.setEndTime(time);
    QPlacePeriod testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj.setEndTime(QTime::currentTime().addSecs(102021));
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlacePeriod);

#include "tst_qplaceperiod.moc"
