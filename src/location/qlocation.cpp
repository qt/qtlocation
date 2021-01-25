/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include <QtLocation/QLocation>

QT_BEGIN_NAMESPACE

namespace QLocation {

/*!
    \namespace QLocation
    \inmodule QtLocation
    \keyword QLocation Namespace

    \brief The QLocation namespace contains miscellaneous identifiers used throughout the
           QtLocation module.
*/

/*!
    \enum QLocation::Visibility

    Defines the visibility of a QPlace or QPlaceCategory.

    \value UnspecifiedVisibility    No explicit visibility has been defined.
    \value DeviceVisibility         Places and categories with DeviceVisibility are only stored on
                                    the local device.
    \value PrivateVisibility        Places and categories with PrivateVisibility are only visible
                                    to the current user.  The data may be stored either locally or
                                    on a remote service or both.
    \value PublicVisibility         Places and categories with PublicVisibility are visible to
                                    everyone.

    A particular manager may support one or more visibility scopes.  For example a manager from one provider may only provide places
    that are public to everyone, whilst another may provide both public and private places.

    \note The meaning of unspecified visibility depends on the context it is used.

    When \e saving a place or category, the
    default visibility is unspecified meaning that the manager chooses an appropriate visibility scope for the item.

    When \e searching for places, unspecified means that places of any scope is returned.
*/
}

QT_END_NAMESPACE
