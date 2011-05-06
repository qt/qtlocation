/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QLANDMARKSORTORDER_P_H
#define QLANDMARKSORTORDER_P_H

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

#include "qlandmarksortorder.h"
#include <QSharedData>

QTM_BEGIN_NAMESPACE

#define Q_IMPLEMENT_LANDMARKSORTORDER_PRIVATE(Class) \
    Class##Private* Class::d_func() { return reinterpret_cast<Class##Private *>(d_ptr.data()); } \
    const Class##Private* Class::d_func() const { return reinterpret_cast<const Class##Private *>(d_ptr.constData()); } \
    Class::Class(const QLandmarkSortOrder& other) : QLandmarkSortOrder() { Class##Private::copyIfPossible(d_ptr, other); }

#define Q_IMPLEMENT_LANDMARKSORTORDER_VIRTUALCTORS(Class, Type) \
    virtual QLandmarkSortOrderPrivate* clone() const { return new Class##Private(*this); } \
    static void copyIfPossible(QSharedDataPointer<QLandmarkSortOrderPrivate>& d_ptr, const QLandmarkSortOrder& other) \
    { \
        if (other.type() == Type) \
            d_ptr = extract_d(other); \
        else \
            d_ptr = new Class##Private(); \
    }

class QLandmarkSortOrderPrivate : public QSharedData
{
public:
    QLandmarkSortOrderPrivate();
    QLandmarkSortOrderPrivate(const QLandmarkSortOrderPrivate &);
    virtual ~QLandmarkSortOrderPrivate();

    virtual bool compare(const QLandmarkSortOrderPrivate *other) const {
        return order == other->order;
    }

    /* Helper functions for C++ protection rules */
    static const QSharedDataPointer<QLandmarkSortOrderPrivate>& extract_d(const QLandmarkSortOrder& other) {return other.d_ptr;}

    Q_IMPLEMENT_LANDMARKSORTORDER_VIRTUALCTORS(QLandmarkSortOrder, QLandmarkSortOrder::NoSort)

    QLandmarkSortOrder::SortType type;
    Qt::SortOrder order;
};

QTM_END_NAMESPACE

#if defined(Q_CC_MWERKS)
// This results in multiple symbol definition errors on all other compilers
// but not having a definition here results in an attempt to use the unspecialized
// clone (which fails because of the pure virtuals above)
template<> QTM_PREPEND_NAMESPACE(QLandmarkSortOrderPrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QLandmarkSortOrderPrivate)>::clone()
{
    return d->clone();
}
#else
template<> QTM_PREPEND_NAMESPACE(QLandmarkSortOrderPrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QLandmarkSortOrderPrivate)>::clone();
#endif

QT_END_NAMESPACE

#endif
