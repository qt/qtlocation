/****************************************************************************
**
** Copyright (C) 2013 Aaron McCarthy <mccarthy.aaron@gmail.com>
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplaceproposedsearchresult.h"
#include "qplaceproposedsearchresult_p.h"

QT_BEGIN_NAMESPACE

bool QPlaceProposedSearchResultPrivate::compare(const QPlaceSearchResultPrivate *other) const
{
    const QPlaceProposedSearchResultPrivate *od = static_cast<const QPlaceProposedSearchResultPrivate *>(other);
    return QPlaceSearchResultPrivate::compare(other) && searchRequest == od->searchRequest;
}

/*!
    \class QPlaceProposedSearchResult
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since QtLocation 5.2

    \brief The QPlaceProposedSearchResult class represents a search result containing a proposed search.

    \sa QPlaceSearchResult
*/

/*!
    Constructs a new proposed search result.
*/
QPlaceProposedSearchResult::QPlaceProposedSearchResult()
:   QPlaceSearchResult(new QPlaceProposedSearchResultPrivate)
{
}

/*!
    \fn QPlaceProposedSearchResult::QPlaceProposedSearchResult(const QPlaceSearchRequest &other)

    Contructs a copy of \a other if possible, otherwise constructs a default proposed search
    result.
*/
Q_IMPLEMENT_SEARCHRESULT_COPY_CTOR(QPlaceProposedSearchResult)

Q_IMPLEMENT_SEARCHRESULT_D_FUNC(QPlaceProposedSearchResult)

/*!
    Destroys the proposed search result.
*/
QPlaceProposedSearchResult::~QPlaceProposedSearchResult()
{
}

/*!
    Returns a place search request that can be used to perform an additional proposed search.
*/
QPlaceSearchRequest QPlaceProposedSearchResult::searchRequest() const
{
    Q_D(const QPlaceProposedSearchResult);
    return d->searchRequest;
}

/*!
    Sets the proposed search request to \a request.
*/
void QPlaceProposedSearchResult::setSearchRequest(const QPlaceSearchRequest &request)
{
    Q_D(QPlaceProposedSearchResult);
    d->searchRequest = request;
}

QT_END_NAMESPACE
