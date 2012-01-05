/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
    \class QPlaceIcon
    \inmodule QtLocation
    \ingroup QtLocation-places
    \ingroup QtLocation-places-data
    \since QtLocation 5.0

    \brief The QPlaceIcon class represents an icon.

    \section2 Usage
    The typical usage of an icon is to use the url() function to specify
    a preferred size and set of flags.

    \snippet snippets/places/requesthandler.h icon

    Note that the parameters are \e {preferred} only.  If a manager backend
    does not support one or more of the specified parameters, the url of the icon that most
    closely matches those parameters is returned.

    \target Icon internals
    \section2 Internals
    Icons are tightly coupled to a particular manager and always have a pointer
    to that manager.  The icon does not have ownership of this pointer.

    The internals of the icon work by specifying either a \e {base} or a
    \e {full} url.

    A \e {base} url may be an incomplete url of the form \e {http://example.com/icon}.
    When a set of icon parameters is provided to the url() function, the manager
    constructs a complete icon url such as \e {http://example.com/icon_32x32_selected.png}.

    A \e {full} url is a complete url which may look something like \e {http://example.com/myicon.png}
    When a full url is specified the url() will always return the complete url.

    Only one \e {base} or \e {full} url may be specified for a single icon, setting one implies clearing the other.
    Whether full and or base urls are supported depends on the manager backend.

    Any valid URL may be returned by the backend, but it would typically
    be either a http://, file://, or data:// URL.
*/

/*!
    \enum QPlaceIcon::IconFlag

    This enum is used to specify different icon states and types.

    The state flags are:
    \value Normal   An icon with no state modifications.  This flag indicates that the user is not
                    interacting with the icon, but the functionality represented by the icon is
                    available.
    \value Disabled An icon with a disabled appearance.  This flag indicates that the functionality
                    represented by the icon is not available.
    \value Active   An icon with an active appearance.  This flag indicates that the functionality
                    represented by the icon is available and the user is interacting with the icon,
                    for example, touching it.
    \value Selected An icon with a selected appearance.  This flag indicates that the item represented
               by the icon is selected.


    The type flags are:
    \value Map An icon intended for display on a map
    \value List An icon intended for display in a list.

    You can use at most one state and one type flag at a time.
*/

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
    Returns true if this icon is equal to \a other, otherwise returns false.
*/
bool QPlaceIcon::operator==(const QPlaceIcon &other) const
{
    return *d == *(other.d);
}

/*!
    \fn QPlaceIcon::operator!=(const QPlaceIcon &other) const

    Returns true if \a other is not equal to this icon, otherwise returns false.
*/

/*!
    Returns an icon url according to the given \a size and \a flags.
    If a base url has been set by setBaseUrl(), the url to the image that best
    fits the specified parameters is returned.

    If a full url has been set by setFullUrl(), the full url is returned.

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
    Sets a full \a url of the resource that represents the image of this
    icon.  Because a full URL is being set, specifying different
    sizes and flags into the url() function will have no effect.

    When calling the this function, the baseUrl() is implictly
    cleared.
*/
void QPlaceIcon::setFullUrl(const QUrl &url)
{
    d->fullUrl = url;
    d->baseUrl.clear();
}

/*!
    Returns a the full url that the icon is based off.
    \sa baseUrl()
*/
QUrl QPlaceIcon::fullUrl() const
{
    return d->fullUrl;
}

/*!
    Returns a base url that the complete icon url is based off.

    E.g. the base url may be http://example.com/icon.
    When calling the url() function the, base url may be used to construct: http://example.com/icon_32x32_selected.png
*/
QUrl QPlaceIcon::baseUrl() const
{
    return d->baseUrl;
}

/*!
    Sets a base \a url that the complete icon url returned by url() is based off.

    When calling this function, the fullUrl() is implicitly cleared.
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
    Sets the \a manager that this icon is associated with.  The icon does not take
    ownership of the pointer.
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
