/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativegeocodemodel_p.h"

#include "qdeclarativegeolocation_p.h"
#include <QtDeclarative/qdeclarativeinfo.h>

#include <qgeoserviceprovider.h>
#include <qgeocodingmanager.h>

QT_BEGIN_NAMESPACE

QDeclarativeGeocodeModel::QDeclarativeGeocodeModel(QObject* parent)
    : QAbstractListModel(parent),
      autoUpdate_(false),
      complete_(false),
      reply_(0),
      plugin_(0),
      boundingArea_(0),
      status_(QDeclarativeGeocodeModel::Null),
      coordinate_(0),
      address_(0)
{
    QHash<int, QByteArray> roleNames;
    roleNames = QAbstractItemModel::roleNames();
    roleNames.insert(LocationRole, "location");
    setRoleNames(roleNames);
}

QDeclarativeGeocodeModel::~QDeclarativeGeocodeModel()
{
    qDeleteAll(declarativeLocations_);
    declarativeLocations_.clear();
    delete reply_;
}

// From QDeclarativeParserStatus
void QDeclarativeGeocodeModel::componentComplete()
{
    complete_ = true;
    if (autoUpdate_)
        update();
}

QGeoBoundingArea* QDeclarativeGeocodeModel::boundingArea()
{
    if (qobject_cast<QDeclarativeGeoBoundingBox*>(boundingArea_) && boundingBox_.isValid()) {
        return &boundingBox_;
    } else if (qobject_cast<QDeclarativeGeoBoundingCircle*>(boundingArea_) && boundingCircle_.isValid()) {
        return &boundingCircle_;
    }
    return 0;
}

void QDeclarativeGeocodeModel::update()
{
    if (!complete_)
        return;

    if (!plugin_) {
        qmlInfo(this) << tr("Cannot geocode, plugin not set.");
        return;
    }

    QGeoServiceProvider *serviceProvider = plugin_->sharedGeoServiceProvider();
    if (!serviceProvider)
        return;

    QGeocodingManager *geocodingManager = serviceProvider->geocodingManager();
    if (!geocodingManager) {
        qmlInfo(this) << tr("Cannot geocode, search manager (/plugin) not set.");
        return;
    }
    if ((!coordinate_ || !coordinate_->coordinate().isValid()) &&
            (!address_ || address_->address().isEmpty())) {
        qmlInfo(this) << tr("Cannot geocode, valid query not set.");
        return;
    }
    abortRequest(); // abort possible previous requests
    setError("");   // clear previous error string

    if (coordinate_) {
        setStatus(QDeclarativeGeocodeModel::Loading);
        reply_ = geocodingManager->reverseGeocode(coordinate_->coordinate(), boundingArea());
        if (reply_->isFinished()) {
            if (reply_->error() == QGeocodeReply::NoError) {
                geocodeFinished(reply_);
            } else {
                geocodeError(reply_, reply_->error(), reply_->errorString());
            }
        }
    } else if (address_) {
        setStatus(QDeclarativeGeocodeModel::Loading);
        reply_ = geocodingManager->geocode(address_->address(), boundingArea());
        if (reply_->isFinished()) {
            if (reply_->error() == QGeocodeReply::NoError) {
                geocodeFinished(reply_);
            } else {
                geocodeError(reply_, reply_->error(), reply_->errorString());
            }
        }
    }
}

void QDeclarativeGeocodeModel::abortRequest()
{
    if (reply_) {
        reply_->abort();
        reply_->deleteLater();
        reply_ = 0;
    }
}

void QDeclarativeGeocodeModel::queryContentChanged()
{
    if (autoUpdate_)
        update();
}

// From QAbstractListModel
int QDeclarativeGeocodeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return declarativeLocations_.count();
}

QVariant QDeclarativeGeocodeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= declarativeLocations_.count())
        return QVariant();
    if (role == QDeclarativeGeocodeModel::LocationRole) {
        QObject* locationObject = declarativeLocations_.at(index.row());
        Q_ASSERT(locationObject);
        return QVariant::fromValue(locationObject);
    }
    return QVariant();
}

void QDeclarativeGeocodeModel::setPlugin(QDeclarativeGeoServiceProvider *plugin)
{
    if (plugin_ == plugin)
        return;
    reset(); // reset the model
    plugin_ = plugin;
    if (complete_)
        emit pluginChanged();
    QGeoServiceProvider *serviceProvider = plugin_->sharedGeoServiceProvider();
    QGeocodingManager *geocodingManager = serviceProvider->geocodingManager();
    if (!geocodingManager || serviceProvider->error() != QGeoServiceProvider::NoError) {
        qmlInfo(this) << tr("Warning: Plugin does not support (reverse) geocoding.");
        return;
    }
    connect(geocodingManager, SIGNAL(finished(QGeocodeReply*)),
            this, SLOT(geocodeFinished(QGeocodeReply*)));
    connect(geocodingManager, SIGNAL(error(QGeocodeReply*,QGeocodeReply::Error,QString)),
            this, SLOT(geocodeError(QGeocodeReply*,QGeocodeReply::Error,QString)));
}

QDeclarativeGeoServiceProvider* QDeclarativeGeocodeModel::plugin() const
{
    return plugin_;
}

void QDeclarativeGeocodeModel::setBounds(QObject* bounds)
{
    if (boundingArea_ == bounds)
            return;
    if (qobject_cast<QDeclarativeGeoBoundingBox*>(bounds)) {
        boundingBox_ = qobject_cast<QDeclarativeGeoBoundingBox*>(bounds)->box();
    } else if (qobject_cast<QDeclarativeGeoBoundingCircle*>(bounds)) {
        boundingCircle_ = qobject_cast<QDeclarativeGeoBoundingCircle*>(bounds)->circle();
    } else {
        qmlInfo(this) << tr("Unsupported bound type (Box and Circle supported)");
        return;
    }
    boundingArea_ = bounds;
    emit boundsChanged();
}

QObject* QDeclarativeGeocodeModel::bounds() const
{
    return boundingArea_;
}

void QDeclarativeGeocodeModel::geocodeFinished(QGeocodeReply *reply)
{
    if (reply->error() != QGeocodeReply::NoError) {
        return;
    }
    int oldCount = declarativeLocations_.count();
    setLocations(reply->locations());
    setError("");
    setStatus(QDeclarativeGeocodeModel::Ready);
    reply->deleteLater();
    reply_ = 0;
    emit locationsChanged();
    if (oldCount != declarativeLocations_.count())
        emit countChanged();
}

void QDeclarativeGeocodeModel::geocodeError(QGeocodeReply *reply,
        QGeocodeReply::Error error,
        const QString &errorString)
{
    Q_UNUSED(error);
    setError(errorString);
    setStatus(QDeclarativeGeocodeModel::Error);
    reply->deleteLater();
    reply_ = 0;
}

QDeclarativeGeocodeModel::Status QDeclarativeGeocodeModel::status() const
{
    return status_;
}

void QDeclarativeGeocodeModel::setStatus(QDeclarativeGeocodeModel::Status status)
{
    if (status_ == status)
        return;
    status_ = status;
    emit statusChanged();
}

QString QDeclarativeGeocodeModel::error() const
{
    return error_;
}

void QDeclarativeGeocodeModel::setError(const QString &error)
{    
    if (error_ == error)
        return;
    error_ = error;
    emit errorChanged();
}

void QDeclarativeGeocodeModel::setLocations(const QList<QGeoLocation> &locations)
{
    beginResetModel();
    qDeleteAll(declarativeLocations_);
    declarativeLocations_.clear();
    for (int i = 0;  i < locations.count(); ++i) {
        QDeclarativeGeoLocation* location = new QDeclarativeGeoLocation(locations.at(i), this);
        declarativeLocations_.append(location);
    }
    endResetModel();
}

int QDeclarativeGeocodeModel::count() const
{
    return declarativeLocations_.count();
}

Q_INVOKABLE QDeclarativeGeoLocation* QDeclarativeGeocodeModel::get(int index)
{
    if (index < 0 || index >= declarativeLocations_.count()) {
        qmlInfo(this) << tr("Error, too big or small index in get(): ") << index;
        return 0;
    }
    return declarativeLocations_.at(index);
}

Q_INVOKABLE void QDeclarativeGeocodeModel::clear()
{
    bool hasChanged = !declarativeLocations_.isEmpty();
    setLocations(QList<QGeoLocation>());
    if (hasChanged)
        emit countChanged();
}

Q_INVOKABLE void QDeclarativeGeocodeModel::reset()
{
    clear();
    abortRequest();
    setError("");
    setStatus(QDeclarativeGeocodeModel::Null);
}

QVariant QDeclarativeGeocodeModel::query() const
{
    return queryVariant_;
}

void QDeclarativeGeocodeModel::setQuery(const QVariant& query)
{
    if (query == queryVariant_)
        return;
    QObject* object = qvariant_cast<QObject*>(query);
    if (qobject_cast<QDeclarativeCoordinate*>(object)) {
        if (coordinate_)
            coordinate_->disconnect(this);
        if (address_)
            address_->disconnect(this);
        coordinate_ = qobject_cast<QDeclarativeCoordinate*>(object);
        address_ = 0;
        connect(coordinate_, SIGNAL(latitudeChanged(double)), this, SLOT(queryContentChanged()));
        connect(coordinate_, SIGNAL(longitudeChanged(double)), this, SLOT(queryContentChanged()));
        connect(coordinate_, SIGNAL(altitudeChanged(double)), this, SLOT(queryContentChanged()));
    } else if (qobject_cast<QDeclarativeGeoAddress*>(object)) {
        if (address_)
            address_->disconnect(this);
        if (coordinate_)
            coordinate_->disconnect(this);
        address_ = qobject_cast<QDeclarativeGeoAddress*>(object);
        connect(address_, SIGNAL(countryChanged()), this, SLOT(queryContentChanged()));
        connect(address_, SIGNAL(countryCodeChanged()), this, SLOT(queryContentChanged()));
        connect(address_, SIGNAL(stateChanged()), this, SLOT(queryContentChanged()));
        connect(address_, SIGNAL(countyChanged()), this, SLOT(queryContentChanged()));
        connect(address_, SIGNAL(cityChanged()), this, SLOT(queryContentChanged()));
        connect(address_, SIGNAL(districtChanged()), this, SLOT(queryContentChanged()));
        connect(address_, SIGNAL(streetChanged()), this, SLOT(queryContentChanged()));
        connect(address_, SIGNAL(postcodeChanged()), this, SLOT(queryContentChanged()));
        coordinate_ = 0;
    } else {
        qmlInfo(this) << tr("Unsupported query type for geocode model (Coordinate and Address supported).");
        return;
    }
    queryVariant_ = query;
    emit queryChanged();
    if (autoUpdate_)
        update();
}


bool QDeclarativeGeocodeModel::autoUpdate() const
{
    return autoUpdate_;
}

void QDeclarativeGeocodeModel::setAutoUpdate(bool update)
{
    if (autoUpdate_ == update)
        return;
    autoUpdate_ = update;
    emit autoUpdateChanged();
}

#include "moc_qdeclarativegeocodemodel_p.cpp"

QT_END_NAMESPACE
