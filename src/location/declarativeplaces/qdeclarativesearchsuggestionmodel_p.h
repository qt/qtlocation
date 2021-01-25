/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QDECLARATIVESEARCHSUGGESTIONMODEL_P_H
#define QDECLARATIVESEARCHSUGGESTIONMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qdeclarativesearchmodelbase_p.h>

#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoServiceProvider;
class QGeoServiceProvider;

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeSearchSuggestionModel : public QDeclarativeSearchModelBase
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

    void clearData(bool suppressSignal = false);

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

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
