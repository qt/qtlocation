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

#ifndef QGEOMAPPINGMANAGERENGINE_H
#define QGEOMAPPINGMANAGERENGINE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include <QList>
#include <QtLocation/private/qlocationglobal_p.h>

QT_BEGIN_NAMESPACE

class QLocale;
class QString;

class QGeoRectangle;
class QGeoCoordinate;
class QGeoCameraCapabilities;
class QGeoMappingManagerPrivate;
class QGeoMapRequestOptions;

class QGeoMappingManagerEnginePrivate;
class QGeoMap;
class QGeoMapType;

class Q_LOCATION_PRIVATE_EXPORT QGeoMappingManagerEngine : public QObject
{
    Q_OBJECT

public:
    explicit QGeoMappingManagerEngine(QObject *parent = nullptr);
    virtual ~QGeoMappingManagerEngine();

    virtual QGeoMap *createMap() = 0;

    QVariantMap parameters() const;

    QString managerName() const;
    int managerVersion() const;

    QList<QGeoMapType> supportedMapTypes() const;

    // the class is private, so this can be virtual here for now.
    QGeoCameraCapabilities cameraCapabilities(int mapId = 0) const;

    void setLocale(const QLocale &locale);
    QLocale locale() const;

    bool isInitialized() const;

Q_SIGNALS:
    void initialized();
    void supportedMapTypesChanged();

protected:
    void setSupportedMapTypes(const QList<QGeoMapType> &supportedMapTypes);
    void setCameraCapabilities(const QGeoCameraCapabilities &capabilities);

    void engineInitialized();

private:
    QGeoMappingManagerEnginePrivate *d_ptr;

    void setManagerName(const QString &managerName);
    void setManagerVersion(int managerVersion);

    Q_DECLARE_PRIVATE(QGeoMappingManagerEngine)
    Q_DISABLE_COPY(QGeoMappingManagerEngine)

    friend class QGeoServiceProvider;
    friend class QGeoServiceProviderPrivate;
};

QT_END_NAMESPACE

#endif
