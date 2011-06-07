/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qlandmarksaverequest.h"
#include "qlandmarkrequests_p.h"
#include "qlandmark.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkSaveRequest
    \brief The QLandmarkSaveRequest class allows a client to asynchronously
    request that certain landmarks be saved by a landmark manager.

    \inmodule QtLocation

    \ingroup landmarks-request
    \since 1.1

    For a QLandmarkSaveRequest, the resultsAvailable() signal will be emitted
    when either an individual items error out (individaul errors may be retrieved
    by calling errorMap()), or when an  overall operation error occurs(which may be
    retrieved by calling error()).

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.
*/

/*!
    Constructs a landmark save request with the given \a manager and \a parent.
*/
QLandmarkSaveRequest::QLandmarkSaveRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkSaveRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkSaveRequest::~QLandmarkSaveRequest()
{
}

/*!
    Returns the list of landmarks to be saved.  If called after the save operation has
    finished, any new landmarks will have had their QLandmarkId set.
    (Landmarks which were updated or failed to save will remain unchanged, use the
     errorMap() function to determine which particular landmarks failed to save.)
    \since 1.1
*/
QList<QLandmark> QLandmarkSaveRequest::landmarks() const
{
    Q_D(const QLandmarkSaveRequest);
    QMutexLocker ml(&d->mutex);
    return d->landmarks;
}

/*!
    Sets the list of \a landmarks to be saved.

    \since 1.1
    \sa setLandmark()
*/
void QLandmarkSaveRequest::setLandmarks(const QList<QLandmark> &landmarks)
{
    Q_D(QLandmarkSaveRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarks = landmarks;
}

/*!
    Convenience function to set a single \a landmark to be saved.  This
    function is the equivalent of calling setLandmarks() with a single
    \a landmark.

    \since 1.1
    \sa setLandmarks()
*/
void QLandmarkSaveRequest::setLandmark(const QLandmark &landmark)
{
    Q_D(QLandmarkSaveRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarks.clear();
    d->landmarks.append(landmark);
}

/*!
    Returns the mapping of input landmark list indices to errors which occurred.
    \since 1.1
*/
QMap<int, QLandmarkManager::Error> QLandmarkSaveRequest::errorMap() const
{
    Q_D(const QLandmarkSaveRequest);
    QMutexLocker ml(&d->mutex);
    return d->errorMap;
}

#include "moc_qlandmarksaverequest.cpp"

QTM_END_NAMESPACE


