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

#include "qgeoserviceproviderfactory.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGeoServiceProviderFactory
    \inmodule QtLocation
    \ingroup QtLocation-impl
    \since 5.6
    \deprecated

    \brief The QGeoServiceProviderFactory class is a factory class used as the
    plugin interface for services related to geographical information.

    \note There are no source or binary compatibility guarantees for the
    backend classes. The API is only guaranteed to work with the Qt version it
    was developed against. API changes will however only be made in minor
    releases. (6.6, 6.7, and so on.)

    Implementers must provide a unique combination of providerName() and
    providerVersion() per plugin.

    The other functions should be overridden if the plugin supports the
    associated set of functionality.
*/

/*!
    \fn QGeoServiceProviderFactory::~QGeoServiceProviderFactory()

    Destroys this QGeoServiceProviderFactory instance.
*/

/*!
    Returns a new QGeoCodingManagerEngine instance, initialized with \a
    parameters, which implements the location geocoding functionality.

    If \a error is not \nullptr it should be set to QGeoServiceProvider::NoError on
    success or an appropriate QGeoServiceProvider::Error on failure.

    If \a errorString is not \nullptr it should be set to a string describing any
    error which occurred.

    The default implementation returns \nullptr, which causes a
    QGeoServiceProvider::NotSupportedError in QGeoServiceProvider.
*/
QGeoCodingManagerEngine *
QGeoServiceProviderFactory::createGeocodingManagerEngine(const QVariantMap &parameters,
                                                         QGeoServiceProvider::Error *error,
                                                         QString *errorString) const
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    return nullptr;
}

/*!
    Returns a new QGeoMappingManagerEngine instance, initialized with \a
    parameters, which implements mapping functionality.

    If \a error is not \nullptr it should be set to QGeoServiceProvider::NoError on
    success or an appropriate QGeoServiceProvider::Error on failure.

    If \a errorString is not \nullptr it should be set to a string describing any
    error which occurred.

    The default implementation returns \nullptr, which causes a
    QGeoServiceProvider::NotSupportedError in QGeoServiceProvider.

    \internal
*/
QGeoMappingManagerEngine *
QGeoServiceProviderFactory::createMappingManagerEngine(const QVariantMap &parameters,
                                                       QGeoServiceProvider::Error *error,
                                                       QString *errorString) const
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    return nullptr;
}

/*!
    Returns a new QGeoRoutingManagerEngine instance, initialized with \a
    parameters, which implements routing functionality.

    If \a error is not \nullptr it should be set to QGeoServiceProvider::NoError on
    success or an appropriate QGeoServiceProvider::Error on failure.

    If \a errorString is not \nullptr it should be set to a string describing any
    error which occurred.

    The default implementation returns \nullptr, which causes a
    QGeoServiceProvider::NotSupportedError in QGeoServiceProvider.
*/
QGeoRoutingManagerEngine *
QGeoServiceProviderFactory::createRoutingManagerEngine(const QVariantMap &parameters,
                                                       QGeoServiceProvider::Error *error,
                                                       QString *errorString) const

{
    Q_UNUSED(parameters);
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    return nullptr;
}

/*!
    Returns a new QPlaceManagerEngine instance, initialized with \a
    parameters, which implements the place searching functionality.

    If \a error is not \nullptr it should be set to QGeoServiceProvider::NoError on
    success or an appropriate QGeoServiceProvider::Error on failure.

    If \a errorString is not \nullptr it should be set to a string describing any
    error which occurred.

    The default implementation returns \nullptr, which causes a
    QGeoServiceProvider::NotSupportedError in QGeoServiceProvider.
*/
QPlaceManagerEngine *
QGeoServiceProviderFactory::createPlaceManagerEngine(const QVariantMap &parameters,
                                                     QGeoServiceProvider::Error *error,
                                                     QString *errorString) const
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);
    Q_UNUSED(errorString);

    return nullptr;
}

/*!
    Notify the plugin when the qml engine is ready. In this moment the plugins can use it
    to register Image Providers.

    The default implementation does nothing.
    \since 5.12
*/
void QGeoServiceProviderFactory::setQmlEngine(QQmlEngine *engine)
{
    Q_UNUSED(engine);
}

QT_END_NAMESPACE
