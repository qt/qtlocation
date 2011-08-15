/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtTest 1.0
import Qt.location 5.0
import Qt.location.test 5.0

Item {
    id: masterItem
    width: 200
    height: 350
    // General-purpose elements for the test:
    Plugin { id: testPlugin; name : "nokia"; PluginParameter {name: "mapping.host"; value: "for.nonexistent"}}
    Coordinate{ id: mapDefaultCenter; latitude: 10; longitude: 30}

    MapCircle {
        objectName: "externalCircle"
        id: externalCircle
        radius: 2000000
        center: mapDefaultCenter
    }

    MapRectangle {
        objectName: "externalRectangle"
        id: externalRectangle
    }

    MapPolygon {
        objectName: "externalPolygon"
        id: externalPolygon
    }

    MapText {
        id: externalText
    }

    MapImage {
        id: externalImage
    }

    MapPolyline {
        id: externalPolyline
    }

    TestModel {
        id: testModel
        datatype: 'coordinate'
        datacount: 7
        delay: 0
    }

    TestModel {
        id: testModel2
        datatype: 'coordinate'
        datacount: 3
        delay: 0
    }

    Map {
        id: mapWithPlugin; center: mapDefaultCenter; plugin: testPlugin;
        anchors.fill: parent; size.width: parent.width; size.height: parent.height; zoomLevel: 2
        MapCircle {
            id: internalCircle
            radius: 2000000
            center: mapDefaultCenter
        }
        MapObjectView {
            id: theObjectView
            model: testModel
            delegate: Component {
                MapCircle {
                    radius: 1500000
                    center: Coordinate {
                        latitude: modeldata.coordinate.latitude;
                        longitude: modeldata.coordinate.longitude;
                    }
                }
            }
        }
    }

    Map {
        id: mapWithoutPlugin; center: mapDefaultCenter;
        anchors.fill: parent; size.width: parent.width; size.height: parent.height; zoomLevel: 2
        MapCircle {
            id: internalCircle2
            radius: 2000000
            center: mapDefaultCenter
        }
        MapObjectView {
            id: theObjectView2
            model: testModel
            delegate: Component {
                MapCircle {
                    radius: 1500000
                    center: Coordinate {
                        latitude: modeldata.coordinate.latitude;
                        longitude: modeldata.coordinate.longitude;
                    }
                }
            }
        }
        MapObjectView {
            id: routeObjectView
            model: routeModel
            delegate: Component {
                MapRoute {
                    route:  routeData
                }
            }
        }
    }

    TestCase {
        name: "MapObjectView"
        function test_a_add_and_remove() {
            // Basic adding and removing of static object
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 8)
            mapWithPlugin.addMapObject(internalCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 8)
            mapWithPlugin.removeMapObject(internalCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 7)
            mapWithPlugin.removeMapObject(internalCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 7)
            // Basic adding and removing of dynamic object
            var dynamicCircle = Qt.createQmlObject( "import QtQuick 2.0; import Qt.location 5.0; MapCircle {radius: 4000; center: mapDefaultCenter}", mapWithPlugin, "");
            mapWithPlugin.addMapObject(dynamicCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 8)
            mapWithPlugin.removeMapObject(dynamicCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 7)
            mapWithPlugin.removeMapObject(dynamicCircle)
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 7)
        }

        SignalSpy {id: model1Spy; target: testModel; signalName: "modelChanged"}
        SignalSpy {id: model2Spy; target: testModel2; signalName: "modelChanged"}
        function test_b_model_change() {
            // Change the model of an MapObjectView on the fly
            // and verify that object counts change accordingly.
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 7)
            testModel.datacount += 2
            testModel2.datacount += 1
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 9)
            theObjectView.model = testModel
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 9)
            theObjectView.model = testModel2
            compare(mapWithPlugin.testGetDeclarativeMapObjectCount(), 4)
        }

        SignalSpy {id: pluginChangedSpy; target: mapWithoutPlugin; signalName: "pluginChanged"}
        function test_c_plugin_set_later() {
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 0)
            mapWithoutPlugin.plugin = testPlugin
            tryCompare(pluginChangedSpy, "count", 1, 1000)
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 10) // 9 from testModel, + 1 from mapcircle
            mapWithoutPlugin.clearMapObjects();
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 9)  // 9 from testModel
            testModel.reset();
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 0)
        }
        Plugin {
            id: testPlugin_immediate;
            name: "qmlgeo.test.plugin"
            parameters: [
                // Parms to guide the test plugin
                PluginParameter { name: "gc_supported"; value: true},
                PluginParameter { name: "gc_finishRequestImmediately"; value: true},
                PluginParameter { name: "gc_validateWellKnownValues"; value: true}
            ]
        }
        RouteQuery {id: routeQuery;
            waypoints: [
                Coordinate {id: fcoordinate1; latitude: 60; longitude: 60},
                Coordinate {id: fcoordinate2; latitude: 61; longitude: 62},
                Coordinate {id: fcoordinate3; latitude: 63; longitude: 64},
                Coordinate {id: fcoordinate4; latitude: 65; longitude: 66},
                Coordinate {id: fcoordinate5; latitude: 67; longitude: 68}
            ]
        }
        // Test routemodel and object list of map
        RouteModel {id: routeModel; plugin: testPlugin_immediate; query: routeQuery }
        SignalSpy {id: objectsChangedSpy; target: mapWithoutPlugin; signalName: "objectsChanged"}
        function test_d_routemodel() {
            objectsChangedSpy.clear()
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 0) // precondition
            compare(mapWithoutPlugin.objects.length, 0)
            compare(objectsChangedSpy.count, 0)
            routeQuery.numberAlternativeRoutes = 4
            routeModel.update();
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 4)
            compare(mapWithoutPlugin.objects.length, 0)
            routeQuery.numberAlternativeRoutes = 3
            routeModel.update();
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 3)
            compare(mapWithoutPlugin.objects.length, 0)
            routeModel.clear();
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 0)
            compare(mapWithoutPlugin.objects.length, 0)
            routeModel.clear(); // clear empty model
            routeQuery.numberAlternativeRoutes = 3
            routeModel.update();
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 3)
            compare(mapWithoutPlugin.objects.length, 0)
            compare(objectsChangedSpy.count, 0)
            mapWithoutPlugin.addMapObject(externalCircle)
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 4)
            compare(mapWithoutPlugin.objects.length, 1)
            compare(objectsChangedSpy.count, 1)
            compare(mapWithoutPlugin.objects[0], externalCircle)
            routeModel.reset(); // all map objects based on model will be cleared
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 1)
            compare(mapWithoutPlugin.objects.length, 1)
            mapWithoutPlugin.clearMapObjects() // clears objects not stemming from model
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 0)
            compare(mapWithoutPlugin.objects.length, 0)
            compare(objectsChangedSpy.count, 2)

            // Test the objects list
            mapWithoutPlugin.addMapObject(externalCircle)
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 1)
            compare(mapWithoutPlugin.objects.length, 1)
            compare(objectsChangedSpy.count, 3)
            compare(mapWithoutPlugin.objects[0], externalCircle)

            mapWithoutPlugin.addMapObject(externalRectangle)
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 2)
            compare(mapWithoutPlugin.objects.length, 2)
            compare(objectsChangedSpy.count, 4)
            compare(mapWithoutPlugin.objects[1], externalRectangle)

            mapWithoutPlugin.addMapObject(externalRectangle)
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 2)
            compare(mapWithoutPlugin.objects.length, 2)
            compare(objectsChangedSpy.count, 4)
            compare(mapWithoutPlugin.objects[1], externalRectangle)

            mapWithoutPlugin.addMapObject(externalPolygon)
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 3)
            compare(mapWithoutPlugin.objects.length, 3)
            compare(objectsChangedSpy.count, 5)
            compare(mapWithoutPlugin.objects[1], externalRectangle)

            mapWithoutPlugin.removeMapObject(externalCircle)
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 2)
            compare(mapWithoutPlugin.objects.length, 2)
            compare(objectsChangedSpy.count, 6)
            compare(mapWithoutPlugin.objects[0], externalRectangle)

            mapWithoutPlugin.removeMapObject(externalRectangle)
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 1)
            compare(mapWithoutPlugin.objects.length, 1)
            compare(objectsChangedSpy.count, 7)
            compare(mapWithoutPlugin.objects[0], externalPolygon)

            mapWithoutPlugin.clearMapObjects()
            compare(mapWithoutPlugin.testGetDeclarativeMapObjectCount(), 0)
            compare(mapWithoutPlugin.objects.length, 0)
            compare(objectsChangedSpy.count, 8)
        }
        function test_e_map_objects_on_delete() {
            // Test that dynamic map & map object creation works
            var dynamicMap = Qt.createQmlObject("import QtQuick 2.0; import Qt.location 5.0; Map {plugin: testPlugin;}", masterItem, "");
            var dynamicCircle = Qt.createQmlObject("import QtQuick 2.0; import Qt.location 5.0; MapCircle {}", masterItem, "");
            compare (dynamicMap.objects.length, 0)
            dynamicMap.addMapObject(dynamicCircle)
            compare (dynamicMap.objects.length, 1)
            dynamicCircle.destroy(2); wait(100) // circle should remove itself from the map
            compare (dynamicMap.objects.length, 0)

            var dynamicRectangle = Qt.createQmlObject("import QtQuick 2.0; import Qt.location 5.0; MapRectangle {}", masterItem, "");
            dynamicMap.addMapObject(dynamicRectangle)
            dynamicMap.destroy(2); wait(100) // must act gracefully although has objects when destroyed
        }
    }
}

