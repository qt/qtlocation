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

#include "qdeclarativesearchmodelbase.h"
#include "qdeclarativeplace_p.h"

#include <QtDeclarative/QDeclarativeInfo>
#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceManager>
#include <QtLocation/QPlaceSearchRequest>
#include <QtLocation/QPlaceSearchReply>

QDeclarativeSearchModelBase::QDeclarativeSearchModelBase(QObject *parent)
:   QAbstractListModel(parent), m_plugin(0), m_reply(0), m_searchArea(0), m_complete(false),
    m_status(Ready)
{
}

QDeclarativeSearchModelBase::~QDeclarativeSearchModelBase()
{
}

QDeclarativeGeoServiceProvider* QDeclarativeSearchModelBase::plugin() const
{
    return m_plugin;
}

void QDeclarativeSearchModelBase::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (m_plugin == plugin)
        return;

    initializePlugin(m_plugin, plugin);

    reset(); // reset the model
    m_plugin = plugin;
    if (m_complete)
        emit pluginChanged();
    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support places.");
        return;
    }
}

QDeclarativeGeoBoundingArea *QDeclarativeSearchModelBase::searchArea() const
{
    return m_searchArea;
}

void QDeclarativeSearchModelBase::setSearchArea(QDeclarativeGeoBoundingArea *searchArea)
{
    if (m_searchArea == searchArea)
        return;

    m_searchArea = searchArea;
    emit searchAreaChanged();
}

int QDeclarativeSearchModelBase::offset() const
{
    return m_request.offset();
}

void QDeclarativeSearchModelBase::setOffset(int offset)
{
    if (m_request.offset() == offset)
        return;

    m_request.setOffset(offset);
    emit offsetChanged();
}

int QDeclarativeSearchModelBase::limit() const
{
    return m_request.limit();
}

void QDeclarativeSearchModelBase::setLimit(int limit)
{
    if (m_request.limit() == limit)
        return;

    m_request.setLimit(limit);
    emit limitChanged();
}

QDeclarativeSearchModelBase::Status QDeclarativeSearchModelBase::status() const
{
    return m_status;
}

void QDeclarativeSearchModelBase::setStatus(Status status)
{
    if (m_status == status)
        return;

    m_status = status;
    emit statusChanged();
}

void QDeclarativeSearchModelBase::execute()
{
    if (!m_plugin) {
        qmlInfo(this) << "plugin not set.";
        return;
    }

    QGeoServiceProvider *serviceProvider = m_plugin->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QPlaceManager *placeManager = serviceProvider->placeManager();
    if (!placeManager) {
        qmlInfo(this) << tr("Places not supported by %1 Plugin.").arg(m_plugin->name());
        return;
    }

    cancel();

    updateSearchRequest();
    m_reply = sendQuery(placeManager, m_request);
    if (!m_reply) {
        setStatus(Error);
        return;
    }

    m_reply->setParent(this);
    connect(m_reply, SIGNAL(finished()), this, SLOT(queryFinished()));
    connect(m_reply, SIGNAL(error(QPlaceReply::Error,QString)),
            this, SLOT(queryError(QPlaceReply::Error,QString)));

    setStatus(Executing);
}

void QDeclarativeSearchModelBase::cancel()
{
    if (!m_reply)
        return;

    QPlaceReply *reply = m_reply;

    if (!reply->isFinished())
        reply->abort();

    reply->deleteLater();
    reply = 0;
    setStatus(Ready);
}

QString QDeclarativeSearchModelBase::errorString() const
{
    return m_errorString;
}

void QDeclarativeSearchModelBase::clearData()
{
}

void QDeclarativeSearchModelBase::updateSearchRequest()
{
    if (m_searchArea)
        m_request.setSearchArea(m_searchArea->area());
}

void QDeclarativeSearchModelBase::classBegin()
{
}

void QDeclarativeSearchModelBase::componentComplete()
{
    m_complete = true;
}

void QDeclarativeSearchModelBase::initializePlugin(QDeclarativeGeoServiceProvider *oldPlugin,
                                                   QDeclarativeGeoServiceProvider *newPlugin)
{
    Q_UNUSED(oldPlugin);
    Q_UNUSED(newPlugin);
}

void QDeclarativeSearchModelBase::queryFinished()
{
    if (!m_reply)
        return;

    QPlaceReply *reply = m_reply;
    m_reply = 0;

    beginResetModel();

    clearData();

    processReply(reply);

    endResetModel();

    reply->deleteLater();
    setStatus(Ready);
}

void QDeclarativeSearchModelBase::queryError(QPlaceReply::Error error, const QString &errorString)
{
    Q_UNUSED(error)

    m_errorString = errorString;
}
