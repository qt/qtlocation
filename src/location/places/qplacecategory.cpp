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

/*!
    \class QPlaceCategory
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since QtLocation 5.0

    \brief The QPlaceCategory class represents a category that a \l QPlace can be associated with.

    Categories are used to search for places based on the categories they are associated with.  The
    list/tree of available categories can be obtained from \l QPlaceManager.  The
    \l QPlaceSearchRequest::setCategories() function can be used to limit the search results to
    places with the specified categories.

    If the \l QGeoServiceProvider supports it, categories can be created and removed.  This
    functionality is available in the \l QPlaceManager class.
*/

/*!
    \fn bool QPlaceCategory::operator!=(const QPlaceCategory &other) const

    Returns true if \a other is not equal to this category; otherwise returns false.
*/

/*!
    Constructs a category.
*/
QPlaceCategory::QPlaceCategory()
    : d(new QPlaceCategoryPrivate)
{
}

/*!
    Constructs a category which is a copy of \a other.
*/
QPlaceCategory::QPlaceCategory(const QPlaceCategory &other)
    :d(other.d)
{
}

/*!
    Destroys the category.
*/
QPlaceCategory::~QPlaceCategory()
{
}

/*!
    Assigns \a other to this category and returns a reference to this category.
*/
QPlaceCategory &QPlaceCategory::operator =(const QPlaceCategory &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if \a other is equal to this category; otherwise returns false.
*/
bool QPlaceCategory::operator==(const QPlaceCategory &other) const
{
    return d->categoryId == other.d->categoryId &&
           d->name == other.d->name &&
           d->visibility == other.d->visibility &&
           d->icon == other.d->icon;
}

/*!
    Returns the id of the category.  The category id is a string which uniquely identifies this category
    within a particular \l QPlaceManager.  The id is only meaningful to the QPlaceManager
    that generated it and is not transferrable between managers.
*/
QString QPlaceCategory::categoryId() const
{
    return d->categoryId;
}

/*!
    Sets the \a id of the category.
*/
void QPlaceCategory::setCategoryId(const QString &id)
{
    d->categoryId = id;
}

/*!
    Returns the name of category.
*/
QString QPlaceCategory::name() const
{
    return d->name;
}

/*!
    Sets the \a name of the category.
*/
void QPlaceCategory::setName(const QString &name)
{
    d->name = name;
}

/*!
    Sets the \a visibility of the category.
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
    Returns the icon associated with the category.
*/
QPlaceIcon QPlaceCategory::icon() const
{
    return d->icon;
}

/*!
    Sets the \a icon of the category.
*/
void QPlaceCategory::setIcon(const QPlaceIcon &icon)
{
    d->icon = icon;
}

QT_END_NAMESPACE
