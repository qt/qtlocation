/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOCODEMODEL_H
#define QDECLARATIVEGEOCODEMODEL_H

#include "qdeclarativegeoserviceprovider_p.h"

#include "qdeclarativegeoboundingcircle_p.h"
#include <qgeocodereply.h>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeParserStatus>
#include "qdeclarativegeolocation_p.h"
#include <QAbstractListModel>
#include <QPointer>

QT_BEGIN_NAMESPACE

class QGeoServiceProvider;
class QGeocodingManager;
class QDeclarativeGeoLocation;

class QDeclarativeGeocodeModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_ENUMS(GeocodeError)

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(QVariant query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QObject* bounds READ bounds WRITE setBounds NOTIFY boundsChanged)
    Q_PROPERTY(GeocodeError error READ error NOTIFY errorChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    enum Status {
        Null,
        Ready,
        Loading,
        Error
    };

    enum GeocodeError {
        NoError = QGeocodeReply::NoError,
        EngineNotSetError = QGeocodeReply::EngineNotSetError,
        CommunicationError = QGeocodeReply::CommunicationError,
        ParseError = QGeocodeReply::ParseError,
        UnsupportedOptionError = QGeocodeReply::UnsupportedOptionError,
        CombinationError = QGeocodeReply::CombinationError,
        UnknownError = QGeocodeReply::UnknownError
    };

    enum Roles {
        LocationRole = Qt::UserRole + 1
    };

    explicit QDeclarativeGeocodeModel(QObject* parent = 0);
    virtual ~QDeclarativeGeocodeModel();

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    // From QAbstractListModel
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    void setBounds(QObject* bounds);
    QObject* bounds() const;

    Status status() const;
    QString errorString() const;
    GeocodeError error() const;

    bool autoUpdate() const;
    void setAutoUpdate(bool update);

    int count() const;
    Q_INVOKABLE QDeclarativeGeoLocation* get(int index);

    int limit() const;
    void setLimit(int limit);
    int offset() const;
    void setOffset(int offset);

    QVariant query() const;
    void setQuery(const QVariant& query);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void reset();

Q_SIGNALS:
    void countChanged();
    void pluginChanged();
    void statusChanged();
    void errorStringChanged();
    void errorChanged();
    void locationsChanged();
    void autoUpdateChanged();
    void boundsChanged();
    void queryChanged();
    void limitChanged();
    void offsetChanged();

public Q_SLOTS:
    void update();

protected Q_SLOTS:
    void queryContentChanged();
    void geocodeFinished(QGeocodeReply *reply);
    void geocodeError(QGeocodeReply *reply,
                     QGeocodeReply::Error error,
                     const QString &errorString);
    void pluginReady();

protected:
    QGeocodingManager* searchManager();
    void setStatus(Status status);
    void setErrorString(const QString &error);
    void setError(GeocodeError error);
    bool autoUpdate_;
    bool complete_;

private:
    void setLocations(const QList<QGeoLocation> &locations);
    QGeoBoundingArea boundingArea();
    void abortRequest();
    QGeocodeReply* reply_;

    QDeclarativeGeoServiceProvider* plugin_;
    QPointer<QObject> boundingArea_;
    QGeoBoundingBox boundingBox_;
    QGeoBoundingCircle boundingCircle_;

    QList<QDeclarativeGeoLocation*> declarativeLocations_;

    Status status_;
    QString errorString_;
    GeocodeError error_;
    QVariant queryVariant_;
    QDeclarativeCoordinate* coordinate_;
    QDeclarativeGeoAddress* address_;
    QString searchString_;

    int limit_;
    int offset_;
};

QT_END_NAMESPACE

#endif
