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
***************************************************************************/

#ifndef QDECLARATIVELANDMARKMODEL_P_H
#define QDECLARATIVELANDMARKMODEL_P_H

#include <qdeclarativelandmark_p.h>
#include <qdeclarativelandmarkfilters_p.h>

#include <qlandmark.h>
#include <qlandmarkmanager.h>
#include <qlandmarksortorder.h>
#include <qlandmarkfetchrequest.h>
#include <qlandmarkimportrequest.h>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeParserStatus>
#include <QAbstractListModel>

QTM_BEGIN_NAMESPACE


class QDeclarativeLandmarkAbstractModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(int limit READ limit WRITE setLimit NOTIFY limitChanged)
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)
    Q_PROPERTY(bool autoUpdate READ autoUpdate WRITE setAutoUpdate NOTIFY autoUpdateChanged)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(SortKey sortBy READ sortBy WRITE setSortBy NOTIFY sortByChanged)
    Q_PROPERTY(SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(SortOrder)
    Q_ENUMS(SortKey)

public:
    explicit QDeclarativeLandmarkAbstractModel(QObject* parent = 0);
    virtual ~QDeclarativeLandmarkAbstractModel();

    enum SortOrder {
        AscendingOrder = Qt::AscendingOrder,
        DescendingOrder = Qt::DescendingOrder,
        NoOrder = Qt::DescendingOrder + 1
    };
    enum SortKey {
        NoSort = QLandmarkSortOrder::NoSort,
        NameSort = QLandmarkSortOrder::NameSort
    };

    SortKey sortBy() const;
    void setSortBy(SortKey key);
    SortOrder sortOrder() const;
    void setSortOrder(SortOrder order);

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    // From QAbstractListModel
    virtual int rowCount(const QModelIndex &parent) const = 0;
    virtual QVariant data(const QModelIndex &index, int role) const = 0;

    bool autoUpdate() const;
    void setAutoUpdate(bool autoUpdate);
    QString error() const;
    int limit();
    void setLimit(int limit);
    int offset();
    void setOffset(int offset);

    void scheduleUpdate();
    Q_INVOKABLE void setDbFileName(QString fileName); //  testing purposes only
    virtual void startUpdate() = 0;

signals:
    void sortByChanged();
    void sortOrderChanged();
    void errorChanged();
    void autoUpdateChanged();
    void limitChanged();
    void offsetChanged();
    void modelChanged();
    void databaseChanged();

public slots:
    void update();

private slots:
    void categoriesChanged(const QList<QLandmarkCategoryId> &);
    void landmarksChanged(const QList<QLandmarkId> &);
    void dataChanged();

protected:
    virtual void cancelUpdate() = 0;
    virtual void cancelImport() {};
    void connectManager();

protected:
    QLandmarkManager* m_manager;
    bool m_componentCompleted;
    bool m_updatePending;
    bool m_autoUpdate;
    QString m_error;
    QString m_dbFileName;
    int m_limit;
    int m_offset;
    QLandmarkSortOrder* m_sortingOrder;
    SortOrder m_sortOrder;
    SortKey m_sortKey;
};

class QDeclarativeLandmarkModel: public QDeclarativeLandmarkAbstractModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QDeclarativeLandmarkFilterBase* filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeLandmark> landmarks READ landmarks NOTIFY landmarksChanged)
    Q_PROPERTY(QString importFile READ importFile WRITE setImportFile NOTIFY importFileChanged)

public:
    explicit QDeclarativeLandmarkModel(QObject* parent = 0);
    virtual ~QDeclarativeLandmarkModel();

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    // Roles for exposing data via model. Only one role because
    // everything can be accessed via QDeclarativeLandmark
    enum Roles {
        LandmarkRole = Qt::UserRole + 500
    };

    QString importFile() const;
    void setImportFile(QString importFile);
    void scheduleImport();
    Q_INVOKABLE void importLandmarks();

    virtual void componentComplete();

    int count();
    QDeclarativeLandmarkFilterBase* filter();
    void setFilter(QDeclarativeLandmarkFilterBase* filter);

    // For testing purpose to access directly ordered data
    QList<QLandmark> landmarkList();

    QDeclarativeListProperty<QDeclarativeLandmark> landmarks();
    static void landmarks_append(QDeclarativeListProperty<QDeclarativeLandmark>* prop, QDeclarativeLandmark* landmark);
    static int landmarks_count(QDeclarativeListProperty<QDeclarativeLandmark>* prop);
    static QDeclarativeLandmark* landmarks_at(QDeclarativeListProperty<QDeclarativeLandmark>* prop, int index);
    static void landmarks_clear(QDeclarativeListProperty<QDeclarativeLandmark>* prop);

protected:
    Q_INVOKABLE void startUpdate();
    Q_INVOKABLE void startImport();

signals:
    void countChanged();
    void filterChanged();
    void landmarksChanged();
    void importFileChanged();

private slots:
    void filterContentChanged();
    void cancelUpdate();
    void cancelImport();
    void fetchRequestStateChanged(QLandmarkAbstractRequest::State);

private:
    bool convertLandmarksToDeclarative();
    void setFetchRange();
    void setFetchOrder();

private:
    QDeclarativeLandmarkFilterBase* m_filter;
    QLandmarkFetchRequest* m_fetchRequest;
    QLandmarkImportRequest* m_importRequest;
    QString m_importFile;
    bool m_importPending;
    // Landmark list received from platform
    QList<QLandmark> m_landmarks;
    // Same landmark list, but as declarative classes
    QList<QDeclarativeLandmark*> m_declarativeLandmarks; // The declarative list
    QSet<QString> m_landmarkIdSet; // For faster lookups when updating items
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkModel));

#endif // QDECLARATIVELANDMARKMODEL_P_H
