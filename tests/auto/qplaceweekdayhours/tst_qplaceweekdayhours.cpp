#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qplaceweekdayhours.h>

QTM_USE_NAMESPACE

class tst_QPlaceWeekdayHours : public QObject
{
    Q_OBJECT

public:
    tst_QPlaceWeekdayHours();

private Q_SLOTS:
    void constructorTest();
    void periodTest();
    void weekdayTest();
    void operatorsTest();
};

tst_QPlaceWeekdayHours::tst_QPlaceWeekdayHours()
{
}

void tst_QPlaceWeekdayHours::constructorTest()
{
    QPlaceWeekdayHours testObj;
    testObj.setWeekday(Qt::Wednesday);
    QPlaceWeekdayHours *testObjPtr = new QPlaceWeekdayHours(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - start time");
    delete testObjPtr;
}

void tst_QPlaceWeekdayHours::weekdayTest()
{
    QPlaceWeekdayHours testObj;
    QVERIFY2(testObj.weekday() == Qt::Monday, "Wrong default value");
    testObj.setWeekday(Qt::Sunday);
    QVERIFY2(testObj.weekday() == Qt::Sunday, "Wrong value returned");
}

void tst_QPlaceWeekdayHours::periodTest()
{
    QPlaceWeekdayHours testObj;
    QVERIFY2(testObj.period() == QPlacePeriod(), "Wrong default value");
    QTime time = QTime::currentTime();
    QPlacePeriod period;
    period.setStartTime(time);
    testObj.setPeriod(period);
    QVERIFY2(testObj.period().startTime() == time, "Wrong value returned");
}

void tst_QPlaceWeekdayHours::operatorsTest()
{
    QPlaceWeekdayHours testObj;
    QTime time = QTime::currentTime();
    testObj.setWeekday(Qt::Sunday);
    QPlaceWeekdayHours testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj.setWeekday(Qt::Thursday);
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QPlaceWeekdayHours);

#include "tst_qplaceweekdayhours.moc"
