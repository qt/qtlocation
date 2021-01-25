/****************************************************************************
**
** Copyright (C) 2013-2018 Esri <contracts@esri.com>
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
**
**
**
****************************************************************************/

#ifndef GEOMAPSOURCE_H
#define GEOMAPSOURCE_H

#include <QtLocation/private/qgeomaptype_p.h>
#include <QtLocation/private/qgeocameracapabilities_p.h>

QT_BEGIN_NAMESPACE

class GeoMapSource : public QGeoMapType
{
public:
    GeoMapSource(QGeoMapType::MapStyle style, const QString &name,
                 const QString &description, bool mobile, bool night, int mapId,
                 const QString &url, const QString &copyright, const QGeoCameraCapabilities &cameraCapabilities);

    inline const QString &url() const;
    inline const QString &copyright() const;

    static QString toFormat(const QString &url);
    static QGeoMapType::MapStyle mapStyle(const QString &styleString);

private:
    QString m_url;
    QString m_copyright;
};

inline const QString &GeoMapSource::url() const
{
    return m_url;
}

inline const QString &GeoMapSource::copyright() const
{
    return m_copyright;
}

QT_END_NAMESPACE

#endif // GEOMAPSOURCE_H
