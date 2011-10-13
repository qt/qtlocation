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

#ifndef QPLACECONTENT_P_H
#define QPLACECONTENT_P_H

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

#include "qplacecontent.h"
#include "qplacesupplier.h"
#include "qplaceuser.h"

#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QUrl>

QT_BEGIN_NAMESPACE


#define Q_IMPLEMENT_CONTENT_D_FUNC(Class) \
    Class##Private* Class::d_func() { return reinterpret_cast<Class##Private *>(d_ptr.data()); } \
    const Class##Private* Class::d_func() const { return reinterpret_cast<const Class##Private *>(d_ptr.constData()); } \

#define Q_IMPLEMENT_CONTENT_COPY_CTOR(Class) \
    Class::Class(const QPlaceContent& other) : QPlaceContent() { Class##Private::copyIfPossible(d_ptr, other); }

#define Q_DEFINE_CONTENT_PRIVATE_HELPER(Class, ContentType) \
    QPlaceContentPrivate* clone() const { return new Class##Private(*this); } \
    virtual QPlaceContent::Type type() const {return ContentType;} \
    static void copyIfPossible(QSharedDataPointer<QPlaceContentPrivate>& d_ptr, const QPlaceContent& other) \
    { \
        if (other.type() == ContentType) \
            d_ptr = extract_d(other); \
        else \
            d_ptr = new Class##Private; \
    }

class QPlaceContentPrivate : public QSharedData
{
public:
    QPlaceContentPrivate(){}
    virtual ~QPlaceContentPrivate(){}

    virtual bool compare(const QPlaceContentPrivate *other) const;
    virtual QPlaceContentPrivate* clone() const = 0;
    virtual QPlaceContent::Type type() const = 0;

    /* Helper functions for C++ protection rules */
    static const QSharedDataPointer<QPlaceContentPrivate>& extract_d(const QPlaceContent& other) {return other.d_ptr;}

    QPlaceSupplier supplier;
    QUrl sourceUrl;
    QPlaceUser user;
    QString attribution;
};

#if defined(Q_CC_MWERKS)
// This results in multiple symbol definition errors on all other compilers
// but not having a definition here results in an attempt to use the unspecialized
// clone (which fails because of the pure virtuals above)
template<> QPlaceContentPrivate *QSharedDataPointer<QPlaceContentPrivate>::clone()
{
    return d->clone();
}
#else
template<> QPlaceContentPrivate *QSharedDataPointer<QPlaceContentPrivate>::clone();
#endif

QT_END_NAMESPACE

#endif

