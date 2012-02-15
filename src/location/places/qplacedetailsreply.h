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

#ifndef QPLACEDETAILSREPLY_H
#define QPLACEDETAILSREPLY_H

#include "qplacereply.h"
#include "qplace.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QPlaceDetailsReplyPrivate;
class Q_LOCATION_EXPORT QPlaceDetailsReply : public QPlaceReply
{
    Q_OBJECT
public:
    explicit QPlaceDetailsReply(QObject *parent = 0);
    virtual ~QPlaceDetailsReply();

    QPlaceReply::Type type() const;

    QPlace place() const;

protected:
    void setPlace(const QPlace &place);

private:
    Q_DISABLE_COPY(QPlaceDetailsReply)
    Q_DECLARE_PRIVATE(QPlaceDetailsReply)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLACEDETAILSREPLY_H
