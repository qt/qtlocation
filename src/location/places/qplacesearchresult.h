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

#ifndef QPLACESEARCHRESULT_H
#define QPLACESEARCHRESULT_H

#include <QSharedDataPointer>
#include <QVariant>
#include <QString>
#include <QtLocation/QPlace>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define Q_DECLARE_SEARCHRESULT_D_FUNC(Class) \
    inline Class##Private *d_func(); \
    inline const Class##Private *d_func() const;\
    friend class Class##Private;

#define Q_DECLARE_SEARCHRESULT_COPY_CTOR(Class) \
    Class(const QPlaceSearchResult &other);

class QPlaceSearchResultPrivate;

class Q_LOCATION_EXPORT QPlaceSearchResult
{
public:
    QPlaceSearchResult();
    QPlaceSearchResult(const QPlaceSearchResult &other);

    virtual ~QPlaceSearchResult();

    QPlaceSearchResult &operator=(const QPlaceSearchResult &other);

    bool operator==(const QPlaceSearchResult &other) const;
    bool operator!=(const QPlaceSearchResult &other) const {
        return !(other == *this);
    }

    enum SearchResultType {
        UnknownSearchResult = 0,
        PlaceResult
    };

    SearchResultType type() const;

    QString title() const;
    void setTitle(const QString &title);

    QPlaceIcon icon() const;
    void setIcon(const QPlaceIcon &icon);

protected:
    explicit QPlaceSearchResult(QPlaceSearchResultPrivate *d);
    QSharedDataPointer<QPlaceSearchResultPrivate> d_ptr;

private:
    inline QPlaceSearchResultPrivate *d_func();
    inline const QPlaceSearchResultPrivate *d_func() const;

    friend class QPlaceSearchResultPrivate;
};

Q_DECLARE_TYPEINFO(QPlaceSearchResult, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceSearchResult)
Q_DECLARE_METATYPE(QPlaceSearchResult::SearchResultType)

QT_END_HEADER

#endif // QPLACESEARCHRESULT_H
