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

#ifndef QGEOCODINGMANAGERENGINE_H
#define QGEOCODINGMANAGERENGINE_H

#include <QtLocation/qlocationglobal.h>
#include <QtCore/QObject>
#include <QtLocation/QGeoCodeReply>

QT_BEGIN_NAMESPACE

class QGeoAddress;
class QGeoCoordinate;
class QGeoShape;
class QGeoCodingManagerEnginePrivate;

class Q_LOCATION_EXPORT QGeoCodingManagerEngine : public QObject
{
    Q_OBJECT
public:
    explicit QGeoCodingManagerEngine(const QVariantMap &parameters, QObject *parent = nullptr);
    virtual ~QGeoCodingManagerEngine();

    QString managerName() const;
    int managerVersion() const;

    virtual QGeoCodeReply *geocode(const QGeoAddress &address, const QGeoShape &bounds);
    virtual QGeoCodeReply *geocode(const QString &address,
                                   int limit,
                                   int offset,
                                   const QGeoShape &bounds);
    virtual QGeoCodeReply *reverseGeocode(const QGeoCoordinate &coordinate,
                                          const QGeoShape &bounds);


    void setLocale(const QLocale &locale);
    QLocale locale() const;

Q_SIGNALS:
    void finished(QGeoCodeReply *reply);
    void errorOccurred(QGeoCodeReply *reply, QGeoCodeReply::Error error,
                       const QString &errorString = QString());

private:
    void setManagerName(const QString &managerName);
    void setManagerVersion(int managerVersion);

    QGeoCodingManagerEnginePrivate *d_ptr;
    Q_DISABLE_COPY(QGeoCodingManagerEngine)

    friend class QGeoServiceProvider;
    friend class QGeoServiceProviderPrivate;
};

QT_END_NAMESPACE

#endif
