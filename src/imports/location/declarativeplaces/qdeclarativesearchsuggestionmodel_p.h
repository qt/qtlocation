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

#ifndef QDECLARATIVESEARCHSUGGESTIONMODEL_P_H
#define QDECLARATIVESEARCHSUGGESTIONMODEL_P_H

#include "qdeclarativesearchmodelbase.h"

#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoServiceProvider;
class QGeoServiceProvider;

class QDeclarativeSearchSuggestionModel : public QDeclarativeSearchModelBase
{
    Q_OBJECT

    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    Q_PROPERTY(QStringList suggestions READ suggestions NOTIFY suggestionsChanged)

public:
    explicit QDeclarativeSearchSuggestionModel(QObject *parent = 0);
    ~QDeclarativeSearchSuggestionModel();

    QString searchTerm() const;
    void setSearchTerm(const QString &searchTerm);

    QStringList suggestions() const;

    void clearData();
    void updateSearchRequest();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    enum Roles {
        SearchSuggestionRole = Qt::UserRole
    };

protected Q_SLOTS:
    virtual void queryFinished();

Q_SIGNALS:
    void searchTermChanged();
    void suggestionsChanged();

protected:
    QPlaceReply *sendQuery(QPlaceManager *manager, const QPlaceSearchRequest &request);

private:
    QStringList m_suggestions;
};

QT_END_NAMESPACE

#endif
