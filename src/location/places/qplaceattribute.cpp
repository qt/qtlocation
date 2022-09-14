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

#include "qplaceattribute_p.h"
#include "qplaceattribute.h"

QT_USE_NAMESPACE

QT_DEFINE_QSDP_SPECIALIZATION_DTOR(QPlaceAttributePrivate)

bool QPlaceAttributePrivate::operator== (const QPlaceAttributePrivate &other) const
{
    return label == other.label
            && text == other.text;
}

bool QPlaceAttributePrivate::isEmpty() const
{
    return  label.isEmpty()
            && text.isEmpty();
}


/*!
    \class QPlaceAttribute
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since 5.6

    \brief The QPlaceAttribute class represents generic attribute information about a place.

    A QPlaceAttribute instance stores an additional piece of information about a place that is not
    otherwise exposed through the QPlace class.  A QPlaceAttribute encapsulates a
    localized label which describes the attribute and rich text string representing the attribute's value.
    Generally, both are intended to be displayed to the end-user as is.

    Some plugins may not support attributes at all, others may only support a
    certain set, others still may support a dynamically changing set of attributes
    over time or even allow attributes to be arbitrarily defined by the client
    application.  The attributes could also vary on a place by place basis,
    for example one place may have opening hours while another does not.
    Consult the \l {Plugin References and Parameters}{plugin
    references} for details.

    \section2 Attribute Types
    The QPlaceAttribute class defines some constant strings which characterize standard \e {attribute types}.
    \list
        \li QPlaceAttribute::OpeningHours
        \li QPlaceAttribute::Payment
        \li QPlaceAttribute::Provider
    \endlist

    There is a class of attribute types of the format x_id_<provider> for example x_id_here.
    This class of attributes is a set of alternative identifiers of the place, from the specified provider's
    perspective.

    The above types are used to access and modify attributes in QPlace via:
    \list
        \li QPlace::extendedAttribute()
        \li QPlace::setExtendedAttribute()
        \li QPlace::removeExtendedAttribute()
        \li QPlace::removeExtendedAttribute()
    \endlist

    The \e {attribute type} is a string type so that providers are able to introduce
    new attributes as necessary.  Custom attribute types should always be prefixed
    by a qualifier in order to avoid conflicts.

    \section3 User Readable and Non-User Readable Attributes
    Some attributes may not be intended to be readable by end users, the label field
    of such attributes are empty to indicate this fact.
*/

/*!
    \qmltype ExtendedAttributes
    \instantiates QQmlPropertyMap
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-data
    \since QtLocation 5.5

    \brief The ExtendedAttributes type holds additional data about a \l Place.

    The ExtendedAttributes type is a map of \l {placeAttribute}{placeAttributes}.  To access
    attributes in the map use the \l keys() method to get the list of keys stored in the map and
    use the \c {[]} operator to access the \l placeAttribute items.

    The following are standard keys that are defined by the API.  \l Plugin
    implementations are free to define additional keys.  Custom keys should
    be qualified by a unique prefix to avoid clashes.
    \table
        \header
            \li key
            \li description
        \row
            \li openingHours
            \li The trading hours of the place
        \row
            \li payment
            \li The types of payment the place accepts, for example visa, mastercard.
        \row
            \li x_provider
            \li The name of the provider that a place is sourced from
        \row
            \li x_id_<provider> (for example x_id_here)
            \li An alternative identifier which identifies the place from the
               perspective of the specified provider.
    \endtable

    Some plugins may not support attributes at all, others may only support a
    certain set, others still may support a dynamically changing set of attributes
    over time or even allow attributes to be arbitrarily defined by the client
    application.  The attributes could also vary on a place by place basis,
    for example one place may have opening hours while another does not.
    Consult the \l {Plugin References and Parameters}{plugin
    references} for details.

    Some attributes may not be intended to be readable by end users, the label field
    of such attributes is empty to indicate this fact.

    \note ExtendedAttributes instances are only ever used in the context of \l {Place}s.  It is not
    possible to create an ExtendedAttributes instance directly or re-assign a \l {Place}'s
    ExtendedAttributes property.  Modification of ExtendedAttributes can only be accomplished
    via Javascript.

    The following example shows how to access all \l {placeAttribute}{placeAttributes} and print
    them to the console:

    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml ExtendedAttributes read

    The following example shows how to assign and modify an attribute:
    \snippet declarative/places.qml ExtendedAttributes write

    \sa placeAttribute, QQmlPropertyMap
*/

/*!
    \qmlmethod variant ExtendedAttributes::keys()

    Returns an array of place attribute keys currently stored in the map.
*/

/*!
    \qmlsignal void ExtendedAttributes::valueChanged(string key, variant value)

    This signal is emitted when the set of attributes changes. \a key is the key
    corresponding to the \a value that was changed.

    The corresponding handler is \c onValueChanged.
*/

/*!
    \qmlvaluetype placeAttribute
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-places
    \ingroup qml-QtLocation5-places-data
    \since QtLocation 5.5

    \brief The placeAttribute type holds generic place attribute information.

    A place attribute stores an additional piece of information about a \l Place that is not
    otherwise exposed through the \l Place type.  A placeAttribute is a textual piece of data,
    accessible through the \l text property, and a \l label.  Both the \l text and \l label
    properties are intended to be displayed to the user.  placeAttributes are stored in an
    \l ExtendedAttributes map with a unique key.

    The following example shows how to display all attributes in a list:

    \snippet declarative/places.qml QtQuick import
    \snippet declarative/maps.qml QtLocation import
    \codeline
    \snippet declarative/places.qml ExtendedAttributes

    The following example shows how to assign and modify an attribute:
    \snippet declarative/places.qml ExtendedAttributes write
*/

/*!
   \variable QPlaceAttribute::OpeningHours
   Specifies the opening hours.
*/
const QString QPlaceAttribute::OpeningHours(QLatin1String("openingHours"));

/*!
   \variable QPlaceAttribute::Payment
   The constant to specify an attribute that defines the methods of payment.
*/
const QString QPlaceAttribute::Payment(QLatin1String("payment"));

/*!
    \variable QPlaceAttribute::Provider
    The constant to specify an attribute that defines which
    provider the place came from.
*/
const QString QPlaceAttribute::Provider(QLatin1String("x_provider"));

/*!
    Constructs an attribute.
*/
QPlaceAttribute::QPlaceAttribute()
    : d_ptr(new QPlaceAttributePrivate)
{
}

/*!
    Destroys the attribute.
*/
QPlaceAttribute::~QPlaceAttribute() = default;

/*!
    Creates a copy of \a other.
*/
QPlaceAttribute::QPlaceAttribute(const QPlaceAttribute &other) noexcept = default;

/*!
    Assigns \a other to this attribute and returns a reference to this
    attribute.
*/
QPlaceAttribute &QPlaceAttribute::operator=(const QPlaceAttribute &other) noexcept
{
    if (this == &other)
        return *this;

    d_ptr = other.d_ptr;
    return *this;
}

/*!
    \fn bool QPlaceAttribute::operator==(const QPlaceAttribute &lhs, const QPlaceAttribute &rhs) noexcept

    Returns true if \a lhs is equal to \a rhs, otherwise
    returns false.
*/

/*!
    \fn bool QPlaceAttribute::operator!=(const QPlaceAttribute &lhs, const QPlaceAttribute &rhs) noexcept

    Returns true if \a lhs is not equal to \a rhs,
    otherwise returns false.
*/

bool QPlaceAttribute::isEqual(const QPlaceAttribute &other) const noexcept
{
    if (d_ptr == other.d_ptr)
        return true;
    return ( *(d_ptr.constData()) == *(other.d_ptr.constData()));
}

/*!
    \qmlproperty string placeAttribute::label

    This property holds the attribute label which is a user visible string
    describing the attribute.
*/

/*!
    \property QPlaceAttribute::label
    \brief a localized label describing the attribute.
*/
QString QPlaceAttribute::label() const
{
    return d_ptr->label;
}

void QPlaceAttribute::setLabel(const QString &label)
{
    d_ptr->label = label;
}

/*!
    \qmlproperty string placeAttribute::text

    This property holds the attribute text which can be used to show additional information about the place.
*/

/*!
    \property QPlaceAttribute::text
    \brief a piece of rich text representing the attribute value.
*/
QString QPlaceAttribute::text() const
{
    return d_ptr->text;
}

void QPlaceAttribute::setText(const QString &text)
{
    d_ptr->text = text;
}

/*!
    Returns a boolean indicating whether the all the fields of the place attribute
    are empty or not.
*/
bool QPlaceAttribute::isEmpty() const
{
    return d_ptr->isEmpty();
}

#include "moc_qplaceattribute.cpp"
