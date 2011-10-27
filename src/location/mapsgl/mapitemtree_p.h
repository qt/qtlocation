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
#ifndef MAPITEMTREE_P_H
#define MAPITEMTREE_P_H

#include <QPoint>
#include <QRect>
#include <QList>
#include <QtLocation/qlocationglobal.h>

#include "intervaltree_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class MapItem;

class Q_LOCATION_EXPORT MapItemTree
{
public:
    MapItemTree();
    ~MapItemTree();

    void insert(MapItem *item);
    void remove(MapItem *item);

    bool isEmpty() const;
    int size() const;

    QList<MapItem*> items() const;
    QList<MapItem*> itemsAt(const QPoint &point) const;
    QList<MapItem*> itemsWithin(const QRect &viewport) const;
    void makeVisible(const QRect& viewport, QList<MapItem*> &added, QList<MapItem*> &removed);

private:
    QRect viewport_;
    AAInterval2DTree<MapItem*> *root_;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // MAPITEMTREE_P__H
