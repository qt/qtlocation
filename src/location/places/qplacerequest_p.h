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

#ifndef QPLACEREQUEST_P_H
#define QPLACEREQUEST_P_H

#include "qplacerequest.h"

QT_BEGIN_NAMESPACE

class QPlaceRequestPrivate : public QSharedData
{
public:
    QPlaceRequestPrivate();
    QPlaceRequestPrivate(const QPlaceRequestPrivate &other);
    virtual ~QPlaceRequestPrivate();

    virtual bool compare(const QPlaceRequestPrivate *other) const;
    virtual QPlaceRequestPrivate *clone() const;
    virtual QPlaceRequest::Type type() const { return QPlaceRequest::ContentType; }
    virtual void clear();

    /* Helper functions for C++ protection rules */
    static const QSharedDataPointer<QPlaceRequestPrivate>& extract_d(const QPlaceRequest& other) {return other.d_ptr;}
    int offset;
    int limit;
};

#if defined(Q_CC_MWERKS)
// This results in multiple symbol definition errors on all other compilers
// but not having a definition here results in an attempt to use the unspecialized
// clone (which fails because of the pure virtuals above)
template<> QPlaceRequestPrivate *QSharedDataPointer<QPlaceRequestPrivate>::clone()
{
    return d->clone();
}
#else
template<> QPlaceRequestPrivate *QSharedDataPointer<QPlaceRequestPrivate>::clone();
#endif

QT_END_NAMESPACE

#endif
