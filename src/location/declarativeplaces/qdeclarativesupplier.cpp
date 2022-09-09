/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#include "qdeclarativesupplier_p.h"

#include <QtLocation/QPlaceIcon>
#include <QtCore/QUrl>

QT_BEGIN_NAMESPACE

/*!
    \qmltype Supplier
    \instantiates QDeclarativeSupplier
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-data
    \since QtLocation 5.5

    \brief Holds data regarding the supplier of a place, a place's image, review, or editorial.

    Each instance represents a set of data about a supplier, which can include
    supplier's name, url and icon.  The supplier is typically a business or organization.

    Note: The Places API only supports suppliers as 'retrieve-only' objects.  Submitting
    suppliers to a provider is not a supported use case.

    \sa ImageModel, ReviewModel, EditorialModel

    \section1 Example

    The following example shows how to create and display a supplier in QML:

    \snippet declarative/places.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml Supplier
*/

QDeclarativeSupplier::QDeclarativeSupplier(QObject *parent)
    : QObject(parent)
{
}

QDeclarativeSupplier::QDeclarativeSupplier(const QPlaceSupplier &src,
                                           QDeclarativeGeoServiceProvider *plugin,
                                           QObject *parent)
    : QObject(parent),
      m_src(src)
{
    setSupplier(src, plugin);
}

QDeclarativeSupplier::~QDeclarativeSupplier()
{
}

/*!
    \internal
*/
void QDeclarativeSupplier::componentComplete()
{
}

/*!
    \qmlproperty QPlaceSupplier Supplier::supplier

    For details on how to use this property to interface between C++ and QML see
    "\l {Supplier - QPlaceSupplier} {Interfaces between C++ and QML Code}".
*/
void QDeclarativeSupplier::setSupplier(const QPlaceSupplier &src, QDeclarativeGeoServiceProvider *)
{
    QPlaceSupplier previous = m_src;
    m_src = src;

   if (previous.name() != m_src.name())
        emit nameChanged();

    if (previous.supplierId() != m_src.supplierId())
        emit supplierIdChanged();

    if (previous.url() != m_src.url())
        emit urlChanged();

    setIcon(m_src.icon());
}

QPlaceSupplier QDeclarativeSupplier::supplier()
{
    m_src.setIcon(m_icon);
    return m_src;
}

/*!
    \qmlproperty string Supplier::supplierId

    This property holds the identifier of the supplier.  The identifier is unique
    to the Plugin backend which provided the supplier and is generally
    not suitable for displaying to the user.
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

    This property holds the name of the supplier which can be displayed
    to the user.

    The name can potentially be localized.  The language is dependent on the
    entity that sets it, typically this is the \l Plugin.  The \l {Plugin::locales}
    property defines what language is used.
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

    This property holds the URL of the supplier's website.
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
QPlaceIcon QDeclarativeSupplier::icon() const
{
    return m_icon;
}

void QDeclarativeSupplier::setIcon(const QPlaceIcon &icon)
{
    if (m_icon == icon)
        return;

    m_icon = icon;
    emit iconChanged();
}

QT_END_NAMESPACE
