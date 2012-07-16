/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLACESEARCHRESULT_P_H
#define QPLACESEARCHRESULT_P_H

#include "qplacesearchresult.h"

#include <QSharedData>
#include <QtLocation/QPlaceIcon>

QT_BEGIN_NAMESPACE

#define Q_IMPLEMENT_SEARCHRESULT_D_FUNC(Class) \
    Class##Private *Class::d_func() { return reinterpret_cast<Class##Private *>(d_ptr.data()); } \
    const Class##Private *Class::d_func() const { return reinterpret_cast<const Class##Private *>(d_ptr.constData()); } \

#define Q_IMPLEMENT_SEARCHRESULT_COPY_CTOR(Class) \
    Class::Class(const QPlaceSearchResult &other) : QPlaceSearchResult() { Class##Private::copyIfPossible(d_ptr, other); }

#define Q_DEFINE_SEARCHRESULT_PRIVATE_HELPER(Class, ResultType) \
    virtual QPlaceSearchResultPrivate *clone() const { return new Class##Private(*this); } \
    virtual QPlaceSearchResult::SearchResultType type() const {return ResultType;} \
    static void copyIfPossible(QSharedDataPointer<QPlaceSearchResultPrivate> &d_ptr, const QPlaceSearchResult &other) \
    { \
        if (other.type() == ResultType) \
            d_ptr = extract_d(other); \
        else \
            d_ptr = new Class##Private; \
    }

class QPlaceSearchResultPrivate : public QSharedData
{
public:
    QPlaceSearchResultPrivate() {}
    virtual ~QPlaceSearchResultPrivate() {}

    virtual bool compare(const QPlaceSearchResultPrivate *other) const;

    static const QSharedDataPointer<QPlaceSearchResultPrivate>
            &extract_d(const QPlaceSearchResult &other) { return other.d_ptr; }

    Q_DEFINE_SEARCHRESULT_PRIVATE_HELPER(QPlaceSearchResult, QPlaceSearchResult::UnknownSearchResult)

    QString title;
    QPlaceIcon icon;
};

template<> QPlaceSearchResultPrivate *QSharedDataPointer<QPlaceSearchResultPrivate>::clone();

QT_END_NAMESPACE

#endif // QPLACESEARCHRESULT_P_H
