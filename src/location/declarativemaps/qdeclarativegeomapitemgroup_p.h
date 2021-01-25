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

#ifndef QDECLARATIVEGEOMAPITEMGROUP_P_H
#define QDECLARATIVEGEOMAPITEMGROUP_P_H

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
#include <QtLocation/private/qdeclarativegeomapitemtransitionmanager_p.h>
#include <QtQuick/QQuickItem>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoMap;
class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoMapItemGroup : public QQuickItem
{
    Q_OBJECT
public:
    explicit QDeclarativeGeoMapItemGroup(QQuickItem *parent = 0);
    virtual ~QDeclarativeGeoMapItemGroup();

    void setParentGroup(QDeclarativeGeoMapItemGroup &parentGroup);
    void setQuickMap(QDeclarativeGeoMap *quickMap);
    QDeclarativeGeoMap *quickMap() const;

    qreal mapItemOpacity() const;

Q_SIGNALS:
    void mapItemOpacityChanged();
    void addTransitionFinished();
    void removeTransitionFinished();

protected:
    // QQmlParserStatus interface
    void classBegin() override;
    void componentComplete() override;

protected slots:
    void onMapSizeChanged();

private:
    QDeclarativeGeoMap *m_quickMap;
    QDeclarativeGeoMapItemGroup *m_parentGroup = nullptr;
    QScopedPointer<QDeclarativeGeoMapItemTransitionManager> m_transitionManager;

    friend class QDeclarativeGeoMapItemView;
    friend class QDeclarativeGeoMapItemTransitionManager;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeGeoMapItemGroup)

#endif // QDECLARATIVEGEOMAPITEMGROUP_P_H
