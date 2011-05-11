/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativegeosearchmodel_p.h"

#include "qdeclarativegeoplace_p.h"
#include "qdeclarativelandmark_p.h"

#include <qgeoserviceprovider.h>
#include <qgeosearchmanager.h>

QTM_BEGIN_NAMESPACE

QDeclarativeGeoSearchModel::QDeclarativeGeoSearchModel(QObject* parent)
    : QAbstractListModel(parent),
      plugin_(0),
      serviceProvider_(0),
      searchManager_(0),
      status_(QDeclarativeGeoSearchModel::Null)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(PlaceRole, "place");
    roleNames.insert(LandmarkRole, "landmark");
    setRoleNames(roleNames);
}

QDeclarativeGeoSearchModel::~QDeclarativeGeoSearchModel()
{
    if (serviceProvider_)
        delete serviceProvider_;
}

// From QDeclarativeParserStatus
void QDeclarativeGeoSearchModel::classBegin() {}
void QDeclarativeGeoSearchModel::componentComplete() {}

// From QAbstractListModel
int QDeclarativeGeoSearchModel::rowCount(const QModelIndex &parent) const
{
    return places_.count();
}

QVariant QDeclarativeGeoSearchModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > places_.count())
        return QVariant();

    QGeoPlace place = places_.at(index.row());

    if (role == QDeclarativeGeoSearchModel::PlaceRole) {
        return QVariant::fromValue(new QDeclarativeGeoPlace(place, const_cast<QDeclarativeGeoSearchModel*>(this)));
    } else if (role == QDeclarativeGeoSearchModel::LandmarkRole) {
        if (place.isLandmark())
            return QVariant::fromValue(QLandmark(place));
    }

    return QVariant();
}

QVariant QDeclarativeGeoSearchModel::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if (section != 0)
        return QVariant();

    if (role == QDeclarativeGeoSearchModel::PlaceRole) {
        return QString("Place");
    } else if (role == QDeclarativeGeoSearchModel::LandmarkRole) {
        return QString("Landmark");
    }

    return QVariant();
}

void QDeclarativeGeoSearchModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (plugin_ || !plugin)
        return;

    plugin_ = plugin;

    emit pluginChanged(plugin_);

    serviceProvider_ = new QGeoServiceProvider(plugin_->name(),
                                               plugin_->parameterMap());

    // check for error

    searchManager_ = serviceProvider_->searchManager();

    connect(searchManager_,
            SIGNAL(finished(QGeoSearchReply*)),
            this,
            SLOT(searchFinished(QGeoSearchReply*)));

    connect(searchManager_,
            SIGNAL(error(QGeoSearchReply*, QGeoSearchReply::Error, QString)),
            this,
            SLOT(searchError(QGeoSearchReply*, QGeoSearchReply::Error, QString)));
}

QDeclarativeGeoServiceProvider* QDeclarativeGeoSearchModel::plugin() const
{
    return plugin_;
}

void QDeclarativeGeoSearchModel::searchFinished(QGeoSearchReply *reply)
{
    if (reply->error() != QGeoSearchReply::NoError)
        return;

    setPlaces(reply->places());

    setError("");
    setStatus(QDeclarativeGeoSearchModel::Ready);

    reply->deleteLater();

    emit placesChanged();
}

void QDeclarativeGeoSearchModel::searchError(QGeoSearchReply *reply,
        QGeoSearchReply::Error error,
        const QString &errorString)
{
    setError(errorString);
    setStatus(QDeclarativeGeoSearchModel::Error);
    reply->deleteLater();
}

QDeclarativeGeoSearchModel::Status QDeclarativeGeoSearchModel::status() const
{
    return status_;
}

void QDeclarativeGeoSearchModel::setStatus(QDeclarativeGeoSearchModel::Status status)
{
    if (status_ == status)
        return;

    status_ = status;

    emit statusChanged(status_);
}

QString QDeclarativeGeoSearchModel::error() const
{
    return error_;
}

void QDeclarativeGeoSearchModel::setError(const QString &error)
{
    if (error_ == error)
        return;

    error_ = error;

    emit errorChanged(error_);
}

QGeoSearchManager* QDeclarativeGeoSearchModel::searchManager()
{
    return searchManager_;
}

QList<QGeoPlace> QDeclarativeGeoSearchModel::places() const
{
    return places_;
}

void QDeclarativeGeoSearchModel::setPlaces(const QList<QGeoPlace> &places)
{
    beginResetModel();
    places_ = places;
    endResetModel();
}

#include "moc_qdeclarativegeosearchmodel_p.cpp"

QTM_END_NAMESPACE
