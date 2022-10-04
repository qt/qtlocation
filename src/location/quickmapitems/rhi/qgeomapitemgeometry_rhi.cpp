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

#include "qgeomapitemgeometry_rhi_p.h"

#include <QThreadPool>
#include <QRunnable>

#include <QtQuick/QSGGeometry>
#include <QtPositioning/private/qwebmercator_p.h>
#include <QtPositioning/private/qlocationutils_p.h>
#include <QtPositioning/QGeoCircle>
#include <QtPositioning/QGeoPolygon>
#include <QtPositioning/QGeoPath>
#include <QtPositioning/QGeoRectangle>

#include <QtLocation/private/qgeomap_p.h>
#include <QtLocation/private/qdeclarativecirclemapitem_p_p.h>
#include <QtLocation/private/qgeosimplify_p.h>

/* poly2tri triangulator includes */
#include <earcut.hpp>
#include <array>

QT_BEGIN_NAMESPACE

/*!
    \internal
    Implementation of QGeoMapItemLODGeometry
*/

struct ThreadPool // to have a thread pool with max 1 thread for geometry processing
{
    ThreadPool ()
    {
        m_threadPool.setMaxThreadCount(1);
    }

    void start(QRunnable *runnable, int priority = 0)
    {
        m_threadPool.start(runnable, priority);
    }

    QThreadPool m_threadPool;
};

Q_GLOBAL_STATIC(ThreadPool, threadPool)

class PolylineSimplifyTask : public QRunnable
{
public:
    PolylineSimplifyTask(const QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>
                                 &input, // reference as it gets copied in the nested call
                         const QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>> &output,
                         double leftBound, unsigned int zoom, QSharedPointer<unsigned int> &working)
        : m_zoom(zoom), m_leftBound(leftBound), m_input(input), m_output(output), m_working(working)
    {
        Q_ASSERT(!input.isNull());
        Q_ASSERT(!output.isNull());
    }

    void run() override
    {
        // Skip sending notifications for now. Updated data will be picked up eventually.
        // ToDo: figure out how to connect a signal from here to a slot in the item.
        *m_working = QGeoMapItemLODGeometry::zoomToLOD(m_zoom);
        const QList<QDeclarativeGeoMapItemUtils::vec2> res =
                QGeoMapItemLODGeometry::getSimplified(
                        *m_input, m_leftBound, QGeoMapItemLODGeometry::zoomForLOD(m_zoom));
        *m_output = res;
        *m_working = 0;
    }

    unsigned int m_zoom;
    double m_leftBound;
    QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>> m_input, m_output;
    QSharedPointer<unsigned int> m_working;
};


QGeoMapItemLODGeometry::QGeoMapItemLODGeometry()
{
    resetLOD();
}

void QGeoMapItemLODGeometry::resetLOD()
{
    // New pointer, some old LOD task might still be running and operating on the old pointers.
    m_verticesLOD[0] = QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>(
            new QList<QDeclarativeGeoMapItemUtils::vec2>);
    for (unsigned int i = 1; i < m_verticesLOD.size(); ++i)
        m_verticesLOD[i] = nullptr; // allocate on first use
    m_screenVertices = m_verticesLOD.front().data(); // resetting pointer to data to be LOD 0
}

QList<QDeclarativeGeoMapItemUtils::vec2> QGeoMapItemLODGeometry::getSimplified(
        QList<QDeclarativeGeoMapItemUtils::vec2>
                &wrappedPath, // reference as it gets copied in the nested call
        double leftBoundWrapped, unsigned int zoom)
{
    // Try a simplify step
    QList<QDoubleVector2D> data;
    for (auto e: wrappedPath)
        data << e.toDoubleVector2D();
    const QList<QDoubleVector2D> simplified = QGeoSimplify::geoSimplifyZL(data,
                                                                leftBoundWrapped,
                                                                zoom);

    data.clear();
    QList<QDeclarativeGeoMapItemUtils::vec2> simple;
    for (auto e: simplified)
        simple << e;
    return simple;
}

bool QGeoMapItemLODGeometry::isLODActive(unsigned int lod) const
{
    return m_screenVertices == m_verticesLOD[zoomToLOD(lod)].data();
}

bool QGeoMapItemLODGeometry::selectLODOnLODMismatch(unsigned int zoom, double leftBound,
                                                    bool closed) const
{
    if (*m_working > 0)
        return false;
    const_cast<QGeoMapItemLODGeometry *>(this)->selectLOD(zoom, leftBound, closed);
    return true;
}

void QGeoMapItemLODGeometry::enqueueSimplificationTask(
        const QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>> &input,
        const QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>> &output, double leftBound,
        unsigned int zoom, QSharedPointer<unsigned int> &working)
{
    Q_ASSERT(!input.isNull());
    Q_ASSERT(!output.isNull());
    PolylineSimplifyTask *task = new PolylineSimplifyTask(input,
                                                          output,
                                                          leftBound,
                                                          zoom,
                                                          working);
    threadPool->start(task);
}

void QGeoMapItemLODGeometry::selectLOD(unsigned int zoom, double leftBound, bool /* closed */) // closed to tell if this is a polygon or a polyline.
{
    unsigned int requestedLod = zoomToLOD(zoom);
    if (!m_verticesLOD[requestedLod].isNull()) {
        m_screenVertices = m_verticesLOD[requestedLod].data();
    } else if (!m_verticesLOD.at(0)->isEmpty()) {
        // if here, zoomToLOD != 0 and no current working task.
        // So select the last filled LOD != m_working (lower-bounded by 1,
        // guaranteed to exist), and enqueue the right one
        m_verticesLOD[requestedLod] = QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>(
                new QList<QDeclarativeGeoMapItemUtils::vec2>);

        for (unsigned int i = requestedLod - 1; i >= 1; i--) {
            if (*m_working != i && !m_verticesLOD[i].isNull()) {
                m_screenVertices = m_verticesLOD[i].data();
                break;
            } else if (i == 1) {
                // get 1 synchronously if not computed already
                m_verticesLOD[1] = QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>(
                        new QList<QDeclarativeGeoMapItemUtils::vec2>);
                *m_verticesLOD[1] = getSimplified(*m_verticesLOD[0], leftBound, zoomForLOD(0));
                if (requestedLod == 1)
                    return;
            }
        }

        enqueueSimplificationTask(m_verticesLOD.at(0),
                                  m_verticesLOD[requestedLod],
                                  leftBound,
                                  zoom,
                                  m_working);

    }
}

void QGeoMapItemLODGeometry::selectLODOnDataChanged(unsigned int zoom, double leftBound) const
{
    unsigned int lod = zoomToLOD(zoom);
    if (lod > 0) {
        // Generate ZL 1 as fallback for all cases != 0. Do not do if 0 is requested
        // (= old behavior, LOD disabled)
        m_verticesLOD[1] = QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>(
                new QList<QDeclarativeGeoMapItemUtils::vec2>);
        *m_verticesLOD[1] = getSimplified( *m_verticesLOD[0], leftBound, zoomForLOD(0));
    }
    if (lod > 1) {
        if (!m_verticesLOD[lod]) {
            m_verticesLOD[lod] = QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>(
                    new QList<QDeclarativeGeoMapItemUtils::vec2>);
        }
        enqueueSimplificationTask(m_verticesLOD.at(0),
                                  m_verticesLOD[lod],
                                  leftBound,
                                  zoom,
                                  m_working);
    }
    m_screenVertices = m_verticesLOD[qMin<unsigned int>(lod, 1)].data(); // return only 0,1 synchronously
}

unsigned int QGeoMapItemLODGeometry::zoomToLOD(unsigned int zoom)
{
    unsigned int res;
    if (zoom > 20)
        res = 0;
    else
        res = qBound<unsigned int>(3, zoom, 20) / 3; // bound LOD'ing between ZL 3 and 20. Every 3 ZoomLevels
    return res;
}

unsigned int QGeoMapItemLODGeometry::zoomForLOD(unsigned int zoom)
{
    unsigned int res = (qBound<unsigned int>(3, zoom, 20) / 3) * 3;
    if (zoom < 6)
        return res;
    return res + 1; // give more resolution when closing in
}

// wrapPath always preserves the geometry
// This one handles holes
static void wrapPath(const QGeoPolygon &poly
                     ,const QGeoCoordinate &geoLeftBound
                     ,const QGeoProjectionWebMercator &p
                     ,QList<QList<QDoubleVector2D> > &wrappedPaths
                     ,QDoubleVector2D *leftBoundWrapped = nullptr)
{
    QList<QList<QDoubleVector2D> > paths;
    for (qsizetype i = 0; i < 1 + poly.holesCount(); ++i) {
        QList<QDoubleVector2D> path;
        if (!i) {
            for (const QGeoCoordinate &c : poly.perimeter())
                path << p.geoToMapProjection(c);
        } else {
            for (const QGeoCoordinate &c : poly.holePath(i-1))
                path << p.geoToMapProjection(c);
        }
        paths.append(path);
    }

    const QDoubleVector2D leftBound = p.geoToMapProjection(geoLeftBound);
    wrappedPaths.clear();

    QList<QDoubleVector2D> wrappedPath;
    // compute 3 sets of "wrapped" coordinates: one w regular mercator, one w regular mercator +- 1.0
    for (const auto &path : paths) {
        wrappedPath.clear();
        for (QDoubleVector2D coord : path) {
            // We can get NaN if the map isn't set up correctly, or the projection
            // is faulty -- probably best thing to do is abort
            if (!qIsFinite(coord.x()) || !qIsFinite(coord.y())) {
                wrappedPaths.clear();
                return;
            }

            const bool isPointLessThanUnwrapBelowX = (coord.x() < leftBound.x());
            // unwrap x to preserve geometry if moved to border of map
            if (isPointLessThanUnwrapBelowX)
                coord.setX(coord.x() + 1.0);
            wrappedPath.append(coord);
        }
        wrappedPaths.append(wrappedPath);
    }

    if (leftBoundWrapped)
        *leftBoundWrapped = leftBound;
}

static void cutPathEars(const QList<QList<QDoubleVector2D>> &wrappedPaths,
                        QList<QDeclarativeGeoMapItemUtils::vec2> &screenVertices,
                        QList<quint32> &screenIndices)
{
    using Coord = double;
    using N = uint32_t;
    using Point = std::array<Coord, 2>;
    screenVertices.clear();
    screenIndices.clear();

    std::vector<std::vector<Point>> polygon;
    std::vector<Point> poly;

    for (const QList<QDoubleVector2D> &wrappedPath: wrappedPaths) {
        poly.clear();
        for (const QDoubleVector2D &v: wrappedPath) {
            screenVertices << v;
            Point pt = {{ v.x(), v.y() }};
            poly.push_back( pt );
        }
        polygon.push_back(poly);
    }

    std::vector<N> indices = qt_mapbox::earcut<N>(polygon);

    for (const auto &i: indices)
        screenIndices << quint32(i);
}

static void cutPathEars(const QList<QDoubleVector2D> &wrappedPath,
                        QList<QDeclarativeGeoMapItemUtils::vec2> &screenVertices,
                        QList<quint32> &screenIndices)
{
    using Coord = double;
    using N = uint32_t;
    using Point = std::array<Coord, 2>;
    screenVertices.clear();
    screenIndices.clear();

    std::vector<std::vector<Point>> polygon;
    std::vector<Point> poly;

    for (const QDoubleVector2D &v: wrappedPath) {
        screenVertices << v;
        Point pt = {{ v.x(), v.y() }};
        poly.push_back( pt );
    }
    polygon.push_back(poly);

    std::vector<N> indices = qt_mapbox::earcut<N>(polygon);

    for (const auto &i: indices)
        screenIndices << quint32(i);
}

void QGeoMapPolygonGeometryOpenGL::allocateAndFillPolygon(QSGGeometry *geom) const
{

    const QList<QDeclarativeGeoMapItemUtils::vec2> &vx = m_screenVertices;
    const QList<quint32> &ix = m_screenIndices;

    geom->allocate(vx.size(), ix.size());
    if (geom->indexType() == QSGGeometry::UnsignedShortType) {
        quint16 *its = geom->indexDataAsUShort();
        for (qsizetype i = 0; i < ix.size(); ++i)
            its[i] = ix[i];
    } else if (geom->indexType() == QSGGeometry::UnsignedIntType) {
        quint32 *its = geom->indexDataAsUInt();
        for (qsizetype i = 0; i < ix.size(); ++i)
            its[i] = ix[i];
    }

    QSGGeometry::Point2D *pts = geom->vertexDataAsPoint2D();
    for (qsizetype i = 0; i < vx.size(); ++i)
        pts[i].set(vx[i].x, vx[i].y);
}


QGeoMapPolygonGeometryOpenGL::QGeoMapPolygonGeometryOpenGL()
{
}

void QGeoMapPolygonGeometryOpenGL::updateSourcePoints(const QGeoMap &map, const QList<QDoubleVector2D> &path)
{
    QList<QGeoCoordinate> geopath;
    for (const auto &c: path)
        geopath.append(QWebMercator::mercatorToCoord(c));
    updateSourcePoints(map, geopath);
}

/*!
    \internal
*/
// This one does only a perimeter
void QGeoMapPolygonGeometryOpenGL::updateSourcePoints(const QGeoMap &map,
                                                const QList<QGeoCoordinate> &perimeter)
{
    if (!sourceDirty_)
        return;
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map.geoProjection());

    // build the actual path
    // The approach is the same as described in QGeoMapPolylineGeometry::updateSourcePoints
    srcOrigin_ = geoLeftBound_;

    QDoubleVector2D leftBoundWrapped;
    // 1) pre-compute 3 sets of "wrapped" coordinates: one w regular mercator, one w regular mercator +- 1.0
    QList<QDoubleVector2D> wrappedPath;
    QDeclarativeGeoMapItemUtils::wrapPath(perimeter, geoLeftBound_, p,
             wrappedPath, &leftBoundWrapped);

    // 1.1) do the same for the bbox
    QList<QDoubleVector2D> wrappedBbox, wrappedBboxPlus1, wrappedBboxMinus1;
    QGeoPolygon bbox(QGeoPath(perimeter).boundingGeoRectangle());
    QDeclarativeGeoMapItemUtils::wrapPath(bbox.perimeter(), bbox.boundingGeoRectangle().topLeft(), p,
             wrappedBbox, wrappedBboxMinus1, wrappedBboxPlus1, &m_bboxLeftBoundWrapped);

    // 2) Store the triangulated polygon, and the wrapped bbox paths.
    //    the triangulations can be used as they are, as they "bypass" the QtQuick display chain
    //    the bbox wraps have to be however clipped, and then projected, in order to figure out the geometry.
    //    Note that this might still cause the geometryChange method to fail under some extreme conditions.
    cutPathEars(wrappedPath, m_screenVertices, m_screenIndices);

    m_wrappedPolygons.resize(3);
    m_wrappedPolygons[0].wrappedBboxes = wrappedBboxMinus1;
    m_wrappedPolygons[1].wrappedBboxes = wrappedBbox;
    m_wrappedPolygons[2].wrappedBboxes = wrappedBboxPlus1;
}

// This one handles whole QGeoPolygon w. holes
void QGeoMapPolygonGeometryOpenGL::updateSourcePoints(const QGeoMap &map, const QGeoPolygon &poly)
{
    if (!sourceDirty_)
        return;
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map.geoProjection());

    // build the actual path
    // The approach is the same as described in QGeoMapPolylineGeometry::updateSourcePoints
    srcOrigin_ = geoLeftBound_;

    QDoubleVector2D leftBoundWrapped;
    QList<QList<QDoubleVector2D>> wrappedPath;
    // 1) pre-compute 3 sets of "wrapped" coordinates: one w regular mercator, one w regular mercator +- 1.0
    wrapPath(poly, geoLeftBound_, p,
             wrappedPath, &leftBoundWrapped);

    // 1.1) do the same for the bbox
    QList<QDoubleVector2D> wrappedBbox, wrappedBboxPlus1, wrappedBboxMinus1;
    QGeoPolygon bbox(poly.boundingGeoRectangle());
    QDeclarativeGeoMapItemUtils::wrapPath(bbox.perimeter(), bbox.boundingGeoRectangle().topLeft(), p,
             wrappedBbox, wrappedBboxMinus1, wrappedBboxPlus1, &m_bboxLeftBoundWrapped);

    // 2) Store the triangulated polygon, and the wrapped bbox paths.
    //    the triangulations can be used as they are, as they "bypass" the QtQuick display chain
    //    the bbox wraps have to be however clipped, and then projected, in order to figure out the geometry.
    //    Note that this might still cause the geometryChange method to fail under some extreme conditions.
    cutPathEars(wrappedPath, m_screenVertices, m_screenIndices);
    m_wrappedPolygons.resize(3);
    m_wrappedPolygons[0].wrappedBboxes = wrappedBboxMinus1;
    m_wrappedPolygons[1].wrappedBboxes = wrappedBbox;
    m_wrappedPolygons[2].wrappedBboxes = wrappedBboxPlus1;
}

void QGeoMapPolygonGeometryOpenGL::updateSourcePoints(const QGeoMap &map, const QGeoRectangle &rect)
{
    if (!sourceDirty_)
        return;
    const QList<QGeoCoordinate> perimeter = QGeoMapItemGeometry::path(rect);
    updateSourcePoints(map, perimeter);
}

/*!
    \internal
*/
void QGeoMapPolygonGeometryOpenGL::updateScreenPoints(const QGeoMap &map, qreal strokeWidth , const QColor &strokeColor)
{
    if (map.viewportWidth() == 0 || map.viewportHeight() == 0) {
        clear();
        return;
    }

    // 1) identify which set to use: std, +1 or -1
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map.geoProjection());
    const QDoubleVector2D leftBoundMercator = p.geoToMapProjection(srcOrigin_);
    m_wrapOffset = p.projectionWrapFactor(leftBoundMercator) + 1; // +1 to get the offset into QLists

    // 1.1) select geometry set
    // This could theoretically be skipped for those polygons whose bbox is not even projectable.
    // However, such optimization could only be introduced if not calculating bboxes lazily.
    // Hence not doing it.
    if (sourceDirty_) {
        m_dataChanged = true;
    }

    if (strokeWidth == 0.0 || strokeColor.alpha() == 0) // or else the geometry of the border is used, so no point in calculating 2 of them
        updateQuickGeometry(p, strokeWidth);
}

void QGeoMapPolygonGeometryOpenGL::updateQuickGeometry(const QGeoProjectionWebMercator &p, qreal /*strokeWidth*/)
{
     // 2) clip bbox
    // BBox handling --  this is related to the bounding box geometry
    // that has to inevitably follow the old projection codepath
    // As it needs to provide projected coordinates for QtQuick interaction.
    // This could be futher optimized to be updated in a lazy fashion.
    const QList<QDoubleVector2D> &wrappedBbox = m_wrappedPolygons.at(m_wrapOffset).wrappedBboxes;
    QList<QList<QDoubleVector2D> > clippedBbox;
    QDoubleVector2D bboxLeftBoundWrapped = m_bboxLeftBoundWrapped;
    bboxLeftBoundWrapped.setX(bboxLeftBoundWrapped.x() + double(m_wrapOffset - 1));
    QDeclarativeGeoMapItemUtils::clipPolygon(wrappedBbox, p, clippedBbox, &bboxLeftBoundWrapped);

    // 3) project bbox
    QPainterPath ppi;
    if (!clippedBbox.size() || clippedBbox.first().size() < 3) {
        sourceBounds_ = screenBounds_ = QRectF();
        firstPointOffset_ = QPointF();
        screenOutline_ = ppi;
        return;
    }

    QDeclarativeGeoMapItemUtils::projectBbox(clippedBbox.first(), p, ppi); // Using first because a clipped box should always result in one polygon
    const QRectF brect = ppi.boundingRect();
    firstPointOffset_ = QPointF(brect.topLeft());
    screenOutline_ = ppi;

    // 4) Set Screen bbox
    screenBounds_ = brect;
    sourceBounds_.setX(0);
    sourceBounds_.setY(0);
    sourceBounds_.setWidth(brect.width());
    sourceBounds_.setHeight(brect.height());
}


void QGeoMapPolylineGeometryOpenGL::updateSourcePoints(const QGeoMap &map, const QGeoPolygon &poly)
{
    if (!sourceDirty_)
        return;
    QGeoPath p(poly.perimeter());
    if (poly.perimeter().size() && poly.perimeter().last() != poly.perimeter().first())
        p.addCoordinate(poly.perimeter().first());
    updateSourcePoints(map, p);
}

void QGeoMapPolylineGeometryOpenGL::updateSourcePoints(const QGeoMap &map, const QGeoPath &poly)
{
    if (!sourceDirty_)
        return;
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map.geoProjection());

    // build the actual path
    // The approach is the same as described in QGeoMapPolylineGeometry::updateSourcePoints


    QDoubleVector2D leftBoundWrapped;
    // 1) pre-compute 3 sets of "wrapped" coordinates: one w regular mercator, one w regular mercator +- 1.0
    QList<QDoubleVector2D> wrappedPath;
    QDeclarativeGeoMapItemUtils::wrapPath(poly.path(), geoLeftBound_, p,
             wrappedPath, &leftBoundWrapped);

    const QGeoRectangle &boundingRectangle = poly.boundingGeoRectangle();
    updateSourcePoints(p, wrappedPath, boundingRectangle);
}

void QGeoMapPolylineGeometryOpenGL::updateSourcePoints(const QGeoProjectionWebMercator &p,
                                                       const QList<QDoubleVector2D> &wrappedPath,
                                                       const QGeoRectangle &boundingRectangle) {
    if (!sourceDirty_)
        return;
    // 1.1) do the same for the bbox
    // Beware: vertical lines (or horizontal lines) might have an "empty" bbox. Check for that

    QGeoCoordinate topLeft = boundingRectangle.topLeft();
    QGeoCoordinate bottomRight = boundingRectangle.bottomRight();
    const qreal epsilon = 0.000001;
    if (qFuzzyCompare(topLeft.latitude(), bottomRight.latitude())) {
        topLeft.setLatitude(qBound(-90.0, topLeft.latitude() + epsilon ,90.0));
        bottomRight.setLatitude(qBound(-90.0, bottomRight.latitude() - epsilon ,90.0));
    }
    if (qFuzzyCompare(topLeft.longitude(), bottomRight.longitude())) {
        topLeft.setLongitude(QLocationUtils::wrapLong(topLeft.longitude() - epsilon));
        bottomRight.setLongitude(QLocationUtils::wrapLong(bottomRight.longitude() + epsilon));
    }
    QGeoPolygon bbox(QGeoRectangle(topLeft, bottomRight));
    QList<QDoubleVector2D> wrappedBbox, wrappedBboxPlus1, wrappedBboxMinus1;
    QDeclarativeGeoMapItemUtils::wrapPath(bbox.perimeter(), bbox.boundingGeoRectangle().topLeft(), p,
             wrappedBbox, wrappedBboxMinus1, wrappedBboxPlus1, &m_bboxLeftBoundWrapped);

    // New pointers, some old LOD task might still be running and operating on the old pointers.
    resetLOD();

    for (const auto &v: qAsConst(wrappedPath)) m_screenVertices->append(v);

    m_wrappedPolygons.resize(3);
    m_wrappedPolygons[0].wrappedBboxes = wrappedBboxMinus1;
    m_wrappedPolygons[1].wrappedBboxes = wrappedBbox;
    m_wrappedPolygons[2].wrappedBboxes = wrappedBboxPlus1;
    srcOrigin_ = geoLeftBound_;
}

void QGeoMapPolylineGeometryOpenGL::updateSourcePoints(const QGeoMap &map, const QGeoRectangle &rect)
{
    const QList<QGeoCoordinate> perimeter = QGeoMapItemGeometry::perimeter(rect);
    updateSourcePoints(map, QGeoPath(perimeter));
}

void QGeoMapPolylineGeometryOpenGL::updateSourcePoints(const QGeoMap &map, const QGeoCircle &circle)
{
    if (!sourceDirty_)
        return;
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map.geoProjection());

    QDoubleVector2D leftBoundWrapped;
    // 1) pre-compute 3 sets of "wrapped" coordinates: one w regular mercator, one w regular mercator +- 1.0
    QList<QGeoCoordinate> path;
    QGeoCoordinate leftBound;
    QList<QDoubleVector2D> wrappedPath;
    QDeclarativeCircleMapItemPrivate::calculatePeripheralPoints(path, circle.center(), circle.radius(), QDeclarativeCircleMapItemPrivate::CircleSamples, leftBound);
    path << path.first();
    geoLeftBound_ = leftBound;
    QDeclarativeGeoMapItemUtils::wrapPath(path, leftBound, p, wrappedPath, &leftBoundWrapped);
    const QGeoRectangle &boundingRectangle = circle.boundingGeoRectangle();
    updateSourcePoints(p, wrappedPath, boundingRectangle);
}

void QGeoMapPolylineGeometryOpenGL::updateScreenPoints(const QGeoMap &map, qreal strokeWidth, bool /*adjustTranslation*/)
{
    if (map.viewportWidth() == 0 || map.viewportHeight() == 0) {
        clear();
        return;
    }

    // 1) identify which set to use: std, +1 or -1
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map.geoProjection());
    const QDoubleVector2D leftBoundMercator = p.geoToMapProjection(srcOrigin_);
    m_wrapOffset = p.projectionWrapFactor(leftBoundMercator) + 1; // +1 to get the offset into QLists

    if (sourceDirty_) {
        // 1.1) select geometry set
        // This could theoretically be skipped for those polylines whose bbox is not even projectable.
        // However, such optimization could only be introduced if not calculating bboxes lazily.
        // Hence not doing it.
//        if (m_screenVertices.size() > 1)
            m_dataChanged = true;
    }

    updateQuickGeometry(p, strokeWidth);
}

void QGeoMapPolylineGeometryOpenGL::updateQuickGeometry(const QGeoProjectionWebMercator &p, qreal strokeWidth)
{
    // 2) clip bbox
    // BBox handling --  this is related to the bounding box geometry
    // that has to inevitably follow the old projection codepath
    // As it needs to provide projected coordinates for QtQuick interaction.
    // This could be futher optimized to be updated in a lazy fashion.
    const QList<QDoubleVector2D> &wrappedBbox = m_wrappedPolygons.at(m_wrapOffset).wrappedBboxes;
    QList<QList<QDoubleVector2D> > clippedBbox;
    QDoubleVector2D bboxLeftBoundWrapped = m_bboxLeftBoundWrapped;
    bboxLeftBoundWrapped.setX(bboxLeftBoundWrapped.x() + double(m_wrapOffset - 1));
    QDeclarativeGeoMapItemUtils::clipPolygon(wrappedBbox, p, clippedBbox, &bboxLeftBoundWrapped, false);

    // 3) project bbox
    QPainterPath ppi;

    if ( !clippedBbox.size() ||
            clippedBbox.first().size() < 3) {
        sourceBounds_ = screenBounds_ = QRectF();
        firstPointOffset_ = QPointF();
        screenOutline_ = ppi;
        return;
    }

    QDeclarativeGeoMapItemUtils::projectBbox(clippedBbox.first(), p, ppi); // Using first because a clipped box should always result in one polygon
    const QRectF brect = ppi.boundingRect();
    firstPointOffset_ = QPointF(brect.topLeft());
    sourceBounds_ = brect;
    screenOutline_ = ppi;

    // 4) Set Screen bbox
    screenBounds_ = brect;
    sourceBounds_.setX(0);
    sourceBounds_.setY(0);
    sourceBounds_.setWidth(brect.width() + strokeWidth);
    sourceBounds_.setHeight(brect.height() + strokeWidth);
}

// Note: this is also slightly incorrect on joins and in the beginning/end of the line
bool QGeoMapPolylineGeometryOpenGL::contains(const QPointF &point, qreal lineWidth, const QGeoProjectionWebMercator &p) const
{
    const double lineHalfWidth = lineWidth * 0.5;
    const QDoubleVector2D pt(point);
    QDoubleVector2D a;
    if (m_screenVertices->size())
        a = p.wrappedMapProjectionToItemPosition(p.wrapMapProjection(m_screenVertices->first().toDoubleVector2D()));
    QDoubleVector2D b;
    for (qsizetype i = 1; i < m_screenVertices->size(); ++i) {
        const auto &screenVertice = m_screenVertices->at(i);
        if (!a.isFinite()) {
            a = p.wrappedMapProjectionToItemPosition(p.wrapMapProjection(screenVertice.toDoubleVector2D()));
            continue;
        }

        b = p.wrappedMapProjectionToItemPosition(p.wrapMapProjection(screenVertice.toDoubleVector2D()));
        if (!b.isFinite()) {
            a = b;
            continue;
        }

        if (b == a)
            continue;

        // Heavily simplifying it here: if a point is not projectable, skip the segment.
        // For a correct solution, the segment should be clipped instead.
        if (distanceTo(a, b, pt) <= lineHalfWidth)
            return true;

        a = b;
    }
    return false;
}

QT_END_NAMESPACE
