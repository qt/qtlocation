/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkattributefilter.h"
#include "qlandmarkfilter_p.h"
#include <QHash>

QTM_BEGIN_NAMESPACE

class QLandmarkAttributeFilterPrivate : public QLandmarkFilterPrivate
{
public:
    QLandmarkAttributeFilterPrivate();
    QLandmarkAttributeFilterPrivate(const QLandmarkAttributeFilterPrivate &other);

    virtual ~QLandmarkAttributeFilterPrivate();

    virtual bool compare(const QLandmarkFilterPrivate *other) const
    {
        //it is assumed that the types and superclass member variables already match
        const QLandmarkAttributeFilterPrivate *od = static_cast<const QLandmarkAttributeFilterPrivate*>(other);
        return (attributes == od->attributes)
            && (flags == od->flags)
            && (operationType == od->operationType);
    }

    Q_IMPLEMENT_LANDMARKFILTER_VIRTUALCTORS(QLandmarkAttributeFilter, QLandmarkFilter::AttributeFilter)

    QHash<QString, QVariant> attributes;
    QHash<QString, QLandmarkFilter::MatchFlags> flags;
    QLandmarkAttributeFilter::OperationType operationType;
};

QLandmarkAttributeFilterPrivate::QLandmarkAttributeFilterPrivate()
    : QLandmarkFilterPrivate(),
    attributes(QHash<QString, QVariant>()),
    flags(QHash<QString, QLandmarkFilter::MatchFlags>()),
    operationType(QLandmarkAttributeFilter::AndOperation)

{
    type = QLandmarkFilter::AttributeFilter;
}

QLandmarkAttributeFilterPrivate::QLandmarkAttributeFilterPrivate(const QLandmarkAttributeFilterPrivate &other)
    : QLandmarkFilterPrivate(other),
    attributes(other.attributes),
    flags(other.flags),
    operationType(other.operationType)
{
}

QLandmarkAttributeFilterPrivate::~QLandmarkAttributeFilterPrivate()
{
}

/*!
    \class QLandmarkAttributeFilter
    \brief The QLandmarkAttributeFilter class provides filtering on various landmark attributes.

    \inmodule QtLocation

    \ingroup landmarks-filter
    \since 1.1

    You can provide various keys which describe the attribute(s) to search.
    Precisely which keys may be used depends on the manager
    and these can be retrieved by using QLandmarkManager::searchableLandmarkAttributeKeys().
    The table below outlines some keys that may be used with the default managers on the
    currently supported platforms.  The match flags may be used for attributes which are
    of string type (typically most, if not all searchable attributes are string types).

    \table
    \header
        \o {3,1} Searchable attributes
    \row
        \o "city"
        \o "country
        \o "countryCode"
    \row
        \o "county"
        \o "description"
        \o "district"
    \row
        \o "name"
        \o "state"
        \o "phoneNumber"
    \row
        \o "postcode"
        \o "street"
        \o
    \endtable

        Please note that different platforms support different capabilities with the attribute filter.
        \list
            \o The S60 3.1, 3.2 and 5.0 platforms do not support an OR type attribute filter.
            \o The S60 3.1, 3.2, 5.0 and Symbian platforms do not support the MatchContains flag.
            \o The S60 3.1, 3.2, 5.0 and Symbian platforms do not support multiple attributes with an
	AND type attribute filter(using a single attribute however is supported).
        \endlist
*/
Q_IMPLEMENT_LANDMARKFILTER_PRIVATE(QLandmarkAttributeFilter);

/*!
    \enum QLandmarkAttributeFilter::OperationType
    Defines how the attribute criteria are combined if more than one attribute is defined.
    \value AndOperation Landmarks must match all the attributes provided by the filter.
    \value OrOperation Landmarks must match at least one attribute provided by the filter.
*/

/*!
    Creates an attribute filter.
*/
QLandmarkAttributeFilter::QLandmarkAttributeFilter()
    : QLandmarkFilter(new QLandmarkAttributeFilterPrivate)
{
}

/*!
    \fn QLandmarkAttributeFilter::QLandmarkAttributeFilter(const QLandmarkFilter &other)
    Constructs a copy of \a other if possible, otherwise constructs a new attribute filter.
*/

/*!
    Destroys the filter.
*/
QLandmarkAttributeFilter::~QLandmarkAttributeFilter()
{
}

/*!
    Returns the value of the attribute corresponding to \a key.

    If the attribute isn't set an invalid QVariant is returned.
*/
QVariant QLandmarkAttributeFilter::attribute(const QString &key) const
{
    Q_D(const QLandmarkAttributeFilter);
    return d->attributes.value(key);
}

/*!
    Sets the \a value of the attribute corresponding to \a key.

    For string based attributes a set of matching \a flags can be provided
    to define how the string values should be matched.
    For non-string based attributes the \a flags are ignored.
    The beahviour of the filter is undefined if an invalid QVariant is used as a \a value
*/
void QLandmarkAttributeFilter::setAttribute(const QString &key, const QVariant &value, QLandmarkFilter::MatchFlags flags)
{
    Q_D(QLandmarkAttributeFilter);
    d->attributes[key] = value;
    d->flags[key] = flags;
}

/*!
    Sets the \a value of all the attributes correponding to those in \a keys.

    For string based attributes a
    set of matching \a flags can be provided to define how the string values should be matched.
    For non-string based attributes the \a flags are ignored.
    The behaviour of the filter is undefined if an invalid QVariant is used as \a value.
*/
void QLandmarkAttributeFilter::setAttributes(const QStringList &keys, const QVariant &value, QLandmarkFilter::MatchFlags flags)
{
    Q_D(QLandmarkAttributeFilter);
    foreach(const QString &key, keys) {
        d->attributes[key] = value;
        d->flags[key] = flags;
    }
}

/*!
    Removes the attribute corresponding to \a key from the filter.
*/
void QLandmarkAttributeFilter::removeAttribute(const QString &key)
{
    Q_D(QLandmarkAttributeFilter);
    d->attributes.remove(key);
    d->flags.remove(key);
}

/*!
    Clears all attributes from the filter.
*/
void QLandmarkAttributeFilter::clearAttributes()
{
    Q_D(QLandmarkAttributeFilter);
    d->attributes.clear();
    d->flags.clear();
}

/*!
    Returns the keys of all attributes set in the filter.
*/
QStringList QLandmarkAttributeFilter::attributeKeys() const
{
    Q_D(const QLandmarkAttributeFilter);
    return d->attributes.keys();
}

/*!
    Returns the operation to be used by the filter when multiple attributes
    are provided.
*/
QLandmarkAttributeFilter::OperationType QLandmarkAttributeFilter::operationType() const
{
    Q_D(const QLandmarkAttributeFilter);
    return d->operationType;
}

/*!
    Sets the operation to be used by the filter when multiple attributes
    are provided to \a operationType.
*/
void QLandmarkAttributeFilter::setOperationType(QLandmarkAttributeFilter::OperationType operationType)
{
    Q_D(QLandmarkAttributeFilter);
    d->operationType = operationType;
}

/*!
    Returns the match flags for a particular \a key.  The match flags are only take into consideration
    when the attribute for a particular key is a string.  In all other cases the match flags are ignored.
*/
QLandmarkFilter::MatchFlags QLandmarkAttributeFilter::matchFlags(const QString &key) const
{
    Q_D(const QLandmarkAttributeFilter);
    return d->flags.value(key);
}

QTM_END_NAMESPACE
