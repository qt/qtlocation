/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QPLACEREVIEW_H
#define QPLACEREVIEW_H

#include <QSharedDataPointer>
#include <QStringList>
#include "qmobilityglobal.h"
#include "qplacesupplier.h"
#include "qplacecontent.h"

QT_BEGIN_NAMESPACE

class QPlaceReviewPrivate;

class Q_LOCATION_EXPORT QPlaceReview : public QPlaceContent
{
public:
    QPlaceReview();
    Q_DECLARE_CONTENT_COPY_CTOR(QPlaceReview)

    virtual ~QPlaceReview();

    QString date() const;
    void setDate(const QString &data);
    QString description() const;
    void setDescription(const QString &data);
    QString language() const;
    void setLanguage(const QString &data);
    int helpfulVotings() const;
    void setHelpfulVotings(const int &votes);
    QStringList mediaIds() const;
    void setMediaIds(const QStringList &data);
    int unhelpfulVotings() const;
    void setUnhelpfulVotings(const int &votes);
    qreal rating() const;
    void setRating(const qreal &data);
    QString reviewId() const;
    void setReviewId(const QString &data);
    QPlaceSupplier supplier() const;
    void setSupplier(const QPlaceSupplier &data);
    QString title() const;
    void setTitle(const QString &data);
    QString userId() const;
    void setUserId(const QString &data);
    QString userName() const;
    void setUserName(const QString &data);
    QUrl originatorUrl() const;
    void setOriginatorUrl(const QUrl &data);

private:
    Q_DECLARE_CONTENT_D_FUNC(QPlaceReview)
};

QT_END_NAMESPACE

#endif // QPLACEREVIEW_H
