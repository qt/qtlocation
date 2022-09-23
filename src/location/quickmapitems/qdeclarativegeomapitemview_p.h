/****************************************************************************
**
** Copyright (C) 2015 Jolla Ltd.
** Contact: Aaron McCarthy <aaron.mccarthy@jollamobile.com>
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOMAPITEMVIEW_H
#define QDECLARATIVEGEOMAPITEMVIEW_H

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
#include <map>
#include <QtCore/QModelIndex>
#include <QtQml/QQmlParserStatus>
#include <QtQml/QQmlIncubator>
#include <QtQml/qqml.h>
#include <private/qqmldelegatemodel_p.h>
#include <QtQuick/private/qquicktransition_p.h>
#include <QtLocation/private/qdeclarativegeomapitemgroup_p.h>

QT_BEGIN_NAMESPACE

class QAbstractItemModel;
class QQmlComponent;
class QQuickItem;
class QDeclarativeGeoMap;
class QDeclarativeGeoMapItemBase;
class QQmlOpenMetaObject;
class QQmlOpenMetaObjectType;
class MapItemViewDelegateIncubator;
class QDeclarativeGeoMapItemViewItemData;
class QDeclarativeGeoMapItemView;
class QDeclarativeGeoMapItemGroup;

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoMapItemView : public QDeclarativeGeoMapItemGroup
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MapItemView)
    QML_ADDED_IN_VERSION(5, 0)
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(bool autoFitViewport READ autoFitViewport WRITE setAutoFitViewport NOTIFY autoFitViewportChanged)
    Q_PROPERTY(QQuickTransition *add MEMBER m_enter REVISION(5, 12))
    Q_PROPERTY(QQuickTransition *remove MEMBER m_exit REVISION(5, 12))
    Q_PROPERTY(QList<QQuickItem *> mapItems READ mapItems REVISION(5, 12))
    Q_PROPERTY(bool incubateDelegates READ incubateDelegates WRITE setIncubateDelegates NOTIFY incubateDelegatesChanged REVISION(5, 12))

public:
    explicit QDeclarativeGeoMapItemView(QQuickItem *parent = nullptr);
    ~QDeclarativeGeoMapItemView();

    QVariant model() const;
    void setModel(const QVariant &);

    QQmlComponent *delegate() const;
    void setDelegate(QQmlComponent *);

    bool autoFitViewport() const;
    void setAutoFitViewport(const bool &fit);

    void setMap(QDeclarativeGeoMap *);
    void removeInstantiatedItems(bool transition = true);
    void instantiateAllItems();

    void setIncubateDelegates(bool useIncubators);
    bool incubateDelegates() const;

    QList<QQuickItem *> mapItems();

    // From QQmlParserStatus
    void componentComplete() override;
    void classBegin() override;

Q_SIGNALS:
    void modelChanged();
    void delegateChanged();
    void autoFitViewportChanged();
    void incubateDelegatesChanged();

private Q_SLOTS:
    void destroyingItem(QObject *object);
    void initItem(int index, QObject *object);
    void createdItem(int index, QObject *object);
    void modelUpdated(const QQmlChangeSet &changeSet, bool reset);
    void exitTransitionFinished();

private:
    void fitViewport();
    void removeDelegateFromMap(int index, bool transition = true);
    void removeDelegateFromMap(QQuickItem *o);
    void transitionItemOut(QQuickItem *o);
    void terminateExitTransition(QQuickItem *o);
    QQmlInstanceModel::ReleaseFlags disposeDelegate(QQuickItem *item);

    void insertInstantiatedItem(int index, QQuickItem *o, bool createdItem);
    void addItemToMap(QDeclarativeGeoMapItemBase *item, int index, bool createdItem);
    void addItemViewToMap(QDeclarativeGeoMapItemView *item, int index, bool createdItem);
    void addItemGroupToMap(QDeclarativeGeoMapItemGroup *item, int index, bool createdItem);
    void addDelegateToMap(QQuickItem *object, int index, bool createdItem = false);

    bool m_componentCompleted = false;
    QQmlIncubator::IncubationMode m_incubationMode = QQmlIncubator::Asynchronous;
    QQmlComponent *m_delegate = nullptr;
    QVariant m_itemModel;
    QDeclarativeGeoMap *m_map = nullptr;
    QList<QQuickItem *> m_instantiatedItems;
    bool m_fitViewport = false;
    bool m_creatingObject = false;
    QQmlDelegateModel *m_delegateModel = nullptr;
    QQuickTransition *m_enter = nullptr;
    QQuickTransition *m_exit = nullptr;

    friend class QDeclarativeGeoMap;
    friend class QDeclarativeGeoMapItemBase;
    friend class QDeclarativeGeoMapItemTransitionManager;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeGeoMapItemView)

#endif
