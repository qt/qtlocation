/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#ifndef QGEOROUTEPARSEROSRMV5_H
#define QGEOROUTEPARSEROSRMV5_H

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


#include <QtLocation/private/qgeorouteparser_p.h>
#include <QtCore/QUrlQuery>

QT_BEGIN_NAMESPACE

class QGeoRouteParserOsrmV5Private;

class Q_LOCATION_PRIVATE_EXPORT QGeoRouteParserOsrmV5Extension
{
public:
    QGeoRouteParserOsrmV5Extension()
    {
    }

    virtual ~QGeoRouteParserOsrmV5Extension()
    {
    }

    virtual void updateQuery(QUrlQuery &query) const = 0;
    virtual void updateSegment(QGeoRouteSegment &segment, const QJsonObject &step, const QJsonObject &maneuver) const = 0;
};

class Q_LOCATION_PRIVATE_EXPORT QGeoRouteParserOsrmV5 : public QGeoRouteParser
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGeoRouteParserOsrmV5)

public:
    QGeoRouteParserOsrmV5(QObject *parent = nullptr);
    virtual ~QGeoRouteParserOsrmV5();

    void setExtension(const QGeoRouteParserOsrmV5Extension *extension);

private:
    Q_DISABLE_COPY(QGeoRouteParserOsrmV5)
};

QT_END_NAMESPACE

#endif // QGEOROUTEPARSEROSRMV5_H
