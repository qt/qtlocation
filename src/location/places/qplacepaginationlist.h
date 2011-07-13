/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QPLACEPAGINATIONLIST_H
#define QPLACEPAGINATIONLIST_H

#include <QSharedDataPointer>
#include <QList>
#include "qmobilityglobal.h"
#include "qplacesupplier.h"

QT_BEGIN_NAMESPACE

/*!
    \class QPlacePaginationList

    \inmodule QPlaces

    \brief The QPlacePaginationList class represents a list object.

    Each QPlacePaginationList represents a pagination-enabled list. It contais list
    of items on current page and ifnormations about indexes of those items.

*/
template <typename T>
class Q_LOCATION_EXPORT QPlacePaginationList
{
public:
    QPlacePaginationList() : startIndx(0) {};
    QPlacePaginationList(const QPlacePaginationList<T> &other) {
        this->startIndx = other.startIndx;
        this->dataList = other.dataList;
    }

    ~QPlacePaginationList() {};

    QPlacePaginationList<T> &operator=(const QPlacePaginationList<T> &other) {
        this->startIndx = other.startIndx;
        this->dataList = other.dataList;
        return *this;
    }

    bool operator==(const QPlacePaginationList<T> &other) const {
        return (
                this->startIndx == other.startIndx
                && this->dataList == other.dataList
        );
    }
    bool operator!=(const QPlacePaginationList<T> &other) const {
        return !(other == *this);
    }

    /*!
        Returns items count on current page.
    */
    int items() { return dataList.count(); }
    /*!
        Returns index of first item on current page.
    */
    int start() { return startIndx; }
    /*!
        Returns index of last item on current page.
    */
    int stop() { return startIndx + dataList.count() - 1; }
    /*!
        Returns items list on current page.
    */
    QList<T> data() { return dataList; }
    /*!
        Sets index of first item on current page.
    */
    void setStart(const int &start) { startIndx = start; }
    /*!
        Sets list of items on current page.
    */
    void setData(const QList<T> &data) { dataList = data; }
    /*!
        Append item to the list.
    */
    void addItem(const T &item) { dataList.append(item); }
private:
    int startIndx;
    QList<T> dataList;
};

QT_END_NAMESPACE

#endif // QPLACEPAGINATIONLIST_H
