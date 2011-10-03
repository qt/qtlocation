/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
#ifndef INTERVALTREE_P_H
#define INTERVALTREE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QString>
#include <QRect>
#include <QPoint>

QT_BEGIN_NAMESPACE

template <class D>
class AAIntervalTreeNodeOperation;

template <class D>
class AAIntervalTreeNode
{
public:
    AAIntervalTreeNode();
    ~AAIntervalTreeNode();

    bool operator == (const AAIntervalTreeNode<D> &other) const;
    bool operator != (const AAIntervalTreeNode<D> &other) const;

    AAIntervalTreeNode<D> * find(int min, int max);

    AAIntervalTreeNode<D> * insert(int min, int max, D data);
    AAIntervalTreeNode<D> * remove(int min, int max, D data);

    void visitRange(int x1, int x2, AAIntervalTreeNodeOperation<D> *op);

    D& data();

    QList<D> items() const;
    QList<D> itemsAt(int x) const;
    QList<D> itemsWithin(int x1, int x2) const;

    QList<AAIntervalTreeNode<D>*> nodes();
    QList<AAIntervalTreeNode<D>*> nodesAt(int x);
    QList<AAIntervalTreeNode<D>*> nodesWithin(int x1, int x2);

    QString print(const QString &indent);

private:
    AAIntervalTreeNode<D> * skew();
    AAIntervalTreeNode<D> * split();

    bool nodeLess(int min, int max) const;
    bool nodeGreater(int min, int max) const;

    int min_;
    int max_;
    int maxTree_;
    D data_;

    int level_;
    AAIntervalTreeNode<D> * left_;
    AAIntervalTreeNode<D> * right_;
};

template <class D>
class AAIntervalTreeNodeOperation
{
public:
    virtual ~AAIntervalTreeNodeOperation() {}
    virtual void apply(D data) = 0;
};

template <class D>
AAIntervalTreeNode<D>::AAIntervalTreeNode()
        : min_(-1),
        max_(-1),
        maxTree_(-1),
        level_(0),
        left_(0),
        right_(0)
{
}

template <class D>
AAIntervalTreeNode<D>::~AAIntervalTreeNode()
{
    if (left_)
        delete left_;
    if (right_)
        delete right_;
}

template <class D>
bool AAIntervalTreeNode<D>::operator == (const AAIntervalTreeNode<D> &other) const
{
    if (min_ != other.min_)
        return false;

    if (max_ != other.max_)
        return false;

    if (data_ != other.data_)
        return false;

    if ((left_ == 0) != (other.left_ == 0))
        return false;

    if ((right_ == 0) != (other.right_ == 0))
        return false;

    if (left_ && !(*left_ == *(other.left_)))
        return false;

    if (right_ && !(*right_ == *(other.right_)))
        return false;

    return true;
}

template <class D>
bool AAIntervalTreeNode<D>::operator != (const AAIntervalTreeNode<D> &other) const
{
    return !(*this == other);
}

template <class D>
bool AAIntervalTreeNode<D>::nodeLess(int min, int max) const
{
    if (this->min_ < min)
        return true;
    if (min < this->min_)
        return false;
    return (this->max_ < max);
}

template <class D>
bool AAIntervalTreeNode<D>::nodeGreater(int min, int max) const
{
    if (min < this->min_)
        return true;
    if (this->min_ < min)
        return false;
    return (max < this->max_);
}

template <class D>
AAIntervalTreeNode<D> * AAIntervalTreeNode<D>::find(int min, int max)
{
    if (nodeGreater(min, max)) {
        if (left_)
            return left_->find(min, max);
        return 0;
    } else if (nodeLess(min, max)) {
        if (right_)
            return right_->find(min, max);
        return 0;
    } else {
        return this;
    }
}

template <class D>
void AAIntervalTreeNode<D>::visitRange(int x1, int x2, AAIntervalTreeNodeOperation<D> *op)
{
    if (left_ && (x1 <= left_->maxTree_))
        left_->visitRange(x1, x2, op);

    if ((min_ <= x2) && (x1 <= max_))
        op->apply(data_);

    if (right_ && (min_ <= x2))
        right_->visitRange(x1, x2, op);
}

template <class D>
AAIntervalTreeNode<D> * AAIntervalTreeNode<D>::insert(int min, int max, D data)
{
    if (level_ == 0) {
        level_ = 1;
        this->min_ = min;
        this->max_ = max;
        this->maxTree_ = max;
        this->data_ = data;
        return this;
    }

    if (nodeGreater(min, max)) {
        if (!left_)
            left_ = new AAIntervalTreeNode<D>();
        left_ = left_->insert(min, max, data);
    } else if (nodeLess(min, max)) {
        if (!right_)
            right_ = new AAIntervalTreeNode<D>();
        right_ = right_->insert(min, max, data);
    } else {
        // TODO handle equal case ie don't silently ignore it
    }

    AAIntervalTreeNode<D> * result = this->skew()->split();

    int m = result->max_;
    if (result->left_)
        m = qMax(m, result->left_->maxTree_);
    if (result->right_)
        m = qMax(m, result->right_->maxTree_);
    result->maxTree_ = m;

    return result;
}

template <class D>
AAIntervalTreeNode<D> * AAIntervalTreeNode<D>::remove(int min, int max, D data)
{
    if (nodeGreater(min, max)) {
        if (left_) {
            AAIntervalTreeNode<D> *newLeft = left_->remove(min, max, data);
            if (newLeft) {
                left_ = newLeft;
            } else {
                delete left_;
                left_ = 0;
            }
        }
    } else if (nodeLess(min, max)) {
        if (right_) {
            AAIntervalTreeNode<D> *newRight = right_->remove(min, max, data);
            if (newRight) {
                right_ = newRight;
            } else {
                delete right_;
                right_ = 0;
            }
        }
    } else {
        // TODO handle multiple equal cases
        // - remove particular case
        // - return unless the list of cases is now empty
        if (left_) {
            if (right_) {
                AAIntervalTreeNode<D> *prev = left_;

                while (prev->right_)
                    prev = prev->right_;

                this->min_ = prev->min_;
                this->max_ = prev->max_;
                this->data_ = prev->data_;

                left_ = left_->remove(this->min_, this->max_, this->data_);
            } else {
                return left_;
            }
        } else  {
            if (right_) {
                return right_;
            } else {
                return 0;
            }
        }
    }

    AAIntervalTreeNode<D> *newRoot = this;

    if ((left_ && (left_->level_ < level_ - 1))
            || (right_ && (right_->level_ < level_ - 1))) {

        if (right_ && (right_->level_ > --level_))
            right_->level_ = level_;

        newRoot = newRoot->skew()->split();
    }

    int m = newRoot->max_;
    if (newRoot->left_)
        m = qMax(m, newRoot->left_->maxTree_);
    if (newRoot->right_)
        m = qMax(m, newRoot->right_->maxTree_);
    newRoot->maxTree_ = m;

    return newRoot;
}

template <class D>
D& AAIntervalTreeNode<D>::data()
{
    return data_;
}

template <class D>
QList<D> AAIntervalTreeNode<D>::items() const
{
    QList<D> result;

    if (left_)
        result.append(left_->items());
    result.append(data_);
    if (right_)
        result.append(right_->items());

    return result;
}

template <class D>
QList<D> AAIntervalTreeNode<D>::itemsAt(int x) const
{
    QList<D> result;

    if (left_ && (x <= left_->maxTree_))
        result.append(left_->itemsAt(x));
    if ((min_ <= x) && (x <= max_))
        result.append(data_);
    if (right_ && (min_ <= x))
        result.append(right_->itemsAt(x));

    return result;
}

template <class D>
QList<D> AAIntervalTreeNode<D>::itemsWithin(int x1, int x2) const
{
    QList<D> result;

    if (left_ && (x1 <= left_->maxTree_))
        result.append(left_->itemsWithin(x1, x2));
    if ((min_ <= x2) && (x1 <= max_))
        result.append(data_);
    if (right_ && (min_ <= x2))
        result.append(right_->itemsWithin(x1, x2));

    return result;
}

template <class D>
QList<AAIntervalTreeNode<D>*> AAIntervalTreeNode<D>::nodes()
{
    QList<AAIntervalTreeNode<D>*> result;

    if (left_)
        result.append(left_->nodes());
    result.append(this);
    if (right_)
        result.append(right_->nodes());

    return result;
}

template <class D>
QList<AAIntervalTreeNode<D>*> AAIntervalTreeNode<D>::nodesAt(int x)
{
    QList<AAIntervalTreeNode<D>*> result;

    if (left_ && (x <= left_->maxTree_))
        result.append(left_->nodesAt(x));
    if ((min_ <= x) && (x <= max_))
        result.append(this);
    if (right_ && (min_ <= x))
        result.append(right_->nodesAt(x));

    return result;
}

template <class D>
QList<AAIntervalTreeNode<D>*> AAIntervalTreeNode<D>::nodesWithin(int x1, int x2)
{
    QList<AAIntervalTreeNode<D>*> result;

    if (left_ && (x1 <= left_->maxTree_))
        result.append(left_->nodesWithin(x1, x2));
    if ((min_ <= x2) && (x1 <= max_))
        result.append(this);
    if (right_ && (min_ <= x2))
        result.append(right_->nodesWithin(x1, x2));

    return result;
}

template <class D>
AAIntervalTreeNode<D> * AAIntervalTreeNode<D>::skew()
{
    if (level_ == 0)
        return this;

    AAIntervalTreeNode<D> *newRoot = this;

    if (left_ && (level_ == left_->level_)) {
        AAIntervalTreeNode<D> *newRight = this;

        newRoot = left_;
        newRight->left_ = newRoot->right_;
        newRoot->right_ = newRight;
    }

    if (newRoot->right_)
        newRoot->right_ = newRoot->right_->skew();

    return newRoot;
}

template <class D>
AAIntervalTreeNode<D> * AAIntervalTreeNode<D>::split()
{
    if (level_ == 0)
        return this;

    AAIntervalTreeNode<D> *newRoot = this;

    if (right_ && right_->right_ && (level_ == right_->right_->level_)) {
        AAIntervalTreeNode<D> *newLeft = this;

        newRoot = right_;
        newLeft->right_ = newRoot->left_;
        newRoot->left_ = newLeft;

        ++(newRoot->level_);

        if (newRoot->right_)
            newRoot->right_ = newRoot->right_->split();
    }

    return newRoot;
}

template <class D>
QString AAIntervalTreeNode<D>::print(const QString &indent)
{
    QString s;
    s += indent + "level " + QString::number(level_) + "\n";
    s += indent + "min " + QString::number(min_) + "\n";
    s += indent + "max " + QString::number(max_) + "\n";
    s += indent + "maxTree " + QString::number(maxTree_) + "\n";
    s += indent + "left\n";
    if (left_)
        s += left_->print(indent + "  ");
    s += indent + "right\n";
    if (right_)
        s += right_->print(indent + "  ");
    return s;
}

template <class D>
class Q_LOCATION_EXPORT AAIntervalTree
{
public:
    AAIntervalTree();
    ~AAIntervalTree();

    bool operator == (const AAIntervalTree<D> &other) const;
    bool operator != (const AAIntervalTree<D> &other) const;

    bool isEmpty() const;

    void insert(int min, int max, D data);
    void remove(int min, int max, D data);

    QList<D> items() const;
    QList<D> itemsAt(int x) const;
    QList<D> itemsWithin(int x1, int x2) const;

    QString print();

private:
    AAIntervalTreeNode<D> * root_;
};

template <class D>
AAIntervalTree<D>::AAIntervalTree()
        : root_(0) {}

template <class D>
AAIntervalTree<D>::~AAIntervalTree()
{
    if (root_)
        delete root_;
}

template <class D>
bool AAIntervalTree<D>::operator == (const AAIntervalTree<D> &other) const
{
    if ((root_ == 0) != (other.root_ == 0))
        return false;

    return (*root_ == *(other.root_));
}

template <class D>
bool AAIntervalTree<D>::operator != (const AAIntervalTree<D> &other) const
{
    return !(*this == other);
}

template <class D>
bool AAIntervalTree<D>::isEmpty() const
{
    return (root_ == 0);
}

template <class D>
void AAIntervalTree<D>::insert(int min, int max, D data)
{
    if (!root_)
        root_ = new AAIntervalTreeNode<D>();
    root_ = root_->insert(min, max, data);
}

template <class D>
void AAIntervalTree<D>::remove(int min, int max, D data)
{
    if (!root_)
        return;

    AAIntervalTreeNode<D> *newRoot = root_->remove(min, max, data);
    if (newRoot) {
        root_ = newRoot;
    } else {
        delete root_;
        root_ = 0;
    }
}

template <class D>
QList<D> AAIntervalTree<D>::items() const
{
    QList<D> result;

    if (root_)
        result = root_->items();

    return result;
}

template <class D>
QList<D> AAIntervalTree<D>::itemsAt(int x) const
{
    QList<D> result;

    if (root_)
        result = root_->itemsAt(x);

    return result;
}

template <class D>
QList<D> AAIntervalTree<D>::itemsWithin(int x1, int x2) const
{
    QList<D> result;

    if (root_)
        result = root_->itemsWithin(x1, x2);

    return result;
}

template <class D>
QString AAIntervalTree<D>::print()
{
    QString s;

    if (root_)
        s = root_->print("");

    return s;
}

template <class D>
class Q_LOCATION_EXPORT AAInterval2DTree
{
public:
    AAInterval2DTree();
    ~AAInterval2DTree();

    bool operator == (const AAInterval2DTree<D> &other) const;
    bool operator != (const AAInterval2DTree<D> &other) const;

    bool isEmpty() const;

    void insert(const QRect &rect, D data);
    void remove(const QRect &rect, D data);

    void visitRect(const QRect &rect, AAIntervalTreeNodeOperation<D> *op);

    QList<D> items() const;
    QList<D> itemsAt(const QPoint &point) const;
    QList<D> itemsWithin(const QRect &viewport) const;

    QString print();

private:
    AAIntervalTreeNode<AAIntervalTreeNode<QList<D> >*> * root_;
};

template <class D>
class InnerOp : public AAIntervalTreeNodeOperation<AAIntervalTreeNode<QList<D> >*>
{
public:
    InnerOp(const QRect &rect, AAIntervalTreeNodeOperation<D> *op)
            : rect_(rect), op_(op) {}

    void apply(AAIntervalTreeNode<QList<D> >* data) {
        if (!data)
            return;
        data->visitRange(rect_.y(), rect_.y() + rect_.height(), op_);
    }

private:
    QRect rect_;
    AAIntervalTreeNodeOperation<D> *op_;
};

template <class D>
AAInterval2DTree<D>::AAInterval2DTree()
        : root_(0) {}

template <class D>
AAInterval2DTree<D>::~AAInterval2DTree()
{
    if (root_)
        delete root_;
}

template <class D>
bool AAInterval2DTree<D>::operator == (const AAInterval2DTree<D> &other) const
{
    if ((root_ == 0) != (other.root_ == 0))
        return false;

    return (*root_ == *(other.root_));
}

template <class D>
bool AAInterval2DTree<D>::operator != (const AAInterval2DTree<D> &other) const
{
    return !(*this == other);
}

template <class D>
bool AAInterval2DTree<D>::isEmpty() const
{
    return (root_ == 0);
}

template <class D>
void AAInterval2DTree<D>::visitRect(const QRect &rect, AAIntervalTreeNodeOperation<D> *op)
{
    if (!root_)
        return;

    AAIntervalTreeNodeOperation<D> *innerOp = new InnerOp<D>(rect, op);

    root_->visitRange(rect.x(), rect.x() + rect.width(), innerOp);

    delete innerOp;
}

template <class D>
void AAInterval2DTree<D>::insert(const QRect &rect, D data)
{
    if (!root_)
        root_ = new AAIntervalTreeNode<AAIntervalTreeNode<QList<D> >*>();

    AAIntervalTreeNode<AAIntervalTreeNode<QList<D> >*> *childNode
    = root_->find(rect.x(), rect.x() + rect.width());

    AAIntervalTreeNode<QList<D> > *child = 0;
    if (childNode)
        child = childNode->data();

    if (!child)
        child = new AAIntervalTreeNode<QList<D> >();

    AAIntervalTreeNode<QList<D> > *grandchildNode
    = child->find(rect.y(), rect.y() + rect.height());

    QList<D> list;

    if (grandchildNode)
        list = grandchildNode->data();

    if (!list.contains(data))
        list.append(data);

    if (grandchildNode)
        grandchildNode->data() = list;
    else
        child = child->insert(rect.y(), rect.y() + rect.height(), list);

    if (childNode)
        childNode->data() = child;
    else
        root_ = root_->insert(rect.x(), rect.x() + rect.width(), child);
}

template <class D>
void AAInterval2DTree<D>::remove(const QRect &rect, D data)
{
    if (!root_)
        return;

    AAIntervalTreeNode<AAIntervalTreeNode<QList<D> >*> *childNode
    = root_->find(rect.x(), rect.x() + rect.width());

    if (!childNode)
        return;

    AAIntervalTreeNode<QList<D> > *child = childNode->data();

    if (!child)
        return;

    AAIntervalTreeNode<QList<D> > *grandchildNode
    = child->find(rect.y(), rect.y() + rect.height());

    if (!grandchildNode)
        return;

    QList<D> list = grandchildNode->data();

    if (!list.contains(data))
        return;

    list.removeAll(data);

    grandchildNode->data() = list;

    if (!list.isEmpty())
        return;

    AAIntervalTreeNode<QList<D> > * newChild = child->remove(rect.y(), rect.y() + rect.height(), list);

    if (newChild) {
        childNode->data() = newChild;
        return;
    }

    AAIntervalTreeNode<AAIntervalTreeNode<QList<D> >*> *newRoot
    = root_->remove(rect.x(), rect.x() + rect.width(), child);

    delete child;

    if (newRoot) {
        root_ = newRoot;
    } else {
        delete root_;
        root_ = 0;
    }
}

template <class D>
QList<D> AAInterval2DTree<D>::items() const
{
    QList<D> result;

    if (root_) {
        QList<AAIntervalTreeNode<AAIntervalTreeNode<QList<D> >*>*> tmp1 = root_->nodes();
        for (int i = 0; i < tmp1.size(); ++i) {
            QList<AAIntervalTreeNode<QList<D> >*> tmp2 = tmp1.at(i)->data()->nodes();
            for (int j = 0; j < tmp2.size(); ++j)
                result.append(tmp2.at(j)->data());
        }
    }

    return result;
}

template <class D>
QList<D> AAInterval2DTree<D>::itemsAt(const QPoint &point) const
{
    QList<D> result;

    if (root_) {
        QList<AAIntervalTreeNode<AAIntervalTreeNode<QList<D> >*>*> tmp1 = root_->nodesAt(point.x());
        for (int i = 0; i < tmp1.size(); ++i) {
            QList<AAIntervalTreeNode<QList<D> >*> tmp2 = tmp1.at(i)->data()->nodesAt(point.y());
            for (int j = 0; j < tmp2.size(); ++j)
                result.append(tmp2.at(j)->data());
        }
    }

    return result;
}

template <class D>
QList<D> AAInterval2DTree<D>::itemsWithin(const QRect &viewport) const
{
    QList<D> result;

    if (root_) {
        QList<AAIntervalTreeNode<AAIntervalTreeNode<QList<D> >*>*> tmp1
        = root_->nodesWithin(viewport.x(), viewport.x() + viewport.width());
        for (int i = 0; i < tmp1.size(); ++i) {
            QList<AAIntervalTreeNode<QList<D> >*> tmp2
            = tmp1.at(i)->data()->nodesWithin(viewport.y(), viewport.y() + viewport.height());
            for (int j = 0; j < tmp2.size(); ++j)
                result.append(tmp2.at(j)->data());
        }
    }

    return result;
}

template <class D>
QString AAInterval2DTree<D>::print()
{
    QString s;

    if (root_)
        s = root_->print("");

    return s;
}

QT_END_NAMESPACE

#endif // INTERVALTREE_P_H

