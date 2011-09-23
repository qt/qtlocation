/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/location

#include <QtTest/QtTest>
#include <qgeoboundingbox.h>
#include <qgeoboundingcircle.h>
#include <qgeocoordinate.h>

QT_USE_NAMESPACE
Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoBoundingBox)

class tst_QGeoBoundingBox : public QObject
{
    Q_OBJECT

private slots:
    void default_constructor();
    void center_constructor();
    void corner_constructor();
    void copy_constructor();
    void assignment();
    void destructor();

    void equality();
    void equality_data();

    void isValid();
    void isValid_data();

    void isEmpty();
    void isEmpty_data();

    void corners();
    void corners_data();

    void setCorners();

    void width();
    void width_data();

    void height();
    void height_data();

    void center();
    void center_data();

    void containsCoord();
    void containsCoord_data();

    void containsBoxAndIntersects();
    void containsBoxAndIntersects_data();

    void translate();
    void translate_data();

    void unite();
    void unite_data();

    void clone();
    void areaComparison();
};

void tst_QGeoBoundingBox::default_constructor()
{
    QGeoBoundingBox box;
    QCOMPARE(box.topLeft().isValid(), false);
    QCOMPARE(box.bottomRight().isValid(), false);
}

void tst_QGeoBoundingBox::center_constructor()
{
    QGeoBoundingBox b1 = QGeoBoundingBox(QGeoCoordinate(5.0, 5.0), 10.0, 10.0);

    QCOMPARE(b1.topLeft(), QGeoCoordinate(10.0, 0.0));
    QCOMPARE(b1.bottomRight(), QGeoCoordinate(0.0, 10.0));
}

void tst_QGeoBoundingBox::corner_constructor()
{
    QGeoBoundingBox b1 = QGeoBoundingBox(QGeoCoordinate(10.0, 0.0),
                                         QGeoCoordinate(0.0, 10.0));

    QCOMPARE(b1.topLeft(), QGeoCoordinate(10.0, 0.0));
    QCOMPARE(b1.bottomRight(), QGeoCoordinate(0.0, 10.0));
}

void tst_QGeoBoundingBox::copy_constructor()
{
    QGeoBoundingBox b1 = QGeoBoundingBox(QGeoCoordinate(10.0, 0.0),
                                         QGeoCoordinate(0.0, 10.0));
    QGeoBoundingBox b2 = QGeoBoundingBox(b1);

    QCOMPARE(b2.topLeft(), QGeoCoordinate(10.0, 0.0));
    QCOMPARE(b2.bottomRight(), QGeoCoordinate(0.0, 10.0));

    b2.setTopLeft(QGeoCoordinate(30.0, 0.0));
    b2.setBottomRight(QGeoCoordinate(0.0, 30.0));
    QCOMPARE(b1.topLeft(), QGeoCoordinate(10.0, 0.0));
    QCOMPARE(b1.bottomRight(), QGeoCoordinate(0.0, 10.0));
}

void tst_QGeoBoundingBox::destructor()
{
    QGeoBoundingBox *box = new QGeoBoundingBox();
    delete box;
    // checking for a crash
}

void tst_QGeoBoundingBox::assignment()
{
    QGeoBoundingBox b1 = QGeoBoundingBox(QGeoCoordinate(10.0, 0.0),
                                         QGeoCoordinate(0.0, 10.0));
    QGeoBoundingBox b2 = QGeoBoundingBox(QGeoCoordinate(20.0, 0.0),
                                         QGeoCoordinate(0.0, 20.0));

    b2 = b1;
    QCOMPARE(b2.topLeft(), QGeoCoordinate(10.0, 0.0));
    QCOMPARE(b2.bottomRight(), QGeoCoordinate(0.0, 10.0));

    b2.setTopLeft(QGeoCoordinate(30.0, 0.0));
    b2.setBottomRight(QGeoCoordinate(0.0, 30.0));
    QCOMPARE(b1.topLeft(), QGeoCoordinate(10.0, 0.0));
    QCOMPARE(b1.bottomRight(), QGeoCoordinate(0.0, 10.0));
}

void tst_QGeoBoundingBox::equality()
{
    QFETCH(QGeoBoundingBox, input1);
    QFETCH(QGeoBoundingBox, input2);
    QFETCH(bool, equal);

    QCOMPARE((input1 == input2), equal);
    QCOMPARE((input1 != input2), !equal);
}

void tst_QGeoBoundingBox::equality_data()
{
    QTest::addColumn<QGeoBoundingBox>("input1");
    QTest::addColumn<QGeoBoundingBox>("input2");
    QTest::addColumn<bool>("equal");

    QGeoCoordinate c1(10, 5);
    QGeoCoordinate c2(5, 10);
    QGeoCoordinate c3(20, 15);
    QGeoCoordinate c4(15, 20);

    QGeoBoundingBox b1(c1, c2);
    QGeoBoundingBox b2(c3, c4);
    QGeoBoundingBox b3(c3, c2);
    QGeoBoundingBox b4(c1, c3);
    QGeoBoundingBox b5(c1, c2);

    QTest::newRow("all unequal")
            << b1 << b2 << false;
    QTest::newRow("top left unequal")
            << b1 << b3 << false;
    QTest::newRow("bottom right unequal")
            << b1 << b4 << false;
    QTest::newRow("equal")
            << b1 << b5 << true;
}

void tst_QGeoBoundingBox::isValid()
{
    QFETCH(QGeoBoundingBox, input);
    QFETCH(bool, valid);

    QCOMPARE(input.isValid(), valid);
}

void tst_QGeoBoundingBox::isValid_data()
{
    QTest::addColumn<QGeoBoundingBox>("input");
    QTest::addColumn<bool>("valid");

    QGeoCoordinate c0;
    QGeoCoordinate c1(10, 5);
    QGeoCoordinate c2(5, 10);

    QTest::newRow("both corners invalid")
        << QGeoBoundingBox(c0, c0) << false;
    QTest::newRow("top left corner invalid")
        << QGeoBoundingBox(c0, c2) << false;
    QTest::newRow("bottom right corner invalid")
        << QGeoBoundingBox(c1, c0) << false;
    QTest::newRow("height in wrong order")
        << QGeoBoundingBox(c2, c1) << false;
    QTest::newRow("both corners valid")
        << QGeoBoundingBox(c1, c2) << true;
}

void tst_QGeoBoundingBox::isEmpty()
{
    QFETCH(QGeoBoundingBox, input);
    QFETCH(bool, empty);

    QCOMPARE(input.isEmpty(), empty);
}

void tst_QGeoBoundingBox::isEmpty_data()
{
    QTest::addColumn<QGeoBoundingBox>("input");
    QTest::addColumn<bool>("empty");

    QGeoCoordinate c0;
    QGeoCoordinate c1(10, 5);
    QGeoCoordinate c2(5, 10);
    QGeoCoordinate c3(10, 10);

    QTest::newRow("both corners invalid")
        << QGeoBoundingBox(c0, c0) << true;
    QTest::newRow("top left corner invalid")
        << QGeoBoundingBox(c0, c2) << true;
    QTest::newRow("bottom right corner invalid")
        << QGeoBoundingBox(c1, c0) << true;
    QTest::newRow("zero width")
            << QGeoBoundingBox(c1, c3) << true;
    QTest::newRow("zero height")
            << QGeoBoundingBox(c3, c2) << true;
    QTest::newRow("zero width and height")
            << QGeoBoundingBox(c1, c1) << true;
    QTest::newRow("non-zero width and height")
            << QGeoBoundingBox(c1, c2) << false;
}

void tst_QGeoBoundingBox::corners()
{
    QFETCH(QGeoBoundingBox, box);
    QFETCH(QGeoCoordinate, topLeft);
    QFETCH(QGeoCoordinate, topRight);
    QFETCH(QGeoCoordinate, bottomLeft);
    QFETCH(QGeoCoordinate, bottomRight);

    QCOMPARE(box.topLeft(), topLeft);
    QCOMPARE(box.topRight(), topRight);
    QCOMPARE(box.bottomLeft(), bottomLeft);
    QCOMPARE(box.bottomRight(), bottomRight);
}

void tst_QGeoBoundingBox::corners_data()
{
    QTest::addColumn<QGeoBoundingBox>("box");
    QTest::addColumn<QGeoCoordinate>("topLeft");
    QTest::addColumn<QGeoCoordinate>("topRight");
    QTest::addColumn<QGeoCoordinate>("bottomLeft");
    QTest::addColumn<QGeoCoordinate>("bottomRight");

    QGeoCoordinate c0;
    QGeoCoordinate tl(10, 5);
    QGeoCoordinate br(5, 10);
    QGeoCoordinate tr(10, 10);
    QGeoCoordinate bl(5, 5);

    QTest::newRow("both invalid")
            << QGeoBoundingBox(c0, c0)
            << c0
            << c0
            << c0
            << c0;
    QTest::newRow("top left invalid")
            << QGeoBoundingBox(c0, br)
            << c0
            << c0
            << c0
            << br;
    QTest::newRow("bottom right invalid")
            << QGeoBoundingBox(tl, c0)
            << tl
            << c0
            << c0
            << c0;
    QTest::newRow("both valid")
            << QGeoBoundingBox(tl, br)
            << tl
            << tr
            << bl
            << br;
}

void tst_QGeoBoundingBox::setCorners()
{
    QGeoBoundingBox box(QGeoCoordinate(10.0, 0.0),
                        QGeoCoordinate(0.0, 10.0));

    box.setTopLeft(QGeoCoordinate(20.0, -10.0));

    QCOMPARE(box.topLeft(), QGeoCoordinate(20.0, -10.0));
    QCOMPARE(box.topRight(), QGeoCoordinate(20.0, 10.0));
    QCOMPARE(box.bottomLeft(), QGeoCoordinate(0.0, -10.0));
    QCOMPARE(box.bottomRight(), QGeoCoordinate(0.0, 10.0));

    box.setTopRight(QGeoCoordinate(30.0, 20.0));

    QCOMPARE(box.topLeft(), QGeoCoordinate(30.0, -10.0));
    QCOMPARE(box.topRight(), QGeoCoordinate(30.0, 20.0));
    QCOMPARE(box.bottomLeft(), QGeoCoordinate(0.0, -10.0));
    QCOMPARE(box.bottomRight(), QGeoCoordinate(0.0, 20.0));

    box.setBottomRight(QGeoCoordinate(-10.0, 30.0));

    QCOMPARE(box.topLeft(), QGeoCoordinate(30.0, -10.0));
    QCOMPARE(box.topRight(), QGeoCoordinate(30.0, 30.0));
    QCOMPARE(box.bottomLeft(), QGeoCoordinate(-10.0, -10.0));
    QCOMPARE(box.bottomRight(), QGeoCoordinate(-10.0, 30.0));

    box.setBottomLeft(QGeoCoordinate(-20.0, -20.0));

    QCOMPARE(box.topLeft(), QGeoCoordinate(30.0, -20.0));
    QCOMPARE(box.topRight(), QGeoCoordinate(30.0, 30.0));
    QCOMPARE(box.bottomLeft(), QGeoCoordinate(-20.0, -20.0));
    QCOMPARE(box.bottomRight(), QGeoCoordinate(-20.0, 30.0));


}

void tst_QGeoBoundingBox::width()
{
    QFETCH(QGeoBoundingBox, box);
    QFETCH(double, oldWidth);
    QFETCH(double, newWidth);
    QFETCH(QGeoBoundingBox, newBox);

    if (qIsNaN(oldWidth))
        QVERIFY(qIsNaN(box.width()));
    else
        QCOMPARE(box.width(), oldWidth);

    box.setWidth(newWidth);

    QCOMPARE(box, newBox);
}

void tst_QGeoBoundingBox::width_data()
{
    QTest::addColumn<QGeoBoundingBox>("box");
    QTest::addColumn<double>("oldWidth");
    QTest::addColumn<double>("newWidth");
    QTest::addColumn<QGeoBoundingBox>("newBox");

    QTest::newRow("invalid box")
            << QGeoBoundingBox()
            << qQNaN()
            << 100.0
            << QGeoBoundingBox();

     QTest::newRow("0 width -> negative width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 90.0),
                                QGeoCoordinate(5.0, 90.0))
             << 0.0
             << -1.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 90.0),
                                QGeoCoordinate(5.0, 90.0));

     QTest::newRow("0 width -> 0 width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 90.0),
                                QGeoCoordinate(5.0, 90.0))
             << 0.0
             << 0.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 90.0),
                                QGeoCoordinate(5.0, 90.0));

     QTest::newRow("0 width -> non wrapping width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 90.0),
                                QGeoCoordinate(5.0, 90.0))
             << 0.0
             << 10.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 85.0),
                                QGeoCoordinate(5.0, 95.0));

     QTest::newRow("0 width -> wrapping width positive")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 90.0),
                                QGeoCoordinate(5.0, 90.0))
             << 0.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -5.0),
                                QGeoCoordinate(5.0, -175.0));

     QTest::newRow("0 width -> wrapping width negative")
             << QGeoBoundingBox(QGeoCoordinate(10.0, -90.0),
                                QGeoCoordinate(5.0, -90.0))
             << 0.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 175.0),
                                QGeoCoordinate(5.0, 5.0));

     QTest::newRow("0 width -> 360 width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 90.0),
                                QGeoCoordinate(5.0, 90.0))
             << 0.0
             << 360.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -180.0),
                                QGeoCoordinate(5.0, 180.0));

     QTest::newRow("0 width -> 360+ width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 90.0),
                                QGeoCoordinate(5.0, 90.0))
             << 0.0
             << 370.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -180.0),
                                QGeoCoordinate(5.0, 180.0));

     QTest::newRow("non wrapping width -> negative width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 85.0),
                                QGeoCoordinate(5.0, 95.0))
             << 10.0
             << -1.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 85.0),
                                QGeoCoordinate(5.0, 95.0));

     QTest::newRow("non wrapping width -> 0 width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 85.0),
                                QGeoCoordinate(5.0, 95.0))
             << 10.0
             << 0.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 90.0),
                                QGeoCoordinate(5.0, 90.0));

     QTest::newRow("non wrapping width -> non wrapping width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 85.0),
                                QGeoCoordinate(5.0, 95.0))
             << 10.0
             << 20.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 80.0),
                                QGeoCoordinate(5.0, 100.0));

     QTest::newRow("non wrapping width -> wrapping width positive")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 85.0),
                                QGeoCoordinate(5.0, 95.0))
             << 10.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -5.0),
                                QGeoCoordinate(5.0, -175.0));

     QTest::newRow("non wrapping width -> wrapping width negative")
             << QGeoBoundingBox(QGeoCoordinate(10.0, -95.0),
                                QGeoCoordinate(5.0, -85.0))
             << 10.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 175.0),
                                QGeoCoordinate(5.0, 5.0));

     QTest::newRow("non wrapping width -> 360 width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 85.0),
                                QGeoCoordinate(5.0, 95.0))
             << 10.0
             << 360.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -180.0),
                                QGeoCoordinate(5.0, 180.0));

     QTest::newRow("non wrapping width width -> 360+ width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 85.0),
                                QGeoCoordinate(5.0, 95.0))
             << 10.0
             << 370.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -180.0),
                                QGeoCoordinate(5.0, 180.0));

     QTest::newRow("wrapping width -> negative width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 175.0),
                                QGeoCoordinate(5.0, -85.0))
             << 100.0
             << -1.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 175.0),
                                QGeoCoordinate(5.0, -85.0));

     QTest::newRow("wrapping width -> 0 width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 175.0),
                                QGeoCoordinate(5.0, -85.0))
             << 100.0
             << 0.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -135.0),
                                QGeoCoordinate(5.0, -135.0));

     QTest::newRow("wrapping width -> non wrapping width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 175.0),
                                QGeoCoordinate(5.0, -85.0))
             << 100.0
             << 80.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -175.0),
                                QGeoCoordinate(5.0, -95.0));

     QTest::newRow("wrapping width -> wrapping width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 175.0),
                                QGeoCoordinate(5.0, -85.0))
             << 100.0
             << 120.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 165.0),
                                QGeoCoordinate(5.0, -75.0));

     QTest::newRow("wrapping width -> 360 width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 175.0),
                                QGeoCoordinate(5.0, -85.0))
             << 100.0
             << 360.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -180.0),
                                QGeoCoordinate(5.0, 180.0));

     QTest::newRow("wrapping width width -> 360+ width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 175.0),
                                QGeoCoordinate(5.0, -85.0))
             << 100.0
             << 370.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, -180.0),
                                QGeoCoordinate(5.0, 180.0));
}

void tst_QGeoBoundingBox::height()
{
    QFETCH(QGeoBoundingBox, box);
    QFETCH(double, oldHeight);
    QFETCH(double, newHeight);
    QFETCH(QGeoBoundingBox, newBox);

    if (qIsNaN(oldHeight))
        QVERIFY(qIsNaN(box.height()));
    else
        QCOMPARE(box.height(), oldHeight);

    box.setHeight(newHeight);
    QCOMPARE(box, newBox);
}

void tst_QGeoBoundingBox::height_data()
{
    QTest::addColumn<QGeoBoundingBox>("box");
    QTest::addColumn<double>("oldHeight");
    QTest::addColumn<double>("newHeight");
    QTest::addColumn<QGeoBoundingBox>("newBox");

    QTest::newRow("invalid box")
            << QGeoBoundingBox()
            << qQNaN()
            << 100.0
            << QGeoBoundingBox();

     QTest::newRow("0 height -> negative height")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 5.0),
                                QGeoCoordinate(10.0, 10.0))
             << 0.0
             << -1.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 5.0),
                                QGeoCoordinate(10.0, 10.0));

     QTest::newRow("0 height -> 0 height")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 5.0),
                                QGeoCoordinate(10.0, 10.0))
             << 0.0
             << 0.0
             << QGeoBoundingBox(QGeoCoordinate(10.0, 5.0),
                                QGeoCoordinate(10.0, 10.0));

     QTest::newRow("0 height -> non zero height")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 5.0),
                                QGeoCoordinate(10.0, 10.0))
             << 0.0
             << 20.0
             << QGeoBoundingBox(QGeoCoordinate(20.0, 5.0),
                                QGeoCoordinate(0.0, 10.0));

     QTest::newRow("0 height -> squash top")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(70.0, 70.0))
             << 0.0
             << 60.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 30.0),
                                QGeoCoordinate(50.0, 70.0));

     QTest::newRow("0 height -> squash bottom")
             << QGeoBoundingBox(QGeoCoordinate(-70.0, 30.0),
                                QGeoCoordinate(-70.0, 70.0))
             << 0.0
             << 60.0
             << QGeoBoundingBox(QGeoCoordinate(-50.0, 30.0),
                                QGeoCoordinate(-90.0, 70.0));

     QTest::newRow("0 height -> 180")
             << QGeoBoundingBox(QGeoCoordinate(0.0, 5.0),
                                QGeoCoordinate(0.0, 10.0))
             << 0.0
             << 180.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 5.0),
                                QGeoCoordinate(-90.0, 10.0));

     QTest::newRow("0 height -> 180 squash top")
             << QGeoBoundingBox(QGeoCoordinate(20.0, 5.0),
                                QGeoCoordinate(20.0, 10.0))
             << 0.0
             << 180.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 5.0),
                                QGeoCoordinate(-50.0, 10.0));

     QTest::newRow("0 height -> 180 squash bottom")
             << QGeoBoundingBox(QGeoCoordinate(-20.0, 5.0),
                                QGeoCoordinate(-20.0, 10.0))
             << 0.0
             << 180.0
             << QGeoBoundingBox(QGeoCoordinate(50.0, 5.0),
                                QGeoCoordinate(-90.0, 10.0));

     QTest::newRow("0 height -> 180+")
             << QGeoBoundingBox(QGeoCoordinate(0.0, 5.0),
                                QGeoCoordinate(0.0, 10.0))
             << 0.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 5.0),
                                QGeoCoordinate(-90.0, 10.0));

     QTest::newRow("0 height -> 180+ squash top")
             << QGeoBoundingBox(QGeoCoordinate(20.0, 5.0),
                                QGeoCoordinate(20.0, 10.0))
             << 0.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 5.0),
                                QGeoCoordinate(-50.0, 10.0));

     QTest::newRow("0 height -> 180+ squash bottom")
             << QGeoBoundingBox(QGeoCoordinate(-20.0, 5.0),
                                QGeoCoordinate(-20.0, 10.0))
             << 0.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(50.0, 5.0),
                                QGeoCoordinate(-90.0, 10.0));

     QTest::newRow("non zero height -> negative height")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << 40.0
             << -1.0
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0));

     QTest::newRow("non zero height -> 0 height")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << 40.0
             << 0.0
             << QGeoBoundingBox(QGeoCoordinate(50.0, 30.0),
                                QGeoCoordinate(50.0, 70.0));

     QTest::newRow("non zero height -> non zero height")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << 40.0
             << 20.0
             << QGeoBoundingBox(QGeoCoordinate(60.0, 30.0),
                                QGeoCoordinate(40.0, 70.0));

     QTest::newRow("non zero height -> squash top")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << 40.0
             << 100.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 30.0),
                                QGeoCoordinate(10.0, 70.0));

     QTest::newRow("non zero height -> squash bottom")
             << QGeoBoundingBox(QGeoCoordinate(-30.0, 30.0),
                                QGeoCoordinate(-70.0, 70.0))
             << 40.0
             << 100.0
             << QGeoBoundingBox(QGeoCoordinate(-10.0, 30.0),
                                QGeoCoordinate(-90.0, 70.0));

     QTest::newRow("non zero height -> 180")
             << QGeoBoundingBox(QGeoCoordinate(20.0, 30.0),
                                QGeoCoordinate(-20.0, 70.0))
             << 40.0
             << 180.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 30.0),
                                QGeoCoordinate(-90.0, 70.0));

     QTest::newRow("non zero height -> 180 squash top")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << 40.0
             << 180.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 30.0),
                                QGeoCoordinate(10.0, 70.0));

     QTest::newRow("non zero height -> 180 squash bottom")
             << QGeoBoundingBox(QGeoCoordinate(-30.0, 30.0),
                                QGeoCoordinate(-70.0, 70.0))
             << 40.0
             << 180.0
             << QGeoBoundingBox(QGeoCoordinate(-10.0, 30.0),
                                QGeoCoordinate(-90.0, 70.0));

     QTest::newRow("non zero height -> 180+")
             << QGeoBoundingBox(QGeoCoordinate(20.0, 30.0),
                                QGeoCoordinate(-20.0, 70.0))
             << 40.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 30.0),
                                QGeoCoordinate(-90.0, 70.0));

     QTest::newRow("non zero height -> 180+ squash top")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << 40.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(90.0, 30.0),
                                QGeoCoordinate(10.0, 70.0));

     QTest::newRow("non zero height -> 180+ squash bottom")
             << QGeoBoundingBox(QGeoCoordinate(-30.0, 30.0),
                                QGeoCoordinate(-70.0, 70.0))
             << 40.0
             << 190.0
             << QGeoBoundingBox(QGeoCoordinate(-10.0, 30.0),
                                QGeoCoordinate(-90.0, 70.0));
}

void tst_QGeoBoundingBox::center()
{
    QFETCH(QGeoBoundingBox, box);
    QFETCH(QGeoCoordinate, oldCenter);
    QFETCH(QGeoCoordinate, newCenter);
    QFETCH(QGeoBoundingBox, newBox);

    QCOMPARE(box.center(), oldCenter);
    box.setCenter(newCenter);
    QCOMPARE(box, newBox);
}

void tst_QGeoBoundingBox::center_data()
{
    QTest::addColumn<QGeoBoundingBox>("box");
    QTest::addColumn<QGeoCoordinate>("oldCenter");
    QTest::addColumn<QGeoCoordinate>("newCenter");
    QTest::addColumn<QGeoBoundingBox>("newBox");

     QTest::newRow("invalid")
          << QGeoBoundingBox()
          << QGeoCoordinate()
          << QGeoCoordinate(0.0, 0.0)
          << QGeoBoundingBox();

     QTest::newRow("zero width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, 5.0),
                                QGeoCoordinate(5.0, 5.0))
          << QGeoCoordinate(7.5, 5.0)
          << QGeoCoordinate(20.0, 20.0)
          << QGeoBoundingBox(QGeoCoordinate(22.5, 20.0),
                             QGeoCoordinate(17.5, 20.0));

     QTest::newRow("360 width")
             << QGeoBoundingBox(QGeoCoordinate(10.0, -180.0),
                                QGeoCoordinate(5.0, 180.0))
          << QGeoCoordinate(7.5, 0.0)
          << QGeoCoordinate(20.0, 20.0)
          << QGeoBoundingBox(QGeoCoordinate(22.5, -180.0),
                             QGeoCoordinate(17.5, 180.0));

     QTest::newRow("zero height")
             << QGeoBoundingBox(QGeoCoordinate(5.0, 5.0),
                                QGeoCoordinate(5.0, 10.0))
          << QGeoCoordinate(5.0, 7.5)
          << QGeoCoordinate(20.0, 20.0)
          << QGeoBoundingBox(QGeoCoordinate(20.0, 17.5),
                             QGeoCoordinate(20.0, 22.5));

     QTest::newRow("180 height -> move")
             << QGeoBoundingBox(QGeoCoordinate(90.0, 5.0),
                                QGeoCoordinate(-90.0, 10.0))
          << QGeoCoordinate(0.0, 7.5)
          << QGeoCoordinate(0.0, 20.0)
          << QGeoBoundingBox(QGeoCoordinate(90.0, 17.5),
                             QGeoCoordinate(-90.0, 22.5));

     QTest::newRow("180 height -> squash top")
             << QGeoBoundingBox(QGeoCoordinate(90.0, 5.0),
                                QGeoCoordinate(-90.0, 10.0))
          << QGeoCoordinate(0.0, 7.5)
          << QGeoCoordinate(-20.0, 20.0)
          << QGeoBoundingBox(QGeoCoordinate(50.0, 17.5),
                             QGeoCoordinate(-90.0, 22.5));

     QTest::newRow("180 height -> squash bottom")
             << QGeoBoundingBox(QGeoCoordinate(90.0, 5.0),
                                QGeoCoordinate(-90.0, 10.0))
          << QGeoCoordinate(0.0, 7.5)
          << QGeoCoordinate(20.0, 20.0)
          << QGeoBoundingBox(QGeoCoordinate(90.0, 17.5),
                             QGeoCoordinate(-50.0, 22.5));

     QTest::newRow("non wrapping -> non wrapping")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << QGeoCoordinate(50.0, 50.0)
             << QGeoCoordinate(10.0, 10.0)
             << QGeoBoundingBox(QGeoCoordinate(30.0, -10.0),
                                QGeoCoordinate(-10.0, 30.0));

     QTest::newRow("non wrapping -> wrapping")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << QGeoCoordinate(50.0, 50.0)
             << QGeoCoordinate(10.0, 170.0)
             << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                                QGeoCoordinate(-10.0, -170.0));

     QTest::newRow("non wrapping -> squash top")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << QGeoCoordinate(50.0, 50.0)
             << QGeoCoordinate(80.0, 50.0)
             << QGeoBoundingBox(QGeoCoordinate(90.0, 30.0),
                                QGeoCoordinate(70.0, 70.0));

     QTest::newRow("non wrapping -> squash bottom")
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0))
             << QGeoCoordinate(50.0, 50.0)
             << QGeoCoordinate(-80.0, 50.0)
             << QGeoBoundingBox(QGeoCoordinate(-70.0, 30.0),
                                QGeoCoordinate(-90.0, 70.0));

     QTest::newRow("wrapping -> non wrapping")
             << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                                QGeoCoordinate(-10.0, -170.0))
             << QGeoCoordinate(10.0, 170.0)
             << QGeoCoordinate(50.0, 50.0)
             << QGeoBoundingBox(QGeoCoordinate(70.0, 30.0),
                                QGeoCoordinate(30.0, 70.0));

     QTest::newRow("wrapping -> wrapping")
             << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                                QGeoCoordinate(-10.0, -170.0))
             << QGeoCoordinate(10.0, 170.0)
             << QGeoCoordinate(10.0, -170.0)
             << QGeoBoundingBox(QGeoCoordinate(30.0, 170.0),
                                QGeoCoordinate(-10.0, -150.0));

     QTest::newRow("wrapping -> squash top")
             << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                                QGeoCoordinate(-10.0, -170.0))
             << QGeoCoordinate(10.0, 170.0)
             << QGeoCoordinate(80.0, 170.0)
             << QGeoBoundingBox(QGeoCoordinate(90.0, 150.0),
                                QGeoCoordinate(70.0, -170.0));

     QTest::newRow("wrapping -> squash bottom")
             << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                                QGeoCoordinate(-10.0, -170.0))
             << QGeoCoordinate(10.0, 170.0)
             << QGeoCoordinate(-80.0, 170.0)
             << QGeoBoundingBox(QGeoCoordinate(-70.0, 150.0),
                                QGeoCoordinate(-90.0, -170.0));
}


void tst_QGeoBoundingBox::containsCoord()
{
    QFETCH(QGeoBoundingBox, box);
    QFETCH(QGeoCoordinate, coord);
    QFETCH(bool, contains);

    QCOMPARE(box.contains(coord), contains);
}

void tst_QGeoBoundingBox::containsCoord_data()
{
    QTest::addColumn<QGeoBoundingBox>("box");
    QTest::addColumn<QGeoCoordinate>("coord");
    QTest::addColumn<bool>("contains");

    QGeoBoundingBox b1(QGeoCoordinate(70, 30), QGeoCoordinate(30, 70));

    double lonLO1 = 20.0;
    double lonL1 = 30.0;
    double lonLI1 = 40.0;
    double lonC1 = 50.0;
    double lonRI1 = 60.0;
    double lonR1 = 70.0;
    double lonRO1 = 80.0;

    double latTO1 = 80.0;
    double latT1 = 70.0;
    double latTI1 = 60.0;
    double latC1 = 50.0;
    double latBI1 = 40.0;
    double latB1 = 30.0;
    double latBO1 = 20.0;

    QTest::newRow("non wrapped - in center")
            << b1 << QGeoCoordinate(latC1, lonC1) << true;
    QTest::newRow("non wrapped - left edge - inside")
            << b1 << QGeoCoordinate(latC1, lonLI1) << true;
    QTest::newRow("non wrapped - left edge")
            << b1 << QGeoCoordinate(latC1, lonL1) << true;
    QTest::newRow("non wrapped - left edge - outside")
            << b1 << QGeoCoordinate(latC1, lonLO1) << false;
    QTest::newRow("non wrapped - right edge - inside")
            << b1 << QGeoCoordinate(latC1, lonRI1) << true;
    QTest::newRow("non wrapped - right edge")
            << b1 << QGeoCoordinate(latC1, lonR1) << true;
    QTest::newRow("non wrapped - right edge - outside")
            << b1 << QGeoCoordinate(latC1, lonRO1) << false;
    QTest::newRow("non wrapped - top edge - inside")
            << b1 << QGeoCoordinate(latTI1, lonC1) << true;
    QTest::newRow("non wrapped - top edge")
            << b1 << QGeoCoordinate(latT1, lonC1) << true;
    QTest::newRow("non wrapped - top edge - outside")
            << b1 << QGeoCoordinate(latTO1, lonC1) << false;
    QTest::newRow("non wrapped - bottom edge - inside")
            << b1 << QGeoCoordinate(latBI1, lonC1) << true;
    QTest::newRow("non wrapped - bottom edge")
            << b1 << QGeoCoordinate(latB1, lonC1) << true;
    QTest::newRow("non wrapped - bottom edge - outside")
            << b1 << QGeoCoordinate(latBO1, lonC1) << false;
    QTest::newRow("non wrapped - top left - inside")
            << b1 << QGeoCoordinate(latTI1, lonLI1) << true;
    QTest::newRow("non wrapped - top left")
            << b1 << QGeoCoordinate(latT1, lonL1) << true;
    QTest::newRow("non wrapped - top left - outside")
            << b1 << QGeoCoordinate(latTO1, lonLO1) << false;
    QTest::newRow("non wrapped - top right - inside")
            << b1 << QGeoCoordinate(latTI1, lonRI1) << true;
    QTest::newRow("non wrapped - top right")
            << b1 << QGeoCoordinate(latT1, lonR1) << true;
    QTest::newRow("non wrapped - top right - outside")
            << b1 << QGeoCoordinate(latTO1, lonRO1) << false;
    QTest::newRow("non wrapped - bottom left - inside")
            << b1 << QGeoCoordinate(latBI1, lonLI1) << true;
    QTest::newRow("non wrapped - bottom left")
            << b1 << QGeoCoordinate(latB1, lonL1) << true;
    QTest::newRow("non wrapped - bottom left - outside")
            << b1 << QGeoCoordinate(latBO1, lonLO1) << false;
    QTest::newRow("non wrapped - bottom right - inside")
            << b1 << QGeoCoordinate(latBI1, lonRI1) << true;
    QTest::newRow("non wrapped - bottom right")
            << b1 << QGeoCoordinate(latB1, lonR1) << true;
    QTest::newRow("non wrapped - bottom right - outside")
            << b1 << QGeoCoordinate(latBO1, lonRO1) << false;

    QGeoBoundingBox b2(QGeoCoordinate(70, 150), QGeoCoordinate(30, -170));

    double lonLO2 = 140.0;
    double lonL2 = 150.0;
    double lonLI2 = 160.0;
    double lonC2 = 170.0;
    double lonRI2 = 180.0;
    double lonR2 = -170.0;
    double lonRO2 = -160.0;

    double latTO2 = 80.0;
    double latT2 = 70.0;
    double latTI2 = 60.0;
    double latC2 = 50.0;
    double latBI2 = 40.0;
    double latB2 = 30.0;
    double latBO2 = 20.0;

    QTest::newRow("wrapped - in center")
            << b2 << QGeoCoordinate(latC2, lonC2) << true;
    QTest::newRow("wrapped - left edge - inside")
            << b2 << QGeoCoordinate(latC2, lonLI2) << true;
    QTest::newRow("wrapped - left edge")
            << b2 << QGeoCoordinate(latC2, lonL2) << true;
    QTest::newRow("wrapped - left edge - outside")
            << b2 << QGeoCoordinate(latC2, lonLO2) << false;
    QTest::newRow("wrapped - right edge - inside")
            << b2 << QGeoCoordinate(latC2, lonRI2) << true;
    QTest::newRow("wrapped - right edge")
            << b2 << QGeoCoordinate(latC2, lonR2) << true;
    QTest::newRow("wrapped - right edge - outside")
            << b2 << QGeoCoordinate(latC2, lonRO2) << false;
    QTest::newRow("wrapped - top edge - inside")
            << b2 << QGeoCoordinate(latTI2, lonC2) << true;
    QTest::newRow("wrapped - top edge")
            << b2 << QGeoCoordinate(latT2, lonC2) << true;
    QTest::newRow("wrapped - top edge - outside")
            << b2 << QGeoCoordinate(latTO2, lonC2) << false;
    QTest::newRow("wrapped - bottom edge - inside")
            << b2 << QGeoCoordinate(latBI2, lonC2) << true;
    QTest::newRow("wrapped - bottom edge")
            << b2 << QGeoCoordinate(latB2, lonC2) << true;
    QTest::newRow("wrapped - bottom edge - outside")
            << b2 << QGeoCoordinate(latBO2, lonC2) << false;
    QTest::newRow("wrapped - top left - inside")
            << b2 << QGeoCoordinate(latTI2, lonLI2) << true;
    QTest::newRow("wrapped - top left")
            << b2 << QGeoCoordinate(latT2, lonL2) << true;
    QTest::newRow("wrapped - top left - outside")
            << b2 << QGeoCoordinate(latTO2, lonLO2) << false;
    QTest::newRow("wrapped - top right - inside")
            << b2 << QGeoCoordinate(latTI2, lonRI2) << true;
    QTest::newRow("wrapped - top right")
            << b2 << QGeoCoordinate(latT2, lonR2) << true;
    QTest::newRow("wrapped - top right - outside")
            << b2 << QGeoCoordinate(latTO2, lonRO2) << false;
    QTest::newRow("wrapped - bottom left - inside")
            << b2 << QGeoCoordinate(latBI2, lonLI2) << true;
    QTest::newRow("wrapped - bottom left")
            << b2 << QGeoCoordinate(latB2, lonL2) << true;
    QTest::newRow("wrapped - bottom left - outside")
            << b2 << QGeoCoordinate(latBO2, lonLO2) << false;
    QTest::newRow("wrapped - bottom right - inside")
            << b2 << QGeoCoordinate(latBI2, lonRI2) << true;
    QTest::newRow("wrapped - bottom right")
            << b2 << QGeoCoordinate(latB2, lonR2) << true;
    QTest::newRow("wrapped - bottom right - outside")
            << b2 << QGeoCoordinate(latBO2, lonRO2) << false;

    QGeoBoundingBox b3(QGeoCoordinate(90, 30), QGeoCoordinate(50, 70));

    double lonLO3 = 20.0;
    double lonL3 = 30.0;
    double lonLI3 = 40.0;
    double lonC3 = 50.0;
    double lonRI3 = 60.0;
    double lonR3 = 70.0;
    double lonRO3 = 80.0;

    double latT3 = 90.0;
    double latTI3 = 80.0;
    double latC3 = 70.0;
    double latBI3 = 60.0;
    double latB3 = 50.0;
    double latBO3 = 40.0;

    QTest::newRow("north pole - in center")
            << b3 << QGeoCoordinate(latC3, lonC3) << true;
    QTest::newRow("north pole - left edge - inside")
            << b3 << QGeoCoordinate(latC3, lonLI3) << true;
    QTest::newRow("north pole - left edge")
            << b3 << QGeoCoordinate(latC3, lonL3) << true;
    QTest::newRow("north pole - left edge - outside")
            << b3 << QGeoCoordinate(latC3, lonLO3) << false;
    QTest::newRow("north pole - right edge - inside")
            << b3 << QGeoCoordinate(latC3, lonRI3) << true;
    QTest::newRow("north pole - right edge")
            << b3 << QGeoCoordinate(latC3, lonR3) << true;
    QTest::newRow("north pole - right edge - outside")
            << b3 << QGeoCoordinate(latC3, lonRO3) << false;
    QTest::newRow("north pole - top edge - inside")
            << b3 << QGeoCoordinate(latTI3, lonC3) << true;
    QTest::newRow("north pole - top edge")
            << b3 << QGeoCoordinate(latT3, lonC3) << true;
    QTest::newRow("north pole - top left - inside")
            << b3 << QGeoCoordinate(latTI3, lonLI3) << true;
    QTest::newRow("north pole - top left")
            << b3 << QGeoCoordinate(latT3, lonL3) << true;
    QTest::newRow("north pole - top left - outside")
            << b3 << QGeoCoordinate(latT3, lonLO3) << true;
    QTest::newRow("north pole - top right - inside")
            << b3 << QGeoCoordinate(latTI3, lonRI3) << true;
    QTest::newRow("north pole - top right")
            << b3 << QGeoCoordinate(latT3, lonR3) << true;
    QTest::newRow("north pole - top right - outside")
            << b3 << QGeoCoordinate(latT3, lonRO3) << true;

    QGeoBoundingBox b4(QGeoCoordinate(-50, 30), QGeoCoordinate(-90, 70));

    double lonLO4 = 20.0;
    double lonL4 = 30.0;
    double lonLI4 = 40.0;
    double lonC4 = 50.0;
    double lonRI4 = 60.0;
    double lonR4 = 70.0;
    double lonRO4 = 80.0;

    double latTO4 = -40.0;
    double latT4 = -50.0;
    double latTI4 = -60.0;
    double latC4 = -70.0;
    double latBI4 = -80.0;
    double latB4 = -90.0;

    QTest::newRow("south pole - in center")
            << b4 << QGeoCoordinate(latC4, lonC4) << true;
    QTest::newRow("south pole - left edge - inside")
            << b4 << QGeoCoordinate(latC4, lonLI4) << true;
    QTest::newRow("south pole - left edge")
            << b4 << QGeoCoordinate(latC4, lonL4) << true;
    QTest::newRow("south pole - left edge - outside")
            << b4 << QGeoCoordinate(latC4, lonLO4) << false;
    QTest::newRow("south pole - right edge - inside")
            << b4 << QGeoCoordinate(latC4, lonRI4) << true;
    QTest::newRow("south pole - right edge")
            << b4 << QGeoCoordinate(latC4, lonR4) << true;
    QTest::newRow("south pole - right edge - outside")
            << b4 << QGeoCoordinate(latC4, lonRO4) << false;
    QTest::newRow("south pole - bottom edge - inside")
            << b4 << QGeoCoordinate(latBI4, lonC4) << true;
    QTest::newRow("south pole - bottom edge")
            << b4 << QGeoCoordinate(latB4, lonC4) << true;
    QTest::newRow("south pole - bottom left - inside")
            << b4 << QGeoCoordinate(latBI4, lonLI4) << true;
    QTest::newRow("south pole - bottom left")
            << b4 << QGeoCoordinate(latB4, lonL4) << true;
    QTest::newRow("south pole - bottom left - outside")
            << b4 << QGeoCoordinate(latB4, lonLO4) << true;
    QTest::newRow("south pole - bottom right - inside")
            << b4 << QGeoCoordinate(latBI4, lonRI4) << true;
    QTest::newRow("south pole - bottom right")
            << b4 << QGeoCoordinate(latB4, lonR4) << true;
    QTest::newRow("south pole - bottom right - outside")
            << b4 << QGeoCoordinate(latB4, lonRO4) << true;
}

void tst_QGeoBoundingBox::containsBoxAndIntersects()
{
    QFETCH(QGeoBoundingBox, box1);
    QFETCH(QGeoBoundingBox, box2);
    QFETCH(bool, contains);
    QFETCH(bool, intersects);

    QCOMPARE(box1.contains(box2), contains);
    QCOMPARE(box1.intersects(box2), intersects);
}

void tst_QGeoBoundingBox::containsBoxAndIntersects_data()
{
    QTest::addColumn<QGeoBoundingBox>("box1");
    QTest::addColumn<QGeoBoundingBox>("box2");
    QTest::addColumn<bool>("contains");
    QTest::addColumn<bool>("intersects");

    QGeoBoundingBox b1(QGeoCoordinate(30.0, -30.0),
                       QGeoCoordinate(-30.0, 30.0));

    QTest::newRow("non wrapped same")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << true << true;

    QTest::newRow("non wrapped smaller")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(20.0, -20.0),
                               QGeoCoordinate(-20.0, 20.0))
            << true << true;

    QTest::newRow("non wrapped larger")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(40.0, -40.0),
                               QGeoCoordinate(-40.0, 40.0))
            << false << true;

    QTest::newRow("non wrapped outside top")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(80.0, -30.0),
                               QGeoCoordinate(50.0, 30.0))
            << false << false;

    QTest::newRow("non wrapped outside bottom")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-50.0, -30.0),
                               QGeoCoordinate(-80.0, 30.0))
            << false << false;

    QTest::newRow("non wrapped outside left")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(30.0, -80.0),
                               QGeoCoordinate(-30.0, -50.0))
            << false << false;

    QTest::newRow("non wrapped outside wrapped")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << false << false;

    QTest::newRow("non wrapped outside right")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(30.0, 50.0),
                               QGeoCoordinate(-30.0, 80.0))
            << false << false;

    QTest::newRow("non wrapped top left cross")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(40.0, -40.0),
                               QGeoCoordinate(20.0, -20.0))
            << false << true;

    QTest::newRow("non wrapped top cross")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(40.0, -10.0),
                               QGeoCoordinate(20.0, 10.0))
            << false << true;

    QTest::newRow("non wrapped top right cross")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(40.0, 20.0),
                               QGeoCoordinate(20.0, 40.0))
            << false << true;

    QTest::newRow("non wrapped left cross")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(10.0, -40.0),
                               QGeoCoordinate(-10.0, -20.0))
            << false << true;

    QTest::newRow("non wrapped right cross")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(10.0, 20.0),
                               QGeoCoordinate(-10.0, 40.0))
            << false << true;

    QTest::newRow("non wrapped bottom left cross")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-20.0, -40.0),
                               QGeoCoordinate(-40.0, -20.0))
            << false << true;

    QTest::newRow("non wrapped bottom cross")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-20.0, -10.0),
                               QGeoCoordinate(-40.0, 10.0))
            << false << true;

    QTest::newRow("non wrapped bottom right cross")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-20.0, 20.0),
                               QGeoCoordinate(-40.0, 40.0))
            << false << true;

    QTest::newRow("non wrapped top left touch outside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(50.0, -50.0),
                               QGeoCoordinate(30.0, -30.0))
            << false << true;

    QTest::newRow("non wrapped top touch outside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(50.0, -10.0),
                               QGeoCoordinate(30.0, 10.0))
            << false << true;

    QTest::newRow("non wrapped top right touch outside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(50.0, 30.0),
                               QGeoCoordinate(30.0, 50.0))
            << false << true;

    QTest::newRow("non wrapped left touch outside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(10.0, -50.0),
                               QGeoCoordinate(-10.0, -30.0))
            << false << true;

    QTest::newRow("non wrapped right touch outside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(10.0, 30.0),
                               QGeoCoordinate(-10.0, 50.0))
            << false << true;

    QTest::newRow("non wrapped bottom left touch outside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-30.0, -30.0),
                               QGeoCoordinate(-50.0, -50.0))
            << false << true;

    QTest::newRow("non wrapped bottom touch outside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-30.0, -10.0),
                               QGeoCoordinate(-50.0, 10.0))
            << false << true;

    QTest::newRow("non wrapped bottom right touch outside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-30.0, 30.0),
                               QGeoCoordinate(-50.0, 50.0))
            << false << true;

    QTest::newRow("non wrapped top left touch inside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(10.0, -10.0))
            << true << true;

    QTest::newRow("non wrapped top touch inside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(30.0, -10.0),
                               QGeoCoordinate(10.0, 10.0))
            << true << true;

    QTest::newRow("non wrapped top right touch inside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(30.0, 10.0),
                               QGeoCoordinate(10.0, 30.0))
            << true << true;

    QTest::newRow("non wrapped left touch inside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(10.0, -30.0),
                               QGeoCoordinate(-10.0, -10.0))
            << true << true;

    QTest::newRow("non wrapped right touch inside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(10.0, 10.0),
                               QGeoCoordinate(-10.0, 30.0))
            << true << true;

    QTest::newRow("non wrapped bottom left touch inside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-10.0, -30.0),
                               QGeoCoordinate(-30.0, -10.0))
            << true << true;

    QTest::newRow("non wrapped bottom touch inside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-10.0, -10.0),
                               QGeoCoordinate(-30.0, 10.0))
            << true << true;

    QTest::newRow("non wrapped bottom right touch inside")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-10.0, 10.0),
                               QGeoCoordinate(-30.0, 30.0))
            << true << true;

    QTest::newRow("non wrapped top lon strip")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(40.0, -40.0),
                               QGeoCoordinate(20.0, 40.0))
            << false << true;

    QTest::newRow("non wrapped center lon strip")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(10.0, -40.0),
                               QGeoCoordinate(-10.0, 40.0))
            << false << true;

    QTest::newRow("non wrapped bottom lon strip")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(-20.0, -40.0),
                               QGeoCoordinate(-40.0, 40.0))
            << false << true;

    QTest::newRow("non wrapped left lat strip")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(40.0, -40.0),
                               QGeoCoordinate(-40.0, -20.0))
            << false << true;

    QTest::newRow("non wrapped center lat strip")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(40.0, -10.0),
                               QGeoCoordinate(-40.0, 10.0))
            << false << true;

    QTest::newRow("non wrapped right lat strip")
            << b1
            << QGeoBoundingBox(QGeoCoordinate(40.0, 20.0),
                               QGeoCoordinate(-40.0, 40.0))
            << false << true;

    QGeoBoundingBox b2(QGeoCoordinate(30.0, 150.0),
                       QGeoCoordinate(-30.0, -150.0));

    QTest::newRow("wrapped same")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << true << true;

    QTest::newRow("wrapped smaller")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(20.0, 160.0),
                               QGeoCoordinate(-20.0, -160.0))
            << true << true;

    QTest::newRow("wrapped larger")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(40.0, 140.0),
                               QGeoCoordinate(-40.0, -140.0))
            << false << true;

    QTest::newRow("wrapped outside top")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(80.0, 150.0),
                               QGeoCoordinate(50.0, -150.0))
            << false << false;

    QTest::newRow("wrapped outside bottom")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-50.0, 150.0),
                               QGeoCoordinate(-80.0, -150.0))
            << false << false;

    QTest::newRow("wrapped outside left")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(30.0, 70.0),
                               QGeoCoordinate(-30.0, 130.0))
            << false << false;

    QTest::newRow("wrapped outside right")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(30.0, -130.0),
                               QGeoCoordinate(-30.0, -70.0))
            << false << false;

    QTest::newRow("wrapped top left cross")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(40.0, 140.0),
                               QGeoCoordinate(20.0, 160.0))
            << false << true;

    QTest::newRow("wrapped top cross")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(40.0, 170.0),
                               QGeoCoordinate(20.0, -170.0))
            << false << true;

    QTest::newRow("wrapped top right cross")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(40.0, -160.0),
                               QGeoCoordinate(20.0, -140.0))
            << false << true;

    QTest::newRow("wrapped left cross")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(10.0, 140.0),
                               QGeoCoordinate(-10.0, 160.0))
            << false << true;

    QTest::newRow("wrapped right cross")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(10.0, -160.0),
                               QGeoCoordinate(-10.0, -140.0))
            << false << true;

    QTest::newRow("wrapped bottom left cross")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-20.0, 140.0),
                               QGeoCoordinate(-40.0, 160.0))
            << false << true;

    QTest::newRow("wrapped bottom cross")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-20.0, 170.0),
                               QGeoCoordinate(-40.0, -170.0))
            << false << true;

    QTest::newRow("wrapped bottom right cross")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-20.0, -160.0),
                               QGeoCoordinate(-40.0, -140.0))
            << false << true;

    QTest::newRow("wrapped top left touch outside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(50.0, 130.0),
                               QGeoCoordinate(30.0, 150.0))
            << false << true;

    QTest::newRow("wrapped top touch outside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(50.0, 170.0),
                               QGeoCoordinate(30.0, -170.0))
            << false << true;

    QTest::newRow("wrapped top right touch outside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(50.0, -150.0),
                               QGeoCoordinate(30.0, -130.0))
            << false << true;

    QTest::newRow("wrapped left touch outside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(10.0, 130.0),
                               QGeoCoordinate(-10.0, 150.0))
            << false << true;

    QTest::newRow("wrapped right touch outside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(10.0, -150.0),
                               QGeoCoordinate(-10.0, -130.0))
            << false << true;

    QTest::newRow("wrapped bottom left touch outside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-30.0, 150.0),
                               QGeoCoordinate(-50.0, 130.0))
            << false << true;

    QTest::newRow("wrapped bottom touch outside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-30.0, 170.0),
                               QGeoCoordinate(-50.0, -170.0))
            << false << true;

    QTest::newRow("wrapped bottom right touch outside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-30.0, -150.0),
                               QGeoCoordinate(-50.0, -130.0))
            << false << true;

    QTest::newRow("wrapped top left touch inside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(10.0, 170.0))
            << true << true;

    QTest::newRow("wrapped top touch inside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(30.0, 170.0),
                               QGeoCoordinate(10.0, -170.0))
            << true << true;

    QTest::newRow("wrapped top right touch inside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(30.0, -170.0),
                               QGeoCoordinate(10.0, -150.0))
            << true << true;

    QTest::newRow("wrapped left touch inside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(10.0, 150.0),
                               QGeoCoordinate(-10.0, 170.0))
            << true << true;

    QTest::newRow("wrapped right touch inside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(10.0, -170.0),
                               QGeoCoordinate(-10.0, -150.0))
            << true << true;

    QTest::newRow("wrapped bottom left touch inside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-10.0, 150.0),
                               QGeoCoordinate(-30.0, 170.0))
            << true << true;

    QTest::newRow("wrapped bottom touch inside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-10.0, 170.0),
                               QGeoCoordinate(-30.0, -170.0))
            << true << true;

    QTest::newRow("wrapped bottom right touch inside")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-10.0, -170.0),
                               QGeoCoordinate(-30.0, -150.0))
            << true << true;

    QTest::newRow("wrapped top lon strip")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(40.0, 140.0),
                               QGeoCoordinate(20.0, -140.0))
            << false << true;

    QTest::newRow("wrapped center lon strip")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(10.0, 140.0),
                               QGeoCoordinate(-10.0, -140.0))
            << false << true;

    QTest::newRow("wrapped bottom lon strip")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(-20.0, 140.0),
                               QGeoCoordinate(-40.0, -140.0))
            << false << true;

    QTest::newRow("wrapped left lat strip")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(40.0, 140.0),
                               QGeoCoordinate(-40.0, 160.0))
            << false << true;

    QTest::newRow("wrapped center lat strip")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(40.0, 170.0),
                               QGeoCoordinate(-40.0, -170.0))
            << false << true;

    QTest::newRow("wrapped right lat strip")
            << b2
            << QGeoBoundingBox(QGeoCoordinate(40.0, -160.0),
                               QGeoCoordinate(-40.0, -140.0))
            << false << true;

    QTest::newRow("north pole touching")
      << QGeoBoundingBox(QGeoCoordinate(90.0, 20.0),
                         QGeoCoordinate(40.0, 40.0))
      << QGeoBoundingBox(QGeoCoordinate(90.0, 60.0),
                         QGeoCoordinate(30.0, 80.0))
      << false << true;

    QTest::newRow("south pole touching")
      << QGeoBoundingBox(QGeoCoordinate(40.0, 20.0),
                         QGeoCoordinate(-90.0, 40.0))
      << QGeoBoundingBox(QGeoCoordinate(30.0, 60.0),
                         QGeoCoordinate(-90.0, 80.0))
      << false << true;
}

void tst_QGeoBoundingBox::translate()
{
    QFETCH(QGeoBoundingBox, box);
    QFETCH(double, degreesLatitude);
    QFETCH(double, degreesLongitude);
    QFETCH(QGeoBoundingBox, newBox);

    QGeoBoundingBox test = box.translated(degreesLatitude, degreesLongitude);
    QCOMPARE(test, newBox);
    box.translate(degreesLatitude, degreesLongitude);
    QCOMPARE(box, newBox);

}

void tst_QGeoBoundingBox::translate_data()
{
    QTest::addColumn<QGeoBoundingBox>("box");
    QTest::addColumn<double>("degreesLatitude");
    QTest::addColumn<double>("degreesLongitude");
    QTest::addColumn<QGeoBoundingBox>("newBox");

    QTest::newRow("invalid")
            << QGeoBoundingBox()
            << 20.0
            << 20.0
            << QGeoBoundingBox();

    QTest::newRow("360 width")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -180.0),
                               QGeoCoordinate(-30.0, 180.0))
            << 20.0
            << 20.0
            << QGeoBoundingBox(QGeoCoordinate(50.0, -180.0),
                               QGeoCoordinate(-10.0, 180.0));

    QTest::newRow("180 height")
            << QGeoBoundingBox(QGeoCoordinate(90.0, -30.0),
                               QGeoCoordinate(-90.0, 30.0))
            << 20.0
            << 20.0
            << QGeoBoundingBox(QGeoCoordinate(90.0, -10.0),
                               QGeoCoordinate(-90.0, 50.0));

    QTest::newRow("non wrapping -> non wrapping")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << 20.0
            << 20.0
            << QGeoBoundingBox(QGeoCoordinate(50.0, -10.0),
                               QGeoCoordinate(-10.0, 50.0));

    QTest::newRow("non wrapping -> wrapping")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 110.0),
                               QGeoCoordinate(-30.0, 170.0))
            << 20.0
            << 20.0
            << QGeoBoundingBox(QGeoCoordinate(50.0, 130.0),
                               QGeoCoordinate(-10.0, -170.0));

    QTest::newRow("non wrapping -> north clip")
            << QGeoBoundingBox(QGeoCoordinate(80.0, -30.0),
                               QGeoCoordinate(20.0, 30.0))
            << 20.0
            << 20.0
            << QGeoBoundingBox(QGeoCoordinate(90.0, -10.0),
                               QGeoCoordinate(40.0, 50.0));

    QTest::newRow("non wrapping -> south clip")
            << QGeoBoundingBox(QGeoCoordinate(-20.0, -30.0),
                               QGeoCoordinate(-80.0, 30.0))
            << -20.0
            << 20.0
            << QGeoBoundingBox(QGeoCoordinate(-40.0, -10.0),
                               QGeoCoordinate(-90.0, 50.0));

    QTest::newRow("wrapping -> non wrapping")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 130.0),
                               QGeoCoordinate(-30.0, -170.0))
            << 20.0
            << -20.0
            << QGeoBoundingBox(QGeoCoordinate(50.0, 110.0),
                               QGeoCoordinate(-10.0, 170.0));

    QTest::newRow("wrapping -> wrapping")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 130.0),
                               QGeoCoordinate(-30.0, -170.0))
            << 20.0
            << 20.0
            << QGeoBoundingBox(QGeoCoordinate(50.0, 150.0),
                               QGeoCoordinate(-10.0, -150.0));

    QTest::newRow("wrapping -> north clip")
            << QGeoBoundingBox(QGeoCoordinate(80.0, 130.0),
                               QGeoCoordinate(20.0, -170.0))
            << 20.0
            << 20.0
            << QGeoBoundingBox(QGeoCoordinate(90.0, 150.0),
                               QGeoCoordinate(40.0, -150.0));

    QTest::newRow("wrapping -> south clip")
            << QGeoBoundingBox(QGeoCoordinate(-20.0, 130.0),
                               QGeoCoordinate(-80.0, -170.0))
            << -20.0
            << 20.0
            << QGeoBoundingBox(QGeoCoordinate(-40.0, 150.0),
                               QGeoCoordinate(-90.0, -150.0));
}

void tst_QGeoBoundingBox::unite()
{
    QFETCH(QGeoBoundingBox, in1);
    QFETCH(QGeoBoundingBox, in2);
    QFETCH(QGeoBoundingBox, out);

    QCOMPARE(in1.united(in2), out);
    QCOMPARE(in2.united(in1), out);

    QCOMPARE(in1 | in2, out);
    QCOMPARE(in2 | in1, out);

    QGeoBoundingBox united1 = QGeoBoundingBox(in1);
    united1 |= in2;
    QCOMPARE(united1, out);

    QGeoBoundingBox united2 = QGeoBoundingBox(in2);
    united2 |= in1;
    QCOMPARE(united2, out);
}

void tst_QGeoBoundingBox::unite_data()
{
    QTest::addColumn<QGeoBoundingBox>("in1");
    QTest::addColumn<QGeoBoundingBox>("in2");
    QTest::addColumn<QGeoBoundingBox>("out");

    QTest::newRow("central and taller")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(50.0, -30.0),
                               QGeoCoordinate(-50.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(50.0, -30.0),
                               QGeoCoordinate(-50.0, 30.0));

    QTest::newRow("central and 180 high")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(90.0, -30.0),
                               QGeoCoordinate(-90.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(90.0, -30.0),
                               QGeoCoordinate(-90.0, 30.0));

    QTest::newRow("central and non overlapping higher")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(60.0, -30.0),
                               QGeoCoordinate(50.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(60.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0));

    QTest::newRow("central and overlapping higher")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(60.0, -30.0),
                               QGeoCoordinate(0.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(60.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0));

    QTest::newRow("central and touching higher")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(60.0, -30.0),
                               QGeoCoordinate(30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(60.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0));

    QTest::newRow("central and non overlapping lower")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(-50.0, -30.0),
                               QGeoCoordinate(-60.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-60.0, 30.0));

    QTest::newRow("central and overlapping lower")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(0.0, -30.0),
                               QGeoCoordinate(-60.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-60.0, 30.0));

    QTest::newRow("central and touching lower")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(-30.0, -30.0),
                               QGeoCoordinate(-60.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-60.0, 30.0));

    QTest::newRow("non wrapping central and wider")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -50.0),
                               QGeoCoordinate(-30.0, 50.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -50.0),
                               QGeoCoordinate(-30.0, 50.0));

    QTest::newRow("non wrapping central and 360 width")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -180.0),
                               QGeoCoordinate(-30.0, 180.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -180.0),
                               QGeoCoordinate(-30.0, 180.0));

    QTest::newRow("non wrapping central and non overlapping non wrapping left")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -110.0),
                               QGeoCoordinate(-30.0, -50.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -110.0),
                               QGeoCoordinate(-30.0, 30.0));

    QTest::newRow("non wrapping central and overlapping non wrapping left")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -80.0),
                               QGeoCoordinate(-30.0, -20.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -80.0),
                               QGeoCoordinate(-30.0, 30.0));

    QTest::newRow("non wrapping central and touching non wrapping left")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -90.0),
                               QGeoCoordinate(-30.0, -30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -90.0),
                               QGeoCoordinate(-30.0, 30.0));

    QTest::newRow("non wrapping central and non overlapping non wrapping right")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 50.0),
                               QGeoCoordinate(-30.0, 110.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 110.0));

    QTest::newRow("non wrapping central and overlapping non wrapping right")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 20.0),
                               QGeoCoordinate(-30.0, 80.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 80.0));

    QTest::newRow("non wrapping central and touching non wrapping right")
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 30.0),
                               QGeoCoordinate(-30.0, 90.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 90.0));

    QTest::newRow("wrapping and wider")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 130.0),
                               QGeoCoordinate(-30.0, -130.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 130.0),
                               QGeoCoordinate(-30.0, -130.0));

    QTest::newRow("wrapping and 360 width")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -180.0),
                               QGeoCoordinate(-30.0, 180.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -180.0),
                               QGeoCoordinate(-30.0, 180.0));

    QTest::newRow("wrapping and non overlapping right")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -130.0),
                               QGeoCoordinate(-30.0, -70.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -70.0));

    QTest::newRow("wrapping and overlapping right")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -160.0),
                               QGeoCoordinate(-30.0, -70.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -70.0));

    QTest::newRow("wrapping and touching right")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -150.0),
                               QGeoCoordinate(-30.0, -90.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -90.0));

    QTest::newRow("wrapping and non overlapping left")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 70.0),
                               QGeoCoordinate(-30.0, 130.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 70.0),
                               QGeoCoordinate(-30.0, -150.0));

    QTest::newRow("wrapping and overlapping left")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 100.0),
                               QGeoCoordinate(-30.0, 160.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 100.0),
                               QGeoCoordinate(-30.0, -150.0));

    QTest::newRow("wrapping and touching left")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 90.0),
                               QGeoCoordinate(-30.0, 150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, 90.0),
                               QGeoCoordinate(-30.0, -150.0));

    QTest::newRow("wrapping and non overlapping center")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -30.0),
                               QGeoCoordinate(-30.0, 30.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -180.0),
                               QGeoCoordinate(-30.0, 180.0));

    QTest::newRow("wrapping and overlapping center")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -160.0),
                               QGeoCoordinate(-30.0, 160.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -180.0),
                               QGeoCoordinate(-30.0, 180.0));

    QTest::newRow("wrapping and touching center")
            << QGeoBoundingBox(QGeoCoordinate(30.0, 150.0),
                               QGeoCoordinate(-30.0, -150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -150.0),
                               QGeoCoordinate(-30.0, 150.0))
            << QGeoBoundingBox(QGeoCoordinate(30.0, -180.0),
                               QGeoCoordinate(-30.0, 180.0));

    QTest::newRow("small gap over zero line")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -20.0),
                                QGeoCoordinate(-30.0, -10.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 10.0),
                                QGeoCoordinate(-30.0, 20.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -20.0),
                                QGeoCoordinate(-30.0, 20.0));

    QTest::newRow("small gap before zero line")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -40.0),
                                QGeoCoordinate(-30.0, -30.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -20.0),
                                QGeoCoordinate(-30.0, -10.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -40.0),
                                QGeoCoordinate(-30.0, -10.0));

    QTest::newRow("small gap after zero line")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 10.0),
                                QGeoCoordinate(-30.0, 20.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 30.0),
                                QGeoCoordinate(-30.0, 40.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 10.0),
                                QGeoCoordinate(-30.0, 40.0));

    QTest::newRow("small gap over dateline")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 160.0),
                                QGeoCoordinate(-30.0, 170.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -170.0),
                                QGeoCoordinate(-30.0, -160.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 160.0),
                                QGeoCoordinate(-30.0, -160.0));

    QTest::newRow("small gap before dateline")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 140.0),
                                QGeoCoordinate(-30.0, 150.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 160.0),
                                QGeoCoordinate(-30.0, 170.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 140.0),
                                QGeoCoordinate(-30.0, 170.0));

    QTest::newRow("small gap after dateline")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -170.0),
                                QGeoCoordinate(-30.0, -160.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -150.0),
                                QGeoCoordinate(-30.0, -140.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -170.0),
                                QGeoCoordinate(-30.0, -140.0));

    QTest::newRow("90-degree inner gap over zero line")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -55.0),
                                QGeoCoordinate(-30.0, -45.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 45.0),
                                QGeoCoordinate(-30.0, 55.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -55.0),
                                QGeoCoordinate(-30.0, 55.0));

    QTest::newRow("90-degree inner gap before zero line")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -20.0),
                                QGeoCoordinate(-30.0, -10.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -65.0),
                                QGeoCoordinate(-30.0, -55.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -65.0),
                                QGeoCoordinate(-30.0, -10.0));

    QTest::newRow("90-degree inner gap after zero line")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 65.0),
                                QGeoCoordinate(-30.0, 75.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 10.0),
                                QGeoCoordinate(-30.0, 20.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 10.0),
                                QGeoCoordinate(-30.0, 75.0));

    QTest::newRow("90-degree inner gap over dateline")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 125.0),
                                QGeoCoordinate(-30.0, 135.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -135.0),
                                QGeoCoordinate(-30.0, -125.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 125.0),
                                QGeoCoordinate(-30.0, -125.0));

    QTest::newRow("90-degree inner gap before dateline")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 160.0),
                                QGeoCoordinate(-30.0, 170.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 50.0),
                                QGeoCoordinate(-30.0, 60.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 50.0),
                                QGeoCoordinate(-30.0, 170.0));

    QTest::newRow("90-degree inner gap after dateline")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -170.0),
                                QGeoCoordinate(-30.0, -160.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -60.0),
                                QGeoCoordinate(-30.0, -50.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -170.0),
                                QGeoCoordinate(-30.0, -50.0));

    QTest::newRow("180-degree inner gap centered on zero line")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -100.0),
                                QGeoCoordinate(-30.0, -90.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 90.0),
                                QGeoCoordinate(-30.0, 100.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 90.0),
                                QGeoCoordinate(-30.0, -90.0));

    QTest::newRow("180-degree outer gap cenetered on zero line")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -90.0),
                                QGeoCoordinate(-30.0, -80.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 80.0),
                                QGeoCoordinate(-30.0, 90.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -90.0),
                                QGeoCoordinate(-30.0, 90.0));

    QTest::newRow("180-degree shift centered on zero line")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -100.0),
                                QGeoCoordinate(-30.0, -80.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 80.0),
                                QGeoCoordinate(-30.0, 100.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -180.0),
                                QGeoCoordinate(-30.0, 180.0));

    QTest::newRow("180-degree inner gap centered on dateline")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 80.0),
                                QGeoCoordinate(-30.0, 90.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -90.0),
                                QGeoCoordinate(-30.0, -80.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0,  -90.0),
                                QGeoCoordinate(-30.0, 90.0));

    QTest::newRow("180-degree outer gap centered on dateline")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 90.0),
                                QGeoCoordinate(-30.0, 100.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -100.0),
                                QGeoCoordinate(-30.0, -90.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 90.0),
                                QGeoCoordinate(-30.0, -90.0));

    QTest::newRow("180-degree shift centered on dateline")
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, 80.0),
                                QGeoCoordinate(-30.0, 100.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0, -100.0),
                                QGeoCoordinate(-30.0, -80.0))
            <<  QGeoBoundingBox(QGeoCoordinate(30.0,  -180.0),
                                QGeoCoordinate(-30.0, 180.0));
}

void tst_QGeoBoundingBox::clone()
{
    //check that the clone copies the same data as the original
    QGeoBoundingBox originalBox;
    originalBox.setTopLeft(QGeoCoordinate(20,20));
    originalBox.setBottomRight(QGeoCoordinate(10,30));

    QGeoBoundingArea *areaPtr = originalBox.clone();
    QVERIFY(areaPtr->type() == QGeoBoundingArea::BoxType);
    QGeoBoundingBox *clonedBoxPtr;
    clonedBoxPtr = static_cast<QGeoBoundingBox*>(areaPtr);
    QVERIFY2(clonedBoxPtr->topLeft() == QGeoCoordinate(20,20),
             "Clone's top left coord does not match original");
    QVERIFY2(clonedBoxPtr->bottomRight() == QGeoCoordinate(10,30),
             "Clone's bottom right coord does not match original");

    //check that when the original is altered, the clone remains unaltered.
    originalBox.setTopLeft(QGeoCoordinate(80,30));
    originalBox.setBottomRight(QGeoCoordinate(10,40));

    QVERIFY2(originalBox.topLeft() == QGeoCoordinate(80,30),
             "Original's top left coord has not changed");
    QVERIFY2(originalBox.bottomRight() == QGeoCoordinate(10,40),
             "Original's bottom right coord has not changed");

    QVERIFY2(clonedBoxPtr->topLeft() == QGeoCoordinate(20,20),
             "Clone's top left coord references the original's");
    QVERIFY2(clonedBoxPtr->bottomRight() == QGeoCoordinate(10,30),
             "Clone's bottom right coord references the original's");
}

void tst_QGeoBoundingBox::areaComparison()
{
    QGeoBoundingBox b1(QGeoCoordinate(20,20), QGeoCoordinate(10,30));
    QGeoBoundingBox b2(QGeoCoordinate(20,20), QGeoCoordinate(10,30));
    QGeoBoundingBox b3(QGeoCoordinate(40,40), QGeoCoordinate(10,30));

    QVERIFY(b1 == b2);
    QVERIFY(!(b1 != b2));

    QVERIFY(!(b1 == b3));
    QVERIFY(b1 != b3);

    QGeoBoundingCircle c1(QGeoCoordinate(20,20), 5000);
    QVERIFY(!(b1 == c1));
    QVERIFY(b1 != c1);

    QGeoBoundingArea *b2Ptr = &b2;
    QVERIFY(b1 == *b2Ptr);
    QVERIFY(!(b1 != *b2Ptr));

    QGeoBoundingArea *b3Ptr = &b3;
    QVERIFY(!(b1 == *b3Ptr));
    QVERIFY(b1 != *b3Ptr);
}

QTEST_MAIN(tst_QGeoBoundingBox)
#include "tst_qgeoboundingbox.moc"

