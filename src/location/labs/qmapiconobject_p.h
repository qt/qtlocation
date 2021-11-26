/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#ifndef QGEOMAPICON_P_H
#define QGEOMAPICON_P_H

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

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qgeomapobject_p.h>
#include <QtCore/QUrl>
#include <QGeoCoordinate>
#include <QtCore/qsize.h>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QMapIconObject : public QGeoMapObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QVariant content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QSizeF iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)

public:
    QMapIconObject(QObject *parent = nullptr);
    ~QMapIconObject() override;

    QVariant content() const;
    QGeoCoordinate coordinate() const;
    QSizeF iconSize() const;

    void setContent(QVariant content);
    void setCoordinate(const QGeoCoordinate &coordinate);
    void setIconSize(const QSizeF &size);

    void setMap(QGeoMap *map) override;

signals:
    void contentChanged(QVariant content);
    void coordinateChanged(QGeoCoordinate coordinate);
    void iconSizeChanged();
};

QT_END_NAMESPACE

#endif // QGEOMAPICON_P_H
