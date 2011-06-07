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

#include "qlandmarkexportrequest.h"
#include "qlandmarkrequests_p.h"
#include <QIODevice>
#include <QFile>


QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkExportRequest
    \brief The QLandmarkExportRequest class allows a client to asynchronously
    request that a landmark manager export a set of landmarks.

    For a QLandmarkExportRequest, the QLandmarkAbstractRequest::stateChanged()
    signal will be emitted when the request is complete.  The resultsAvailable() signal
    will be emitted if an overall operational error occurs.

    If no landmarks identifiers are set using setLandmarkIds() or
    an empty list is passed in, then all the landmarks will be exported.

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    \inmodule QtLocation

    \ingroup landmarks-request
    \since 1.1
*/

/*!
    Constructs a landmark export request with the given \a manager and \a parent.
*/
QLandmarkExportRequest::QLandmarkExportRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkExportRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkExportRequest::~QLandmarkExportRequest()
{
}

/*!
    Returns the device to which landmarks are written.
    \since 1.1
*/
QIODevice *QLandmarkExportRequest::device() const
{
    Q_D(const QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    return d->device;
}

/*!
    Sets the \a device to which landmarks are written.
    \since 1.1
*/
void QLandmarkExportRequest::setDevice(QIODevice *device)
{
    Q_D(QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    d->device = device;
}

/*!
    If the currently assigned device is a QFile, or if setFileName() has
    been called, this function returns the name of the file to be
    written to.  In all other cases, it returns an empty string.

    \since 1.1
    \sa setFileName(), setDevice()
*/
QString QLandmarkExportRequest::fileName() const
{
    Q_D(const QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    QFile *file = qobject_cast<QFile *>(d->device);
    return file ? file->fileName() : QString();
}

/*!
    Sets the name of the file to be written to \a fileName.  Internally,
    QLandmarkExportRequest will create a QFile use this as the
    device to export to.

    \since 1.1
    \sa fileName(), setDevice()
*/
void QLandmarkExportRequest::setFileName(const QString &fileName)
{
    Q_D(QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    d->device = new QFile(fileName);
}

/*!
    Returns the data format for the export operation.
    \since 1.1
*/
QString QLandmarkExportRequest::format() const
{
    Q_D(const QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    return d->format;
}

/*!
    Sets the data \a format for the export operation.
    \since 1.1
*/
void QLandmarkExportRequest::setFormat(const QString &format)
{
    Q_D(QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    d->format = format;
}

/*!
    Returns the transfer option of the export request.
    The transfer option defines how categories are treated
    during the export operation.  The default option is
    QLandmarkManager::IncludeCategoryData.

    \since 1.1
*/
QLandmarkManager::TransferOption QLandmarkExportRequest::transferOption() const
{
    Q_D(const QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    return d->option;
}

/*!
    Sets the transfer \a option of the export request.
    The transfer \a option defines how categories are treated
    during the export operation.
    \since 1.1
*/
void QLandmarkExportRequest::setTransferOption(QLandmarkManager::TransferOption option)
{
    Q_D(QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    d->option = option;
}

/*!
    Returns the list of identifiers of landmarks that will be exported.
    By default the list of identifiers is empty, indicating that all
    landmarks should be exported.
    \since 1.1
*/
QList<QLandmarkId> QLandmarkExportRequest::landmarkIds() const
{
    Q_D(const QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    return d->landmarkIds;
}

/*!
    Sets the landmarks to be exported by specifying a list of
    \a landmarkIds.
    \since 1.1
*/
void QLandmarkExportRequest::setLandmarkIds(const QList<QLandmarkId> &landmarkIds)
{
    Q_D(QLandmarkExportRequest);
    QMutexLocker ml(&d->mutex);
    d->landmarkIds = landmarkIds;
}

#include "moc_qlandmarkexportrequest.cpp"

QTM_END_NAMESPACE


