/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QObject>
#include <QtTest/QtTest>

#include <mapitem.h>
#include "mapitemtree_p.h"

#include <QSet>
#include <QHash>

#include <QMetaType>

#include <cmath>

QT_USE_NAMESPACE

uint qHash(const QRect &rect)
{
    uint result = qHash(rect.x());
    result = 17 * result + qHash(rect.y());
    result = 17 * result + qHash(rect.width());
    result = 17 * result + qHash(rect.height());
    return result;
}

class tst_MapItemTree : public QObject
{
    Q_OBJECT

public:
    tst_MapItemTree() {

    }

private slots:
    void insertRemove();
    void itemsAt_allQuads();
    void itemsAt_singleQuad();
    void itemsWithin_allQuads();
    void itemsWithin_singleQuad();
    void makeVisible();

private:
    MapItemTree* setupTree();
    MapItem* dummyItemFromRect(const QRect &rect);
};

MapItem* tst_MapItemTree::dummyItemFromRect(const QRect &rect)
{
    MapItem *item = new MapItem();
    item->setBounds(rect);
    return item;
}

void tst_MapItemTree::insertRemove()
{
//    MapItemTree *root = new MapItemTree(QRect(0, 0, 512, 512));
    MapItemTree *root = new MapItemTree();

    MapItem* item1 = dummyItemFromRect(QRect(16, 16, 16, 16));
    MapItem* item2 = dummyItemFromRect(QRect(272, 16, 16, 16));
    MapItem* item3 = dummyItemFromRect(QRect(16, 272, 16, 16));
    MapItem* item4 = dummyItemFromRect(QRect(272, 272, 16, 16));
    MapItem* item5 = dummyItemFromRect(QRect(248, 248, 16, 16));

    MapItem* badItem1 = dummyItemFromRect(QRect(17, 17, 16, 16));
    MapItem* badItem2 = dummyItemFromRect(QRect(273, 17, 16, 16));
    MapItem* badItem3 = dummyItemFromRect(QRect(17, 273, 16, 16));
    MapItem* badItem4 = dummyItemFromRect(QRect(273, 273, 16, 16));
    MapItem* badItem5 = dummyItemFromRect(QRect(249, 249, 16, 16));

    QCOMPARE(root->isEmpty(), true);
    QCOMPARE(root->size(), 0);
    QCOMPARE(root->items().size(), 0);

    root->insert(item1);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 1);
    QCOMPARE(root->items().size(), 1);

    root->insert(item2);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 2);
    QCOMPARE(root->items().size(), 2);

    root->insert(item3);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 3);
    QCOMPARE(root->items().size(), 3);

    root->insert(item4);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 4);
    QCOMPARE(root->items().size(), 4);

    root->insert(item5);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 5);
    QCOMPARE(root->items().size(), 5);

    root->remove(badItem1);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 5);
    QCOMPARE(root->items().size(), 5);

    root->remove(badItem2);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 5);
    QCOMPARE(root->items().size(), 5);

    root->remove(badItem3);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 5);
    QCOMPARE(root->items().size(), 5);

    root->remove(badItem4);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 5);
    QCOMPARE(root->items().size(), 5);

    root->remove(badItem5);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 5);
    QCOMPARE(root->items().size(), 5);

    root->remove(item1);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 4);
    QCOMPARE(root->items().size(), 4);

    root->remove(item2);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 3);
    QCOMPARE(root->items().size(), 3);

    root->remove(item3);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 2);
    QCOMPARE(root->items().size(), 2);

    root->remove(item4);

    QCOMPARE(root->isEmpty(), false);
    QCOMPARE(root->size(), 1);
    QCOMPARE(root->items().size(), 1);

    root->remove(item5);

    QCOMPARE(root->isEmpty(), true);
    QCOMPARE(root->size(), 0);
    QCOMPARE(root->items().size(), 0);

    root->remove(badItem1);

    QCOMPARE(root->isEmpty(), true);
    QCOMPARE(root->size(), 0);
    QCOMPARE(root->items().size(), 0);

    root->remove(badItem2);

    QCOMPARE(root->isEmpty(), true);
    QCOMPARE(root->size(), 0);
    QCOMPARE(root->items().size(), 0);

    root->remove(badItem3);

    QCOMPARE(root->isEmpty(), true);
    QCOMPARE(root->size(), 0);
    QCOMPARE(root->items().size(), 0);

    root->remove(badItem4);

    QCOMPARE(root->isEmpty(), true);
    QCOMPARE(root->size(), 0);
    QCOMPARE(root->items().size(), 0);

    root->remove(badItem5);

    QCOMPARE(root->isEmpty(), true);
    QCOMPARE(root->size(), 0);
    QCOMPARE(root->items().size(), 0);

    delete root;

    delete item1;
    delete item2;
    delete item3;
    delete item4;
    delete item5;

    delete badItem1;
    delete badItem2;
    delete badItem3;
    delete badItem4;
    delete badItem5;
}

void tst_MapItemTree::itemsAt_allQuads()
{
    MapItemTree *tree = setupTree();

    QList<int> empty;
    empty.append(0);
    empty.append(2);
    empty.append(3);
    empty.append(5);
    empty.append(6);
    empty.append(9);
    empty.append(10);
    empty.append(12);
    empty.append(13);
    empty.append(15);

    QList<int> full;
    full.append(1);
    full.append(4);
    full.append(7);
    full.append(8);
    full.append(11);
    full.append(14);

    // Test mid points of the "cells"

    for (int x = 0; x < 16; ++x) {
        for (int i = 0; i < empty.size(); ++i) {
            int y = empty.at(i);
            QCOMPARE(tree->itemsAt(QPoint(x * 16 + 8, y * 16 + 8)).size(), 0);
        }
    }

    for (int i = 0; i < empty.size(); ++i) {
        int x = empty.at(i);
        for (int y = 0; y < 16; ++y) {
            QCOMPARE(tree->itemsAt(QPoint(x * 16 + 8, y * 16 + 8)).size(), 0);
        }
    }

    for (int i = 0; i < full.size(); ++i) {
        int x = full.at(i);
        for (int j = 0; j < full.size(); ++j) {
            int y = full.at(j);
            QList<MapItem*> items = tree->itemsAt(QPoint(x * 16 + 8, y * 16 + 8));

            QCOMPARE(items.size(), 1);

            QRect r = items.at(0)->bounds();

            if (x != 8) {
                QCOMPARE(r.x(), x * 16);
                if (x != 7) {
                    QCOMPARE(r.width(), 15);
                } else {
                    QCOMPARE(r.width(), 31);
                }
            } else {
                QCOMPARE(r.x(), (x - 1) * 16);
                QCOMPARE(r.width(), 31);
            }

            if (y != 8) {
                QCOMPARE(r.y(), y * 16);
                if (y != 7) {
                    QCOMPARE(r.height(), 15);
                } else {
                    QCOMPARE(r.height(), 31);
                }
            } else {
                QCOMPARE(r.y(), (y - 1) * 16);
                QCOMPARE(r.height(), 31);
            }
        }
    }

    QList<MapItem*> items = tree->items();
    delete tree;
    qDeleteAll(items);
}

void tst_MapItemTree::itemsAt_singleQuad()
{
    MapItemTree *tree = setupTree();

    // Test that we include x and x+width but don't go
    // beyond that range on either side

    QList<QPair<int, bool> > vals;
    vals.append(QPair<int, bool>(15, false));
    vals.append(QPair<int, bool>(16, true));
    vals.append(QPair<int, bool>(24, true));
    vals.append(QPair<int, bool>(31, true));
    vals.append(QPair<int, bool>(32, false));

    for (int i = 0; i < vals.size(); ++i) {

        QPair<int, bool> px = vals.at(i);

        for (int j = 0; j < vals.size(); ++j) {

            QPair<int, bool> py = vals.at(j);

            QList<MapItem*> items = tree->itemsAt(QPoint(px.first, py.first));

            if (px.second && py.second) {
                QCOMPARE(items.size(), 1);
                QCOMPARE(items.at(0)->bounds(), QRect(16, 16, 15, 15));
            } else {
                QCOMPARE(items.size(), 0);
            }
        }
    }

    QList<MapItem*> items = tree->items();
    delete tree;
    qDeleteAll(items);
}

struct intervalAll {
    intervalAll(int start, int end, int count)
            : start(start),
            end(end),
            count(count) {}

    int start;
    int end;
    int count;
};

void tst_MapItemTree::itemsWithin_allQuads()
{
    MapItemTree *tree = setupTree();

    QList<int> start;
    start.append(16);
    start.append(64);
    start.append(112);
    start.append(176);
    start.append(224);

    QList<int> end;
    end.append(31);
    end.append(79);
    end.append(143);
    end.append(191);
    end.append(239);

    QList<intervalAll> vals;

    for (int i = 0; i < start.size(); ++i) {

        int s = start.at(i);

        vals.append(intervalAll(s - 1, s, 1));
        vals.append(intervalAll(s, s + 1, 1));
        vals.append(intervalAll(s - 1, s + 1, 1));

        for (int j = i; j < end.size(); ++j) {

            int e = end.at(j);
            int c = j - i + 1;

            vals.append(intervalAll(s - 1, e + 1, c));
            vals.append(intervalAll(s, e, c));
            vals.append(intervalAll(s + 1, e - 1, c));
        }
    }

    for (int i = 0; i < end.size() - 1; ++i) {
        int e = end.at(i);
        for (int j = i + 1; j < start.size(); ++j) {
            int s = start.at(j);
            vals.append(intervalAll(e + 1, s - 1, j - i - 1));
        }
    }

    for (int i = 0; i < vals.size(); ++i) {

        intervalAll ix = vals.at(i);

        for (int j = 0; j < vals.size(); ++j) {

            intervalAll iy = vals.at(j);

            QRect r(ix.start, iy.start, ix.end - ix.start, iy.end - iy.start);

            QList<MapItem*> items = tree->itemsWithin(r);

            QCOMPARE(items.size(), ix.count * iy.count);
        }
    }

    QList<MapItem*> items = tree->items();
    delete tree;
    qDeleteAll(items);
}

struct intervalSingle {
    intervalSingle(int start, int end, bool contains)
            : start(start),
            end(end),
            contains(contains) {}

    int start;
    int end;
    bool contains;
};

void tst_MapItemTree::itemsWithin_singleQuad()
{
    MapItemTree *tree = setupTree();

    QList<intervalSingle> vals;
    vals.append(intervalSingle(14, 15, false));
    vals.append(intervalSingle(15, 16, true));
    vals.append(intervalSingle(15, 17, true));
    vals.append(intervalSingle(16, 17, true));

    vals.append(intervalSingle(15, 32, true));
    vals.append(intervalSingle(16, 31, true));
    vals.append(intervalSingle(17, 30, true));

    vals.append(intervalSingle(30, 31, true));
    vals.append(intervalSingle(30, 32, true));
    vals.append(intervalSingle(31, 32, true));
    vals.append(intervalSingle(32, 33, false));

    for (int i = 0; i < vals.size(); ++i) {

        intervalSingle ix = vals.at(i);

        for (int j = 0; j < vals.size(); ++j) {

            intervalSingle iy = vals.at(j);

            QRect r(ix.start, iy.start, ix.end - ix.start, iy.end - iy.start);

            QList<MapItem*> items = tree->itemsWithin(r);

            if (ix.contains && iy.contains) {
                QCOMPARE(items.size(), 1);
                QCOMPARE(items.at(0)->bounds(), QRect(16, 16, 15, 15));
            } else {
                QCOMPARE(items.size(), 0);
            }
        }
    }

    QList<MapItem*> items = tree->items();
    delete tree;
    qDeleteAll(items);
}

void tst_MapItemTree::makeVisible()
{
    MapItemTree *tree = setupTree();

    QList<MapItem*> added;
    QList<MapItem*> removed;

    tree->makeVisible(QRect(0, 0, 1, 1), added, removed);
    QCOMPARE(added.size(), 0);
    QCOMPARE(removed.size(), 25);

    QList<int> start;
    start.append(16);
    start.append(64);
    start.append(112);
    start.append(176);
    start.append(224);

    QList<int> end;
    end.append(31);
    end.append(79);
    end.append(143);
    end.append(191);
    end.append(239);

    QList<intervalAll> vals;

    for (int i = 0; i < start.size(); ++i) {

        int s = start.at(i);

        vals.append(intervalAll(s - 1, s, 1));
        vals.append(intervalAll(s, s + 1, 1));
        vals.append(intervalAll(s - 1, s + 1, 1));

        for (int j = i; j < end.size(); ++j) {

            int e = end.at(j);
            int c = j - i + 1;

            vals.append(intervalAll(s - 1, e + 1, c));
            vals.append(intervalAll(s, e, c));
            vals.append(intervalAll(s + 1, e - 1, c));
        }
    }

    for (int i = 0; i < end.size() - 1; ++i) {
        int e = end.at(i);
        for (int j = i + 1; j < start.size(); ++j) {
            int s = start.at(j);
            vals.append(intervalAll(e + 1, s - 1, j - i - 1));
        }
    }

    for (int i = 0; i < vals.size(); ++i) {

        intervalAll ix = vals.at(i);

        for (int j = 0; j < vals.size(); ++j) {

            intervalAll iy = vals.at(j);

            QRect r(ix.start, iy.start, ix.end - ix.start, iy.end - iy.start);

            tree->makeVisible(r, added, removed);

            QCOMPARE(added.size(), ix.count * iy.count);
            QCOMPARE(removed.size(), 0);

            tree->makeVisible(QRect(0, 0, 1, 1), added, removed);

            QCOMPARE(added.size(), 0);
            QCOMPARE(removed.size(), ix.count * iy.count);
        }
    }

    QList<MapItem*> items = tree->items();
    delete tree;
    qDeleteAll(items);
}

MapItemTree* tst_MapItemTree::setupTree()
{
    MapItemTree *root = new MapItemTree();

    root->insert(dummyItemFromRect(QRect(16, 16, 15, 15)));
    root->insert(dummyItemFromRect(QRect(64, 16, 15, 15)));
    root->insert(dummyItemFromRect(QRect(112, 16, 31, 15)));
    root->insert(dummyItemFromRect(QRect(176, 16, 15, 15)));
    root->insert(dummyItemFromRect(QRect(224, 16, 15, 15)));

    root->insert(dummyItemFromRect(QRect(16, 64, 15, 15)));
    root->insert(dummyItemFromRect(QRect(64, 64, 15, 15)));
    root->insert(dummyItemFromRect(QRect(112, 64, 31, 15)));
    root->insert(dummyItemFromRect(QRect(176, 64, 15, 15)));
    root->insert(dummyItemFromRect(QRect(224, 64, 15, 15)));

    root->insert(dummyItemFromRect(QRect(16, 112, 15, 31)));
    root->insert(dummyItemFromRect(QRect(64, 112, 15, 31)));
    root->insert(dummyItemFromRect(QRect(112, 112, 31, 31)));
    root->insert(dummyItemFromRect(QRect(176, 112, 15, 31)));
    root->insert(dummyItemFromRect(QRect(224, 112, 15, 31)));

    root->insert(dummyItemFromRect(QRect(16, 176, 15, 15)));
    root->insert(dummyItemFromRect(QRect(64, 176, 15, 15)));
    root->insert(dummyItemFromRect(QRect(112, 176, 31, 15)));
    root->insert(dummyItemFromRect(QRect(176, 176, 15, 15)));
    root->insert(dummyItemFromRect(QRect(224, 176, 15, 15)));

    root->insert(dummyItemFromRect(QRect(16, 224, 15, 15)));
    root->insert(dummyItemFromRect(QRect(64, 224, 15, 15)));
    root->insert(dummyItemFromRect(QRect(112, 224, 31, 15)));
    root->insert(dummyItemFromRect(QRect(176, 224, 15, 15)));
    root->insert(dummyItemFromRect(QRect(224, 224, 15, 15)));

    QList<MapItem*> added;
    QList<MapItem*> removed;
    root->makeVisible(QRect(0, 0, 256, 256), added, removed);

    return root;
}

QTEST_APPLESS_MAIN(tst_MapItemTree)

#include "tst_mapitemtree.moc"

