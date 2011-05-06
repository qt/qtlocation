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
#include "qlandmarkabstractrequest.h"
#include "qlandmarkabstractrequest_p.h"
#include "qlandmarkmanagerengine.h"
#include "qlandmarkmanager_p.h"
#include <QDebug>
#include <QMutexLocker>

QTM_USE_NAMESPACE

QLandmarkAbstractRequestPrivate::QLandmarkAbstractRequestPrivate(QLandmarkManager *mgr)
    : type(QLandmarkAbstractRequest::InvalidRequest),
      state(QLandmarkAbstractRequest::InactiveState),
      error(QLandmarkManager::NoError),
      errorString(QString()),
      manager(mgr)
{
}

/*!
    \class QLandmarkAbstractRequest
    \brief The QLandmarkAbstractRequest class provides the interface
    from which all asynchronous request classes inherit.


    \inmodule QtLocation
    \since 1.1

    \ingroup landmarks-request

    It allows a client to asynchronously request some functionality of a
    particular QLandmarkManager. Instances of the class will emit signals when
    the state of the request changes, or when more results become available.

    Clients should not attempt to create instances of this class directly, but
    should instead use the use-case-specific classes derived from this class.

    All such request classes have a similar interface: clients set the
    parameters of the asynchronous call, then call the start() slot of the request. The manager
    will then enqueue or begin to process the request, at which point the
    request's state will transition from the \c InactiveState to \c ActiveState.
    After any state transition, the request will emit the stateChanged()
    signal. The manager may (if it supports it)
    periodically update the request with results, at
    which point the request will emit the resultsAvailable() signal. These
    results are not guaranteed to have a stable ordering. Error information is
    considered a result, so some requests will emit the resultsAvailable()
    signal even if no results are possible from the request (for example, a
    landmark remove request) when the manager updates the request with
    information about any errors which may have occurred.

    Clients can choose which signals they wish to handle from a request. If the
    client is not interested in interim results, they can choose to handle only
    the stateChanged() signal, and in the slot to which that signal is
    connected, check whether the state has changed to the \c FinishedState
    (which signifies that the manager has finished handling the request, and
    that the request will not be updated with any more results).  If the client
    is not interested in any results (including error information), they may
    choose to delete the request after calling \l start(), or simply may not
    connect the the request's signals to any slots.  (Please see the note
    below if you are working on Symbian with QtMobility 1.1.0)

    If the request is allocated via operator new, the client must delete the
    request when they are no longer using it in order to avoid leaking memory.
    That is, the client retains ownership of the request.

    The client may delete a heap-allocated request in various ways: by deleting
    it directly (but not within a slot connected to a signal emitted by the
    request), or by using the deleteLater() slot to schedule the request for
    deletion when control returns to the event loop (from within a slot
    connected to a signal emitted by the request, for example \l
    stateChanged()).

    An active request may be deleted by the client, but the client will
    not receive notifications about whether the request succeeded or not,
    nor the results of the request.

    Because clients retain ownership of any request object, and may delete a
    request object at any time, the manager engine,  implementers must be
    careful to ensue that they do not assume that a request has not been
    deleted at some time point during processing of a request, particularly
    if the engine has a multithreaded implementation.
*/

QLandmarkAbstractRequestPrivate::~QLandmarkAbstractRequestPrivate()
{
}

void QLandmarkAbstractRequestPrivate::notifyEngine(QLandmarkAbstractRequest* request)
{
        Q_ASSERT(request);
        QLandmarkAbstractRequestPrivate* d = request->d_ptr;
        if (d) {
            QMutexLocker ml(&d->mutex);
            QLandmarkManagerEngine *engine = QLandmarkManagerPrivate::getEngine(d->manager);
            ml.unlock();
            if (engine) {
                engine->requestDestroyed(request);
            }
        }
}

/*!
    \enum QLandmarkAbstractRequest::RequestType
    Defines the possible types of asynchronous requests.
    \value  InvalidRequest An invalid request
    \value  LandmarkIdFetchRequest A request to fetch a list of landmark
            identifiers.
    \value  CategoryIdFetchRequest A request to fetch a list of catgory
            identifiers.

    \value  LandmarkFetchRequest A request to fetch a list of landmarks
    \value  LandmarkFetchByIdRequest A request to fetch a list of landmarks by id.
    \value  CategoryFetchRequest A request to fetch a list of categories
    \value  CategoryFetchByIdRequest A request to fetch a list of categories by id
    \value  LandmarkSaveRequest A request to save a list of landmarks.
    \value  LandmarkRemoveRequest A request to remove a list of landmarks.
    \value  CategorySaveRequest A request to save a list of categories.
    \value  CategoryRemoveRequest A request to remove a list of categories.
    \value  ImportRequest A request import landmarks.
    \value  ExportRequest A request export landmarks.
*/

/*!
    \enum QLandmarkAbstractRequest::State
    Defines the possible states of asynchronous requests.
    \value  InactiveState Operation not yet started.
    \value  ActiveState Operation started, not yet finished.
    \value  FinishedState Operation completed.  (Can be mean either successful or
            unsuccessful completion).
*/

/*!
    Constructs a new, invalid asynchronous request with the given \a manager and \a parent.
*/
QLandmarkAbstractRequest::QLandmarkAbstractRequest(QLandmarkManager *manager, QObject *parent)
    : QObject(parent),
      d_ptr(new QLandmarkAbstractRequestPrivate(manager))
{
}

/*!
    \internal
*/
QLandmarkAbstractRequest::QLandmarkAbstractRequest(QLandmarkAbstractRequestPrivate *dd, QObject *parent)
    : QObject(parent),
     d_ptr(dd)
{
}

/*!
    Destroys the asynchronous request.  Because the request object is effectiely a handle to a
    request operation, the operation may continue or it may just be canceled, depending upon
    the enine implementation, even though the request itself has been destroyed.
    The sqlite engine continues the operation behind the scenes if the
    request is destroyed whilst active.  For the symbian engine see the note below.
*/
QLandmarkAbstractRequest::~QLandmarkAbstractRequest()
{
    QLandmarkAbstractRequestPrivate::notifyEngine(this);
    delete d_ptr;
}

/*!
    Returns the type of this asynchronous request.
*/
QLandmarkAbstractRequest::RequestType QLandmarkAbstractRequest::type() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->type;
}

/*!
    Returns the state of the request
*/
QLandmarkAbstractRequest::State QLandmarkAbstractRequest::state()
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->state;
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Inactive state;
    otherwise, returns false.
    \sa state()
*/
bool QLandmarkAbstractRequest::isInactive() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->state == QLandmarkAbstractRequest::InactiveState;
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Active state;
    otherwise, returns false.
    \sa state()
*/
bool QLandmarkAbstractRequest::isActive() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->state == QLandmarkAbstractRequest::ActiveState;
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Finished state;
    otherwise, returns false.
    \sa state()
*/
bool QLandmarkAbstractRequest::isFinished() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->state == QLandmarkAbstractRequest::FinishedState;
}

/*!
    Returns the overall error of the most recent asynchronous operation.
    \sa errorString()
*/
QLandmarkManager::Error QLandmarkAbstractRequest::error() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->error;
}

/*!
    Returns a human readable string of the last error
    that occurred.  This error string is intended to be used
    by developers only and should not be seen by end users.
    \sa error()
*/
QString QLandmarkAbstractRequest::errorString() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->errorString;
}

/*!
    Returns a pointer to the landmark manager which
    this request operates on.
*/
QLandmarkManager *QLandmarkAbstractRequest::manager() const
{
    QMutexLocker ml(&d_ptr->mutex);
    return d_ptr->manager;
}

/*!
    Sets the \a manager which this request operates on.

    Note that if a NULL manager is set, the functions
    start(), cancel() and waitForFinished() will return false and
    error will be set to QLandmarkManager::InvalidManagerError.

    A manager cannot be assigned while the request is in the
    QLandmarkAbstractRequest::ActiveState.
*/
void QLandmarkAbstractRequest::setManager(QLandmarkManager *manager)
{
    QMutexLocker ml(&d_ptr->mutex);
    if (d_ptr->state == QLandmarkAbstractRequest::ActiveState && d_ptr->manager)
        return;
    d_ptr->manager = manager;
}

/*!
    Attempts to start the request.

    Returns true if the request was started, otherwise false. Trying to start a
    request that is already active returns false.
    \sa cancel().
*/
bool QLandmarkAbstractRequest::start()
{
    QMutexLocker ml(&d_ptr->mutex);
    if (!d_ptr->manager) {
        d_ptr->error = QLandmarkManager::BadArgumentError;
        d_ptr->errorString = "No manager assigned to landmark request object";
        qWarning() << d_ptr->errorString;
        return false;
    }
    QLandmarkManagerEngine *engine = d_ptr->manager->engine();
    if (!engine) {
        d_ptr->error = QLandmarkManager::InvalidManagerError;
        d_ptr->errorString = "The manager is invalid";
        return false;
    }

    if (d_ptr->state != QLandmarkAbstractRequest::ActiveState) {
        ml.unlock();
        return engine->startRequest(this);
    }
     else {
        return false;
     }
}

/*!
    Notifies the request that it should be canceled.

    Returns true if the request was successfully notified
    that it should be canceled.  The request may or may not honor
    the cancel notification.  Returns false if the notification
    could not be made or the request is not in the
    QLandmarkManager::Active state.

    \sa start()
*/
bool QLandmarkAbstractRequest::cancel()
{
    QMutexLocker ml(&d_ptr->mutex);
    if (!d_ptr->manager) {
        d_ptr->error = QLandmarkManager::BadArgumentError;
        d_ptr->errorString = "No manager assigned to landmark request object";
        qWarning() << d_ptr->errorString;
        return false;
    }
    QLandmarkManagerEngine *engine = d_ptr->manager->engine();

    if(d_ptr->state == QLandmarkAbstractRequest::ActiveState) {
        ml.unlock();
        return engine->cancelRequest(this);
    }
    else
        return false;
}

/*!
    Blocks until the request has been completed or until \a msecs milliseconds
    has elapsed.  If \a msecs is zero or negative, this function will block indefinitely.

    Returns true if the request was canceled or completed
    within the given period, otherwise returns false.  Some backends may be unable
    to support this  operation safely and will return false immediately.

    (Note: This function is not supported for an import request
    with the symbian manager which always returns false.  As of
    Qt Mobility 1.1.1 waitForFinished() is supported using the sqlite manager).

    Note that any signals generated while waiting for the request to be complete
    may be queued and delivered sometime after this function has returned, when
    the calling thread's event loop is dispatched.  If your code depends on
    your slots being invoked, you may need to process events after calling
    this function.
*/
bool QLandmarkAbstractRequest::waitForFinished(int msecs)
{

    QMutexLocker ml(&d_ptr->mutex);
    if (!d_ptr->manager) {
        d_ptr->error = QLandmarkManager::BadArgumentError;
        d_ptr->errorString = "No manager assigned to landmark request object";
        qWarning() << d_ptr->errorString;
        return false;
    }
    QLandmarkManagerEngine *engine = d_ptr->manager->engine();

    switch(d_ptr->state) {
        case QLandmarkAbstractRequest::ActiveState:
            ml.unlock();
            return engine->waitForRequestFinished(this, msecs);
        case QLandmarkAbstractRequest::FinishedState:
            return true;
        default:
            return false;
    }
}

/*!
  \fn void QLandmarkAbstractRequest::resultsAvailable()
  This signal is emitted when new results are available.  Results
  can include the operation error which may be accessed via error(),
  or derived-class specific results which are accessible through
  the derived class API.

  \sa error()
*/

/*!
    \fn void QLandmarkAbstractRequest::stateChanged(QLandmarkAbstractRequest::State newState)
    This signal is emitted when the state of the request is changed.  The new state of
    the request will be contained in \a newState.
*/

#include "moc_qlandmarkabstractrequest.cpp"





