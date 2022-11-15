/****************************************************************************
**
** Copyright (C) 2023 The Qt Company Ltd.
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

import QtQuick
import QtLocation as QL
import QtPositioning as QP

/*!
    \qmltype MapView
    \inqmlmodule QtLocation
    \brief An interactive map viewer component.

    MapView wraps a Map and adds the typical interactive features:
    changing the zoom level, panning and tilting the map.

    The implementation is a QML assembly of smaller building blocks that are
    available separately. In case you want to make changes in your own version
    of this component, you can copy the QML, which is installed into the
    \c qml/QtLocation module directory, and modify it as needed.

    \sa Map
*/
Item {
    /*!
        \qmlproperty Map MapView::map

        This property provides access to the underlying Map instance.
    */
    property alias map: map

    /*!
        \qmlproperty real minimumZoomLevel

        The minimum zoom level according to the size of the view.

        \sa Map::minimumZoomLevel
    */
    // TODO how do we calculate that? map.minimumZoomLevel is 0, but it stops you from zooming out that far
    property real minimumZoomLevel: map.minimumZoomLevel

    /*!
        \qmlproperty real minimumZoomLevel

        The maximum valid zoom level for the map.

        \sa Map::maximumZoomLevel
    */
    property real maximumZoomLevel: map.maximumZoomLevel

    // --------------------------------
    // implementation
    id: root
    Component.onCompleted: map.resetPinchMinMax()

    QL.Map {
        id: map
        width: parent.width
        height: parent.height
        tilt: tiltHandler.persistentTranslation.y / -5
        property bool pinchAdjustingZoom: false

        onZoomLevelChanged: if (!pinchAdjustingZoom) resetPinchMinMax()

        function resetPinchMinMax() {
            pinch.persistentScale = 1
            pinch.scaleAxis.minimum = Math.pow(2, root.minimumZoomLevel - map.zoomLevel + 1)
            pinch.scaleAxis.maximum = Math.pow(2, root.maximumZoomLevel - map.zoomLevel - 1)
        }

        PinchHandler {
            id: pinch
            target: null
            property real rawBearing: 0
            property QP.geoCoordinate startCentroid
            onActiveChanged: if (active) {
                flickAnimation.stop()
                pinch.startCentroid = map.toCoordinate(pinch.centroid.position, false)
            } else {
                flickAnimation.restart(centroid.velocity)
                map.resetPinchMinMax()
            }
            onScaleChanged: (delta) => {
                map.pinchAdjustingZoom = true
                map.zoomLevel += Math.log2(delta)
                map.alignCoordinateToPoint(pinch.startCentroid, pinch.centroid.position)
                map.pinchAdjustingZoom = false
            }
            onRotationChanged: (delta) => {
                pinch.rawBearing -= delta
                // snap to 0° if we're close enough
                map.bearing = (Math.abs(pinch.rawBearing) < 5) ? 0 : pinch.rawBearing
                map.alignCoordinateToPoint(pinch.startCentroid, pinch.centroid.position)
            }
            grabPermissions: PointerHandler.TakeOverForbidden
        }
        // TODO use BoundaryRule to limit zoom?
        WheelHandler {
            id: wheel
            onWheel: (event) => {
                const loc = map.toCoordinate(wheel.point.position)
                map.zoomLevel += event.angleDelta.y / 120
                map.alignCoordinateToPoint(loc, wheel.point.position)
            }
        }
        DragHandler {
            id: drag
            signal flickStarted // for autotests only
            signal flickEnded
            target: null
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
            onActiveChanged: if (active) {
                flickAnimation.stop()
            } else {
                flickAnimation.restart(centroid.velocity)
            }
        }

        property vector3d animDest
        onAnimDestChanged: if (flickAnimation.running) {
            const delta = Qt.vector2d(animDest.x - flickAnimation.animDestLast.x, animDest.y - flickAnimation.animDestLast.y)
            map.pan(-delta.x, -delta.y)
            flickAnimation.animDestLast = animDest
        }

        Vector3dAnimation on animDest {
            id: flickAnimation
            property vector3d animDestLast
            from: Qt.vector3d(0, 0, 0)
            duration: 500
            easing.type: Easing.OutQuad
            onStarted: drag.flickStarted()
            onStopped: drag.flickEnded()

            function restart(vel) {
                stop()
                map.animDest = Qt.vector3d(0, 0, 0)
                animDestLast = Qt.vector3d(0, 0, 0)
                to = Qt.vector3d(vel.x / duration * 100, vel.y / duration * 100, 0)
                start()
            }
        }

        DragHandler {
            id: tiltHandler
            minimumPointCount: 2
            maximumPointCount: 2
            target: null
            xAxis.enabled: false
            grabPermissions: PointerHandler.TakeOverForbidden
            onActiveChanged: if (active) flickAnimation.stop()
        }
    }
}
