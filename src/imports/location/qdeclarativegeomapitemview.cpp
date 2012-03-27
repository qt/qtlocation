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

#include "qdeclarativegeomapitemview_p.h"
#include "qdeclarativegeomapmousearea_p.h"
#include "qdeclarativegeomap_p.h"
#include "qdeclarativegeomapitembase_p.h"

#include <QtCore/QAbstractItemModel>
#include <QtQml/QQmlParserStatus>
#include <QtQml/QQmlContext>
#include <QtQml/private/qqmlopenmetaobject_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass MapItemView QDeclarativeGeoMapItemView
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-maps
    \since Qt Location 5.0
    \inherits QQuickItem

    \brief The MapItemView is used to populate Map from a model.

    The MapItemView is used to populate Map with MapItems from a model.
    The MapItemView element only makes sense when contained in a Map,
    meaning that it has no standalone presentation.

    \section2 Example Usage

    This example demonstrates how to use the MapViewItem element to display
    a \l{Route}{route} on a \l{Map}{map}:

    \snippet snippets/declarative/maps.qml QtQuick import
    \snippet snippets/declarative/maps.qml QtLocation import
    \codeline
    \snippet snippets/declarative/maps.qml MapRoute
*/

QDeclarativeGeoMapItemView::QDeclarativeGeoMapItemView(QQuickItem *parent)
    : QObject(parent), visible_(true), componentCompleted_(false), delegate_(0),
      model_(0), map_(0)
{
}

QDeclarativeGeoMapItemView::~QDeclarativeGeoMapItemView()
{
    if (map_)
        removeInstantiatedItems();
}

/*!
    \internal
*/
void QDeclarativeGeoMapItemView::componentComplete()
{
    componentCompleted_ = true;
}

/*!
    \qmlproperty model QtLocation5::MapItemView::model

    This property holds the model that provides data for
    populating data with delegates.
*/
QVariant QDeclarativeGeoMapItemView::model() const
{
    return modelVariant_;
}

void QDeclarativeGeoMapItemView::setModel(const QVariant &model)
{
    if (!model.isValid() || model == modelVariant_)
        return;
    QObject *object = qvariant_cast<QObject*>(model);
    QAbstractItemModel* itemModel;
    if (!object || !(itemModel = qobject_cast<QAbstractItemModel*>(object))) {
        return;
    }
    modelVariant_ = model;
    model_ = itemModel;
    // At the moment maps only works with selected models. Because of this tight
    // restriction, we are not listening to all change signals.
    QObject::connect(model_, SIGNAL(modelReset()), this, SLOT(modelReset()));
    QObject::connect(model_, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(modelRowsRemoved(QModelIndex, int, int)));
    QObject::connect(model_, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(modelRowsInserted(QModelIndex, int, int)));
    repopulate();
    emit modelChanged();
}

/*!
    \internal
*/
void QDeclarativeGeoMapItemView::modelReset()
{
    repopulate();
}

/*!
    \internal
*/
void QDeclarativeGeoMapItemView::modelRowsInserted(QModelIndex, int start, int end)
{
    if (!componentCompleted_ || !map_ || !delegate_ || !model_) {
        QDeclarativeGeoMapItemBase* mapItem;
        for (int i = start; i <= end; ++i) {
            mapItem = createItem(i);
            if (!mapItem) {
                break;
            }
            mapItemList_.append(mapItem);
            map_->addMapItem(mapItem);
        }
    }
}

/*!
    \internal
*/
void QDeclarativeGeoMapItemView::modelRowsRemoved(QModelIndex, int start, int end)
{
    if (!componentCompleted_ || !map_ || !delegate_ || !model_)
        return;
    for (int i = end; i >= start; --i) {
        QDeclarativeGeoMapItemBase *mapItem = mapItemList_.takeAt(i);
        Q_ASSERT(mapItem);
        if (!mapItem) // bad
            break;
        map_->removeMapItem(mapItem);
        delete mapItem;
    }
}

/*!
    \qmlproperty Component QtLocation5::MapItemView::delegate

    This property holds the delegate which defines how each item in the
    model should be displayed. The Component must contain exactly one
    MapItem -derived element as the root element.

*/
QQmlComponent* QDeclarativeGeoMapItemView::delegate() const
{
    return delegate_;
}

void QDeclarativeGeoMapItemView::setDelegate(QQmlComponent *delegate)
{
    if (!delegate)
        return;
    delegate_ = delegate;

    repopulate();
    emit delegateChanged();
}

/*!
    \internal
*/
void QDeclarativeGeoMapItemView::setMapData(QDeclarativeGeoMap* map)
{
    if (!map || map_) // changing map on the fly not supported
        return;
    map_ = map;
}

/*!
    \internal
*/
void QDeclarativeGeoMapItemView::removeInstantiatedItems()
{
    if (!map_)
        return;
    for (int i = 0; i < mapItemList_.count(); ++ i) {
        map_->removeMapItem(mapItemList_.at(i));
    }
    qDeleteAll(mapItemList_);
    mapItemList_.clear();
}

/*!
    \internal
    Removes and repopulates all items.
*/
void QDeclarativeGeoMapItemView::repopulate()
{
    if (!componentCompleted_ || !map_ || !delegate_ || !model_)
        return;
    // Free any earlier instances
    removeInstantiatedItems();

    // Iterate model data and instantiate delegates.
    // We could use more specialized landmark model calls here too,
    // but hopefully the support will be leveraged to a general model
    // level.
    QDeclarativeGeoMapItemBase* mapItem;
    for (int i = 0; i < model_->rowCount(); ++i) {
        mapItem = createItem(i);
        Q_ASSERT(mapItem);
        if (!mapItem) // bad
            break;
        mapItemList_.append(mapItem);
        map_->addMapItem(mapItem);
    }
}

/*!
    \internal
    Currently item creation is tightly bound to models providing
    QObject* as data. Some day this may be leveraged to any user defined
    model or for example XML model.
*/
QDeclarativeGeoMapItemBase* QDeclarativeGeoMapItemView::createItem(int modelRow)
{
    if (!delegate_ || !model_)
        return NULL;
    QModelIndex index = model_->index(modelRow, 0); // column 0
    if (!index.isValid()) {
        qWarning() << "QDeclarativeGeoMapItemView Index is not valid: " << modelRow;
        return NULL;
    }

    QObject *model = new QObject(this);
    QQmlOpenMetaObject *modelMetaObject = new QQmlOpenMetaObject(model);

    QHashIterator<int, QByteArray> iterator(model_->roleNames());
    QQmlContext *itemContext = new QQmlContext(qmlContext(this));
    while (iterator.hasNext()) {
        iterator.next();
        QVariant modelData = model_->data(index, iterator.key());
        if (!modelData.isValid())
            continue;

        itemContext->setContextProperty(QString::fromLatin1(iterator.value().constData()),
                                        modelData);

        modelMetaObject->setValue(iterator.value(), modelData);
    }
    itemContext->setContextProperty(QLatin1String("model"), model);

    QObject* obj = delegate_->create(itemContext);

    if (!obj) {
        qWarning() << "QDeclarativeGeoMapItemView map item creation failed.";
        delete itemContext;
        return 0;
    }
    QDeclarativeGeoMapItemBase *declMapObj =  qobject_cast<QDeclarativeGeoMapItemBase*>(obj);
    if (!declMapObj) {
        qWarning() << "QDeclarativeGeoMapItemView map item delegate is of unsupported type.";
        delete itemContext;
        return 0;
    }
    itemContext->setParent(declMapObj);
    model->setParent(declMapObj);
    return declMapObj;
}

/*!
    \qmlproperty bool QtLocation5::MapItemView::visible

    This property holds whether the delegate objects created from the
    model are visible or not. Default value is true.
*/

void QDeclarativeGeoMapItemView::setVisible(bool visible)
{
    // TODO!
    Q_UNUSED(visible);
}

bool QDeclarativeGeoMapItemView::isVisible() const
{
    return visible_;
}

/*!
    \qmlproperty int QtLocation5::MapItemView::z

    This property holds the z-value of the MapItemView.
    It determines the z-value of the instantiated delegates.

    As with other Map items, objects with same z-value are
    drawn in insertion order.

*/
void QDeclarativeGeoMapItemView::setZValue(qreal zValue)
{
    // TODO z values
    Q_UNUSED(zValue);
    // group_.setZValue(zValue);
    // emit zChanged();
}

qreal QDeclarativeGeoMapItemView::zValue()
{
    // TODO z values what to do
    return 1.0;
    //return group_.zValue();
}

#include "moc_qdeclarativegeomapitemview_p.cpp"

QT_END_NAMESPACE
