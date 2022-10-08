/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeotiledmap_test.h"
#include <QtCore/QMetaProperty>
#include <QtPositioning/QGeoCoordinate>
#include <QtLocation/private/qgeotiledmap_p_p.h>

QT_USE_NAMESPACE

class QGeoTiledMapTestPrivate: public QGeoTiledMapPrivate
{
    Q_DECLARE_PUBLIC(QGeoTiledMapTest)
public:
    QGeoTiledMapTestPrivate(QGeoTiledMappingManagerEngine *engine,
                            const QGeoTiledMapTestOptions &options)
        : QGeoTiledMapPrivate(engine), m_options(options)
    {

    }

    ~QGeoTiledMapTestPrivate()
    {

    }

    void setVisibleArea(const QRectF &visibleArea) override
    {
        if (m_options.supportVisibleArea)
            return QGeoTiledMapPrivate::setVisibleArea(visibleArea);
        else
            return QGeoMapPrivate::setVisibleArea(visibleArea);
    }

    QRectF visibleArea() const override
    {
        if (m_options.supportVisibleArea)
            return QGeoTiledMapPrivate::visibleArea();
        else
            return QGeoMapPrivate::visibleArea();
    }

    QGeoTiledMapTestOptions m_options;
};

QGeoTiledMapTest::QGeoTiledMapTest(QGeoTiledMappingManagerEngine *engine,
                                   const QGeoTiledMapTestOptions &options,
                                   QObject *parent)
:   QGeoTiledMap(*new QGeoTiledMapTestPrivate(engine, options), engine, parent), m_engine(engine)
{
}
