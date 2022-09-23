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

#ifndef QGEOMAPTYPE_H
#define QGEOMAPTYPE_H

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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QVariantMap>
#include <QtLocation/private/qlocationglobal_p.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE

class QGeoCameraCapabilities;
class QGeoMapTypePrivate;
QT_DECLARE_QSDP_SPECIALIZATION_DTOR_WITH_EXPORT(QGeoMapTypePrivate, Q_LOCATION_PRIVATE_EXPORT)

class Q_LOCATION_PRIVATE_EXPORT QGeoMapType
{
    Q_GADGET
    QML_VALUE_TYPE(mapType)
    Q_ENUMS(MapStyle)

    Q_PROPERTY(MapStyle style READ style CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(bool mobile READ mobile CONSTANT)
    Q_PROPERTY(bool night READ night CONSTANT)
    Q_PROPERTY(QGeoCameraCapabilities cameraCapabilities READ cameraCapabilities CONSTANT)
    Q_PROPERTY(QVariantMap metadata READ metadata CONSTANT)

    Q_MOC_INCLUDE(<QtLocation/private/qgeocameracapabilities_p.h>)
public:
    enum MapStyle { // ### Qt6: change this to be a QFlags instead, or remove.
        NoMap = 0,
        StreetMap,
        SatelliteMapDay,
        SatelliteMapNight,
        TerrainMap,
        HybridMap,
        TransitMap,
        GrayStreetMap,
        PedestrianMap,
        CarNavigationMap,
        CycleMap,
        CustomMap = 100
    };

    QGeoMapType();
    QGeoMapType(const QGeoMapType &other) noexcept;
    QGeoMapType(QGeoMapType &&other) noexcept = default;
    QGeoMapType(MapStyle style, const QString &name, const QString &description, bool mobile,
                bool night, int mapId, const QByteArray &pluginName,
                const QGeoCameraCapabilities &cameraCapabilities,
                const QVariantMap &metadata = QVariantMap());
    ~QGeoMapType();

    QGeoMapType &operator=(const QGeoMapType &other) noexcept;
    QT_MOVE_ASSIGNMENT_OPERATOR_IMPL_VIA_MOVE_AND_SWAP(QGeoMapType)

    void swap(QGeoMapType &other) noexcept { d_ptr.swap(other.d_ptr); }

    friend inline bool operator==(const QGeoMapType &lhs, const QGeoMapType &rhs) noexcept
    { return lhs.isEqual(rhs); }
    friend inline bool operator!=(const QGeoMapType &lhs, const QGeoMapType &rhs) noexcept
    { return !lhs.isEqual(rhs); }

    MapStyle style() const;
    QString name() const;
    QString description() const;
    bool mobile() const;
    bool night() const;
    int mapId() const;
    QByteArray pluginName() const;
    QGeoCameraCapabilities cameraCapabilities() const;
    QVariantMap metadata() const;

private:
    QSharedDataPointer<QGeoMapTypePrivate> d_ptr;

    bool isEqual(const QGeoMapType &other) const noexcept;
};

namespace QGeoMapTypeForeignNamespace
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QGeoMapType)
    QML_NAMED_ELEMENT(MapType)
};

QT_END_NAMESPACE

#endif // QGEOMAPTYPE_H
