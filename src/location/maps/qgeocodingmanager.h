/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#ifndef QGEOCODINGMANAGER_H
#define QGEOCODINGMANAGER_H

#include <QtLocation/QGeoCodeReply>
#include <QtPositioning/QGeoShape>

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QLocale;

class QGeoAddress;
class QGeoCodingManagerEngine;
class QGeoCodingManagerPrivate;

class Q_LOCATION_EXPORT QGeoCodingManager : public QObject
{
    Q_OBJECT
public:
    ~QGeoCodingManager();

    QString managerName() const;
    int managerVersion() const;

    QGeoCodeReply *geocode(const QGeoAddress &address,
                           const QGeoShape &bounds = QGeoShape());
    QGeoCodeReply *geocode(const QString &searchString,
                            int limit = -1,
                            int offset = 0,
                           const QGeoShape &bounds = QGeoShape());

    QGeoCodeReply *reverseGeocode(const QGeoCoordinate &coordinate,
                                  const QGeoShape &bounds = QGeoShape());

    void setLocale(const QLocale &locale);
    QLocale locale() const;

Q_SIGNALS:
    void finished(QGeoCodeReply *reply);
    void errorOccurred(QGeoCodeReply *reply, QGeoCodeReply::Error error,
                       const QString &errorString = QString());

private:
    explicit QGeoCodingManager(QGeoCodingManagerEngine *engine, QObject *parent = nullptr);

    QGeoCodingManagerPrivate *d_ptr;
    Q_DISABLE_COPY(QGeoCodingManager)

    friend class QGeoServiceProvider;
    friend class QGeoServiceProviderPrivate;
};

QT_END_NAMESPACE

#endif
