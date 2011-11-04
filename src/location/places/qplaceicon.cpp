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

#include "qplaceicon.h"
#include "qplaceicon_p.h"
#include "qplacemanager.h"
#include "qplacemanagerengine.h"

QT_USE_NAMESPACE


QPlaceIconPrivate::QPlaceIconPrivate()
    : QSharedData(), manager(0)
{
}

QPlaceIconPrivate::QPlaceIconPrivate(const QPlaceIconPrivate&other)
    : QSharedData(other),
      manager(other.manager),
      baseUrl(other.baseUrl),
      fullUrl(other.fullUrl)
{
}

QPlaceIconPrivate::~QPlaceIconPrivate()
{
}

QPlaceIconPrivate &QPlaceIconPrivate::operator=(const QPlaceIconPrivate &other)
{
    manager = other.manager;
    baseUrl = other.baseUrl;
    fullUrl = other.fullUrl;
    return *this;
}

bool QPlaceIconPrivate::operator == (const QPlaceIconPrivate &other) const
{
    return manager == other.manager
            && baseUrl == other.baseUrl
            && fullUrl == other.fullUrl;
}

/*!
    Constructs an icon.
*/
QPlaceIcon::QPlaceIcon()
    : d(new QPlaceIconPrivate)
{
}

/*!
    Constructs a copy of \a other.
*/
QPlaceIcon::QPlaceIcon(const QPlaceIcon &other)
    : d(other.d)
{
}

/*!
    Destroys the icon.
*/
QPlaceIcon::~QPlaceIcon()
{
}

/*!
    Assigns \a other to this icon and returns a reference to this icon.
*/
QPlaceIcon &QPlaceIcon::operator=(const QPlaceIcon &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if this icon is equal to \a other.  Otherwise returns false.
*/
bool QPlaceIcon::operator==(const QPlaceIcon &other) const
{
    return *d == *(other.d);
}

/*!
    Returns an icon url according to the given \a size and \a flags.
    If a base url has been set by setBaseUrl(), the url to the image that best
    fits the specified parameters is returned.

    If a full url has been set by setUrl(), the full url is returned.

    If no manager has been assigned to the icon a default constructed QUrl
    is returned.
*/
QUrl QPlaceIcon::url(const QSize &size, QPlaceIcon::IconFlags flags) const
{
    if (!d->manager)
        return QUrl();

    if (!d->fullUrl.isEmpty())
        return d->fullUrl;


    return d->manager->d->constructIconUrl(*this, size, flags);
}

/*!
    Sets a full URL of the resource that represents the image of this
    icon.  Because a full URL is being set, specifying different
    sizes and flags into the url() function will have no effect.

    When calling the setUrl() function, the base url is implictly
    cleared.
*/
void QPlaceIcon::setFullUrl(const QUrl &url)
{
    d->fullUrl = url;
    d->baseUrl.clear();
}

QUrl QPlaceIcon::fullUrl() const
{
    return d->fullUrl;
}

/*!
    Returns a base url that the comlete icon url is based off.
    E.g. the base url may be http://example.com/icon
    When calling the url() function the base url may be used as a hint
    to construct http://example.com/icon_32x32_selected.png
*/
QUrl QPlaceIcon::baseUrl() const
{
    return d->baseUrl;
}

/*!
    Sets a base url that the complete icon url returned by url() is based off.

    When calling setBaseUrl() function, the full url set by
    setUrl() is implicitly cleared.
*/
void QPlaceIcon::setBaseUrl(const QUrl &url)
{
    d->baseUrl = url;
    d->fullUrl.clear();
}

/*!
    Returns the manager that this icon is associated with.
*/
QPlaceManager *QPlaceIcon::manager() const
{
    return d->manager;
}

/*!
    Sets the \a manager that this icon is associated with.
*/
void QPlaceIcon::setManager(QPlaceManager *manager)
{
    d->manager = manager;
}

/*!
    Returns a boolean indicating whether the all the fields of the icon are empty or not.
*/
bool QPlaceIcon::isEmpty() const
{
    return (d->baseUrl.isEmpty()
            && d->fullUrl.isEmpty()
            && d->manager == 0);
}
