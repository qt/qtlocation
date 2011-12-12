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

#ifndef QDECLARATIVESEARCHRESULTMODEL_P_H
#define QDECLARATIVESEARCHRESULTMODEL_P_H

#include "qdeclarativesearchmodelbase.h"
#include "qdeclarativecategory_p.h"
#include "qdeclarativeplace_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGeoServiceProvider;

class QDeclarativeSearchResultModel : public QDeclarativeSearchModelBase
{
    Q_OBJECT

    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCategory> categories READ categories NOTIFY categoriesChanged)
    Q_PROPERTY(int maximumCorrections READ maximumCorrections WRITE setMaximumCorrections NOTIFY maximumCorrectionsChanged)
    Q_PROPERTY(RelevanceHint relevanceHint READ relevanceHint WRITE setRelevanceHint NOTIFY relevanceHintChanged)
    Q_PROPERTY(QDeclarativePlace::Visibility visibilityScope READ visibilityScope WRITE setVisibilityScope NOTIFY visibilityScopeChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)

    Q_ENUMS(SearchResultType RelevanceHint)

public:
    enum SearchResultType {
        PlaceResult = QPlaceSearchResult::PlaceResult,
        CorrectionResult = QPlaceSearchResult::CorrectionResult,
        UnknownSearchResult = QPlaceSearchResult::UnknownSearchResult
    };

    enum RelevanceHint {
        UnspecifiedHint = QPlaceSearchRequest::UnspecifiedHint,
        DistanceHint = QPlaceSearchRequest::DistanceHint,
        LexicalPlaceNameHint = QPlaceSearchRequest::LexicalPlaceNameHint
    };

    explicit QDeclarativeSearchResultModel(QObject *parent = 0);
    ~QDeclarativeSearchResultModel();

    QString searchTerm() const;
    void setSearchTerm(const QString &searchTerm);

    QDeclarativeListProperty<QDeclarativeCategory> categories();
    static void categories_append(QDeclarativeListProperty<QDeclarativeCategory> *list,
                                  QDeclarativeCategory* category);
    static int categories_count(QDeclarativeListProperty<QDeclarativeCategory> *list);
    static QDeclarativeCategory* category_at(QDeclarativeListProperty<QDeclarativeCategory> *list, int index);
    static void categories_clear(QDeclarativeListProperty<QDeclarativeCategory> *list);

    QDeclarativeSearchResultModel::RelevanceHint relevanceHint() const;
    void setRelevanceHint(QDeclarativeSearchResultModel::RelevanceHint hint);

    int maximumCorrections() const;
    void setMaximumCorrections(int corrections);

    QDeclarativePlace::Visibility visibilityScope() const;
    void setVisibilityScope(QDeclarativePlace::Visibility visibilityScope);

    void clearData();
    void updateSearchRequest();
    void processReply(QPlaceReply *reply);

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    enum Roles {
        SearchResultTypeRole = Qt::UserRole,
        DistanceRole,
        PlaceRole,
        CorrectionRole
    };
    Q_INVOKABLE QVariant data(int index, const QString &roleName) const;

signals:
    void searchTermChanged();
    void categoriesChanged();
    void maximumCorrectionsChanged();
    void relevanceHintChanged();
    void visibilityScopeChanged();
    void rowCountChanged();

protected:
    QPlaceReply *sendQuery(QPlaceManager *manager, const QPlaceSearchRequest &request);
    virtual void initializePlugin(QDeclarativeGeoServiceProvider *plugin);

private slots:
    void placeUpdated(const QString &placeId);
    void placeRemoved(const QString &placeId);

private:
    int getRow(const QString &placeId) const;
    QList<QPlaceSearchResult> m_results;
    QList<QDeclarativePlace *> m_places;
    QList<QDeclarativeCategory*> m_categories;
    QtLocation::VisibilityScope m_visibilityScope;
    QPlaceManager *m_placeManager;
};

QT_END_NAMESPACE

#endif // QDECLARATIVESEARCHRESULTMODEL_P_H
