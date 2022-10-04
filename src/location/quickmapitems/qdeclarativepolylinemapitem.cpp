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

#include "qdeclarativepolylinemapitem_p.h"
#include "qdeclarativepolylinemapitem_p_p.h"
#include "qdeclarativerectanglemapitem_p_p.h"
#include "qdeclarativecirclemapitem_p_p.h"
#include "qdeclarativegeomapitemutils_p.h"
#include "error_messages_p.h"
#include "qgeosimplify_p.h"

#include <QtCore/QScopedValueRollback>
#include <qnumeric.h>
#include <QThreadPool>
#include <QRunnable>
#include <QPainter>
#include <QPainterPath>
#include <QtQml/QQmlInfo>

#include <QtGui/private/qtriangulatingstroker_p.h>
#include <QtGui/private/qtriangulator_p.h>
#include <QtQuick/private/qsgmaterialshader_p.h>
#include <QtPositioning/private/qlocationutils_p.h>
#include <QtPositioning/private/qdoublevector2d_p.h>
#include <QtPositioning/private/qwebmercator_p.h>
#include <QtPositioning/private/qclipperutils_p.h>
#include <QtPositioning/private/qgeopath_p.h>
#include <QtLocation/private/qgeomapparameter_p.h>
#include <QtLocation/private/qgeomap_p.h>

#include <array>

QT_BEGIN_NAMESPACE

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


static bool get_line_intersection(const double p0_x,
                                 const double p0_y,
                                 const double p1_x,
                                 const double p1_y,
                                 const double p2_x,
                                 const double p2_y,
                                 const double p3_x,
                                 const double p3_y,
                                 double *i_x,
                                 double *i_y,
                                 double *i_t)
{
    const double s10_x = p1_x - p0_x;
    const double s10_y = p1_y - p0_y;
    const double s32_x = p3_x - p2_x;
    const double s32_y = p3_y - p2_y;

    const double denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0.0)
        return false; // Collinear
    const bool denomPositive = denom > 0;

    const double s02_x = p0_x - p2_x;
    const double s02_y = p0_y - p2_y;
    const double s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0.0) == denomPositive)
        return false; // No collision

    const double t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0.0) == denomPositive)
        return false; // No collision

    if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
        return false; // No collision
    // Collision detected
    *i_t = t_numer / denom;
    *i_x = p0_x + (*i_t * s10_x);
    *i_y = p0_y + (*i_t * s10_y);

    return true;
}

enum SegmentType {
    NoIntersection,
    OneIntersection,
    TwoIntersections
};

static QList<QList<QDoubleVector2D> > clipLine(
        const QList<QDoubleVector2D> &l,
        const QList<QDoubleVector2D> &poly)
{
    QList<QList<QDoubleVector2D> > res;
    if (poly.size() < 2 || l.size() < 2)
        return res;

    // Step 1: build edges
    std::vector<std::array<double, 4> > edges;
    for (qsizetype i = 1; i < poly.size(); i++)
        edges.push_back({ { poly.at(i-1).x(), poly.at(i-1).y(), poly.at(i).x(), poly.at(i).y() } });
    edges.push_back({ { poly.at(poly.size()-1).x(), poly.at(poly.size()-1).y(), poly.at(0).x(), poly.at(0).y() } });

    // Step 2: check each segment against each edge
    QList<QDoubleVector2D> subLine;
    std::array<double, 4> intersections = { { 0.0, 0.0, 0.0, 0.0 } };

    for (qsizetype i = 0; i < l.size() - 1; ++i) {
        SegmentType type = NoIntersection;
        double t = -1; // valid values are in [0, 1]. Only written if intersects
        double previousT = t;
        double i_x, i_y;

        const int firstContained = QClipperUtils::pointInPolygon(l.at(i), poly);
        const int secondContained = QClipperUtils::pointInPolygon(l.at(i+1), poly);

        if (firstContained && secondContained) { // Second most common condition, test early and skip inner loop if possible
            if (!subLine.size())
                subLine.push_back(l.at(i)); // the initial element has to be pushed now.
            subLine.push_back(l.at(i+1));
            continue;
        }

        for (unsigned int j = 0; j < edges.size(); ++j) {
            const bool intersects = get_line_intersection(l.at(i).x(),
                                                         l.at(i).y(),
                                                         l.at(i+1).x(),
                                                         l.at(i+1).y(),
                                                         edges.at(j).at(0),
                                                         edges.at(j).at(1),
                                                         edges.at(j).at(2),
                                                         edges.at(j).at(3),
                                                         &i_x,
                                                         &i_y,
                                                         &t);
            if (intersects) {
                if (previousT >= 0.0) { //One intersection already hit
                    if (t < previousT) { // Reorder
                        intersections[2] = intersections[0];
                        intersections[3] = intersections[1];
                        intersections[0] = i_x;
                        intersections[1] = i_y;
                    } else {
                        intersections[2] = i_x;
                        intersections[3] = i_y;
                    }

                    type = TwoIntersections;
                    break; // no need to check anything else
                } else { // First intersection
                    intersections[0] = i_x;
                    intersections[1] = i_y;
                    type = OneIntersection;
                }
                previousT = t;
            }
        }

        if (type == NoIntersection) {
            if (!firstContained && !secondContained) { // Both outside
                subLine.clear();
            } else if (firstContained && secondContained) {
                // Handled above already.
            } else { // Mismatch between PointInPolygon and get_line_intersection. Treat it as no intersection
                if (subLine.size())
                    res.push_back(subLine);
                subLine.clear();
            }
        } else if (type == OneIntersection) { // Need to check the following cases to avoid mismatch with PointInPolygon result.
            if (firstContained <= 0 && secondContained > 0) { // subLine MUST be empty
                if (!subLine.size())
                    subLine.push_back(QDoubleVector2D(intersections[0], intersections[1]));
                subLine.push_back(l.at(i+1));
            } else if (firstContained > 0 && secondContained <= 0) { // subLine MUST NOT be empty
                if (!subLine.size())
                    subLine.push_back(l.at(i));
                subLine.push_back(QDoubleVector2D(intersections[0], intersections[1]));
                res.push_back(subLine);
                subLine.clear();
            } else {
                if (subLine.size())
                    res.push_back(subLine);
                subLine.clear();
            }
        } else { // Two
            // restart strip
            subLine.clear();
            subLine.push_back(QDoubleVector2D(intersections[0], intersections[1]));
            subLine.push_back(QDoubleVector2D(intersections[2], intersections[3]));
            res.push_back(subLine);
            subLine.clear();
        }
    }

    if (subLine.size())
        res.push_back(subLine);
    return res;
}

/*!
    \qmltype MapPolyline
    \instantiates QDeclarativePolylineMapItem
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps
    \since QtLocation 5.0

    \brief The MapPolyline type displays a polyline on a map.

    The MapPolyline type displays a polyline on a map, specified in terms of an ordered list of
    \l {coordinate}{coordinates}.  The \l {coordinate}{coordinates} on
    the path cannot be directly changed after being added to the Polyline.  Instead, copy the
    \l path into a var, modify the copy and reassign the copy back to the \l path.

    \code
    var path = mapPolyline.path;
    path[0].latitude = 5;
    mapPolyline.path = path;
    \endcode

    Coordinates can also be added and removed at any time using the \l addCoordinate and
    \l removeCoordinate methods.

    By default, the polyline is displayed as a 1-pixel thick black line. This
    can be changed using the \l line.width and \l line.color properties.

    \section2 Performance

    MapPolylines have a rendering cost that is O(n) with respect to the number
    of vertices. This means that the per frame cost of having a polyline on
    the Map grows in direct proportion to the number of points in the polyline.

    Like the other map objects, MapPolyline is normally drawn without a smooth
    appearance. Setting the \l {Item::opacity}{opacity} property will force the object to
    be blended, which decreases performance considerably depending on the hardware in use.

    \section2 Example Usage

    The following snippet shows a MapPolyline with 4 points, making a shape
    like the top part of a "question mark" (?), near Brisbane, Australia.
    The line drawn is 3 pixels in width and green in color.

    \code
    Map {
        MapPolyline {
            line.width: 3
            line.color: 'green'
            path: [
                { latitude: -27, longitude: 153.0 },
                { latitude: -27, longitude: 154.1 },
                { latitude: -28, longitude: 153.5 },
                { latitude: -29, longitude: 153.5 }
            ]
        }
    }
    \endcode

    \image api-mappolyline.png
*/

/*!
    \qmlproperty bool QtLocation::MapPolyline::autoFadeIn

    This property holds whether the item automatically fades in when zooming into the map
    starting from very low zoom levels. By default this is \c true.
    Setting this property to \c false causes the map item to always have the opacity specified
    with the \l QtQuick::Item::opacity property, which is 1.0 by default.

    \since 5.14
*/

QDeclarativeMapLineProperties::QDeclarativeMapLineProperties(QObject *parent)
    : QObject(parent)
{
}

/*!
    \internal
*/
QColor QDeclarativeMapLineProperties::color() const
{
    return color_;
}

/*!
    \internal
*/
void QDeclarativeMapLineProperties::setColor(const QColor &color)
{
    if (color_ == color)
        return;

    color_ = color;
    emit colorChanged(color_);
}

/*!
    \internal
*/
qreal QDeclarativeMapLineProperties::width() const
{
    return width_;
}

/*!
    \internal
*/
void QDeclarativeMapLineProperties::setWidth(qreal width)
{
    if (width_ == width)
        return;

    width_ = width;
    emit widthChanged(width_);
}

QGeoMapPolylineGeometry::QGeoMapPolylineGeometry()
{
}

QList<QList<QDoubleVector2D> > QGeoMapPolylineGeometry::clipPath(const QGeoMap &map,
                                                           const QList<QDoubleVector2D> &path,
                                                           QDoubleVector2D &leftBoundWrapped)
{
    /*
     * Approach:
     * 1) project coordinates to wrapped web mercator, and do unwrapBelowX
     * 2) if the scene is tilted, clip the geometry against the visible region (this may generate multiple polygons)
     * 2.1) recalculate the origin and geoLeftBound to prevent these parameters from ending in unprojectable areas
     * 2.2) ensure the left bound does not wrap around due to QGeoCoordinate <-> clipper conversions
     */
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map.geoProjection());
    srcOrigin_ = geoLeftBound_;

    double unwrapBelowX = 0;
    leftBoundWrapped = p.wrapMapProjection(p.geoToMapProjection(geoLeftBound_));
    if (preserveGeometry_)
        unwrapBelowX = leftBoundWrapped.x();

    QList<QDoubleVector2D> wrappedPath;
    wrappedPath.reserve(path.size());
    QDoubleVector2D wrappedLeftBound(qInf(), qInf());
    // 1)
    for (const auto &coord : path) {
        QDoubleVector2D wrappedProjection = p.wrapMapProjection(coord);

        // We can get NaN if the map isn't set up correctly, or the projection
        // is faulty -- probably best thing to do is abort
        if (!qIsFinite(wrappedProjection.x()) || !qIsFinite(wrappedProjection.y()))
            return QList<QList<QDoubleVector2D> >();

        const bool isPointLessThanUnwrapBelowX = (wrappedProjection.x() < leftBoundWrapped.x());
        // unwrap x to preserve geometry if moved to border of map
        if (preserveGeometry_ && isPointLessThanUnwrapBelowX) {
            double distance = wrappedProjection.x() - unwrapBelowX;
            if (distance < 0.0)
                distance += 1.0;
            wrappedProjection.setX(unwrapBelowX + distance);
        }
        if (wrappedProjection.x() < wrappedLeftBound.x() || (wrappedProjection.x() == wrappedLeftBound.x() && wrappedProjection.y() < wrappedLeftBound.y())) {
            wrappedLeftBound = wrappedProjection;
        }
        wrappedPath.append(wrappedProjection);
    }

#ifdef QT_LOCATION_DEBUG
    m_wrappedPath = wrappedPath;
#endif

    // 2)
    QList<QList<QDoubleVector2D> > clippedPaths;
    const QList<QDoubleVector2D> &visibleRegion = p.projectableGeometry();
    if (visibleRegion.size()) {
        clippedPaths = clipLine(wrappedPath, visibleRegion);

        // 2.1) update srcOrigin_ and leftBoundWrapped with the point with minimum X
        QDoubleVector2D lb(qInf(), qInf());
        for (const QList<QDoubleVector2D> &path: clippedPaths) {
            for (const QDoubleVector2D &p: path) {
                if (p == leftBoundWrapped) {
                    lb = p;
                    break;
                } else if (p.x() < lb.x() || (p.x() == lb.x() && p.y() < lb.y())) {
                    // y-minimization needed to find the same point on polygon and border
                    lb = p;
                }
            }
        }
        if (qIsInf(lb.x()))
            return QList<QList<QDoubleVector2D> >();

        // 2.2) Prevent the conversion to and from clipper from introducing negative offsets which
        //      in turn will make the geometry wrap around.
        lb.setX(qMax(wrappedLeftBound.x(), lb.x()));
        leftBoundWrapped = lb;
    } else {
        clippedPaths.append(wrappedPath);
    }

#ifdef QT_LOCATION_DEBUG
    m_clippedPaths = clippedPaths;
#endif

    return clippedPaths;
}

void QGeoMapPolylineGeometry::pathToScreen(const QGeoMap &map,
                                           const QList<QList<QDoubleVector2D> > &clippedPaths,
                                           const QDoubleVector2D &leftBoundWrapped)
{
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(map.geoProjection());
    // 3) project the resulting geometry to screen position and calculate screen bounds
    double minX = qInf();
    double minY = qInf();
    double maxX = -qInf();
    double maxY = -qInf();
    srcOrigin_ = p.mapProjectionToGeo(p.unwrapMapProjection(leftBoundWrapped));
    QDoubleVector2D origin = p.wrappedMapProjectionToItemPosition(leftBoundWrapped);
    for (const QList<QDoubleVector2D> &path: clippedPaths) {
        QDoubleVector2D lastAddedPoint;
        for (qsizetype i = 0; i < path.size(); ++i) {
            QDoubleVector2D point = p.wrappedMapProjectionToItemPosition(path.at(i));
            point = point - origin; // (0,0) if point == geoLeftBound_

            minX = qMin(point.x(), minX);
            minY = qMin(point.y(), minY);
            maxX = qMax(point.x(), maxX);
            maxY = qMax(point.y(), maxY);

            if (i == 0) {
                srcPoints_ << point.x() << point.y();
                srcPointTypes_ << QPainterPath::MoveToElement;
                lastAddedPoint = point;
            } else {
                if ((point - lastAddedPoint).manhattanLength() > 3 ||
                        i == path.size() - 1) {
                    srcPoints_ << point.x() << point.y();
                    srcPointTypes_ << QPainterPath::LineToElement;
                    lastAddedPoint = point;
                }
            }
        }
    }

    sourceBounds_ = QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
}

/*!
    \internal
*/
void QGeoMapPolylineGeometry::updateSourcePoints(const QGeoMap &map,
                                                 const QList<QDoubleVector2D> &path,
                                                 const QGeoCoordinate geoLeftBound)
{
    if (!sourceDirty_)
        return;

    geoLeftBound_ = geoLeftBound;

    // clear the old data and reserve enough memory
    srcPoints_.clear();
    srcPoints_.reserve(path.size() * 2);
    srcPointTypes_.clear();
    srcPointTypes_.reserve(path.size());

    /*
     * Approach:
     * 1) project coordinates to wrapped web mercator, and do unwrapBelowX
     * 2) if the scene is tilted, clip the geometry against the visible region (this may generate multiple polygons)
     * 3) project the resulting geometry to screen position and calculate screen bounds
     */

    QDoubleVector2D leftBoundWrapped;
    // 1, 2)
    const QList<QList<QDoubleVector2D> > &clippedPaths = clipPath(map, path, leftBoundWrapped);

    // 3)
    pathToScreen(map, clippedPaths, leftBoundWrapped);
}

// ***  SCREEN CLIPPING *** //

enum ClipPointType {
    InsidePoint  = 0x00,
    LeftPoint    = 0x01,
    RightPoint   = 0x02,
    BottomPoint  = 0x04,
    TopPoint     = 0x08
};

static inline int clipPointType(qreal x, qreal y, const QRectF &rect)
{
    int type = InsidePoint;
    if (x < rect.left())
        type |= LeftPoint;
    else if (x > rect.right())
        type |= RightPoint;
    if (y < rect.top())
        type |= TopPoint;
    else if (y > rect.bottom())
        type |= BottomPoint;
    return type;
}

static void clipSegmentToRect(qreal x0, qreal y0, qreal x1, qreal y1, const QRectF &clipRect,
                              QList<qreal> &outPoints, QList<QPainterPath::ElementType> &outTypes)
{
    int type0 = clipPointType(x0, y0, clipRect);
    int type1 = clipPointType(x1, y1, clipRect);
    bool accept = false;

    while (true) {
        if (!(type0 | type1)) {
            accept = true;
            break;
        } else if (type0 & type1) {
            break;
        } else {
            qreal x = 0.0;
            qreal y = 0.0;
            int outsideType = type0 ? type0 : type1;

            if (outsideType & BottomPoint) {
                x = x0 + (x1 - x0) * (clipRect.bottom() - y0) / (y1 - y0);
                y = clipRect.bottom() - 0.1;
            } else if (outsideType & TopPoint) {
                x = x0 + (x1 - x0) * (clipRect.top() - y0) / (y1 - y0);
                y = clipRect.top() + 0.1;
            } else if (outsideType & RightPoint) {
                y = y0 + (y1 - y0) * (clipRect.right() - x0) / (x1 - x0);
                x = clipRect.right() - 0.1;
            } else if (outsideType & LeftPoint) {
                y = y0 + (y1 - y0) * (clipRect.left() - x0) / (x1 - x0);
                x = clipRect.left() + 0.1;
            }

            if (outsideType == type0) {
                x0 = x;
                y0 = y;
                type0 = clipPointType(x0, y0, clipRect);
            } else {
                x1 = x;
                y1 = y;
                type1 = clipPointType(x1, y1, clipRect);
            }
        }
    }

    if (accept) {
        if (outPoints.size() >= 2) {
            qreal lastX, lastY;
            lastY = outPoints.at(outPoints.size() - 1);
            lastX = outPoints.at(outPoints.size() - 2);

            if (!qFuzzyCompare(lastY, y0) || !qFuzzyCompare(lastX, x0)) {
                outTypes << QPainterPath::MoveToElement;
                outPoints << x0 << y0;
            }
        } else {
            outTypes << QPainterPath::MoveToElement;
            outPoints << x0 << y0;
        }

        outTypes << QPainterPath::LineToElement;
        outPoints << x1 << y1;
    }
}

static void clipPathToRect(const QList<qreal> &points,
                           const QList<QPainterPath::ElementType> &types, const QRectF &clipRect,
                           QList<qreal> &outPoints, QList<QPainterPath::ElementType> &outTypes)
{
    outPoints.clear();
    outPoints.reserve(points.size());
    outTypes.clear();
    outTypes.reserve(types.size());

    qreal lastX = 0;
    qreal lastY = 0; // or else used uninitialized
    for (qsizetype i = 0; i < types.size(); ++i) {
        if (i > 0 && types[i] != QPainterPath::MoveToElement) {
            qreal x = points[i * 2], y = points[i * 2 + 1];
            clipSegmentToRect(lastX, lastY, x, y, clipRect, outPoints, outTypes);
        }

        lastX = points[i * 2];
        lastY = points[i * 2 + 1];
    }
}

////////////////////////////////////////////////////////////////////////////

/*!
    \internal
*/
void QGeoMapPolylineGeometry::updateScreenPoints(const QGeoMap &map,
                                                 qreal strokeWidth,
                                                 bool adjustTranslation)
{
    if (!screenDirty_)
        return;

    QPointF origin = map.geoProjection().coordinateToItemPosition(srcOrigin_, false).toPointF();

    if (!qIsFinite(origin.x()) || !qIsFinite(origin.y()) || srcPointTypes_.size() < 2) { // the line might have been clipped away.
        clear();
        return;
    }

    // Create the viewport rect in the same coordinate system
    // as the actual points
    QRectF viewport(0, 0, map.viewportWidth(), map.viewportHeight());
    viewport.adjust(-strokeWidth, -strokeWidth, strokeWidth * 2, strokeWidth * 2);
    viewport.translate(-1 * origin);

    QList<qreal> points;
    QList<QPainterPath::ElementType> types;

    if (clipToViewport_) {
        // Although the geometry has already been clipped against the visible region in wrapped mercator space.
        // This is currently still needed to prevent a number of artifacts deriving from QTriangulatingStroker processing
        // very large lines (that is, polylines that span many pixels in screen space)
        clipPathToRect(srcPoints_, srcPointTypes_, viewport, points, types);
    } else {
        points = srcPoints_;
        types = srcPointTypes_;
    }

    QVectorPath vp(points.data(), types.size(), types.data());
    QTriangulatingStroker ts;
    // As of Qt5.11, the clip argument is not actually used, in the call below.
    ts.process(vp, QPen(QBrush(Qt::black), strokeWidth), QRectF(), QPainter::Antialiasing);

    clear();

    // Nothing is on the screen
    if (ts.vertexCount() == 0)
        return;

    // QTriangulatingStroker#vertexCount is actually the length of the array,
    // not the number of vertices
    screenVertices_.reserve(ts.vertexCount());

    QRectF bb;

    QPointF pt;
    const float *vs = ts.vertices();
    for (int i = 0; i < (ts.vertexCount()/2*2); i += 2) {
        pt = QPointF(vs[i], vs[i + 1]);
        screenVertices_ << pt;

        if (!qIsFinite(pt.x()) || !qIsFinite(pt.y()))
            break;

        if (!bb.contains(pt)) {
            if (pt.x() < bb.left())
                bb.setLeft(pt.x());

            if (pt.x() > bb.right())
                bb.setRight(pt.x());

            if (pt.y() < bb.top())
                bb.setTop(pt.y());

            if (pt.y() > bb.bottom())
                bb.setBottom(pt.y());
        }
    }

    screenBounds_ = bb;
    const QPointF strokeOffset = (adjustTranslation) ? QPointF(strokeWidth, strokeWidth) * 0.5: QPointF();
    this->translate( -1 * sourceBounds_.topLeft() + strokeOffset);
}

void QGeoMapPolylineGeometry::clearSource()
{
    srcPoints_.clear();
    srcPointTypes_.clear();
}

bool QGeoMapPolylineGeometry::contains(const QPointF &point) const
{
    // screenOutline_.contains(screenPoint) doesn't work, as, it appears, that
    // screenOutline_ for QGeoMapPolylineGeometry is empty (QRectF(0,0 0x0))
    const QList<QPointF> &verts = vertices();
    QPolygonF tri;
    for (const auto &v : verts) {
        tri << v;
        if (tri.size() == 3) {
            if (tri.containsPoint(point, Qt::OddEvenFill))
                return true;
            tri.remove(0);
        }
    }

    return false;
}

/*
 * QDeclarativePolygonMapItem Private Implementations
 */

QDeclarativePolylineMapItemPrivate::~QDeclarativePolylineMapItemPrivate() {}

QDeclarativePolylineMapItemPrivateCPU::~QDeclarativePolylineMapItemPrivateCPU() {}


void QDeclarativePolylineMapItemPrivateCPU::regenerateCache()
{
    if (!m_poly.map() || m_poly.map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
        return;
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_poly.map()->geoProjection());
    m_geopathProjected.clear();
    m_geopathProjected.reserve(m_poly.m_geopath.size());
    for (const QGeoCoordinate &c : m_poly.m_geopath.path())
        m_geopathProjected << p.geoToMapProjection(c);
}

void QDeclarativePolylineMapItemPrivateCPU::updateCache()
{
    if (!m_poly.map() || m_poly.map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
        return;
    const QGeoProjectionWebMercator &p = static_cast<const QGeoProjectionWebMercator&>(m_poly.map()->geoProjection());
    m_geopathProjected << p.geoToMapProjection(m_poly.m_geopath.path().last());
}

void QDeclarativePolylineMapItemPrivateCPU::updatePolish()
{
    if (m_poly.m_geopath.path().length() < 2) { // Possibly cleared
        m_geometry.clear();
        m_poly.setWidth(0);
        m_poly.setHeight(0);
        return;
    }
    QScopedValueRollback<bool> rollback(m_poly.m_updatingGeometry);
    m_poly.m_updatingGeometry = true;

    const QGeoMap *map = m_poly.map();
    const qreal borderWidth = m_poly.m_line.width();

    m_geometry.updateSourcePoints(*map, m_geopathProjected, m_poly.m_geopath.boundingGeoRectangle().topLeft());
    m_geometry.updateScreenPoints(*map, borderWidth);

    m_poly.setWidth(m_geometry.sourceBoundingBox().width() + borderWidth);
    m_poly.setHeight(m_geometry.sourceBoundingBox().height() + borderWidth);

    // it has to be shifted so that the center of the line is on the correct geocoord
    m_poly.setPositionOnMap(m_geometry.origin(), -1 * m_geometry.sourceBoundingBox().topLeft()
                            + QPointF(borderWidth, borderWidth) * 0.5 );
}

QSGNode *QDeclarativePolylineMapItemPrivateCPU::updateMapItemPaintNode(QSGNode *oldNode,
                                                        QQuickItem::UpdatePaintNodeData * /*data*/)
{
    if (!m_node || !oldNode) {
        m_node = new MapPolylineNode();
        if (oldNode) {
            delete oldNode;
            oldNode = nullptr;
        }
    } else {
        m_node = static_cast<MapPolylineNode *>(oldNode);
    }

    //TODO: update only material
    if (m_geometry.isScreenDirty() || m_poly.m_dirtyMaterial || !oldNode) {
        m_node->update(m_poly.m_line.color(), &m_geometry);
        m_geometry.setPreserveGeometry(false);
        m_geometry.markClean();
        m_poly.m_dirtyMaterial = false;
    }
    return m_node;
}
bool QDeclarativePolylineMapItemPrivateCPU::contains(const QPointF &point) const
{
    return m_geometry.contains(point);
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
    QDeclarativeCircleMapItemPrivateCPU::calculatePeripheralPoints(path, circle.center(), circle.radius(), QDeclarativeCircleMapItemPrivateCPU::CircleSamples, leftBound);
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


QDeclarativePolylineMapItemPrivateOpenGLLineStrip::~QDeclarativePolylineMapItemPrivateOpenGLLineStrip() {}

QDeclarativePolylineMapItemPrivateOpenGLExtruded::~QDeclarativePolylineMapItemPrivateOpenGLExtruded() {}

/*
 * QDeclarativePolygonMapItem Implementation
 */

struct PolylineBackendSelector
{
    PolylineBackendSelector()
    {
        backend = (qgetenv("QTLOCATION_OPENGL_ITEMS").toInt()) ? QDeclarativePolylineMapItem::OpenGLExtruded : QDeclarativePolylineMapItem::Software;
    }
    QDeclarativePolylineMapItem::Backend backend = QDeclarativePolylineMapItem::Software;
};

Q_GLOBAL_STATIC(PolylineBackendSelector, mapPolylineBackendSelector)

QDeclarativePolylineMapItem::QDeclarativePolylineMapItem(QQuickItem *parent)
    : QDeclarativeGeoMapItemBase(parent), m_line(this),
      m_d(new QDeclarativePolylineMapItemPrivateCPU(*this))
{
    m_itemType = QGeoMap::MapPolyline;
    m_geopath = QGeoPathEager();
    setFlag(ItemHasContents, true);
    QObject::connect(&m_line, &QDeclarativeMapLineProperties::colorChanged,
                     this, &QDeclarativePolylineMapItem::updateAfterLinePropertiesChanged);
    QObject::connect(&m_line, &QDeclarativeMapLineProperties::widthChanged,
                     this, &QDeclarativePolylineMapItem::updateAfterLinePropertiesChanged);
    setBackend(mapPolylineBackendSelector->backend);
}

QDeclarativePolylineMapItem::~QDeclarativePolylineMapItem()
{
}

/*!
    \internal
*/
void QDeclarativePolylineMapItem::updateAfterLinePropertiesChanged()
{
    m_d->onLinePropertiesChanged();
}

/*!
    \internal
*/
void QDeclarativePolylineMapItem::setMap(QDeclarativeGeoMap *quickMap, QGeoMap *map)
{
    QDeclarativeGeoMapItemBase::setMap(quickMap,map);
    if (map)
        m_d->onMapSet();
}

/*!
    \qmlproperty list<coordinate> MapPolyline::path

    This property holds the ordered list of coordinates which
    define the polyline.
*/

QList<QGeoCoordinate> QDeclarativePolylineMapItem::path() const
{
    return m_geopath.path();
}

void QDeclarativePolylineMapItem::setPath(const QList<QGeoCoordinate> &value)
{
    setPathFromGeoList(value);
}

/*!
    \qmlmethod void MapPolyline::setPath(geopath path)

    Sets the \a path using a geopath type.

    \since 5.10

    \sa path
*/
void QDeclarativePolylineMapItem::setPath(const QGeoPath &path)
{
    if (m_geopath.path() == path.path())
        return;

    m_geopath = QGeoPathEager(path);
    m_d->onGeoGeometryChanged();
    emit pathChanged();
}

/*!
    \internal
*/
void QDeclarativePolylineMapItem::setPathFromGeoList(const QList<QGeoCoordinate> &path)
{
    if (m_geopath.path() == path)
        return;

    m_geopath.setPath(path);

    m_d->onGeoGeometryChanged();
    emit pathChanged();
}

/*!
    \qmlmethod int MapPolyline::pathLength()

    Returns the number of coordinates of the polyline.

    \since QtLocation 5.6

    \sa path
*/
int QDeclarativePolylineMapItem::pathLength() const
{
    return m_geopath.path().length();
}

/*!
    \qmlmethod void MapPolyline::addCoordinate(coordinate)

    Adds the specified \a coordinate to the end of the path.

    \sa insertCoordinate, removeCoordinate, path
*/
void QDeclarativePolylineMapItem::addCoordinate(const QGeoCoordinate &coordinate)
{
    if (!coordinate.isValid())
        return;

    m_geopath.addCoordinate(coordinate);

    m_d->onGeoGeometryUpdated();
    emit pathChanged();
}

/*!
    \qmlmethod void MapPolyline::insertCoordinate(index, coordinate)

    Inserts a \a coordinate to the path at the given \a index.

    \since QtLocation 5.6

    \sa addCoordinate, removeCoordinate, path
*/
void QDeclarativePolylineMapItem::insertCoordinate(int index, const QGeoCoordinate &coordinate)
{
    if (index < 0 || index > m_geopath.path().length())
        return;

    m_geopath.insertCoordinate(index, coordinate);

    m_d->onGeoGeometryChanged();
    emit pathChanged();
}

/*!
    \qmlmethod void MapPolyline::replaceCoordinate(index, coordinate)

    Replaces the coordinate in the current path at the given \a index
    with the new \a coordinate.

    \since QtLocation 5.6

    \sa addCoordinate, insertCoordinate, removeCoordinate, path
*/
void QDeclarativePolylineMapItem::replaceCoordinate(int index, const QGeoCoordinate &coordinate)
{
    if (index < 0 || index >= m_geopath.path().length())
        return;

    m_geopath.replaceCoordinate(index, coordinate);

    m_d->onGeoGeometryChanged();
    emit pathChanged();
}

/*!
    \qmlmethod coordinate MapPolyline::coordinateAt(index)

    Gets the coordinate of the polyline at the given \a index.
    If the index is outside the path's bounds then an invalid
    coordinate is returned.

    \since QtLocation 5.6
*/
QGeoCoordinate QDeclarativePolylineMapItem::coordinateAt(int index) const
{
    if (index < 0 || index >= m_geopath.path().length())
        return QGeoCoordinate();

    return m_geopath.coordinateAt(index);
}

/*!
    \qmlmethod coordinate MapPolyline::containsCoordinate(coordinate)

    Returns true if the given \a coordinate is part of the path.

    \since QtLocation 5.6
*/
bool QDeclarativePolylineMapItem::containsCoordinate(const QGeoCoordinate &coordinate)
{
    return m_geopath.containsCoordinate(coordinate);
}

/*!
    \qmlmethod void MapPolyline::removeCoordinate(coordinate)

    Removes \a coordinate from the path. If there are multiple instances of the
    same coordinate, the one added last is removed.

    If \a coordinate is not in the path this method does nothing.

    \sa addCoordinate, insertCoordinate, path
*/
void QDeclarativePolylineMapItem::removeCoordinate(const QGeoCoordinate &coordinate)
{
    int length = m_geopath.path().length();
    m_geopath.removeCoordinate(coordinate);
    if (m_geopath.path().length() == length)
        return;

    m_d->onGeoGeometryChanged();
    emit pathChanged();
}

/*!
    \qmlmethod void MapPolyline::removeCoordinate(index)

    Removes a coordinate from the path at the given \a index.

    If \a index is invalid then this method does nothing.

    \since QtLocation 5.6

    \sa addCoordinate, insertCoordinate, path
*/
void QDeclarativePolylineMapItem::removeCoordinate(int index)
{
    if (index < 0 || index >= m_geopath.path().length())
        return;

    m_geopath.removeCoordinate(index);

    m_d->onGeoGeometryChanged();
    emit pathChanged();
}

/*!
    \qmlpropertygroup Location::MapPolyline::line
    \qmlproperty int MapPolyline::line.width
    \qmlproperty color MapPolyline::line.color

    This property is part of the line property group. The line
    property group holds the width and color used to draw the line.

    The width is in pixels and is independent of the zoom level of the map.
    The default values correspond to a black border with a width of 1 pixel.

    For no line, use a width of 0 or a transparent color.
*/

QDeclarativeMapLineProperties *QDeclarativePolylineMapItem::line()
{
    return &m_line;
}

/*!
    \internal
    \qmlproperty MapPolyline.Backend QtLocation::MapPolyline::backend

    This property holds which backend is in use to render the map item.
    Valid values are \b MapPolyline.Software and \b{MapPolyline.OpenGLLineStrip}
    and \b{MapPolyline.OpenGLExtruded}.
    The default value is \b{MapPolyline.Software}.

    \note \b{The release of this API with Qt 5.15 is a Technology Preview}.
    Ideally, as the OpenGL backends for map items mature, there will be
    no more need to also offer the legacy software-projection backend.
    So this property will likely disappear at some later point.
    To select OpenGL-accelerated item backends without using this property,
    it is also possible to set the environment variable \b QTLOCATION_OPENGL_ITEMS
    to \b{1}.
    Also note that all current OpenGL backends won't work as expected when enabling
    layers on the individual item, or when running on OpenGL core profiles greater than 2.x.

    \since 5.15
*/
QDeclarativePolylineMapItem::Backend QDeclarativePolylineMapItem::backend() const
{
    return m_backend;
}

void QDeclarativePolylineMapItem::setBackend(QDeclarativePolylineMapItem::Backend b)
{
    if (b == m_backend)
        return;
    m_backend = b;
    std::unique_ptr<QDeclarativePolylineMapItemPrivate> d(
            (m_backend == Software)
                    ? static_cast<QDeclarativePolylineMapItemPrivate *>(
                            new QDeclarativePolylineMapItemPrivateCPU(*this))
                    : ((m_backend == OpenGLExtruded)
                               ? static_cast<QDeclarativePolylineMapItemPrivate *>(
                                       new QDeclarativePolylineMapItemPrivateOpenGLExtruded(*this))
                               : static_cast<QDeclarativePolylineMapItemPrivate *>(
                                       new QDeclarativePolylineMapItemPrivateOpenGLLineStrip(
                                               *this))));
    m_d.swap(d);
    m_d->onGeoGeometryChanged();
    emit backendChanged();
}

/*!
    \internal
*/
void QDeclarativePolylineMapItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (newGeometry.topLeft() == oldGeometry.topLeft() || !map() || !m_geopath.isValid() || m_updatingGeometry) {
        QDeclarativeGeoMapItemBase::geometryChange(newGeometry, oldGeometry);
        return;
    }
    // TODO: change the algorithm to preserve the distances and size!
    QGeoCoordinate newCenter = map()->geoProjection().itemPositionToCoordinate(QDoubleVector2D(newGeometry.center()), false);
    QGeoCoordinate oldCenter = map()->geoProjection().itemPositionToCoordinate(QDoubleVector2D(oldGeometry.center()), false);
    if (!newCenter.isValid() || !oldCenter.isValid())
        return;
    double offsetLongi = newCenter.longitude() - oldCenter.longitude();
    double offsetLati = newCenter.latitude() - oldCenter.latitude();
    if (offsetLati == 0.0 && offsetLongi == 0.0)
        return;

    m_geopath.translate(offsetLati, offsetLongi);
    m_d->onGeoGeometryChanged();
    emit pathChanged();

    // Not calling QDeclarativeGeoMapItemBase::geometryChange() as it will be called from a nested
    // call to this function.
}

/*!
    \internal
*/
void QDeclarativePolylineMapItem::afterViewportChanged(const QGeoMapViewportChangeEvent &event)
{
    if (event.mapSize.isEmpty())
        return;

    m_d->afterViewportChanged();
}

/*!
    \internal
*/
void QDeclarativePolylineMapItem::updatePolish()
{
    if (!map() || map()->geoProjection().projectionType() != QGeoProjection::ProjectionWebMercator)
        return;
    m_d->updatePolish();
}

/*!
    \internal
*/
QSGNode *QDeclarativePolylineMapItem::updateMapItemPaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    return m_d->updateMapItemPaintNode(oldNode, data);
}

bool QDeclarativePolylineMapItem::contains(const QPointF &point) const
{
    return m_d->contains(point);
}

const QGeoShape &QDeclarativePolylineMapItem::geoShape() const
{
    return m_geopath;
}

void QDeclarativePolylineMapItem::setGeoShape(const QGeoShape &shape)
{
    const QGeoPath geopath(shape); // if shape isn't a path, path will be created as a default-constructed path
    setPath(geopath);
}

//////////////////////////////////////////////////////////////////////

/*!
    \internal
*/
VisibleNode::VisibleNode() : m_blocked{true}, m_visible{true}
{

}

VisibleNode::~VisibleNode()
{

}

/*!
    \internal
*/
bool VisibleNode::subtreeBlocked() const
{
    return m_blocked || !m_visible;
}

/*!
    \internal
*/
void VisibleNode::setSubtreeBlocked(bool blocked)
{
    m_blocked = blocked;
}

bool VisibleNode::visible() const
{
    return m_visible;
}

/*!
    \internal
*/
void VisibleNode::setVisible(bool visible)
{
    m_visible = visible;
}

/*!
    \internal
*/
MapItemGeometryNode::~MapItemGeometryNode()
{

}

bool MapItemGeometryNode::isSubtreeBlocked() const
{
    return subtreeBlocked();
}


/*!
    \internal
*/
MapPolylineNode::MapPolylineNode() :
    geometry_(QSGGeometry::defaultAttributes_Point2D(),0)
{
    geometry_.setDrawingMode(QSGGeometry::DrawTriangleStrip);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);
}


/*!
    \internal
*/
MapPolylineNode::~MapPolylineNode()
{
}

/*!
    \internal
*/
void MapPolylineNode::update(const QColor &fillColor,
                             const QGeoMapItemGeometry *shape)
{
    if (shape->size() == 0) {
        setSubtreeBlocked(true);
        return;
    } else {
        setSubtreeBlocked(false);
    }

    QSGGeometry *fill = QSGGeometryNode::geometry();
    shape->allocateAndFill(fill);
    markDirty(DirtyGeometry);

    if (fillColor != fill_material_.color()) {
        fill_material_.setColor(fillColor);
        setMaterial(&fill_material_);
        markDirty(DirtyMaterial);
    }
}

MapPolylineNodeOpenGLLineStrip::MapPolylineNodeOpenGLLineStrip()
: geometry_(QSGGeometry::defaultAttributes_Point2D(), 0)
{
    geometry_.setDrawingMode(QSGGeometry::DrawLineStrip);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&geometry_);
}

MapPolylineNodeOpenGLLineStrip::~MapPolylineNodeOpenGLLineStrip()
{

}

void MapPolylineNodeOpenGLLineStrip::update(const QColor &fillColor,
                                   const qreal lineWidth,
                                   const QGeoMapPolylineGeometryOpenGL *shape,
                                   const QMatrix4x4 &geoProjection,
                                   const QDoubleVector3D &center,
                                   const Qt::PenCapStyle /*capStyle*/)
{
    if (shape->m_screenVertices->size() < 2) {
        setSubtreeBlocked(true);
        return;
    } else {
        setSubtreeBlocked(false);
    }

    QSGGeometry *fill = QSGGeometryNode::geometry();
    if (shape->m_dataChanged) {
        shape->allocateAndFillLineStrip(fill);
        markDirty(DirtyGeometry);
        shape->m_dataChanged = false;
    }
    fill->setLineWidth(lineWidth);
    fill_material_.setLineWidth(lineWidth); // to make the material not compare equal if linewidth changes

//    if (fillColor != fill_material_.color())
    {
        fill_material_.setWrapOffset(shape->m_wrapOffset - 1);
        fill_material_.setColor(fillColor);
        fill_material_.setGeoProjection(geoProjection);
        fill_material_.setCenter(center);
        setMaterial(&fill_material_);
        markDirty(DirtyMaterial);
    }
}

MapPolylineShaderLineStrip::MapPolylineShaderLineStrip() : QSGMaterialShader(*new QSGMaterialShaderPrivate(this))
{
    setShaderFileName(VertexStage, QLatin1String(":/location/quickmapitems/shaders/polyline_linestrip.vert.qsb"));
    setShaderFileName(FragmentStage, QLatin1String(":/location/quickmapitems/shaders/polyline_linestrip.frag.qsb"));
}

bool MapPolylineShaderLineStrip::updateUniformData(QSGMaterialShader::RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == nullptr || newEffect->type() == oldEffect->type());
    MapPolylineMaterial *oldMaterial = static_cast<MapPolylineMaterial *>(oldEffect);
    MapPolylineMaterial *newMaterial = static_cast<MapPolylineMaterial *>(newEffect);

    const QColor &c = newMaterial->color();
    const QMatrix4x4 &geoProjection = newMaterial->geoProjection();
    const QDoubleVector3D &center = newMaterial->center();

    QVector4D vecCenter, vecCenter_lowpart;
    for (int i = 0; i < 3; i++)
        QLocationUtils::split_double(center.get(i), &vecCenter[i], &vecCenter_lowpart[i]);
    vecCenter[3] = 0;
    vecCenter_lowpart[3] = 0;

    int offset = 0;
    char *buf_p = state.uniformData()->data();

    if (state.isMatrixDirty()) {
        const QMatrix4x4 m = state.projectionMatrix();
        memcpy(buf_p + offset, m.constData(), 4*4*4);
    }
    offset += 4*4*4;

    memcpy(buf_p + offset, geoProjection.constData(), 4*4*4); offset+=4*4*4;

    memcpy(buf_p + offset, &vecCenter, 4*4); offset += 4*4;

    memcpy(buf_p + offset, &vecCenter_lowpart, 4*4); offset+=4*4;

    if (state.isOpacityDirty()) {
        const float opacity = state.opacity();
        memcpy(buf_p + offset, &opacity, 4);
    }
    offset += 4;

    float wrapOffset = newMaterial->wrapOffset();
    memcpy(buf_p + offset, &wrapOffset, 4); offset+=4;

    offset+=8; // float padding

    if (oldMaterial == nullptr || c != oldMaterial->color() || state.isOpacityDirty()) {
        float opacity = state.opacity() * c.alphaF();
        QVector4D v(c.redF() * opacity,
                    c.greenF() *  opacity,
                    c.blueF() * opacity,
                    opacity);
        memcpy(buf_p + offset, &v, 4*4);
    }
    offset+=4*4;

    return true;
}

QSGMaterialShader *MapPolylineMaterial::createShader(QSGRendererInterface::RenderMode renderMode) const
{
    Q_UNUSED(renderMode);
    return new MapPolylineShaderLineStrip();
}

QSGMaterialType *MapPolylineMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

int MapPolylineMaterial::compare(const QSGMaterial *other) const
{
    const MapPolylineMaterial &o = *static_cast<const MapPolylineMaterial *>(other);
    if (o.m_center == m_center && o.m_geoProjection == m_geoProjection && o.m_wrapOffset == m_wrapOffset && o.m_lineWidth == m_lineWidth)
        return QSGFlatColorMaterial::compare(other);
    return -1;
}

const QSGGeometry::AttributeSet &MapPolylineNodeOpenGLExtruded::attributesMapPolylineTriangulated()
{
    return MapPolylineEntry::attributes();
}

MapPolylineNodeOpenGLExtruded::MapPolylineNodeOpenGLExtruded()
: m_geometryTriangulating(MapPolylineNodeOpenGLExtruded::attributesMapPolylineTriangulated(),
            0 /* vtx cnt */, 0 /* index cnt */, QSGGeometry::UnsignedIntType /* index type */)
{
    m_geometryTriangulating.setDrawingMode(QSGGeometry::DrawTriangles);
    QSGGeometryNode::setMaterial(&fill_material_);
    QSGGeometryNode::setGeometry(&m_geometryTriangulating);
}

MapPolylineNodeOpenGLExtruded::~MapPolylineNodeOpenGLExtruded()
{

}

bool QGeoMapPolylineGeometryOpenGL::allocateAndFillEntries(QSGGeometry *geom,
                                                           bool closed,
                                                           unsigned int zoom) const
{
    // Select LOD. Generate if not present. Assign it to m_screenVertices;
    if (m_dataChanged) {
        // it means that the data really changed.
        // So synchronously produce LOD 1, and enqueue the requested one if != 0 or 1.
        // Select 0 if 0 is requested, or 1 in all other cases.
        selectLODOnDataChanged(zoom, m_bboxLeftBoundWrapped.x());
    } else {
        // Data has not changed, but active LOD != requested LOD.
        // So, if there are no active tasks, try to change to the correct one.
        if (!selectLODOnLODMismatch(zoom, m_bboxLeftBoundWrapped.x(), closed))
            return false;
    }

    const QList<QDeclarativeGeoMapItemUtils::vec2> &v = *m_screenVertices;
    if (v.size() < 2) {
        geom->allocate(0, 0);
        return true;
    }
    const int numSegments = (v.size() - 1);

    const int numIndices = numSegments * 6; // six vertices per line segment
    geom->allocate(numIndices);
    MapPolylineNodeOpenGLExtruded::MapPolylineEntry *vertices =
            static_cast<MapPolylineNodeOpenGLExtruded::MapPolylineEntry *>(geom->vertexData());

    for (int i = 0; i < numSegments; ++i) {
        MapPolylineNodeOpenGLExtruded::MapPolylineEntry e;
        const QDeclarativeGeoMapItemUtils::vec2 &cur = v[i];
        const QDeclarativeGeoMapItemUtils::vec2 &next = v[i+1];
        e.triangletype = 1.0;
        e.next = next;
        e.prev = cur;
        e.pos = cur;
        e.direction = 1.0;
        e.vertextype = -1.0;
        vertices[i*6] = e;
        e.direction = -1.0;
        vertices[i*6+1] = e;
        e.pos = next;
        e.vertextype = 1.0;
        vertices[i*6+2] = e;

        // Second tri
        e.triangletype = -1.0;
        e.direction = -1.0;
        vertices[i*6+3] = e;
        e.direction = 1.0;
        vertices[i*6+4] = e;
        e.pos = cur;
        e.vertextype = -1.0;
        vertices[i*6+5] = e;

        if (i != 0) {
           vertices[i*6].prev = vertices[i*6+1].prev = vertices[i*6+5].prev = v[i-1];
        } else {
            if (closed) {
                vertices[i*6].prev = vertices[i*6+1].prev = vertices[i*6+5].prev = v[numSegments - 1];
            } else {
                vertices[i*6].triangletype = vertices[i*6+1].triangletype = vertices[i*6+5].triangletype = 2.0;
            }
        }
        if (i != numSegments - 1) {
            vertices[i*6+2].next = vertices[i*6+3].next = vertices[i*6+4].next = v[i+2];
        } else {
            if (closed) {
                vertices[i*6+2].next = vertices[i*6+3].next = vertices[i*6+4].next = v[1];
            } else {
                vertices[i*6+2].triangletype = vertices[i*6+3].triangletype = vertices[i*6+4].triangletype = 3.0;
            }
        }
    }
    return true;
}

void QGeoMapPolylineGeometryOpenGL::allocateAndFillLineStrip(QSGGeometry *geom,
                                                             int lod) const
{
    // Select LOD. Generate if not present. Assign it to m_screenVertices;
    Q_UNUSED(lod);

    const QList<QDeclarativeGeoMapItemUtils::vec2> &vx = *m_screenVertices;
    geom->allocate(vx.size());

    QSGGeometry::Point2D *pts = geom->vertexDataAsPoint2D();
    for (qsizetype i = 0; i < vx.size(); ++i)
        pts[i].set(vx[i].x, vx[i].y);
}

void MapPolylineNodeOpenGLExtruded::update(const QColor &fillColor,
                                   float lineWidth,
                                   const QGeoMapPolylineGeometryOpenGL *shape,
                                   const QMatrix4x4 &geoProjection,
                                   const QDoubleVector3D &center,
                                   const Qt::PenCapStyle capStyle,
                                   bool closed,
                                   unsigned int zoom)
{
    // shape->size() == number of triangles
    if (shape->m_screenVertices->size() < 2
            || lineWidth < 0.5 || fillColor.alpha() == 0) { // number of points
        setSubtreeBlocked(true);
        return;
    } else {
        setSubtreeBlocked(false);
    }

    QSGGeometry *fill = QSGGeometryNode::geometry();
    if (shape->m_dataChanged || !shape->isLODActive(zoom) || !fill->vertexCount()) { // fill->vertexCount for when node gets destroyed by MapItemBase bcoz of opacity, then recreated.
        if (shape->allocateAndFillEntries(fill, closed, zoom)) {
            markDirty(DirtyGeometry);
            shape->m_dataChanged = false;
        }
    }

    // Update this
//    if (fillColor != fill_material_.color())
    {
        fill_material_.setWrapOffset(shape->m_wrapOffset - 1);
        fill_material_.setColor(fillColor);
        fill_material_.setGeoProjection(geoProjection);
        fill_material_.setCenter(center);
        fill_material_.setLineWidth(lineWidth);
        fill_material_.setMiter(capStyle != Qt::FlatCap);
        setMaterial(&fill_material_);
        markDirty(DirtyMaterial);
    }
}

MapPolylineShaderExtruded::MapPolylineShaderExtruded() : QSGMaterialShader(*new QSGMaterialShaderPrivate(this))
{
    // Heavily adapted from https://github.com/mattdesl/webgl-lines/blob/master/projected/vert.glsl,
    // that is (c) Matt DesLauriers, and released under the MIT license.
    setShaderFileName(VertexStage, QLatin1String(":/location/quickmapitems/shaders/polyline_extruded.vert.qsb"));
    setShaderFileName(FragmentStage, QLatin1String(":/location/quickmapitems/shaders/polyline_extruded.frag.qsb"));
}

bool MapPolylineShaderExtruded::updateUniformData(QSGMaterialShader::RenderState &state, QSGMaterial *newEffect, QSGMaterial *oldEffect)
{
    Q_ASSERT(oldEffect == nullptr || newEffect->type() == oldEffect->type());
    MapPolylineMaterialExtruded *oldMaterial = static_cast<MapPolylineMaterialExtruded *>(oldEffect);
    MapPolylineMaterialExtruded *newMaterial = static_cast<MapPolylineMaterialExtruded *>(newEffect);

    const QColor &c = newMaterial->color();
    const QMatrix4x4 &geoProjection = newMaterial->geoProjection();
    const QDoubleVector3D &center = newMaterial->center();

    // It is safer to use vec4 instead on vec3, as described in:
    // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout
    QVector4D vecCenter, vecCenter_lowpart;
    for (int i = 0; i < 3; i++)
        QLocationUtils::split_double(center.get(i), &vecCenter[i], &vecCenter_lowpart[i]);
    vecCenter[3] = 0;
    vecCenter_lowpart[3] = 0;

    int offset = 0;
    char *buf_p = state.uniformData()->data();

    if (state.isMatrixDirty()) {
        const QMatrix4x4 m = state.projectionMatrix();
        memcpy(buf_p + offset, m.constData(), 4*4*4);
    }
    offset += 4*4*4;

    memcpy(buf_p + offset, geoProjection.constData(), 4*4*4); offset+=4*4*4;

    memcpy(buf_p + offset, &vecCenter, 4*4); offset += 4*4;

    memcpy(buf_p + offset, &vecCenter_lowpart, 4*4); offset+=4*4;

    const float lineWidth = newMaterial->lineWidth();
    memcpy(buf_p + offset, &lineWidth, 4); offset+=4;

    const QRectF viewportRect = state.viewportRect();
    const float aspect = float(viewportRect.width() / viewportRect.height());
    memcpy(buf_p + offset, &aspect, 4); offset+=4;

    offset += 4; // Padding

    int miter = newMaterial->miter();
    memcpy(buf_p + offset, &miter, 4); offset+=4;

    if (oldMaterial == nullptr || c != oldMaterial->color() || state.isOpacityDirty()) {
        float opacity = state.opacity() * c.alphaF();
        QVector4D v(c.redF() * opacity,
                    c.greenF() *  opacity,
                    c.blueF() * opacity,
                    opacity);
        memcpy(buf_p + offset, &v, 4*4);
    }
    offset+=4*4;

    const float wrapOffset = newMaterial->wrapOffset();
    memcpy(buf_p + offset, &wrapOffset, 4); offset+=4;

    return true;
}

QSGMaterialShader *MapPolylineMaterialExtruded::createShader(QSGRendererInterface::RenderMode renderMode) const
{
    Q_UNUSED(renderMode);
    return new MapPolylineShaderExtruded();
}

QSGMaterialType *MapPolylineMaterialExtruded::type() const
{
    static QSGMaterialType type;
    return &type;
}

int MapPolylineMaterialExtruded::compare(const QSGMaterial *other) const
{
    const MapPolylineMaterialExtruded &o = *static_cast<const MapPolylineMaterialExtruded *>(other);
    if (o.m_miter == m_miter)
        return MapPolylineMaterial::compare(other);
    return -1;
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

    ~PolylineSimplifyTask() override;

    void run() override
    {
        // Skip sending notifications for now. Updated data will be picked up eventually.
        // ToDo: figure out how to connect a signal from here to a slot in the item.
        *m_working = QGeoMapPolylineGeometryOpenGL::zoomToLOD(m_zoom);
        const QList<QDeclarativeGeoMapItemUtils::vec2> res =
                QGeoMapPolylineGeometryOpenGL::getSimplified(
                        *m_input, m_leftBound, QGeoMapPolylineGeometryOpenGL::zoomForLOD(m_zoom));
        *m_output = res;
        *m_working = 0;
    }

    unsigned int m_zoom;
    double m_leftBound;
    QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>> m_input, m_output;
    QSharedPointer<unsigned int> m_working;
};

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

PolylineSimplifyTask::~PolylineSimplifyTask() {}

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
                *m_verticesLOD[1] = getSimplified( *m_verticesLOD[0],
                                                   leftBound,
                                                   zoomForLOD(0));
                if (requestedLod == 1)
                    return;
            }
        }

        enqueueSimplificationTask(  m_verticesLOD.at(0),
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
        *m_verticesLOD[1] = getSimplified( *m_verticesLOD[0],
                leftBound,
                zoomForLOD(0));
    }
    if (lod > 1) {
        if (!m_verticesLOD[lod])
            m_verticesLOD[lod] = QSharedPointer<QList<QDeclarativeGeoMapItemUtils::vec2>>(
                    new QList<QDeclarativeGeoMapItemUtils::vec2>);
        enqueueSimplificationTask(  m_verticesLOD.at(0),
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

QT_END_NAMESPACE
