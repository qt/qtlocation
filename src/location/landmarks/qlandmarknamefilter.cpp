/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarknamefilter.h"
#include "qlandmarknamefilter_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkNameFilter
    \brief The QLandmarkNameFilter class is used to search for landmarks by name.

    \inmodule QtLocation
    \since 1.1

    \ingroup landmarks-filter

    Please note that different platforms support different capabilities with the attribute filter.
    \list
        \o The S60 3.1, 3.2 and 5.0 platforms do not support the MatchContains flag while the Symbian
    platform does.
        \o Note also that MatchContains is supported using the sparql and sqlite \l {Landmark Managers and Plugins} {managers}.
    \endlist
*/

Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkNameFilter)

/*!
    Creates a filter that selects landmarks by \a name.
*/
QLandmarkNameFilter::QLandmarkNameFilter(const QString &name)
        : QLandmarkFilter(new QLandmarkNameFilterPrivate(name)) {}

/*!
    \fn QLandmarkNameFilter::QLandmarkNameFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new name filter.
*/

/*!
    Destroys the filter.
*/
QLandmarkNameFilter::~QLandmarkNameFilter()
{
    // pointer deleted in superclass destructor
}

/*!
    Returns the name that the filter will use to determine matches.
*/
QString QLandmarkNameFilter::name() const
{
    Q_D(const QLandmarkNameFilter);
    return d->name;
}

/*!
    Sets the \a name that the filter will use to determine matches.
*/
void QLandmarkNameFilter::setName(const QString &name)
{
    Q_D(QLandmarkNameFilter);
    d->name = name;
}

/*!
    Returns the matching criteria of the filter.
*/
QLandmarkFilter::MatchFlags QLandmarkNameFilter::matchFlags() const
{
    Q_D(const QLandmarkNameFilter);
    return d->flags;
}

/*!
    Sets the matching criteria to those defined in \a flags.
*/
void QLandmarkNameFilter::setMatchFlags(QLandmarkFilter::MatchFlags flags)
{
    Q_D(QLandmarkNameFilter);
    d->flags = flags;
}

/*******************************************************************************
*******************************************************************************/

QLandmarkNameFilterPrivate::QLandmarkNameFilterPrivate(const QString &name)
        : name(name),
          flags(0)
{
    type = QLandmarkFilter::NameFilter;
}

QLandmarkNameFilterPrivate::QLandmarkNameFilterPrivate(const QLandmarkNameFilterPrivate &other)
        : QLandmarkFilterPrivate(other),
        name(other.name),
        flags(other.flags) {}

QLandmarkNameFilterPrivate::~QLandmarkNameFilterPrivate() {}

QTM_END_NAMESPACE
