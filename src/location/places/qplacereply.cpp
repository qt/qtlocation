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

#include "qplacereply.h"
#include "qplacereply_p.h"

QT_USE_NAMESPACE

/*!
    \class QPlaceReply

    \inmodule Location

    \brief The QPlaceReply class manages an operation started by
    an instance of QPlaceManager.

    \ingroup places-main
*/

/*!
    \enum QPlaceReply::Error

    Describes an error which prevented completion of an operation.
    \value NoError
        No error has occurred
    \value DoesNotExistError
        An entity does not exist eg an import file.
    \value PlaceDoesNotExistError
        A specified place could not be found
    \value CategoryDoesNotExistError
        A specified category could not be found
    \value CommunicationError
        An error occurred communicating with the service provider.
    \value ParseError
        The response from the service provider or an import file was in an unrecognizable format
    \value PermissionsError
        The operation failed because of insufficient permissions.
    \value UnsupportedError
        The operation was not supported by the service provider.
    \value CancelError
        The operation was canceled.
    \value UnknownError
        An error occurred which does not fit into any of the other categories.
*/

/*!
    \enum QPlaceReply::Type

    Describes the reply's type.
    \value Reply
        This is a generic reply.
    \value TextPredictionReply
        Thi is the reply for a text prediction operation.
    \value ReviewReply
        This is a reply for the retrieval of place reviews.
    \value ContentReply
        This is a reply for the retrieval of place content.
    \value PlaceDetailsReply
        This is a reply for the retrieval of place details.
    \value PlaceSearchReply
        This is a reply for a place search operation.
*/

/*!
    Constructs a reply object with a given \a parent.
*/
QPlaceReply::QPlaceReply(QObject *parent)
    : QObject(parent),d_ptr(new QPlaceReplyPrivate)
{
}

QPlaceReply::QPlaceReply(QPlaceReplyPrivate *dd, QObject *parent)
    : QObject(parent),d_ptr(dd)
{
}

/*!
    Destructor.
*/
QPlaceReply::~QPlaceReply()
{
    if (!isFinished()) {
        abort();
    }
    delete d_ptr;
}

/*!
    Return true if the reply has completed.
*/
bool QPlaceReply::isFinished() const
{
    return d_ptr->isFinished;
}

/*!
    Return type of operation.
*/
QPlaceReply::Type QPlaceReply::type() const
{
    return QPlaceReply::Reply;
}

/*!
    Sets the status of whether the reply is \a finished
    or not.  This function does not cause the finished() signal
    to be emitted.
*/
void QPlaceReply::setFinished(bool finished)
{
    d_ptr->isFinished = finished;
}

/*!
    Sets the \a error and \a errorString of the reply.
    This function does not cause the error(QPlaceReply::Error, const QString &errorString)
    signal to be emitted.
*/
void QPlaceReply::setError(QPlaceReply::Error error, const QString &errorString)
{
    d_ptr->error = error;
    d_ptr->errorString = errorString;
}

/*!
    Returns the error string.
*/
QString QPlaceReply::errorString() const
{
    return d_ptr->errorString;
}

/*!
    Returns error code.
*/
QPlaceReply::Error QPlaceReply::error() const
{
    return d_ptr->error;
}

/*!
    Aborts the operation.
*/
void QPlaceReply::abort()
{
}

/*!
    \fn void QPlaceReply::finished()

    This signal is emitted when this reply has finished processing.

    If error() equals QPlaceReply::NoError then the processing
    finished successfully.

    This signal and QPlaceManager::finished() will be
    emitted at the same time.

    \note Do no delete this reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*!
    \fn void QPlaceReply::error(QPlaceReply::Error error, const QString &errorString)

    This signal is emitted when an error has been detected in the processing of
    this reply. The finished() signal will probably follow.

    The error will be described by the error code \a error. If \a errorString is
    not empty it will contain a textual description of the error meant for
    developers and not end users.

    This signal and QPlaceManager::error() will be emitted at the same time.

    \note Do no delete this reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/
