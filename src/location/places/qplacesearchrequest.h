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

#ifndef QPLACESEARCHREQUEST_H
#define QPLACESEARCHREQUEST_H

#include <QtCore/QSharedDataPointer>
#include <QtLocation/placemacro.h>
#include <QtLocation/QPlaceCategory>
#include <QtLocation/qtlocation.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE



class QGeoBoundingArea;
class QPlaceSearchRequestPrivate;

class Q_LOCATION_EXPORT QPlaceSearchRequest
{
public:
    enum RelevanceHint {
        UnspecifiedHint,
        DistanceHint,
        LexicalPlaceNameHint
    };

    QPlaceSearchRequest();
    QPlaceSearchRequest(const QPlaceSearchRequest &other);


    QPlaceSearchRequest& operator=(const QPlaceSearchRequest &other);

    bool operator==(const QPlaceSearchRequest &other) const;
    bool operator!=(const QPlaceSearchRequest &other) const;

    ~QPlaceSearchRequest();

    QString searchTerm() const;
    void setSearchTerm(const QString &term);

    QList<QPlaceCategory> categories() const;
    void setCategory(const QPlaceCategory &category);
    void setCategories(const QList<QPlaceCategory> &categories);

    QGeoBoundingArea searchArea() const;
    void setSearchArea(const QGeoBoundingArea &area);

    int maximumCorrections() const;
    void setMaximumCorrections(int number);

    QtLocation::VisibilityScope visibilityScope() const;
    void setVisibilityScope(QtLocation::VisibilityScope visibilityScopes);

    RelevanceHint relevanceHint() const;
    void setRelevanceHint(RelevanceHint hint);

    int offset() const;
    void setOffset(int offset);
    int limit() const;
    void setLimit(int limit);

    void clear();

private:
    QSharedDataPointer<QPlaceSearchRequestPrivate> d_ptr;
    inline QPlaceSearchRequestPrivate *d_func();
    inline const QPlaceSearchRequestPrivate *d_func() const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLACESEARCHQUERY_H
