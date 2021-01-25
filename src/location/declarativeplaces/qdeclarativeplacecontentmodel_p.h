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

#ifndef QDECLARATIVEPLACECONTENTMODEL_H
#define QDECLARATIVEPLACECONTENTMODEL_H

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
#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlParserStatus>
#include <QtLocation/QPlaceContent>
#include <QtLocation/QPlaceContentReply>

QT_BEGIN_NAMESPACE

class QDeclarativePlace;
class QDeclarativeGeoServiceProvider;
class QGeoServiceProvider;
class QDeclarativeSupplier;
class QDeclarativePlaceUser;

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePlaceContentModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativePlace *place READ place WRITE setPlace NOTIFY placeChanged)
    Q_PROPERTY(int batchSize READ batchSize WRITE setBatchSize NOTIFY batchSizeChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)

    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QDeclarativePlaceContentModel(QPlaceContent::Type type, QObject *parent = 0);
    ~QDeclarativePlaceContentModel();

    QDeclarativePlace *place() const;
    void setPlace(QDeclarativePlace *place);

    int batchSize() const;
    void setBatchSize(int batchSize);

    int totalCount() const;

    void clearData();

    void initializeCollection(int totalCount, const QPlaceContent::Collection &collection);

    // from QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    enum Roles {
        SupplierRole = Qt::UserRole,
        PlaceUserRole,
        AttributionRole,
        UserRole //indicator for next conten type specific role
    };

    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);

    // from QQmlParserStatus
    void classBegin();
    void componentComplete();

Q_SIGNALS:
    void placeChanged();
    void batchSizeChanged();
    void totalCountChanged();

private Q_SLOTS:
    void fetchFinished();

protected:
    QPlaceContent::Collection m_content;
    QMap<QString, QDeclarativeSupplier *> m_suppliers;
    QMap<QString, QDeclarativePlaceUser *>m_users;

private:
    QDeclarativePlace *m_place;
    QPlaceContent::Type m_type;
    int m_batchSize;
    int m_contentCount;

    QPlaceContentReply *m_reply;
    QPlaceContentRequest m_nextRequest;

    bool m_complete;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEPLACECONTENTMODEL_H
