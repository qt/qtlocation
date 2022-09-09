/****************************************************************************
**
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

#include "qdeclarativecontactdetails_p.h"

#include <QPlaceContactDetail>

QT_BEGIN_NAMESPACE

/*!
    \qmltype ContactDetails
    \instantiates QDeclarativeContactDetails
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-data
    \since QtLocation 5.5

    \brief The ContactDetails type holds contact details for a \l Place.

    The ContactDetails type is a map of \l {QtLocation::ContactDetail}{ContactDetail} objects.
    To access contact details in the map use the \l keys() method to get the list of keys stored in
    the map and then use the \c {[]} operator to access the
    \l {QtLocation::ContactDetail}{ContactDetail} items.

    The following keys are defined in the API.  \l Plugin implementations are free to define
    additional keys.

    \list
        \li phone
        \li fax
        \li email
        \li website
    \endlist

    ContactDetails instances are only ever used in the context of \l {Place}{Places}.  It is not possible
    to create a ContactDetails instance directly or re-assign ContactDetails instances to \l {Place}{Places}.
    Modification of ContactDetails can only be accomplished via Javascript.

    \section1 Examples

    The following example shows how to access all \l {QtLocation::ContactDetail}{ContactDetails}
    and print them to the console:

    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml ContactDetails read

    The returned list of contact details is an \l {QObjectList-based model}{object list} and so can be used directly as a data model.  For example, the
    following demonstrates how to display a list of contact phone numbers in a list view:

    \snippet declarative/places.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml ContactDetails phoneList

    The following example demonstrates how to assign a single phone number to a place in JavaScript:
    \snippet declarative/places.qml  ContactDetails write single

    The following demonstrates how to assign multiple phone numbers to a place in JavaScript:
    \snippet declarative/places.qml  ContactDetails write multiple
*/

/*!
    \qmlmethod variant ContactDetails::keys()

    Returns an array of contact detail keys currently stored in the map.
*/
QDeclarativeContactDetails::QDeclarativeContactDetails(QObject *parent)
    : QQmlPropertyMap(parent)
{
}

QVariant QDeclarativeContactDetails::updateValue(const QString &, const QVariant &input)
{
    if (input.metaType() == QMetaType::fromType<QPlaceContactDetail>()) {
        QVariantList varList;
        varList.append(input);
        return varList;
    }

    return input;
}

QT_END_NAMESPACE
