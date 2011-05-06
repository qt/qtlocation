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

#include "qlandmarkremoverequest.h"
#include "qlandmarkrequests_p.h"
#include <QMap>

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkRemoveRequest
    \brief The QLandmarkRemoveRequest class allows a client to asynchronously
    request that certain landmarks be removed from a landmark manager.

    For a QLandmarkRemoveRequest, the resultsAvailable() signal will be emitted
    when either an individual items error out (individaul errors may be retrieved
    by calling errorMap()), or when an overall operation error occurs(which may be
    retrieved by calling error()).

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    \inmodule QtLocation

    \ingroup landmarks-request
    \since 1.1
*/

/*!
    Constructs a landmark remove request with the given \a manager and \a parent.
*/
QLandmarkRemoveRequest::QLandmarkRemoveRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkRemoveRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkRemoveRequest::~QLandmarkRemoveRequest()
{
}

/*!
    Returns the list of identifiers of landmarks which will be removed.
*/
QList<QLandmarkId> QLandmarkRemoveRequest::landmarkIds() const
{
    Q_D(const QLandmarkRemoveRequest);
    QMutexLocker ml(&d->mutex);
    return d->landmarkIds;
}

/*!
    Sets the list of \a landmarkIds of landmarks which will be removed.

    \sa setLandmarkId()
*/
void QLandmarkRemoveRequest::setLandmarkIds(const QList<QLandmarkId> &landmarkIds)
{
    Q_D(QLandmarkRemoveRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarkIds = landmarkIds;
}

/*!
    Convenience function to set the \a landmarkId of a single landmark to be removed.
    It is the equivalent of calling setLandmarkIds() with a single \a landmarkId
    in the ID list.

    \sa setLandmarkIds()
*/
void QLandmarkRemoveRequest::setLandmarkId(const QLandmarkId &landmarkId)
{
    Q_D(QLandmarkRemoveRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarkIds.clear();
    d->landmarkIds.append((landmarkId));
}

/*!
    Convenience function to set the list of \a landmarks which will be removed.
    This function is the equivalent of calling setLandmarkIds() with the IDs of
    the \a landmarks.

    \sa setLandmarkIds()
*/
void QLandmarkRemoveRequest::setLandmarks(const QList<QLandmark> &landmarks)
{
    Q_D(QLandmarkRemoveRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarkIds.clear();
    for (int i=0; i < landmarks.count(); ++i)
        d->landmarkIds.append(landmarks.at(i).landmarkId());
}

/*!
    Convenience function that sets a single \a landmark to be removed.
    This function is the equivalent of calling setLandmarkIds()
    with the ID of \a landmark.
*/
void QLandmarkRemoveRequest::setLandmark(const QLandmark &landmark)
{
    Q_D(QLandmarkRemoveRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarkIds.clear();
    d->landmarkIds.append(landmark.landmarkId());
}

/*!
    Returns the mapping of input landmark ID list indices
    to the errors which occurred.
*/
QMap<int, QLandmarkManager::Error> QLandmarkRemoveRequest::errorMap() const
{
    Q_D(const QLandmarkRemoveRequest);
    QMutexLocker ml(&d->mutex);
    return d->errorMap;
}

#include "moc_qlandmarkremoverequest.cpp"

QTM_END_NAMESPACE


