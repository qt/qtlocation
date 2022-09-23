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

#include <QtLocation/qplaceratings.h>
#include <QtLocation/qplaceicon.h>
#include <QtLocation/qplacesupplier.h>

QT_BEGIN_NAMESPACE

namespace {

bool convertToGadget(const QMetaObject &metaObject, const QVariantMap &map, void *gadget)
{
    for (auto &&[key, value] : map.asKeyValueRange()) {
        const int propIndex = metaObject.indexOfProperty(key.toUtf8());
        if (propIndex == -1) {
            qCritical("No property %s in %s", qPrintable(key), metaObject.className());
            return false;
        }
        const QMetaProperty prop = metaObject.property(propIndex);
        bool successfulWrite = false;
        if (value.metaType() != prop.metaType()) {
            QVariant coercedValue = value;
            if (!coercedValue.convert(prop.metaType())) {
                qCritical("Could not convert value from %s to %s for property %s::%s",
                          value.typeName(), prop.typeName(), metaObject.className(), qPrintable(key));
                return false;
            }
            successfulWrite = prop.writeOnGadget(gadget, coercedValue);
        } else {
            successfulWrite = prop.writeOnGadget(gadget, value);
        }
        if (!successfulWrite) {
            qCritical("Could not set property %s on %s", qPrintable(key), metaObject.className());
            return false;
        }
    }
    return true;
}

template<typename SourceType, typename GadgetType>
bool converterToGadget(const void *src, void *gadget)
{
    const QMetaObject &metaObject = GadgetType::staticMetaObject;
    QVariantMap variantMap;
    if constexpr (std::is_same_v<SourceType, QJSValue>) {
        const QJSValue &jsValue = *static_cast<const QJSValue *>(src);
        const QVariant &variant = jsValue.toVariant();
        if (variant.metaType() != QMetaType::fromType<QVariantMap>())
            return false;
        variantMap = variant.toMap();
    } else {
        static_assert(std::is_same_v<SourceType, QVariantMap>);
        variantMap = *static_cast<const QVariantMap *>(src);
    }
    return convertToGadget(metaObject, variantMap, gadget);
}

template<typename GadgetType>
bool registerConverterToGadget()
{
    if (!QMetaType::registerConverterFunction(converterToGadget<QJSValue, GadgetType>,
                    QMetaType::fromType<QJSValue>(), QMetaType::fromType<GadgetType>())) {
        qCritical("Failed to register conversion function from QJSValue to %s",
                  GadgetType::staticMetaObject.className());
        return false;
    }
    if (!QMetaType::registerConverterFunction(converterToGadget<QVariantMap, GadgetType>,
                    QMetaType::fromType<QVariantMap>(), QMetaType::fromType<GadgetType>())) {
        qCritical("Failed to register conversion function from QVariantMap to %s",
                  GadgetType::staticMetaObject.className());
        return false;
    }

    return true;
}

} // anonymous namespace

void registerConverters()
{
    registerConverterToGadget<QPlaceRatings>();
    registerConverterToGadget<QPlaceIcon>();
    registerConverterToGadget<QPlaceSupplier>();
}

Q_CONSTRUCTOR_FUNCTION(registerConverters);



QT_END_NAMESPACE
