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

#include "qdeclarativesupplier_p.h"

#include <QtCore/QUrl>

QT_USE_NAMESPACE

/*!
    \qmlclass Supplier QDeclarativeSupplier

    \brief The Supplier element holds supplier data.
    \ingroup qml-places
    \since 5.0

    Supplier cointains many properties holding data of the supplier like name,
    id, etc.
*/

QDeclarativeSupplier::QDeclarativeSupplier(QObject* parent)
    : QObject(parent), m_icon(0)
{
}

QDeclarativeSupplier::QDeclarativeSupplier(const QPlaceSupplier &src,
        QObject *parent)
        : QObject(parent),
          m_src(src),
          m_icon(0)
{
    //Note: icon needs to be assigned externally
    //to the QDeclarativeSupplier since the icon needs
    //to know which plugin it comes from.
}

QDeclarativeSupplier::~QDeclarativeSupplier()
{
}

void QDeclarativeSupplier::setSupplier(const QPlaceSupplier &src)
{
    QPlaceSupplier previous = m_src;
    m_src = src;

   if (previous.name() != m_src.name()) {
        emit nameChanged();
    }
    if (previous.supplierId() != m_src.supplierId()) {
        emit supplierIdChanged();
    }
    if (previous.url() != m_src.url()) {
        emit urlChanged();
    }

    //Note: icon needs to be assigned externally
    //to the QDeclarativeSupplier since the icon needs
    //to know which plugin it comes from.
}

QPlaceSupplier QDeclarativeSupplier::supplier()
{
    m_src.setIcon(m_icon ? m_icon->icon() : QPlaceIcon());
    return m_src;
}

/*!
    \qmlproperty string Supplier::supplierId

    This property holds id.
*/

void QDeclarativeSupplier::setSupplierId(const QString &supplierId)
{
    if (m_src.supplierId() != supplierId) {
        m_src.setSupplierId(supplierId);
        emit supplierIdChanged();
    }
}

QString QDeclarativeSupplier::supplierId() const
{
    return m_src.supplierId();
}

/*!
    \qmlproperty string Supplier::name

    This property holds name.
*/

void QDeclarativeSupplier::setName(const QString &name)
{
    if (m_src.name() != name) {
        m_src.setName(name);
        emit nameChanged();
    }
}

QString QDeclarativeSupplier::name() const
{
    return m_src.name();
}

/*!
    \qmlproperty url Supplier::url

    This property holds supplier URL.
*/

void QDeclarativeSupplier::setUrl(const QUrl &url)
{
    if (m_src.url() != url) {
        m_src.setUrl(url);
        emit urlChanged();
    }
}

QUrl QDeclarativeSupplier::url() const
{
    return m_src.url();
}

/*!
    \qmlproperty PlaceIcon Supplier::icon

    This property holds the icon of the supplier.
*/
QDeclarativePlaceIcon *QDeclarativeSupplier::icon() const
{
    return m_icon;
}

void QDeclarativeSupplier::setIcon(QDeclarativePlaceIcon *icon)
{
    if (m_icon == icon)
        return;

    if (m_icon && m_icon->parent() == this)
        delete m_icon;

    m_icon = icon;
    emit iconChanged();
}
