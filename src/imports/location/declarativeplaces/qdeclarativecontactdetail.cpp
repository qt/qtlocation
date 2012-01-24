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

#include "qdeclarativecontactdetail_p.h"

/*!
    \qmlclass ContactDetails QDeclarativePropertyMap
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The ContactDetails element holds contact details for a \l Place.

    The ContactDetails element is a map of \l {QtLocation5::ContactDetail}{ContactDetail} objects.
    To access contact details in the map use the \l keys() method to get the list of keys stored in
    the map and then use the \c {[]} operator to access the
    \l {QtLocation5::ContactDetail}{ContactDetail} items.

    The following keys are defined in the API.  \l Plugin implementations are free to define
    additional keys.

    \list
        \o phone
        \o fax
        \o email
        \o website
    \endlist

    The following example shows how to access all \l {QtLocation5::ContactDetail}{ContactDetails}
    and print them to the console:

    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml ContactDetails read

    The returned list of contact details is an \l {QObjectList-based model}{object list} and so can be used directly as a data model.  For example, the
    following demonstrates how to display a list of contact phone numbers in a list view:

    \snippet snippets/declarative/places.qml QtQuick import
    \snippet snippets/declarative/places.qml QtLocation import
    \codeline
    \snippet snippets/declarative/places.qml ContactDetails phoneList

    The following example demonstrates how to assign a single phone number to a place in javascript:
    \snippet snippets/declarative/places.qml  ContactDetails write single

    The following demonstrates how to assign multiple phone numbers to a place in javascript:
    \snippet snippets/declarative/places.qml  ContactDetails write multiple

    Note, due to limitations of the QDeclarativePropertyMap, it is not possible
    to declaratively specify the contact details in QML, it can only be accomplished
    via javascript.
*/

/*!
    \qmlmethod variant ContactDetails::keys()

    Returns an array of contact detail keys currently stored in the map.
*/

/*!
    \qmlclass ContactDetail QDeclarativeContactDetail
    \inqmlmodule QtLocation 5
    \ingroup qml-QtLocation5-places
    \since QtLocation 5.0

    \brief The ContactDetail element holds a contact detail such as a phone number or a website
           address.

    The ContactDetail provides a single detail on how one could contact a \l Place.  The
    ContactDetail consists of a \l label, which is a localized string describing the contact
    method, and a \l value representing the actual contact detail.
*/
QDeclarativeContactDetail::QDeclarativeContactDetail(QObject *parent)
    : QObject(parent)
{
}

QDeclarativeContactDetail::QDeclarativeContactDetail(const QPlaceContactDetail &src, QObject *parent)
    : QObject(parent), m_contactDetail(src)
{
}

QDeclarativeContactDetail::~QDeclarativeContactDetail()
{
}

/*!
    \qmlproperty QPlaceContactDetail QtLocation5::ContactDetail::contactDetail

    This property is used to provide an interface between C++ and QML code.  First a pointer to a
    ContactDetail object must be obtained from C++, then use the
    \l {QObject::property()}{property()} and \l {QObject::setProperty()}{setProperty()} functions
    to get and set the \c contactDetail property.

    The following gets the QPlaceContactDetail representing this object from C++:

    \snippet snippets/cpp/cppqml.cpp ContactDetail get

    The following sets the properties of this object based on a QPlaceContactDetail object from
    C++:

    \snippet snippets/cpp/cppqml.cpp ContactDetail set
*/
void QDeclarativeContactDetail::setContactDetail(const QPlaceContactDetail &src)
{
    QPlaceContactDetail prevContactDetail = m_contactDetail;
    m_contactDetail = src;

    if (m_contactDetail.label() != prevContactDetail.label())
        emit labelChanged();
    if (m_contactDetail.value() != prevContactDetail.value())
        emit valueChanged();
}

QPlaceContactDetail QDeclarativeContactDetail::contactDetail() const
{
    return m_contactDetail;
}

/*!
    \qmlproperty string QtLocation5::ContactDetail::label

    This property holds a localized label describing the contact detail.
*/
QString QDeclarativeContactDetail::label() const
{
    return m_contactDetail.label();
}

void QDeclarativeContactDetail::setLabel(const QString &label)
{
    if (m_contactDetail.label()!= label) {
        m_contactDetail.setLabel(label);
        emit labelChanged();
    }
}

/*!
    \qmlproperty string QtLocation5::ContactDetail::value

    This property holds the value of the contact detail which may be a phone number, an email
    address, a website url etc.
*/
QString QDeclarativeContactDetail::value() const
{
    return m_contactDetail.value();
}

void QDeclarativeContactDetail::setValue(const QString &value)
{
    if (m_contactDetail.value()!= value) {
        m_contactDetail.setValue(value);
        emit valueChanged();
    }
}
