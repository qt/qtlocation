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

#include "qlandmarkimportrequest.h"
#include "qlandmarkrequests_p.h"
#include <QFile>

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkImportRequest
    \brief The QLandmarkImportRequest class allows a client to asynchronously
    request that a landmark manager import a set of landmarks.

    For a QLandmarkImportRequest, the resultsAvailable() signal will be emitted
    when landmarks have been successfully imported by the manager.  The list
    of added landmarks may be retrieved by the landmarkIds() function.  The resultsAvailable()
    signal may also be emitted if an overall operational error occurs.

    Please see the class documentation for QLandmarkAbstractRequest for more information about
    the usage of request classes and ownership semantics.

    Note that for S60 and Symbian platforms, the waitForFinished() function is not
    supported for import requests and will always return false.

    \inmodule QtLocation
    \since 1.1

    \ingroup landmarks-request
*/

/*!
    Constructs a landmark import request with the given \a manager \a parent.
*/
QLandmarkImportRequest::QLandmarkImportRequest(QLandmarkManager *manager, QObject *parent)
    : QLandmarkAbstractRequest(new QLandmarkImportRequestPrivate(manager), parent)
{
}

/*!
    Destroys the request object.
*/
QLandmarkImportRequest::~QLandmarkImportRequest()
{
}

/*!
    Returns the device from which landmarks are read.
*/
QIODevice *QLandmarkImportRequest::device() const
{
    Q_D(const QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    return d->device;
}

/*!
    Sets the \a device from which landmarks are read.
*/
void QLandmarkImportRequest::setDevice(QIODevice *device)
{
    Q_D(QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    d->device = device;
}

/*!
    If the currently assigned device is a QFile, or if setFileName() has
    been called, this function returns the name of the file to be
    read from.  In all other cases, it returns an empty string.

    \sa setFileName(), setDevice()
*/
QString QLandmarkImportRequest::fileName() const
{
    Q_D(const QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    QFile *file = qobject_cast<QFile *>(d->device);
    return file ? file->fileName() : QString();
}

/*!
    Sets the name of the file to be read from to \a fileName.  Internally,
    QLandmarkImportRequest will create a QFile and use this as the
    device to import from.

    \sa fileName(), setDevice()
*/
void QLandmarkImportRequest::setFileName(const QString &fileName)
{
    Q_D(QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    d->device = new QFile(fileName);
}

/*!
    Returns the expected data format for the import operation.
    By default no format is set, indicating that the manager
    will try to automatically detect the format of the import file.
*/
QString QLandmarkImportRequest::format() const
{
    Q_D(const QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    return d->format;
}

/*!
    Sets the expected data \a format for the import operation.
    If an empty string is set, then during the import,
    an attempt is made to auto detect the format.
*/
void QLandmarkImportRequest::setFormat(const QString &format)
{
    Q_D(QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    d->format = format;
}

/*!
    Returns the transfer option of the import request.
    The transfer option defines how categories are treated
    during the import operation.  The default option is
    QLandmarkManager::IncludeCategoryData.

*/
QLandmarkManager::TransferOption QLandmarkImportRequest::transferOption() const
{
    Q_D(const QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    return d->option;
}

/*!
    Sets the transfer \a option of the import request.
    The transfer \a option defines how categories are treated
    during the import operation.
*/
void QLandmarkImportRequest::setTransferOption(QLandmarkManager::TransferOption option)
{
    Q_D(QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    d->option = option;
}

/*!
    Returns identifier of the category which all landmarks are assigned to if
    QLandmarkManager::AttachSingleCategory is used
    as the import option.
*/
QLandmarkCategoryId QLandmarkImportRequest::categoryId() const
{
    Q_D(const QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    return d->categoryId;
}

/*!
    Sets the identifier of the category to which all landmarks are assigned to,
    if QLandmarkManager::AttachSingleCategory is used as the import option, to
    \a categoryId.
*/
void QLandmarkImportRequest::setCategoryId(const QLandmarkCategoryId &categoryId)
{
    Q_D(QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    d->categoryId = categoryId;
}

/*!
    Returns a list of identifiers of landmarks that have been imported.
*/
QList<QLandmarkId> QLandmarkImportRequest::landmarkIds() const
{
    Q_D(const QLandmarkImportRequest);
    QMutexLocker ml(&d->mutex);
    return d->landmarkIds;
}

#include "moc_qlandmarkimportrequest.cpp"

QTM_END_NAMESPACE


