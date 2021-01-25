/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Copyright (C) 2018 Julian Sherollari <jdotsh@gmail.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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
**
**
**
****************************************************************************/

#ifndef QGEOJSON_H
#define QGEOJSON_H

#include <QtCore/qvariant.h>
#include <QtCore/qjsondocument.h>
#include <QtLocation/private/qlocationglobal_p.h>

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


QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QGeoJson
{
public:

    // This method imports a GeoJSON file to a QVariantList
    static QVariantList importGeoJson(const QJsonDocument &doc);

    // This method exports a GeoJSON file from a QVariantList
    static QJsonDocument exportGeoJson(const QVariantList &list);

    // This method exports the content of the imported QVariantList in a
    // readable format
    static QString toString(const QVariantList
    &importedGeoJson);

};

QT_END_NAMESPACE

#endif // QGEOJSON_H
