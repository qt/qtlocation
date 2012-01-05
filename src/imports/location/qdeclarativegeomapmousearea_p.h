/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEGEOMAPMOUSEAREA_H
#define QDECLARATIVEGEOMAPMOUSEAREA_H

#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeomap_p.h"
#include "qdeclarativegeomapmouseevent_p.h"

#include <QtQuick/QQuickItem>
#include <QtQuick/private/qquickmousearea_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoMapMouseArea : public QQuickMouseArea
{
    Q_OBJECT

public:
    QDeclarativeGeoMapMouseArea(QQuickItem *parent = 0);
    ~QDeclarativeGeoMapMouseArea();

    Q_INVOKABLE QDeclarativeCoordinate* mouseToCoordinate(QQuickMouseEvent* event);

    // From QDeclarativeParserStatus
    virtual void componentComplete();

protected:
    // from QQuickItem
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private Q_SLOTS:
    void dragActiveChanged();

private:
    QQuickItem* parentMapItem();
    QDeclarativeGeoMap* map();

private:
    bool componentCompleted_;
    bool dragActive_;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoMapMouseArea));

#endif
