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

#include "qlandmarkid.h"
#include "qlandmarkid_p.h"
#include <QString>

QTM_USE_NAMESPACE

/*!
    \class QLandmarkId

    \inmodule QtLocation

    \ingroup landmarks-main
    \since 1.1

    \brief The QLandmarkId class provides a unique identifier for
    a landmark.

    It consists of a manager URI which identifies the manager which contains
    the landmark, and a manager specific local id for that landmark.
*/

/*!

    Creates an invalid identifier.

     The local id and manager uri are empty strings.
*/
QLandmarkId::QLandmarkId()
        : d(new QLandmarkIdPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkId::QLandmarkId(const QLandmarkId &other)
        : d(other.d)
{
}

/*!
    Destroys the identifier.
*/
QLandmarkId::~QLandmarkId()
{
}

/*!
    Returns true if the identifier is valid, meaning that both
    a manager URI and local id have been set.
*/
bool QLandmarkId::isValid() const
{
    return (!d->m_uri.isEmpty() && !d->m_localId.isEmpty());
}

/*!
    Returns the local id of the landmark identifier.

    This id is specific to a particular landmark manager.
*/
QString QLandmarkId::localId() const
{
    return d->m_localId;
}

/*!
    Sets the local \a id of the landmark identifier.
*/
void QLandmarkId::setLocalId(const QString &id)
{
    d->m_localId = id;
}

/*!
    Returns the URI of the manager which this landmark identifer
    belongs to.
*/
QString QLandmarkId::managerUri() const
{
    return d->m_uri;
}

/*!
    Sets the manager \a uri which this landmark identifier
    belongs to.
*/
void QLandmarkId::setManagerUri(const QString &uri)
{
    d->m_uri = uri;
}

/*!
    Assigns \a other to this landmark identifier and returns a reference
    to this landmark identifier.
*/
QLandmarkId &QLandmarkId::operator=(const QLandmarkId & other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

/*!
    Returns true if this landmark identifer is equal to \a other, otherwise
    returns false.

    Two QLandmarkIds are considered equal if both have the same
    manager URI and local id.

    \sa operator!=()
*/
bool QLandmarkId::operator==(const QLandmarkId &other) const
{
    return ((d->m_localId == other.d->m_localId)
            && (d->m_uri == other.d->m_uri));
}

/*!
    Returns true if this landmark identifier is not equal to \a other,
    otherwise returns false.

    If either the manager URIs or local ids differ, then the
    QLandmarkIds are not considered equal.

    \sa operator==()
*/
bool QLandmarkId::operator!=(const QLandmarkId &other) const
{
    return !this->operator ==(other);
}


