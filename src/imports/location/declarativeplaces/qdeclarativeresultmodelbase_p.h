/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVERESULTMODELBASE_H
#define QDECLARATIVERESULTMODELBASE_H

#include "qdeclarativesearchmodelbase.h"
#include "qdeclarativeplace_p.h"

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QPlaceSearchReply>

class QDeclarativeResultModelBase : public QDeclarativeSearchModelBase
{
    Q_OBJECT

public:
    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)
    Q_PROPERTY(QDeclarativeGeoServiceProvider *favoritesPlugin READ favoritesPlugin WRITE setFavoritesPlugin NOTIFY favoritesPluginChanged)
    Q_PROPERTY(QVariantMap favoritesMatchParameters READ favoritesMatchParameters WRITE setFavoritesMatchParameters NOTIFY favoritesMatchParametersChanged)

    explicit QDeclarativeResultModelBase(QObject *parent = 0);

    QDeclarativeGeoServiceProvider *favoritesPlugin() const;
    void setFavoritesPlugin(QDeclarativeGeoServiceProvider *plugin);

    QVariantMap favoritesMatchParameters() const;
    void setFavoritesMatchParameters(const QVariantMap &parameters);

    virtual void clearData();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data (const QModelIndex &index, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant data(int index, const QString &roleName) const;

Q_SIGNALS:
    void rowCountChanged();
    void favoritesPluginChanged();
    void favoritesMatchParametersChanged();

protected Q_SLOTS:
    virtual void queryFinished();

private Q_SLOTS:
    void updateLayout(const QList<QPlace> &favoritePlaces = QList<QPlace>());

protected:
    enum Roles {
        DistanceRole = Qt::UserRole,
        PlaceRole
    };

    QList<QPlaceSearchResult> m_results;
    QList<QPlaceSearchResult> m_resultsBuffer;
    QList<QDeclarativePlace *> m_places;

    QDeclarativeGeoServiceProvider *m_favoritesPlugin;
    QVariantMap m_matchParameters;
};

#endif
