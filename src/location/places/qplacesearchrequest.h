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

#ifndef QPLACESEARCHREQUEST_H
#define QPLACESEARCHREQUEST_H

#include <QSharedDataPointer>
#include "qgeoboundingarea.h"
#include "qplacecategory.h"
#include "qplacerequest.h"
#include "qplacemanager.h"

#include <QString>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Location)

class QPlaceSearchRequestPrivate;

class Q_LOCATION_EXPORT QPlaceSearchRequest : public QPlaceRequest
{
public:
    enum RelevanceHint {
        DistanceHint,
        RatingHint,
        AlphabetHint
    };

    QPlaceSearchRequest();
    Q_DECLARE_COPY_CTOR(QPlaceSearchRequest, QPlaceRequest)
    virtual ~QPlaceSearchRequest();

    QString searchTerm() const;
    void setSearchTerm(const QString &term);

    QList<QPlaceCategory> categories() const;
    void setCategory(const QPlaceCategory &category);

    QGeoBoundingArea *searchArea() const;
    void setSearchArea(QGeoBoundingArea *area);

    int didYouMeanSuggestionNumber() const;
    void setDidYouMeanSuggestionNumber(const int &number);

    QPlaceManager::VisibilityScopes visibilityScope() const;
    void setVisibilityScope(QPlaceManager::VisibilityScopes scope);

private:
    Q_DECLARE_D_FUNC(QPlaceSearchRequest)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLACESEARCHQUERY_H
