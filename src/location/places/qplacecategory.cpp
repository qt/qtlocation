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

#include "qplacecategory.h"
#include "qplacecategory_p.h"

QT_BEGIN_NAMESPACE

QPlaceCategoryPrivate::QPlaceCategoryPrivate()
:   visibility(QtLocation::UnspecifiedVisibility)
{
}

QPlaceCategoryPrivate::QPlaceCategoryPrivate(const QPlaceCategoryPrivate &other)
:   QSharedData(other), categoryId(other.categoryId), name(other.name), visibility(other.visibility),
    icon(other.icon)
{
}

QPlaceCategoryPrivate::~QPlaceCategoryPrivate()
{
}

QPlaceCategoryPrivate &QPlaceCategoryPrivate::operator=(const QPlaceCategoryPrivate &other)
{
    categoryId = other.categoryId;
    name = other.name;
    icon = other.icon;
    return *this;
}

bool QPlaceCategoryPrivate::operator==(const QPlaceCategoryPrivate &other) const
{
    return (categoryId == other.categoryId)
            && (icon == other.icon)
            && (name == other.name);
}

/*!
    \class QPlaceCategory

    \inmodule QPlaces

    \brief The QPlaceCategory class represents a category object.

    Each QPlaceCategory represents a category object with a number of attributes
    such as name, id etc. Each QPlaceCategory is associated with place.

    Category objects are read-only, e.g. user of API might get business info object
    associated to specific place but can not edit its content. User might also create new
    contact object and add it to place.

    QPlaceCategory is an in memory representation of a contact object.
*/


/*
    Default constructor. Constructs a new category object.
*/
QPlaceCategory::QPlaceCategory()
    : d(new QPlaceCategoryPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceCategory::QPlaceCategory(const QPlaceCategory &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceCategory::~QPlaceCategory()
{
}

QPlaceCategory &QPlaceCategory::operator =(const QPlaceCategory &other) {
    d = other.d;
    return *this;
}

bool QPlaceCategory::operator==(const QPlaceCategory &other) const
{
    return d->categoryId == other.d->categoryId &&
           d->name == other.d->name &&
           d->visibility == other.d->visibility;
}

/*!
    Returns category id.
*/
QString QPlaceCategory::categoryId() const
{
    return d->categoryId;
}

/*!
    Sets category id to \a catID.
*/
void QPlaceCategory::setCategoryId(const QString &catID)
{
    d->categoryId = catID;
}

/*!
    Returns name of category.
*/
QString QPlaceCategory::name() const
{
    return d->name;
}

/*!
    Sets category name.
*/
void QPlaceCategory::setName(const QString &name)
{
    d->name = name;
}

/*!
    Sets the visibility of the category to \a visibility.
*/
void QPlaceCategory::setVisibility(QtLocation::Visibility visibility)
{
    d->visibility = visibility;
}

/*!
    Returns the visibility of the category.
*/
QtLocation::Visibility QPlaceCategory::visibility() const
{
    return d->visibility;
}

/*!
    Returns the category icon
*/
QPlaceIcon QPlaceCategory::icon() const
{
    return d->icon;
}

/*!
    Sets the category icon
*/
void QPlaceCategory::setIcon(const QPlaceIcon &icon)
{
    d->icon = icon;
}

QT_END_NAMESPACE
