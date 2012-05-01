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

#ifndef QDECLARATIVESEARCHMODELBASE_H
#define QDECLARATIVESEARCHMODELBASE_H

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlParserStatus>
#include <QtLocation/QPlaceSearchRequest>
#include <QtLocation/QPlaceSearchResult>
#include <QtLocation/QPlaceReply>

#include "qdeclarativegeoserviceprovider_p.h"

QT_BEGIN_NAMESPACE

class QPlaceManager;
class QPlaceSearchRequest;
class QPlaceSearchReply;
class QDeclarativePlace;
class QDeclarativeGeoBoundingArea;

class QDeclarativeSearchModelBase : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QDeclarativeGeoBoundingArea *searchArea READ searchArea WRITE setSearchArea NOTIFY searchAreaChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_ENUMS(Status)

    Q_INTERFACES(QQmlParserStatus)

public:
    enum Status {
        Null,
        Ready,
        Loading,
        Error
    };

    explicit QDeclarativeSearchModelBase(QObject *parent = 0);
    ~QDeclarativeSearchModelBase();

    QDeclarativeGeoServiceProvider* plugin() const;
    void setPlugin(QDeclarativeGeoServiceProvider *plugin);

    QDeclarativeGeoBoundingArea *searchArea() const;
    void setSearchArea(QDeclarativeGeoBoundingArea *searchArea);

    int offset() const;
    void setOffset(int offset);

    int limit() const;
    void setLimit(int limit);

    Status status() const;
    void setStatus(Status status, const QString &errorString = QString());

    Q_INVOKABLE void update();

    Q_INVOKABLE void cancel();
    Q_INVOKABLE void reset();

    Q_INVOKABLE QString errorString() const;

    virtual void clearData();
    virtual void updateSearchRequest();

    // From QQmlParserStatus
    virtual void classBegin();
    virtual void componentComplete();

signals:
    void pluginChanged();
    void searchAreaChanged();
    void offsetChanged();
    void limitChanged();
    void statusChanged();

protected:
    virtual void initializePlugin(QDeclarativeGeoServiceProvider *plugin);

protected slots:
    virtual void queryFinished() = 0;

private slots:
    void pluginNameChanged();

protected:
    virtual QPlaceReply *sendQuery(QPlaceManager *manager, const QPlaceSearchRequest &request) = 0;

    QPlaceSearchRequest m_request;
    QDeclarativeGeoServiceProvider *m_plugin;
    QPlaceReply *m_reply;

private:
    QDeclarativeGeoBoundingArea *m_searchArea;

    bool m_complete;
    Status m_status;
    QString m_errorString;
};

QT_END_NAMESPACE

#endif // QDECLARATIVESEARCHMODELBASE_H
