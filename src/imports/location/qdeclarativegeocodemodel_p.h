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

#ifndef QDECLARATIVEGEOCODEMODEL_H
#define QDECLARATIVEGEOCODEMODEL_H

#include "qdeclarativegeoserviceprovider_p.h"

#include "qdeclarativegeoboundingcircle_p.h"
#include <qgeosearchreply.h>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeParserStatus>
#include "qdeclarativegeolocation_p.h"
#include <QAbstractListModel>
#include <QPointer>

QTM_BEGIN_NAMESPACE

class QGeoServiceProvider;
class QGeoSearchManager;
class QDeclarativeGeoLocation;

class QDeclarativeGeocodeModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_ENUMS(Status)

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QVariant query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY(QObject* bounds READ bounds WRITE setBounds NOTIFY boundsChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    enum Status {
        Null,
        Ready,
        Loading,
        Error
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
    QString error() const;

    bool autoUpdate() const;
    void setAutoUpdate(bool update);

    int count() const;
    Q_INVOKABLE QDeclarativeGeoLocation* get(int index);

    QVariant query() const;
    void setQuery(const QVariant& query);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void reset();

Q_SIGNALS:
    void countChanged();
    void pluginChanged();
    void statusChanged();
    void errorChanged();
    void locationsChanged();
    void autoUpdateChanged();
    void boundsChanged();
    void queryChanged();

public Q_SLOTS:
    void update();

protected Q_SLOTS:
    void queryContentChanged();
    void searchFinished(QGeoSearchReply *reply);
    void searchError(QGeoSearchReply *reply,
                     QGeoSearchReply::Error error,
                     const QString &errorString);

protected:
    QGeoSearchManager* searchManager();
    void setStatus(Status status);
    void setError(const QString &error);
    bool autoUpdate_;
    bool complete_;

private:
    void setLocations(const QList<QGeoLocation> &locations);
    QGeoBoundingArea* boundingArea();
    void abortRequest();
    QGeoSearchReply* reply_;

    QDeclarativeGeoServiceProvider* plugin_;
    QGeoServiceProvider* serviceProvider_;
    QGeoSearchManager* searchManager_;
    QPointer<QObject> boundingArea_;
    QGeoBoundingBox boundingBox_;
    QGeoBoundingCircle boundingCircle_;

    QList<QDeclarativeGeoLocation*> declarativeLocations_;

    Status status_;
    QString error_;
    QVariant queryVariant_;
    QDeclarativeCoordinate* coordinate_;
    QDeclarativeGeoAddress* address_;
};

QTM_END_NAMESPACE

#endif
