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

#include "qlandmarkcategory.h"
#include "qlandmarkcategory_p.h"
#include "qlandmarkcategoryid.h"

#include <QStringList>
#include <QVariant>

#ifdef CATEGORYPRIVATE_DEBUG
#include <QDebug>
#endif

QTM_USE_NAMESPACE

// ----- QLandmarkCategoryPrivate -----

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate()
    : QSharedData(),
      name(QString()),
      iconUrl(QUrl()),
      id(QLandmarkCategoryId())
{
}

QLandmarkCategoryPrivate::QLandmarkCategoryPrivate(const QLandmarkCategoryPrivate &other)
    : QSharedData(other),
      name(other.name),
      iconUrl(other.iconUrl),
      id(other.id)
{
}

QLandmarkCategoryPrivate::~QLandmarkCategoryPrivate()
{
}

QLandmarkCategoryPrivate& QLandmarkCategoryPrivate::operator= (const QLandmarkCategoryPrivate & other)
{
    name = other.name;
    iconUrl = other.iconUrl;
    id = other.id;

    return *this;
}

bool QLandmarkCategoryPrivate::operator == (const QLandmarkCategoryPrivate &other) const
{

#ifdef CATEGORYPRIVATE_DEBUG
    qDebug() << "name: " << (name == other.name);
    qDebug() << "id:" << (id == other.id);
#endif
    return ((name == other.name)
            && (iconUrl == other.iconUrl)
            && (id == other.id));
}

/*!
    \class QLandmarkCategory

    \brief The QLandmarkCategory class designates a grouping of
    landmarks of similar type.

    \inmodule QtLocation

    \ingroup landmarks-main
    \since 1.1

    Landmarks of similar type may be grouped together into categories,
    e.g. restaurants, accommodation etc,  a QLandmarkCategory object
    represents one of these.

    More than one category can be assigned to a landmark.  Assignment
    of a category to a landmark is achieved by using the QLandmark::setCategoryIds()
    or QLandmark::addCategoryId() functions.

    Some categories may be considered read-only by a manager and cannot
    be saved.  To see if a category is read only, the id of the cateory must be passed
    into the QLandmarkManager::isReadOnly() function. Localization is only possible for categories that
    are read-only.

    A QLandmarkCategory instance is an in memory representation and may
    not reflect the state of the category found in persistent storage,
    until the appropriate synchronization method is called on the
    QLandmarkManager(e.g.\l {QLandmarkManager::saveCategory()} {saveCategory()},
    \l {QLandmarkManager::removeCategory()} {removeCategory()}).
*/

/*!
    Constructs an empty category.

    A new category will be assigned an invalid QLandmarkCategoryId.
*/
QLandmarkCategory::QLandmarkCategory()
    : d(new QLandmarkCategoryPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QLandmarkCategory::QLandmarkCategory(const QLandmarkCategory &other)
    : d(other.d)
{
}

/*!
    Destroys the category.
*/
QLandmarkCategory::~QLandmarkCategory()
{
}
/*!
    Assigns \a other to this category and returns a reference to this category.
*/
QLandmarkCategory& QLandmarkCategory::operator= (const QLandmarkCategory & other)
{
    *d = *(other.d);
    return *this;
}

/*!
    Returns true if this category is equal to \a other, otherwise
    returns false.

    \sa operator!=()
*/
bool QLandmarkCategory::operator== (const QLandmarkCategory &other) const
{
    return (*d == *(other.d));
}

/*!
    Returns true if this category is not equal to \a other, otherwise
    returns false.

    \sa operator==()
*/
bool QLandmarkCategory::operator!= (const QLandmarkCategory &other) const
{
    return !(*this == other);
}

/*!
    Returns the name of the category.
*/
QString QLandmarkCategory::name() const
{
    return d->name;
}

/*!
    Sets the \a name of the category.

    Using the default manager on the Symbian platform, the catgory name is retricted to a length of 124 characters.
*/
void QLandmarkCategory::setName(const QString &name)
{
    d->name = name;
}

/*!
    Returns the url of the category's icon.
*/
QUrl QLandmarkCategory::iconUrl() const
{
    return d->iconUrl;
}

/*!
    Sets the \a url of the category's icon
*/
void QLandmarkCategory::setIconUrl(const QUrl &url)
{
    d->iconUrl = url;
}


/*!
    Returns the identifier of the category.
*/
QLandmarkCategoryId QLandmarkCategory::categoryId() const
{
    return d->id;
}

/*!
    Sets the \a id of the category.

    Note that saving a new category using a QLandmarkManager
    will automatically assign the category a valid identifier.
*/
void QLandmarkCategory::setCategoryId(const QLandmarkCategoryId &id)
{
    d->id = id;
}

/*!
    Clears all the data fields of the category.
*/
void QLandmarkCategory::clear()
{
    d->name.clear();
    d->iconUrl.clear();
    d->id = QLandmarkCategoryId();
}
