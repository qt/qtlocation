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

#ifndef QDECLARATIVEGEOSEARCHMODEL_H
#define QDECLARATIVEGEOSEARCHMODEL_H

#include "qdeclarativegeoserviceprovider_p.h"

#include <qgeosearchreply.h>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeParserStatus>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE

class QGeoServiceProvider;
class QGeoSearchManager;
class QDeclarativeGeoPlace;

class QDeclarativeGeoSearchModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_ENUMS(Status)

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    enum Status {
        Null,
        Ready,
        Loading,
        Error
    };

    enum Roles {
        PlaceRole = Qt::UserRole + 499,
        LandmarkRole = Qt::UserRole + 500
    };

    explicit QDeclarativeGeoSearchModel(QObject* parent = 0);
    virtual ~QDeclarativeGeoSearchModel();

    // From QDeclarativeParserStatus
    virtual void classBegin();
    virtual void componentComplete();

    // From QAbstractListModel
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    Status status() const;

    QString error() const;

Q_SIGNALS:
    void pluginChanged(QDeclarativeGeoServiceProvider *plugin);
    void statusChanged(QDeclarativeGeoSearchModel::Status status);
    void errorChanged(const QString &error);
    void placesChanged();

private Q_SLOTS:
    void searchFinished(QGeoSearchReply *reply);
    void searchError(QGeoSearchReply *reply,
                     QGeoSearchReply::Error error,
                     const QString &errorString);

protected:
    QGeoSearchManager* searchManager();

    QList<QGeoPlace> places() const;

    void setStatus(Status status);

    void setError(const QString &error);

private:
    void setPlaces(const QList<QGeoPlace> &places);

    QDeclarativeGeoServiceProvider* plugin_;
    QGeoServiceProvider* serviceProvider_;
    QGeoSearchManager* searchManager_;

    Status status_;
    QString error_;
    QList<QGeoPlace> places_;
};


QTM_END_NAMESPACE

#endif
