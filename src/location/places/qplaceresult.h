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

#ifndef QPLACERESULT_H
#define QPLACERESULT_H

#include <QtLocation/QPlace>
#include <QtLocation/QPlaceSearchResult>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QPlaceResultPrivate;

class Q_LOCATION_EXPORT QPlaceResult : public QPlaceSearchResult
{
public:
    QPlaceResult();

#ifdef Q_QDOC
    QPlaceResult::QPlaceResult(const QPlaceSearchResult &other);
#else
     Q_DECLARE_SEARCHRESULT_COPY_CTOR(QPlaceResult);
#endif

    virtual ~QPlaceResult();

    qreal distance() const;
    void setDistance(qreal distance);

    QPlace place() const;
    void setPlace(const QPlace &place);

    bool isSponsored() const;
    void setSponsored(bool sponsored);

private:
    Q_DECLARE_SEARCHRESULT_D_FUNC(QPlaceResult);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
