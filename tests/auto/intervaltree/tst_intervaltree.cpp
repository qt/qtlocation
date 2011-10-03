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

#include "intervaltree_p.h"

#include <QHash>

QT_USE_NAMESPACE

uint qHash(const QRect &rect)
{
    uint result = qHash(rect.x());
    result = 17 * result + qHash(rect.y());
    result = 17 * result + qHash(rect.width());
    result = 17 * result + qHash(rect.height());
    return result;
}

class tst_IntervalTree : public QObject
{
    Q_OBJECT

public:
    tst_IntervalTree() {

    }

private slots:
    void insert_min();
    void remove_min();

    void insert_max();
    void remove_max();

    void insert_minmax();
    void remove_minmax();

    void itemsAt();
    void itemsWithin();

    void insert2D();
    void insert2D_sameRect_sameData();
    void insert2D_sameRect_uniqueData();

    void remove2D();
    void remove2D_duplicates();

    void itemsAt2D();
    void itemsWithin2D();
};

void tst_IntervalTree::insert_min()
{
    // effectively tests all the sizes lower than this as it goes
    int size = 8;

    // tree contains [1..size], inserted in order
    AAIntervalTree<int> tree;
    QList<int> data;
    int limit = 1;

    for (int j = 1; j <= size; ++j) {
        tree.insert(j, 0, j);
        data.append(j);
        limit *= j;
    }

    QCOMPARE(tree.items(), data);

    // we insert [1..size] into testTree in every possible order
    // and compare the inorder traversal
    for (int i = 0; i < limit; ++i) {
        QList<int> tmp = data;
        QList<int> input;
        int div = 1;
        for (int j = size; j > 0; --j) {
            input.append(tmp.takeAt((i / div) % j));
            div *= j;
        }

        AAIntervalTree<int> testTree;
        for (int j = 0; j < input.size(); ++j) {
            int k = input.at(j);
            testTree.insert(k, 0, k);
        }

        QCOMPARE(testTree.items(), data);
    }
}

void tst_IntervalTree::remove_min()
{
    // effectively tests all the sizes lower than this as it goes
    int size = 8;

    QList<int> data;
    int limit = 1;

    for (int j = 1; j <= size; ++j) {
        data.append(j);
        limit *= j;
    }

    for (int i = 0; i < limit; ++i) {
        QList<int> tmp = data;
        QList<int> input;
        int div = 1;
        for (int j = size; j > 0; --j) {
            input.append(tmp.takeAt((i / div) % j));
            div *= j;
        }

        // tree contains [1..size], inserted in order
        AAIntervalTree<int> tree;
        for (int j = 1; j <= size; ++j) {
            tree.insert(j, 0, j);
        }

        // we remove [1..size] into tree in every possible order
        // and compare the inorder traversal to a sorted list from
        // which we remove the same values
        QList<int> tmp2 = data;
        for (int j = 0; j < input.size(); ++j) {
            int k = input.at(j);
            tmp2.removeAll(k);
            tree.remove(k, 0, k);

            QCOMPARE(tree.items(), tmp2);
        }
    }
}

void tst_IntervalTree::insert_max()
{
    // effectively tests all the sizes lower than this as it goes
    int size = 8;

    // tree contains [1..size], inserted in order
    AAIntervalTree<int> tree;
    QList<int> data;
    int limit = 1;

    for (int j = 1; j <= size; ++j) {
        tree.insert(0, j, j);
        data.append(j);
        limit *= j;
    }

    QCOMPARE(tree.items(), data);

    // we insert [1..size] into testTree in every possible order
    // and compare the inorder traversal
    for (int i = 0; i < limit; ++i) {
        QList<int> tmp = data;
        QList<int> input;
        int div = 1;
        for (int j = size; j > 0; --j) {
            input.append(tmp.takeAt((i / div) % j));
            div *= j;
        }

        AAIntervalTree<int> testTree;
        for (int j = 0; j < input.size(); ++j) {
            int k = input.at(j);
            testTree.insert(0, k, k);
        }

        QCOMPARE(testTree.items(), data);
    }
}

void tst_IntervalTree::remove_max()
{
    // effectively tests all the sizes lower than this as it goes
    int size = 8;

    QList<int> data;
    int limit = 1;

    for (int j = 1; j <= size; ++j) {
        data.append(j);
        limit *= j;
    }

    for (int i = 0; i < limit; ++i) {
        QList<int> tmp = data;
        QList<int> input;
        int div = 1;
        for (int j = size; j > 0; --j) {
            input.append(tmp.takeAt((i / div) % j));
            div *= j;
        }

        // tree contains [1..size], inserted in order
        AAIntervalTree<int> tree;
        for (int j = 1; j <= size; ++j) {
            tree.insert(0, j, j);
        }

        // we remove [1..size] into tree in every possible order
        // and compare the inorder traversal to a sorted list from
        // which we remove the same values
        QList<int> tmp2 = data;
        for (int j = 0; j < input.size(); ++j) {
            int k = input.at(j);
            tmp2.removeAll(k);
            tree.remove(0, k, k);

            QCOMPARE(tree.items(), tmp2);
        }
    }
}


void tst_IntervalTree::insert_minmax()
{
    // effectively tests all the sizes lower than this as it goes
    int size = 6;

    // tree contains [(1,1),(1,2)..(size, size+1),(size,size+2)], inserted in order
    AAIntervalTree<int> tree;
    QList<int> data;
    int limit = 1;

    for (int j = 1; j <= size; ++j) {
        tree.insert(j, j + 1, j * 2);
        data.append(j * 2);
        tree.insert(j, j + 2, j * 2 + 1);
        data.append(j * 2 + 1);
        limit *= 2 * j;
    }

    QCOMPARE(tree.items(), data);

    // we insert the same data into testTree in every possible order
    // and compare the inorder traversal
    for (int i = 0; i < limit; ++i) {
        QList<int> tmp = data;
        QList<int> input;
        int div = 1;
        for (int j = size * 2; j > 0; --j) {
            input.append(tmp.takeAt((i / div) % j));
            div *= j;
        }

        AAIntervalTree<int> testTree;
        for (int j = 0; j < input.size(); ++j) {
            int k = input.at(j);
            testTree.insert(k / 2, k / 2 + 1 + k % 2, k);
        }

        QCOMPARE(testTree.items(), data);
    }
}

void tst_IntervalTree::remove_minmax()
{
    // effectively tests all the sizes lower than this as it goes
    int size = 6;

    QList<int> data;
    int limit = 1;

    for (int j = 1; j <= size; ++j) {
        data.append(j * 2);
        data.append(j * 2 + 1);
        limit *= 2 * j;
    }

    for (int i = 0; i < limit; ++i) {
        QList<int> tmp = data;
        QList<int> input;
        int div = 1;
        for (int j = 2 * size; j > 0; --j) {
            input.append(tmp.takeAt((i / div) % j));
            div *= j;
        }

        // tree contains [1..size], inserted in order
        AAIntervalTree<int> tree;
        for (int j = 1; j <= size; ++j) {
            tree.insert(j, j + 1, 2 * j);
            tree.insert(j, j + 2, 2 * j + 1);
        }

        // we remove [1..size] into tree in every possible order
        // and compare the inorder traversal to a sorted list from
        // which we remove the same values
        QList<int> tmp2 = data;
        for (int j = 0; j < input.size(); ++j) {
            int k = input.at(j);
            tmp2.removeAll(k);
            tree.remove(k / 2, k / 2 + 1 + k % 2, k);

            QCOMPARE(tree.items(), tmp2);
        }
    }
}

struct ItemTriple {
    ItemTriple(int min, int max, int value)
            : min(min),
            max(max),
            value(value) {}

    int min;
    int max;
    int value;
};

void tst_IntervalTree::itemsAt()
{
    QList<ItemTriple> triples;

    triples << ItemTriple(1, 4, 1);
    triples << ItemTriple(6, 9, 2);
    triples << ItemTriple(7, 11, 3);
    triples << ItemTriple(9, 10, 4);
    triples << ItemTriple(16, 24, 5);
    triples << ItemTriple(17, 22, 6);
    triples << ItemTriple(18, 20, 7);
    triples << ItemTriple(20, 21, 8);
    triples << ItemTriple(26, 31, 9);
    triples << ItemTriple(27, 27, 10);

    QHash<int, QSet<int> > data;

    AAIntervalTree<int> tree;

    for (int i = 0; i < triples.size(); ++i) {
        ItemTriple t1 = triples.at(i);
        tree.insert(t1.min, t1.max, t1.value);
        for (int j = t1.min; j <= t1.max; ++j) {
            data[j].insert(t1.value);
        }
    }

    for (int i = 1; i <= 31; ++i) {
        QCOMPARE(tree.itemsAt(i).toSet(), data.value(i));
    }
}

void tst_IntervalTree::itemsWithin()
{
    QList<ItemTriple> triples;

    triples << ItemTriple(1, 4, 1);
    triples << ItemTriple(6, 9, 2);
    triples << ItemTriple(7, 11, 3);
    triples << ItemTriple(9, 10, 4);
    triples << ItemTriple(16, 24, 5);
    triples << ItemTriple(17, 22, 6);
    triples << ItemTriple(18, 20, 7);
    triples << ItemTriple(20, 21, 8);
    triples << ItemTriple(26, 31, 9);
    triples << ItemTriple(27, 27, 10);

    QHash<int, QSet<int> > data;

    AAIntervalTree<int> tree;

    for (int i = 0; i < triples.size(); ++i) {
        ItemTriple t1 = triples.at(i);
        tree.insert(t1.min, t1.max, t1.value);
        for (int j = t1.min; j <= t1.max; ++j) {
            data[j].insert(t1.value);
        }
    }

    for (int i = 1; i <= 31; ++i) {
        for (int j = i; j <= 31; ++j) {
            QSet<int> d;
            for (int k = i; k <= j; ++k)
                d += data.value(k);
            QCOMPARE(tree.itemsWithin(i, j).toSet(), d);
        }
    }
}

void tst_IntervalTree::insert2D()
{
    AAInterval2DTree<int> *tree = new AAInterval2DTree<int>();

    int size = 6;
    int index = 0;

    QList<int> data;

    for (int x1 = 0; x1 < size; ++x1) {
        for (int x2 = x1; x2 < size; ++x2) {
            for (int y1 = 0; y1 < size; ++y1) {
                for (int y2 = y1; y2 < size; ++y2) {
                    tree->insert(QRect(x1, y1, x2 - x1, y2 - y1), index);
                    data.append(index);
                    ++index;
                }
            }
        }
    }

    QCOMPARE(tree->items(), data);

    delete tree;
}

void tst_IntervalTree::insert2D_sameRect_sameData()
{
    AAInterval2DTree<int> *tree = new AAInterval2DTree<int>();

    int size = 6;
    int index = 0;

    QList<int> data;

    for (int x1 = 0; x1 < size; ++x1) {
        for (int x2 = x1; x2 < size; ++x2) {
            for (int y1 = 0; y1 < size; ++y1) {
                for (int y2 = y1; y2 < size; ++y2) {
                    tree->insert(QRect(x1, y1, x2 - x1, y2 - y1), index);
                    tree->insert(QRect(x1, y1, x2 - x1, y2 - y1), index);
                    data.append(index);
                    ++index;
                }
            }
        }
    }

    QCOMPARE(tree->items(), data);

    delete tree;
}


void tst_IntervalTree::insert2D_sameRect_uniqueData()
{
    AAInterval2DTree<int> *tree = new AAInterval2DTree<int>();

    int size = 6;
    int index = 0;

    QList<int> data;

    for (int x1 = 0; x1 < size; ++x1) {
        for (int x2 = x1; x2 < size; ++x2) {
            for (int y1 = 0; y1 < size; ++y1) {
                for (int y2 = y1; y2 < size; ++y2) {
                    tree->insert(QRect(x1, y1, x2 - x1, y2 - y1), index);
                    data.append(index);
                    ++index;
                    tree->insert(QRect(x1, y1, x2 - x1, y2 - y1), index);
                    data.append(index);
                    ++index;
                }
            }
        }
    }

    QCOMPARE(tree->items(), data);

    delete tree;
}

void tst_IntervalTree::remove2D()
{
    int size = 6;
    int index = 0;

    QHash<QRect, int> indexHash;

    QList<QList<QPair<QRect, int> > > removals;

    QList<QPair<QRect, int> > removal1;

    for (int x1 = 0; x1 < size; ++x1) {
        for (int x2 = x1; x2 < size; ++x2) {
            for (int y1 = 0; y1 < size; ++y1) {
                for (int y2 = y1; y2 < size; ++y2) {
                    QRect r = QRect(x1, y1, x2 - x1, y2 - y1);
                    indexHash[r] = index;
                    removal1 << QPair<QRect, int>(QRect(x1, y1, x2 - x1, y2 - y1), index);
                    ++index;
                }
            }
        }
    }

    removals << removal1;

    QList<QPair<QRect, int> > removal2;

    for (int x1 = 0; x1 < size; ++x1) {
        for (int y1 = 0; y1 < size; ++y1) {
            for (int x2 = x1; x2 < size; ++x2) {
                for (int y2 = y1; y2 < size; ++y2) {
                    QRect r = QRect(x1, y1, x2 - x1, y2 - y1);
                    removal2 << QPair<QRect, int>(r, indexHash.value(r, -1));
                }
            }
        }
    }

    removals << removal2;

    QList<QPair<QRect, int> > removal3;

    for (int x1 = 0; x1 < size; ++x1) {
        for (int y1 = 0; y1 < size; ++y1) {
            for (int y2 = y1; y2 < size; ++y2) {
                for (int x2 = x1; x2 < size; ++x2) {
                    QRect r = QRect(x1, y1, x2 - x1, y2 - y1);
                    removal3 << QPair<QRect, int>(r, indexHash.value(r, -1));
                }
            }
        }
    }

    removals << removal3;

    QList<QPair<QRect, int> > removal4;

    for (int y1 = 0; y1 < size; ++y1) {
        for (int x1 = 0; x1 < size; ++x1) {
            for (int x2 = x1; x2 < size; ++x2) {
                for (int y2 = y1; y2 < size; ++y2) {
                    QRect r = QRect(x1, y1, x2 - x1, y2 - y1);
                    removal4 << QPair<QRect, int>(r, indexHash.value(r, -1));
                }
            }
        }
    }

    removals << removal4;

    QList<QPair<QRect, int> > removal5;

    for (int y1 = 0; y1 < size; ++y1) {
        for (int x1 = 0; x1 < size; ++x1) {
            for (int y2 = y1; y2 < size; ++y2) {
                for (int x2 = x1; x2 < size; ++x2) {
                    QRect r = QRect(x1, y1, x2 - x1, y2 - y1);
                    removal5 << QPair<QRect, int>(r, indexHash.value(r, -1));
                }
            }
        }
    }

    removals << removal5;

    QList<QPair<QRect, int> > removal6;

    for (int y1 = 0; y1 < size; ++y1) {
        for (int y2 = y1; y2 < size; ++y2) {
            for (int x1 = 0; x1 < size; ++x1) {
                for (int x2 = x1; x2 < size; ++x2) {
                    QRect r = QRect(x1, y1, x2 - x1, y2 - y1);
                    removal6 << QPair<QRect, int>(r, indexHash.value(r, -1));
                }
            }
        }
    }

    removals << removal6;

    index = 0;

    for (int i = 0; i < removals.size(); ++i) {
        AAInterval2DTree<int> *tree = new AAInterval2DTree<int>();

        QList<int> data;

        for (int x1 = 0; x1 < size; ++x1) {
            for (int x2 = x1; x2 < size; ++x2) {
                for (int y1 = 0; y1 < size; ++y1) {
                    for (int y2 = y1; y2 < size; ++y2) {
                        tree->insert(QRect(x1, y1, x2 - x1, y2 - y1), index);
                        data.append(index);
                        ++index;
                    }
                }
            }
        }

        QCOMPARE(tree->items(), data);

        QList<QPair<QRect, int> > removal = removals.at(i);

        for (int j = 0; j < removal.size(); ++j) {
            QPair<QRect, int> p = removal.at(j);
            tree->remove(p.first, p.second);
            data.removeAll(p.second);

            QCOMPARE(tree->items(), data);
        }

        delete tree;
    }
}

void tst_IntervalTree::remove2D_duplicates()
{
    AAInterval2DTree<int> *tree = new AAInterval2DTree<int>();

    QList<int> data;

    QList<QPair<QRect, int> > items;

    int size = 6;
    int index = 0;

    for (int x1 = 0; x1 < size; ++x1) {
        for (int x2 = x1; x2 < size; ++x2) {
            for (int y1 = 0; y1 < size; ++y1) {
                for (int y2 = y1; y2 < size; ++y2) {
                    QRect r = QRect(x1, y1, x2 - x1, y2 - y1);
                    tree->insert(r, index);
                    data.append(index);
                    items << QPair<QRect, int>(r, index);
                    ++index;
                    tree->insert(r, index);
                    data.append(index);
                    items << QPair<QRect, int>(r, index);
                    ++index;
                }
            }
        }
    }

    QCOMPARE(tree->items(), data);

    for (int i = 0; i < items.size(); ++i) {
        QPair<QRect, int> p = items.at(i);
        tree->remove(p.first, p.second);
        data.removeAll(p.second);
        QCOMPARE(tree->items(), data);
    }

    delete tree;
}

void tst_IntervalTree::itemsAt2D()
{
    QList<ItemTriple> triples;

    triples << ItemTriple(1, 4, 1);
    triples << ItemTriple(6, 9, 2);
    triples << ItemTriple(7, 11, 3);
    triples << ItemTriple(9, 10, 4);
    triples << ItemTriple(16, 24, 5);
    triples << ItemTriple(17, 22, 6);
    triples << ItemTriple(18, 20, 7);
    triples << ItemTriple(20, 21, 8);
    triples << ItemTriple(26, 31, 9);
    triples << ItemTriple(27, 27, 10);

    QHash<int, QSet<int> > data;

    AAInterval2DTree<int> tree;

    for (int i = 0; i < triples.size(); ++i) {
        ItemTriple t1 = triples.at(i);
        for (int j = 0; j < triples.size(); ++j) {
            ItemTriple t2 = triples.at(i);
            tree.insert(QRect(t1.min, t2.min, t1.max - t1.min, t2.max - t2.min), t1.value * 32 + t2.value);
            for (int k = t1.min; k <= t1.max; ++k) {
                for (int l = t2.min; l <= t2.max; ++l) {
                    data[k * 32 + l].insert(t1.value * 32 + t2.value);
                }
            }
        }
    }

    for (int i = 1; i <= 31; ++i) {
        for (int j = 1; j <= 31; ++j) {
            QCOMPARE(tree.itemsAt(QPoint(i, j)).toSet(), data.value(i * 32 + j));
        }
    }
}

void tst_IntervalTree::itemsWithin2D()
{
    QList<ItemTriple> triples;

    triples << ItemTriple(1, 4, 1);
    triples << ItemTriple(6, 9, 2);
    triples << ItemTriple(7, 11, 3);
    triples << ItemTriple(9, 10, 4);
    triples << ItemTriple(16, 24, 5);
    triples << ItemTriple(17, 22, 6);
    triples << ItemTriple(18, 20, 7);
    triples << ItemTriple(20, 21, 8);
    triples << ItemTriple(26, 31, 9);
    triples << ItemTriple(27, 27, 10);

    QHash<int, QSet<int> > data;

    AAInterval2DTree<int> tree;

    for (int i = 0; i < triples.size(); ++i) {
        ItemTriple t1 = triples.at(i);
        for (int j = 0; j < triples.size(); ++j) {
            ItemTriple t2 = triples.at(i);
            tree.insert(QRect(t1.min, t2.min, t1.max - t1.min, t2.max - t2.min), t1.value * 32 + t2.value);
            for (int k = t1.min; k <= t1.max; ++k) {
                for (int l = t2.min; l <= t2.max; ++l) {
                    data[k * 32 + l].insert(t1.value * 32 + t2.value);
                }
            }
        }
    }

    for (int i1 = 1; i1 <= 31; ++i1) {
        for (int i2 = i1; i2 <= 31; ++i2) {
            for (int j1 = 1; j1 <= 31; ++j1) {
                for (int j2 = j1; j2 <= 31; ++j2) {
                    QSet<int> d;
                    for (int x = i1; x <= i2; ++x) {
                        for (int y = j1; y <= j2; ++y) {
                            d += data.value(32 * x + y);
                        }
                    }
                    QCOMPARE(tree.itemsWithin(QRect(i1, j1, i2 - i1, j2 - j1)).toSet(), d);
                }
            }
        }
    }
}


QTEST_APPLESS_MAIN(tst_IntervalTree)
#include "tst_intervaltree.moc"


