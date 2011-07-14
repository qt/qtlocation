#include <QtCore/QString>
#include <QtTest/QtTest>

#include <qgeoboundingbox.h>

QT_USE_NAMESPACE

class tst_QGeoBoundingBox : public QObject
{
    Q_OBJECT

public:
    tst_QGeoBoundingBox();

private Q_SLOTS:
    void constructorTest();
    void topLeftTest();
    void bottomRightTest();
    void isValidTest();
    void operatorsTest();
};

tst_QGeoBoundingBox::tst_QGeoBoundingBox()
{
}

void tst_QGeoBoundingBox::constructorTest()
{
    QGeoBoundingBox testObj;
    Q_UNUSED(testObj);

    QGeoBoundingBox *testObjPtr = new QGeoBoundingBox(testObj);
    QVERIFY2(testObjPtr != NULL, "Copy constructor - null");
    QVERIFY2(*testObjPtr == testObj, "Copy constructor - compare");
    delete testObjPtr;
}

void tst_QGeoBoundingBox::topLeftTest()
{
    QGeoBoundingBox testObj;
    QVERIFY2(testObj.topLeft().isValid() == false, "Wrong default value");
    QGeoCoordinate coordinate;
    coordinate.setLatitude(30);
    coordinate.setLongitude(20);
    testObj.setTopLeft(coordinate);
    QVERIFY2(testObj.topLeft() == coordinate, "Wrong value returned");
}

void tst_QGeoBoundingBox::bottomRightTest()
{
    QGeoBoundingBox testObj;
    QVERIFY2(testObj.bottomRight().isValid() == false, "Wrong default value");
    QGeoCoordinate coordinate;
    coordinate.setLatitude(30);
    coordinate.setLongitude(20);
    testObj.setBottomRight(coordinate);
    QVERIFY2(testObj.bottomRight() == coordinate, "Wrong value returned");
}

void tst_QGeoBoundingBox::isValidTest()
{
    QGeoBoundingBox testObj;
    QVERIFY2(testObj.isValid() == false, "Wrong default check");
    QGeoCoordinate coordinate;
    coordinate.setLatitude(30);
    coordinate.setLongitude(20);
    testObj.setBottomRight(coordinate);
    QVERIFY2(testObj.isValid() == false, "Wrong top/left check");
    testObj.setTopLeft(coordinate);
    testObj.setBottomRight(QGeoCoordinate());
    QVERIFY2(testObj.isValid() == false, "Wrong bottom/right check");
    testObj.setBottomRight(coordinate);
    QVERIFY2(testObj.isValid() == true, "Box is valid");
}

void tst_QGeoBoundingBox::operatorsTest()
{
    QGeoBoundingBox testObj;
    QGeoCoordinate coordinate;
    coordinate.setLatitude(30);
    coordinate.setLongitude(20);
    testObj.setTopLeft(coordinate);
    QGeoBoundingBox testObj2;
    testObj2 = testObj;
    QVERIFY2(testObj == testObj2, "Not copied correctly");
    testObj2.setBottomRight(coordinate);
    QVERIFY2(testObj != testObj2, "Object should be different");
}

QTEST_APPLESS_MAIN(tst_QGeoBoundingBox);

#include "tst_qgeoboundingbox.moc"
