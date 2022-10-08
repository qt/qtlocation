/****************************************************************************
**
** Copyright (C) 2017 Mapbox, Inc.
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

#ifndef QQMAPBOXGLSTYLECHANGE_P_H
#define QQMAPBOXGLSTYLECHANGE_P_H

#include <QtCore/QList>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtGui/QImage>
#include <QtLocation/private/qdeclarativecirclemapitem_p.h>
#include <QtLocation/private/qdeclarativegeomapitembase_p.h>
#include <QtLocation/private/qdeclarativepolygonmapitem_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>
#include <QtLocation/private/qdeclarativerectanglemapitem_p.h>

#include <QMapboxGL>

class QMapboxGLStyleChange
{
public:
    virtual ~QMapboxGLStyleChange() = default;

    static QList<QSharedPointer<QMapboxGLStyleChange>> addMapItem(QDeclarativeGeoMapItemBase *, const QString &before);
    static QList<QSharedPointer<QMapboxGLStyleChange>> removeMapItem(QDeclarativeGeoMapItemBase *);

    virtual void apply(QMapboxGL *map) = 0;
};

class QMapboxGLStyleSetLayoutProperty : public QMapboxGLStyleChange
{
public:
    static QList<QSharedPointer<QMapboxGLStyleChange>> fromMapItem(QDeclarativeGeoMapItemBase *);

    void apply(QMapboxGL *map) override;

private:
    static QList<QSharedPointer<QMapboxGLStyleChange>> fromMapItem(QDeclarativePolylineMapItem *);

    QMapboxGLStyleSetLayoutProperty() = default;
    QMapboxGLStyleSetLayoutProperty(const QString &layer, const QString &property, const QVariant &value);

    QString m_layer;
    QString m_property;
    QVariant m_value;
};

class QMapboxGLStyleSetPaintProperty : public QMapboxGLStyleChange
{
public:
    static QList<QSharedPointer<QMapboxGLStyleChange>> fromMapItem(QDeclarativeGeoMapItemBase *);

    void apply(QMapboxGL *map) override;

private:
    static QList<QSharedPointer<QMapboxGLStyleChange>> fromMapItem(QDeclarativeRectangleMapItem *);
    static QList<QSharedPointer<QMapboxGLStyleChange>> fromMapItem(QDeclarativeCircleMapItem *);
    static QList<QSharedPointer<QMapboxGLStyleChange>> fromMapItem(QDeclarativePolygonMapItem *);
    static QList<QSharedPointer<QMapboxGLStyleChange>> fromMapItem(QDeclarativePolylineMapItem *);

    QMapboxGLStyleSetPaintProperty() = default;
    QMapboxGLStyleSetPaintProperty(const QString &layer, const QString &property, const QVariant &value);

    QString m_layer;
    QString m_property;
    QVariant m_value;
};

class QMapboxGLStyleAddLayer : public QMapboxGLStyleChange
{
public:
    static QSharedPointer<QMapboxGLStyleChange> fromFeature(const QMapbox::Feature &feature, const QString &before);

    void apply(QMapboxGL *map) override;

private:
    QMapboxGLStyleAddLayer() = default;

    QVariantMap m_params;
    QString m_before;
};

class QMapboxGLStyleRemoveLayer : public QMapboxGLStyleChange
{
public:
    explicit QMapboxGLStyleRemoveLayer(const QString &id);

    void apply(QMapboxGL *map) override;

private:
    QMapboxGLStyleRemoveLayer() = default;

    QString m_id;
};

class QMapboxGLStyleAddSource : public QMapboxGLStyleChange
{
public:
    static QSharedPointer<QMapboxGLStyleChange> fromFeature(const QMapbox::Feature &feature);
    static QSharedPointer<QMapboxGLStyleChange> fromMapItem(QDeclarativeGeoMapItemBase *);

    void apply(QMapboxGL *map) override;

private:
    QMapboxGLStyleAddSource() = default;

    QString m_id;
    QVariantMap m_params;
};

class QMapboxGLStyleRemoveSource : public QMapboxGLStyleChange
{
public:
    explicit QMapboxGLStyleRemoveSource(const QString &id);

    void apply(QMapboxGL *map) override;

private:
    QMapboxGLStyleRemoveSource() = default;

    QString m_id;
};

class QMapboxGLStyleSetFilter : public QMapboxGLStyleChange
{
public:
    void apply(QMapboxGL *map) override;

private:
    QMapboxGLStyleSetFilter() = default;

    QString m_layer;
    QVariant m_filter;
};

class QMapboxGLStyleAddImage : public QMapboxGLStyleChange
{
public:
    void apply(QMapboxGL *map) override;

private:
    QMapboxGLStyleAddImage() = default;

    QString m_name;
    QImage m_sprite;
};

#endif // QQMAPBOXGLSTYLECHANGE_P_H
