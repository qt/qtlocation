// Copyright (C) 2020 The Qt Company Ltd.
// Copyright (C) 2018 Julian Sherollari <jdotsh@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgeojson_p.h"
#include <qvariant.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qjsonarray.h>
#include <qgeocoordinate.h>
#include <qgeocircle.h>
#include <qgeopath.h>
#include <qgeopolygon.h>
#include <qtextstream.h>

QT_BEGIN_NAMESPACE

/*! \namespace QGeoJson
    \inmodule QtLocation
    \keyword QGeoJson Namespace

    \brief The QGeoJson namespace contains functions to import and export
           geolocated information from and to GeoJson files.

    \since 5.13

    The methods in the QGeoJson namespace can be used to convert between a
    GeoJSON document (see the \l {https://en.wikipedia.org/wiki/GeoJSON}
    {Wikipedia page}, \l {https://tools.ietf.org/html/rfc7946} {RFC}) and a
    \l QVariantList of \l QVariantMap elements ready to be used as Model in
    a \l MapItemView.

    \note This private class is part of Qt labs, thus not stable API, it is
    part of the experimental components of QtLocation. Until it is promoted to
    public API, it may be subject to source and binary-breaking changes.

    \section2 Importing GeoJSON

    The importGeoJson() method accepts a \l QJsonDocument from which it
    extracts a single \l {https://tools.ietf.org/html/rfc7159} {JSON} object.
    The GeoJSON RFC expects that a valid GeoJSON Document has in its root
    a single JSON object. This method doesn't perform any validation on the
    input. The importer returns a QVariantList containing a single QVariantMap.
    This map has always at least 2 (key, value) pairs. The first one has \c
    type as key, and the corresponding value is a string identifying the
    GeoJSON type. This value can be one of the GeoJSON object types: \c Point,
    \c MultiPoint, \c LineString, \c MultiLineString, \c Polygon, \c
    MultiPolygon, \c GeometryCollection, \c FeatureCollection. The second pair
    has \c data as key, and the corresponding value can be either a QGeoShape
    or a list, depending on the GeoJSON type. The next section provides details
    about the conversation form GeoJson objects to objects known in Qt Location.
    The \c Feature type is converted into the type of the
    geometry contained within, with an additional (key, value) pair, where the
    key is \c properties and the value is a \l QVariantMap. Thus, a feature Map
    is distinguishable from the corresponding geometry, by looking for a \c
    properties member.

    \section3 Structure of the data node

    For the single type geometry objects (\c Point, \c LineString, and \c
    Polygon), the value corresponding to the \c data key is a QGeoShape:

    \list
    \li When the type is \c Point, the data is a QGeoCircle with the point
        coordinates stored in the center property.

        For example, the following GeoJSON document contains a \c Point
        geometry:

        \code
        {
          "type" : "Point",
          "data" : [60.0, 11.0]
        }
        \endcode

        it is converted to a QVariantMap with the following structure:

        \code
        {
          type : "Point"
          data : QGeoCircle({60.000, 11.000}, -1)
        }
        \endcode

    \li When the type is \c LineString the data is a QGeoPath.

        For example, the following GeoJSON document contains a \c LineString
        geometry:

        \code
        {
            "type" : "LineString",
            "coordinates" : [[13.5, 43],[10.73, 59.92]]
        }
        \endcode

        it is converted to a QVariantMap with the following structure:

        \code
        {
          type : "LineString",
          data : QGeoPath([{43.000, 13.500}, {59.920, 10.730}])
        }
        \endcode

    \li When the type is \c Polygon, the data is a QGeoPolygon (holes are
        supported).

        For example, the following GeoJSON document contains a \c Polygon
        geometry:

        \code
        {
            "type" : "Polygon",
            "coordinates" : [
              [[17.13, 51.11],
               [30.54, 50.42],
               [26.70, 58.36],
               [17.13, 51.11]]
            ],
            "bbox" : [60, 60, -60, -60]

        }
        \endcode

        it is converted to a QVariantMap with the following structure:

        \code
        {
          type : "Polygon"
          data : QGeoPolygon([{51.110, 17.130}, {50.420,30.540}, {58.360, 26.700}, {51.110, 17.130}])
        }
        \endcode

    \endlist

    For the homogeneously typed multipart geometry objects (\c MultiPoint, \c
    MultiLineString, \c MultiPolygon) the value corresponding to the \c data
    key is a QVariantList. Each element of the list is a QVariantMap of one of
    the above listed types. The elements in this list will be all of the same
    GeoJSON type:

    \list
    \li When the type is \c MultiPoint, the data is a List of Points.

        For example, the following GeoJSON document contains a \c MultiPoint
        geometry:

        \code
        {
            "type" : "MultiPoint",
            "coordinates" : [
              [11,60],
              [5.5,60.3],
              [5.7,58.90]
            ]
        }
        \endcode

        it is converted to a QVariantMap with the following structure:

        \code
        {
          type : "MultiPoint"
          data : [
            {
              type : "Point"
              data : QGeoCircle({60.000, 11.000}, -1)
            },
            {
              type : "Point"
              data : QGeoCircle({60.300, 5.500}, -1)
            },
            {
              type : "Point"
              data : QGeoCircle({58.900, 5.700}, -1)
            }
          ]
        }
        \endcode

    \li When the type is \c MultiLineString, the data is a List of LineStrings.

        For example, the following GeoJSON document contains a \c MultiLineString
        geometry:

        \code
        {
            "type" : "MultiLineString",
            "coordinates" : [
              [[13.5, 43], [10.73, 59.92]],
              [[9.15, 45], [-3.15, 58.90]]
            ]
        }
        \endcode

        it is converted to a QVariantMap with the following structure:

        \code
        {
          type : "MultiLineString"
          data : [
            {
              type : "LineString"
              data : QGeoPath([{45.000, 9.150}, {58.900, -3.150}])
            },
            {
              type : "LineString"
              data : QGeoPath([{43.000, 13.500}, {59.920, 10.730}])
            }
          ]
        }
        \endcode

    \li When the type is \c MultiPolygon, the data is a List of Polygons.

        For example, the following GeoJSON document contains a \c MultiPolygon
        geometry:

        \code
        {
            "type" : "MultiPolygon",
            "coordinates" : [
               [
                 [
                   [17.13, 51.11],
                   [30.54, 50.42],
                   [26.74, 58.36],
                   [17.13, 51.11]
                 ]
                ],
               [
                 [
                   [19.84, 41.33],
                   [30.45, 49.26],
                   [17.07, 50.10],
                   [19.84, 41.33]
                 ]
               ]
            ]
        }
        \endcode

        it is converted to a QVariantMap with the following structure:

        \code
        {
          type : "MultiPolygon"
          data : [
            {
              type : "Polygon"
              data : QGeoPolygon([{51.110, 17.130}, {50.420,30.540}, {58.360, 26.700}, {51.110, 17.130}])
            },
            {
              type : "Polygon"
              data : QGeoPolygon([{41.330, 19.840}, {49.260,30.450}, {50.100, 17.070}, {41.330, 19.840}])
            }
          ]
        }
        \endcode

    \endlist

    The \c GeometryCollection is a heterogeneous composition of other geometry
    types. In the resulting QVariantMap, the value of the \c data member is a
    QVariantList populated by QVariantMaps of various geometries, including the
    GeometryCollection itself.

    For example, the following \c GeometryCollection:

    \code
    {
        "type" : "GeometryCollection",
        "geometries" : [
            {
                "type" : "MultiPoint",
                "coordinates" : [
                  [11,60], [5.5,60.3], [5.7,58.90]
                ]
            },
            {
                "type" : "MultiLineString",
                "coordinates" : [
                  [[13.5, 43], [10.73, 59.92]],
                  [[9.15, 45], [-3.15, 58.90]]
                ]
            },
            {
                "type" : "MultiPolygon",
                "coordinates" : [
                    [
                      [[17.13, 51.11],
                       [30.54, 50.42],
                       [26.74, 58.36],
                       [17.13, 51.11]]
                    ],
                    [
                      [[19.84, 41.33],
                       [30.45, 49.26],
                       [17.07, 50.10],
                       [19.84, 41.33]]
                    ]
                ]
            }
        ]
    }
    \endcode

    it is converted to a QVariantMap with the following structure:

    \code
    {
      type : "GeometryCollection"
      data : [
        {
          type : "MultiPolygon"
          data : [
            {
              type : "Polygon"
              data : QGeoPolygon([{41.330, 19.840}, {49.260, 30.450}, {50.100, 17.070}, {41.330, 19.840}])
            }
            {
              type : "Polygon"
              data : QGeoPolygon([{51.110, 17.130}, {50.420, 30.540}, {58.360, 26.740}, {51.110, 17.130}])
            }
          ]
        }
        {
          type : "MultiLineString"
          data : [
            {
              type : "LineString"
              data : QGeoPath([{45.000, 9.150}, {58.900, -3.150}])
            }
            {
              type : "LineString"
              data : QGeoPath([{43.000, 13.500}, {59.920, 10.730}])
            }
          ]
        }
        {
          type : "MultiPoint"
          data : [
            {
              type : Point
              data : QGeoCircle({58.900, 5.700}, -1)
            },
            {
              type : Point
              data : QGeoCircle({60.300, 5.500}, -1)
            },
            {
              type : Point
              data : QGeoCircle({60.000, 11.000}, -1)
            }
          ]
        }
      ]
    }
    \endcode

    The \c Feature object, which consists of one of the previous geometries
    together with related attributes, is structured like one of the 7 above
    mentioned geometry types, plus a \c properties member. The value of this
    member is a QVariantMap. The only way to distinguish a Feature from the
    included geometry is to check if a \c properties node is present in the
    QVariantMap.

    For example, the following \c Feature:

    \code
    {
        "type" : "Feature",
        "id" : "Poly",
        "properties" : {
            "text" : "This is a Feature with a Polygon"
        },
        "geometry" : {
            "type" : "Polygon",
            "coordinates" : [
                [[17.13, 51.11],
                 [30.54, 50.42],
                 [26.70, 58.36],
                 [17.13, 51.11]],
                [[23.46, 54.36],
                 [20.52, 51.91],
                 [28.25, 51.50],
                 [26.80, 54.36],
                 [23.46, 54.36]]
            ]
        }
    }
    \endcode

    it is converted to a QVariantMap with the following structure:

    \code
    {
      type : "Polygon"
      data : QGeoPolygon([{51.110, 17.130}, {50.420,30.540}, {58.360, 26.700}, {51.110, 17.130}])
      properties : {text : "This is a Feature with a Polygon"}
    }
    \endcode

    The \c FeatureCollection is a composition of Feature objects. The value of
    the \c data member in a FeatureCollection is a QVariantList populated by
    Feature type QVariantMaps.

    For example, the following \c FeatureCollection:

    \code
    {
        "type" : "FeatureCollection",
        "features" : [
            {
                "type" : "Feature",
                "id" : "Poly",
                "properties" : {
                    "text" : "This is a Feature with a Polygon"
                },
                "geometry" : {
                   "type" : "Polygon",
                   "coordinates" : [
                      [[17.13, 51.11],
                       [30.54, 50.42],
                       [26.70, 58.36],
                       [17.13, 51.11]],
                      [[23.46, 54.36],
                       [20.52, 51.91],
                       [28.25, 51.50],
                       [26.80, 54.36],
                       [23.46, 54.36]]
                   ]
               }
           },
           {
               "type" : "Feature",
               "id" : "MultiLine",
               "properties" : {
                   "text" : "This is a Feature with a MultiLineString"
               },
               "geometry" : {
                  "type" : "MultiLineString",
                  "coordinates" : [
                    [[13.5, 43], [10.73, 59.92]],
                    [[9.15, 45], [-3.15, 58.90]]
                  ]
               }
           }
        ]
    }
    \endcode

    it is converted to a QVariantMap with the following structure:

    \code
    {
      type : "FeatureCollection"
      data : [
        {
          type : "MultiLineString"
          data : [
            {
              type : "LineString"
              data : QGeoPath([{45.000, 9.150}, {58.900, -3.150}])
            }
            {
              type : "LineString"
              data : QGeoPath([{43.000, 13.500}, {59.920, 10.730}])
            }
          ]
          properties : {text : "This is a Feature with a MultiLineString"}
        },
        {
          type : "Polygon"
          data : QGeoPolygon({51.110, 17.130}, {50.420, 30.540}, {58.360, 26.700}, {51.110, 17.130})
          properties : {text : "This is a Feature with a Polygon"}
        }
      ]
    }
    \endcode

    \section2 Exporting GeoJSON

    The exporter accepts the QVariantList returned by the \l {Importing GeoJSON}
    {importer}, and returns a JSON document. The exporter is complementary to
    the importer because it executes the inverse action.

    \section2 The toString function

    The \l toString outputs, for debugging purposes, the content of a
    QVariantList structured like \l importGeoJson does, to a QString using a
    prettyfied format.
*/

static QGeoCoordinate importPosition(const QVariant &position)
{
    QGeoCoordinate returnedCoordinates;
    const QVariantList positionList = position.value<QVariantList>();
    for (qsizetype i  = 0; i < positionList.size(); ++i) { // Iterating Point coordinates arrays
        switch (i) {
        case 0:
            returnedCoordinates.setLongitude(positionList.at(i).toDouble());
            break;
        case 1:
            returnedCoordinates.setLatitude(positionList.at(i).toDouble());
            break;
        case 2:
            returnedCoordinates.setAltitude(positionList.at(i).toDouble());
            break;
        default:
            break;
        }
    }
    return returnedCoordinates;
}

static QList<QGeoCoordinate> importArrayOfPositions(const QVariant &arrayOfPositions)
{
    QList<QGeoCoordinate> returnedCoordinates;
    const QVariantList positionsList = arrayOfPositions.value<QVariantList>();
    for (const auto &position : positionsList) // Iterating the LineString coordinates nested arrays
        returnedCoordinates.append(importPosition(position)); // Populating the QList of coordinates
    return returnedCoordinates;
}

static QList<QList<QGeoCoordinate>> importArrayOfArrayOfPositions(const QVariant &arrayOfArrayofPositions)
{
    QList<QList<QGeoCoordinate>> returnedCoordinates;
    const QVariantList positionsList = arrayOfArrayofPositions.value<QVariantList>();
    for (const auto &position : positionsList) // Iterating the Polygon coordinates nested arrays
        returnedCoordinates << importArrayOfPositions(position);
    return returnedCoordinates;
}

static QGeoCircle importPoint(const QVariantMap &inputMap)
{
    QGeoCircle returnedObject;
    QGeoCoordinate center;
    QVariant valueCoords = inputMap.value(QStringLiteral("coordinates"));
    center = importPosition(valueCoords);
    returnedObject.setCenter(center);
    return returnedObject;
}

static QGeoPath importLineString(const QVariantMap &inputMap)
{
    QGeoPath returnedObject;
    QList <QGeoCoordinate> coordinatesList;
    const QVariant valueCoordinates = inputMap.value(QStringLiteral("coordinates"));
    coordinatesList = importArrayOfPositions(valueCoordinates);
    returnedObject.setPath(coordinatesList);
    return returnedObject;
}

static QGeoPolygon importPolygon(const QVariantMap &inputMap)
{
    QGeoPolygon returnedObject;
    const QVariant valueCoordinates = inputMap.value(QStringLiteral("coordinates"));
    const QList<QList<QGeoCoordinate>> perimeters = importArrayOfArrayOfPositions(valueCoordinates);
    for (qsizetype i  = 0; i < perimeters.size(); ++i) { // Import an array of QList<QGeocoordinates>
        if (i == 0)
            returnedObject.setPerimeter(perimeters.at(i)); // External perimeter
        else
            returnedObject.addHole(perimeters.at(i)); // Inner perimeters
    }
    return returnedObject;
}

static QVariantList importMultiPoint(const QVariantMap &inputMap)
{
    QVariantList returnedObject;
    const QVariantList coordinatesList = inputMap.value(QStringLiteral("coordinates")).value<QVariantList>();
    QVariantMap singlePointMap;
    QGeoCircle parsedPoint;
    for (const auto &coordinate : coordinatesList) { // Iterating MultiPoint coordinates nasted arrays
        parsedPoint.setCenter(importPosition(coordinate));
        singlePointMap.insert(QStringLiteral("type"), QStringLiteral("Point"));
        singlePointMap.insert(QStringLiteral("data"), QVariant::fromValue(parsedPoint));
        returnedObject.append(QVariant::fromValue(singlePointMap));
    }
    return returnedObject;
}

static QVariantList importMultiLineString(const QVariantMap &inputMap)
{
    QVariantList returnedObject;
    QGeoPath parsedLineString;
    const QVariant listCoords = inputMap.value(QStringLiteral("coordinates"));
    const QVariantList list = listCoords.value<QVariantList>();
    QVariantMap singleLinestringMap;
    for (int i  = 0; i < list.size(); ++i) { // Iterating the MultiLineString coordinates nasted arrays using importArrayOfPositions
        singleLinestringMap.clear();
        const QList <QGeoCoordinate> coordinatesList = importArrayOfPositions(list.at(i));
        singleLinestringMap.insert(QStringLiteral("type"), QStringLiteral("LineString"));
        parsedLineString.setPath(coordinatesList);
        singleLinestringMap.insert(QStringLiteral("data"), QVariant::fromValue(parsedLineString));
        returnedObject.append(QVariant::fromValue(singleLinestringMap));
    }
    return returnedObject;
}

static QVariantList importMultiPolygon(const QVariantMap &inputMap)
{
    QVariantList returnedObject;
    QGeoPolygon singlePoly;
    QVariantMap singlePolygonMap;
    const QVariant valueCoordinates = inputMap.value(QStringLiteral("coordinates"));
    const QVariantList list = valueCoordinates.value<QVariantList>();
    for (int i = 0; i < list.size(); ++i) { // Iterating the MultiPolygon coordinates nasted arrays
    singlePolygonMap.clear();
        const QList<QList<QGeoCoordinate>> coordinatesList = importArrayOfArrayOfPositions(list.at(i));

        for (int j = 0; j < coordinatesList.size(); ++j) {
            if (j == 0)
                singlePoly.setPerimeter(coordinatesList.at(j));
            else
                singlePoly.addHole(coordinatesList.at(j));
        }
        singlePolygonMap.insert(QStringLiteral("type"), QStringLiteral("Polygon"));
        singlePolygonMap.insert(QStringLiteral("data"), QVariant::fromValue(singlePoly));
        returnedObject.append(QVariant::fromValue(singlePolygonMap));
    }
    return returnedObject;
}

static QVariantMap importGeometry(const QVariantMap &inputMap); // Function prototype for a tail recursion

static QVariantList importGeometryCollection(const QVariantMap &inputMap)
{
    QVariantList returnedObject;
    const QVariant listGeometries = inputMap.value(QStringLiteral("geometries"));
    const QVariantList list = listGeometries.value<QVariantList>(); // QVariantList of heterogeneous composition of the other geometry types
    for (int i = 0; i < list.size(); ++i) {
        QVariantMap geometryMap = list.at(i).value<QVariantMap>();
        QVariantMap geoMap = importGeometry(geometryMap);
        returnedObject.append(geoMap);
    }
    return returnedObject;
}

static QVariantMap importGeometry(const QVariantMap &inputMap)
{
    QVariantMap returnedObject;
    QString geometryTypes[] = {
        QStringLiteral("Point"),
        QStringLiteral("MultiPoint"),
        QStringLiteral("LineString"),
        QStringLiteral("MultiLineString"),
        QStringLiteral("Polygon"),
        QStringLiteral("MultiPolygon"),
        QStringLiteral("GeometryCollection")
    };
    enum geoTypeSwitch {
        Point,
        MultiPoint,
        LineString,
        MultiLineString,
        Polygon,
        MultiPolygon,
        GeometryCollection
    };
    for (int i = 0; i<7; ++i) {
        if (inputMap.value(QStringLiteral("type")).value<QString>() == geometryTypes[i]) {
            switch (i) {
            case Point: {
                returnedObject.insert(QStringLiteral("type"), QStringLiteral("Point"));
                returnedObject.insert(QStringLiteral("data"), QVariant::fromValue(importPoint(inputMap)));
                break;
            }
            case MultiPoint: {
                returnedObject.insert(QStringLiteral("type"), QStringLiteral("MultiPoint"));
                returnedObject.insert(QStringLiteral("data"), QVariant::fromValue(importMultiPoint(inputMap)));
                break;
            }
            case LineString: {
                returnedObject.insert(QStringLiteral("type"), QStringLiteral("LineString"));
                returnedObject.insert(QStringLiteral("data"), QVariant::fromValue(importLineString(inputMap)));
                break;
            }
            case MultiLineString: {
                returnedObject.insert(QStringLiteral("type"), QStringLiteral("MultiLineString"));
                returnedObject.insert(QStringLiteral("data"), QVariant::fromValue(importMultiLineString(inputMap)));
                break;
            }
            case Polygon: {
                returnedObject.insert(QStringLiteral("type"), QStringLiteral("Polygon"));
                returnedObject.insert(QStringLiteral("data"), QVariant::fromValue(importPolygon(inputMap)));
                break;
            }
            case MultiPolygon: {
                returnedObject.insert(QStringLiteral("type"), QStringLiteral("MultiPolygon"));
                returnedObject.insert(QStringLiteral("data"), QVariant::fromValue(importMultiPolygon(inputMap)));
                break;
            }
            case GeometryCollection: {
                returnedObject.insert(QStringLiteral("type"), QStringLiteral("GeometryCollection"));
                returnedObject.insert(QStringLiteral("data"), QVariant::fromValue(importGeometryCollection(inputMap)));
                break;
            }
            default:
                break;
            }
        }
    }
    return returnedObject;
}

static QVariantList importFeatureCollection(const QVariantMap &inputMap)
{
    QVariantList returnedObject;
    const QVariantList featuresList = inputMap.value(QStringLiteral("features")).value<QVariantList>();
    for (int i = 0; i < featuresList.size(); ++i) {
        QVariantMap inputFeatureMap = featuresList.at(i).value<QVariantMap>();
        QVariantMap singleFeatureMap = importGeometry(inputFeatureMap.value(QStringLiteral("geometry")).value<QVariantMap>());
        const QVariantMap importedProperties = inputFeatureMap.value(QStringLiteral("properties")).value<QVariantMap>();
        singleFeatureMap.insert(QStringLiteral("properties"), importedProperties);
        if (inputFeatureMap.contains(QStringLiteral("id"))) {
            QVariant importedId = inputFeatureMap.value(QStringLiteral("id")).value<QVariant>();
            singleFeatureMap.insert(QStringLiteral("id"), importedId);
        }
        returnedObject.append(singleFeatureMap);
    }
    return returnedObject;
}

static QJsonValue exportPosition(const QGeoCoordinate &obtainedCoordinates)
{
    QJsonValue geoLat = obtainedCoordinates.latitude();
    QJsonValue geoLong = obtainedCoordinates.longitude();
    QJsonArray array = {geoLong, geoLat};
    QJsonValue geoAlt;
    if (!qIsNaN(obtainedCoordinates.altitude())) {
        geoAlt = obtainedCoordinates.altitude();
        array.append(geoAlt);
    }
    QJsonValue geoArray = array;
    return geoArray;
}

static QJsonValue exportArrayOfPositions(const QList<QGeoCoordinate> &obtainedCoordinatesList)
{
    QJsonValue lineCoordinates;
    QJsonValue multiPosition;
    QJsonArray arrayPosition;
    for (int i = 0; i < obtainedCoordinatesList.size(); ++i) {
        multiPosition = exportPosition(obtainedCoordinatesList.at(i));
        arrayPosition.append(multiPosition);
    }
    lineCoordinates = arrayPosition;
    return lineCoordinates;
}

static QJsonValue exportArrayOfArrayOfPositions(const QList<QList<QGeoCoordinate>> &obtainedCoordinates)
{
    QJsonValue lineCoordinates;
    QJsonValue polyCoordinates;
    QJsonArray arrayPath;
    for (int i = 0; i < obtainedCoordinates.size(); ++i) {
        lineCoordinates = exportArrayOfPositions(obtainedCoordinates.at(i));
        arrayPath.append(lineCoordinates);
    }
    polyCoordinates = arrayPath;
    return polyCoordinates;
}

static QJsonObject exportPoint(const QVariantMap &pointMap)
{
    QJsonObject parsedPoint;
    QGeoCircle circle = pointMap.value(QStringLiteral("data")).value<QGeoCircle>();
    parsedPoint.insert(QStringLiteral("type"), QJsonValue(QStringLiteral("Point")));
    parsedPoint.insert(QStringLiteral("coordinates"), exportPosition(circle.center()));
    return parsedPoint;
}

static QJsonObject exportLineString(const QVariantMap &lineStringMap)
{
    QJsonObject parsedLineString;
    QList <QGeoCoordinate> linestringPath = lineStringMap.value(QStringLiteral("data")).value<QGeoPath>().path();
    parsedLineString.insert(QStringLiteral("type"), QJsonValue(QStringLiteral("LineString")));
    parsedLineString.insert(QStringLiteral("coordinates"), exportArrayOfPositions(linestringPath));
    return parsedLineString;
}

static QJsonObject exportPolygon(const QVariantMap &polygonMap)
{
    QVariant polygonVariant = polygonMap.value(QStringLiteral("data"));
    QJsonObject parsedPolygon;
    QJsonValue polyCoordinates;
    QList<QList<QGeoCoordinate>> obtainedCoordinatesPoly;
    QGeoPolygon parsedPoly = polygonVariant.value<QGeoPolygon>();
    obtainedCoordinatesPoly << parsedPoly.perimeter();
    if (parsedPoly.holesCount()!=0)
        for (int i = 0; i < parsedPoly.holesCount(); ++i) {
            obtainedCoordinatesPoly << parsedPoly.holePath(i);
        }
    polyCoordinates = exportArrayOfArrayOfPositions(obtainedCoordinatesPoly);
    parsedPolygon.insert(QStringLiteral("type"), QJsonValue(QStringLiteral("Polygon")));
    parsedPolygon.insert(QStringLiteral("coordinates"), polyCoordinates);
    return parsedPolygon;
}

static QJsonObject exportMultiPoint(const QVariantMap &multiPointMap)
{
    QJsonObject parsedMultiPoint;
    QList <QGeoCoordinate> obtainedCoordinatesMP;
    QVariantList multiCircleVariantList = multiPointMap.value(QStringLiteral("data")).value<QVariantList>();
    for (const QVariant &exCircleVariantMap: multiCircleVariantList) {
        obtainedCoordinatesMP << exCircleVariantMap.value<QVariantMap>().value(QStringLiteral("data")).value<QGeoCircle>().center();
    }
    QJsonValue multiPosition = exportArrayOfPositions(obtainedCoordinatesMP);
    parsedMultiPoint.insert(QStringLiteral("type"), QJsonValue(QStringLiteral("MultiPoint")));
    parsedMultiPoint.insert(QStringLiteral("coordinates"), multiPosition);
    return parsedMultiPoint;
}

static QJsonObject exportMultiLineString(const QVariantMap &multiLineStringMap)
{
    QJsonObject parsedMultiLineString;
    QList<QList<QGeoCoordinate>> extractedCoordinatesValue;
    QVariant multiPathVariant = multiLineStringMap.value(QStringLiteral("data"));
    QVariantList multiPathList = multiPathVariant.value<QVariantList>();
    for (int i = 0; i < multiPathList.size(); ++i) {
        extractedCoordinatesValue << multiPathList.at(i).value<QVariantMap>().value(QStringLiteral("data")).value<QGeoPath>().path();
    }
    QJsonValue exportedCoordinatesValue = exportArrayOfArrayOfPositions(extractedCoordinatesValue);
    parsedMultiLineString.insert(QStringLiteral("type"), QJsonValue(QStringLiteral("MultiLineString")));
    parsedMultiLineString.insert(QStringLiteral("coordinates"), exportedCoordinatesValue);
    return parsedMultiLineString;
}

static QJsonObject exportMultiPolygon(const QVariantMap &multiPolygonMap)
{
    QJsonObject parsedMultiPolygon;
    QJsonValue polyCoordinates;
    QJsonArray parsedArrayPolygon;
    QList<QList<QGeoCoordinate>> extractedCoordinatesValue;
    QVariant multiPolygonVariant = multiPolygonMap.value(QStringLiteral("data"));
    QVariantList multiPolygonList = multiPolygonVariant.value<QVariantList>();
    int polyHoles = 0;
    int currentHole;
    for (int i = 0; i < multiPolygonList.size(); ++i) { // Start parsing Polygon list
        extractedCoordinatesValue << multiPolygonList.at(i).value<QVariantMap>().value(QStringLiteral("data")).value<QGeoPolygon>().perimeter(); // Extract external polygon path
        polyHoles = multiPolygonList.at(i).value<QVariantMap>().value(QStringLiteral("data")).value<QGeoPolygon>().holesCount();
        if (polyHoles) // Check if the polygon has holes
            for (currentHole = 0 ; currentHole < polyHoles; currentHole++)
                extractedCoordinatesValue << multiPolygonList.at(i).value<QVariantMap>().value(QStringLiteral("data")).value<QGeoPolygon>().holePath(currentHole);
        polyCoordinates = exportArrayOfArrayOfPositions(extractedCoordinatesValue); // Generates QJsonDocument compatible value
        parsedArrayPolygon.append(polyCoordinates); // Adds one level of nesting in coordinates
        extractedCoordinatesValue.clear(); // Clears the temporary polygon linear ring storage
    }
    QJsonValue exportedCoordinatesNodeValue = parsedArrayPolygon;
    parsedMultiPolygon.insert(QStringLiteral("type"), QJsonValue(QStringLiteral("MultiPolygon")));
    parsedMultiPolygon.insert(QStringLiteral("coordinates"), exportedCoordinatesNodeValue);
    return parsedMultiPolygon;
}

static QJsonObject exportGeometry(const QVariantMap &geometryMap); // Function prototype

static QJsonObject exportGeometryCollection(const QVariantMap &geometryCollection)
{
    QJsonObject parsed;
    QJsonObject parsedGeometry;
    QJsonValue valueGeometries;
    QJsonArray parsedGeometries;
    QVariantList geometriesList = geometryCollection.value(QStringLiteral("data")).value<QVariantList>();
    for (int i = 0; i < geometriesList.size(); ++i) {
        parsedGeometry = exportGeometry(geometriesList.at(i).value<QVariantMap>());
        valueGeometries = parsedGeometry;
        parsedGeometries.append(valueGeometries);
    }
    QJsonValue exportedGeometriesValue = parsedGeometries;
    parsed.insert(QStringLiteral("type"), QJsonValue(QStringLiteral("GeometryCollection")));
    parsed.insert(QStringLiteral("geometries"), exportedGeometriesValue);
    return parsed;
}

static QJsonObject exportGeometry(const QVariantMap &geometryMap)
{
    QJsonObject exportedGeometry;
    if (geometryMap.value(QStringLiteral("type")) == QStringLiteral("Point"))
        exportedGeometry = exportPoint(geometryMap);
    if (geometryMap.value(QStringLiteral("type")) == QStringLiteral("MultiPoint"))
        exportedGeometry = exportMultiPoint(geometryMap);
    if (geometryMap.value(QStringLiteral("type")) == QStringLiteral("LineString"))
        exportedGeometry = exportLineString(geometryMap);
    if (geometryMap.value(QStringLiteral("type")) == QStringLiteral("MultiLineString"))
        exportedGeometry = exportMultiLineString(geometryMap);
    if (geometryMap.value(QStringLiteral("type")) == QStringLiteral("Polygon"))
        exportedGeometry = exportPolygon(geometryMap);
    if (geometryMap.value(QStringLiteral("type")) == QStringLiteral("MultiPolygon"))
        exportedGeometry = exportMultiPolygon(geometryMap);
    if (geometryMap.value(QStringLiteral("type")) == QStringLiteral("GeometryCollection"))
        exportedGeometry = exportGeometryCollection(geometryMap);
    return exportedGeometry;
}

static QJsonObject exportFeature(const QVariantMap &featureMap)
{
    QJsonObject exportedFeature;
    QJsonValue geometryNodeValue = QJsonValue(exportGeometry(featureMap));
    QJsonValue propertiesNodeValue = featureMap.value(QStringLiteral("properties")).value<QVariant>().toJsonValue();
    QJsonValue idNodeValue = featureMap.value(QStringLiteral("id")).value<QVariant>().toJsonValue();
    exportedFeature.insert(QStringLiteral("type"), QJsonValue(QStringLiteral("Feature")));
    exportedFeature.insert(QStringLiteral("geometry"), geometryNodeValue);
    exportedFeature.insert(QStringLiteral("properties"), propertiesNodeValue);
    if (!idNodeValue.isNull()) // this value is optional
        exportedFeature.insert(QStringLiteral("id"), idNodeValue);
    return exportedFeature;
}

static QJsonObject exportFeatureCollection(const QVariantMap &featureCollection)
{
    QJsonObject exportedFeatureCollection;
    QJsonArray featureArray;
    QVariantList featureList  = featureCollection.value(QStringLiteral("data")).value<QVariantList>();
    for (int i = 0; i < featureList.size(); ++i) {
        featureArray.append(QJsonValue(exportFeature(featureList.at(i).value<QVariantMap>())));
    }
    exportedFeatureCollection.insert(QStringLiteral("type"), QJsonValue(QStringLiteral("FeatureCollection")));
    exportedFeatureCollection.insert(QStringLiteral("features"), QJsonValue(featureArray) );
    return exportedFeatureCollection;
}

namespace QGeoJson
{

/*!
This method imports the \a geoJson document, expected to contain valid GeoJSON
data, into a QVariantList structured like described in the section \l
{Importing GeoJSON}.

\note This method performs no validation on the input.

\sa exportGeoJson
*/
QVariantList importGeoJson(const QJsonDocument &geoJson)
{
    QVariantList returnedList;
    QJsonObject object = geoJson.object(); // Read json object from imported doc
    QVariantMap rootGeoJsonObject = object.toVariantMap(); // Extraced map using Qt's API
    QString geoType[] = {
        QStringLiteral("Point"),
        QStringLiteral("MultiPoint"),
        QStringLiteral("LineString"),
        QStringLiteral("MultiLineString"),
        QStringLiteral("Polygon"),
        QStringLiteral("MultiPolygon"),
        QStringLiteral("GeometryCollection"),
        QStringLiteral("Feature"),
        QStringLiteral("FeatureCollection")
    };
    enum geoTypeSwitch {
        Point,
        MultiPoint,
        LineString,
        MultiLineString,
        Polygon,
        MultiPolygon,
        GeometryCollection,
        Feature,
        FeatureCollection
    };
    QVariantMap parsedGeoJsonMap;

    // Checking whether the JSON object has a "type" member
    const QVariant keyVariant = rootGeoJsonObject.value(QStringLiteral("type"));
    if (!keyVariant.isValid()) {
        // Type check failed
    }
    QString valueType = keyVariant.value<QString>();

    // Checking whether the "type" member has a GeoJSON admitted value
    for (int i = 0; i < 9; ++i) {
        if (valueType == geoType[i]) {
            switch (i) {
            case Point: {
                QGeoCircle circle = importPoint(rootGeoJsonObject);
                QVariant dataNodeValue = QVariant::fromValue(circle);
                parsedGeoJsonMap.insert(QStringLiteral("type"), QStringLiteral("Point"));
                parsedGeoJsonMap.insert(QStringLiteral("data"), dataNodeValue);
                break;
            }
            case MultiPoint: {
                QVariantList multiCircle = importMultiPoint(rootGeoJsonObject);
                QVariant dataNodeValue = QVariant::fromValue(multiCircle);
                QList <QGeoCircle> testlist;
                parsedGeoJsonMap.insert(QStringLiteral("type"), QStringLiteral("MultiPoint"));
                parsedGeoJsonMap.insert(QStringLiteral("data"), dataNodeValue);
                break;
            }
            case LineString: {
                QGeoPath lineString = importLineString(rootGeoJsonObject);
                QVariant dataNodeValue = QVariant::fromValue(lineString);
                parsedGeoJsonMap.insert(QStringLiteral("type"), QStringLiteral("LineString"));
                parsedGeoJsonMap.insert(QStringLiteral("data"), dataNodeValue);
                break;
            }
            case MultiLineString: {
                QVariantList multiLineString = importMultiLineString(rootGeoJsonObject);
                QVariant dataNodeValue = QVariant::fromValue(multiLineString);
                parsedGeoJsonMap.insert(QStringLiteral("type"), QStringLiteral("MultiLineString"));
                parsedGeoJsonMap.insert(QStringLiteral("data"), dataNodeValue);
                break;
            }
            case Polygon: {
                QGeoPolygon poly = importPolygon(rootGeoJsonObject);
                QVariant dataNodeValue = QVariant::fromValue(poly);
                parsedGeoJsonMap.insert(QStringLiteral("type"), QStringLiteral("Polygon"));
                parsedGeoJsonMap.insert(QStringLiteral("data"), dataNodeValue);
                break;
            }
            case MultiPolygon: {
                QVariantList multiPoly = importMultiPolygon(rootGeoJsonObject);
                QVariant dataNodeValue = QVariant::fromValue(multiPoly);
                parsedGeoJsonMap.insert(QStringLiteral("type"), QStringLiteral("MultiPolygon"));
                parsedGeoJsonMap.insert(QStringLiteral("data"), dataNodeValue);
                break;
            }
            // List of GeoJson geometry objects
            case GeometryCollection: {
                QVariantList multiGeo = importGeometryCollection(rootGeoJsonObject);
                QVariant dataNodeValue = QVariant::fromValue(multiGeo);
                parsedGeoJsonMap.insert(QStringLiteral("type"), QStringLiteral("GeometryCollection"));
                parsedGeoJsonMap.insert(QStringLiteral("data"), dataNodeValue);
                break;
            }
            // Single GeoJson geometry object with properties
            case Feature: {
                parsedGeoJsonMap = importGeometry(rootGeoJsonObject.value(QStringLiteral("geometry")).value<QVariantMap>());
                QVariantMap importedProperties = rootGeoJsonObject.value(QStringLiteral("properties")).value<QVariantMap>();
                parsedGeoJsonMap.insert(QStringLiteral("properties"), importedProperties);
                if (rootGeoJsonObject.contains(QStringLiteral("id"))){
                    QVariant importedId = rootGeoJsonObject.value(QStringLiteral("id")).value<QVariant>();
                    parsedGeoJsonMap.insert(QStringLiteral("id"), importedId);
                }
                break;
            }
            // Heterogeneous list of GeoJSON geometries with properties
            case FeatureCollection: {
                QVariantList featCollection = importFeatureCollection(rootGeoJsonObject);
                QVariant dataNodeValue = QVariant::fromValue(featCollection);
                parsedGeoJsonMap.insert(QStringLiteral("type"), QStringLiteral("FeatureCollection"));
                parsedGeoJsonMap.insert(QStringLiteral("data"), dataNodeValue);
                break;
            }
            default:
                break;
            }
            QVariant bboxNodeValue = rootGeoJsonObject.value(QStringLiteral("bbox"));
            if (bboxNodeValue.isValid()) {
                parsedGeoJsonMap.insert(QStringLiteral("bbox"), bboxNodeValue);
            }
            returnedList.append(parsedGeoJsonMap);
        } else if (i >= 9) {
            // Error
            break;
        }
    }
    return returnedList;
}

/*!
This method exports the QVariantList \a geoData, expected to be structured like
described in the section \l {Importing GeoJSON}, to a QJsonDocument containing
the data converted to GeoJSON.

\note This method performs no validation on the input.

\sa importGeoJson
*/
QJsonDocument exportGeoJson(const QVariantList &geoData)
{
    if (geoData.isEmpty())
        return QJsonDocument();
    QVariantMap exportMap = geoData.at(0).value<QVariantMap>(); // Extracting the QVMap
    QJsonObject newObject;
    QJsonDocument newDocument;
    if (exportMap.contains(QStringLiteral("properties"))) {
        newObject = exportFeature(exportMap);
    } else {
        if (exportMap.value(QStringLiteral("type")) == QStringLiteral("Point")) // Check the value corresponding to the key "Point"
            newObject = exportPoint(exportMap);
        if (exportMap.value(QStringLiteral("type")) == QStringLiteral("MultiPoint"))
            newObject = exportMultiPoint(exportMap);
        if (exportMap.value(QStringLiteral("type")) == QStringLiteral("LineString"))
            newObject = exportLineString(exportMap);
        if (exportMap.value(QStringLiteral("type")) == QStringLiteral("MultiLineString"))
            newObject = exportMultiLineString(exportMap);
        if (exportMap.value(QStringLiteral("type")) == QStringLiteral("Polygon"))
            newObject = exportPolygon(exportMap);
        if (exportMap.value(QStringLiteral("type")) == QStringLiteral("MultiPolygon"))
            newObject = exportMultiPolygon(exportMap);
        if (exportMap.value(QStringLiteral("type")) == QStringLiteral("GeometryCollection"))
            newObject = exportGeometryCollection(exportMap);
        if (exportMap.value(QStringLiteral("type")) == QStringLiteral("FeatureCollection"))
            newObject = exportFeatureCollection(exportMap);
    }
    if (exportMap.contains((QStringLiteral("bbox")))) {
        QJsonArray bboxArray;
        QVariantList bboxList = exportMap.value(QStringLiteral("bbox")).value<QVariantList>();
        for (int i = 0; i < bboxList.size(); ++i) {
            bboxArray.append(QJsonValue(bboxList.at(i).value<double>()));
        }
        newObject.insert(QStringLiteral("bbox"), QJsonValue(bboxArray));
    }
    newDocument.setObject(newObject);
    return newDocument;
}

// Functions for toString
QTextStream &operator<<(QTextStream &stream, const QGeoCoordinate &crd)
{
    stream << "{ " << QString::number(crd.latitude(), 'f', 3) << ", "
                   << QString::number(crd.longitude(), 'f', 3) << ", "
                   << QString::number(crd.altitude(), 'f', 3) << " }";
    return stream;
}

QTextStream &operator<<(QTextStream &stream, const QGeoShape &shape)
{
    switch (shape.type()) {
    case QGeoShape::CircleType: {
            QGeoCircle circle(shape);
            stream << "QGeoCircle(" <<circle.center() << ", "<< QString::number(circle.radius()) << ")";
            break;
        }
    case QGeoShape::PathType: {
            QGeoPath path(shape);
            stream << "QGeoPath(";
            for (auto c: path.path())
                stream << c << ", ";
            stream << ")";
            break;
        }
    case QGeoShape::PolygonType: {
            QGeoPolygon poly(shape);
            stream << "QGeoPolygon(";
            for (auto c: poly.perimeter())
                stream << c << ", ";
            stream << ")";
            break;
        }
    default:
        stream << "QGeoShape(Unknown)";
        break;
    }
    return stream;
}


static QString printQvariant(const QVariant v, int tabs = 0) {
    static constexpr QStringView sTab(u"  ");
    QString sTabs;
    QString res;
    QTextStream stream(&res);
    for (int i  = 0; i< tabs; i++) {
        sTabs += sTab;
    }
    if (v.typeId() == QMetaType::QVariantList) {
        stream << sTabs << "[\n";
        const QVariantList &l = v.toList();
        for (int i  = 0; i < l.size(); ++i)
            stream << printQvariant(l.at(i), tabs + 1);
        stream << sTabs << "]\n";
    } else if (v.typeId() == QMetaType::QVariantMap) {
        stream  << sTabs << "{\n";
        const QVariantList &l = v.toList();
        const QVariantMap &map = v.toMap();

        // Either one or the other are valid
        if (!map.keys().isEmpty()) {
            // Handle type first, to easy reading
            if (map.contains(QStringLiteral("type"))) {
                stream  << sTabs << sTab << QStringLiteral("type") << " : "
                        << printQvariant(map[QStringLiteral("type")], tabs + 1).remove(QRegularExpression(QStringLiteral("^[ ]*")));;
            }
            for (QVariantMap::const_iterator iter = map.begin(); iter != map.end(); ++iter) {
                if (iter.key() == QStringLiteral("type"))
                    continue;
                stream  << sTabs << sTab << iter.key() << " : " << printQvariant(iter.value(), tabs + 1).remove(QRegularExpression(QStringLiteral("^[ ]*")));;
            }
        }
        for (int i  = 0; i < l.size(); ++i)
            stream << printQvariant(l.at(i), tabs + 1);
        stream << sTabs << "}\n";
    } else {
        stream << sTabs;
        QGeoShape workigGeometry;
        if ( v.canConvert<QGeoShape>()) {
            workigGeometry = v.value<QGeoShape>();
            if (workigGeometry.type() == QGeoShape::CircleType) {
                QGeoCircle circle = v.value<QGeoCircle>();
                stream << circle<< "\n";
            } else if (workigGeometry.type() == QGeoShape::PathType) {
                QGeoPath path = v.value<QGeoPath>();
                stream << path<< "\n";
            } else if (workigGeometry.type() == QGeoShape::PolygonType) {
                QGeoPolygon polygon = v.value<QGeoPolygon>();
                stream << polygon<< "\n";
            }
        } else {
            if (v.isNull())
                stream << "null\n";
            else
                stream << v.toString() << "\n";
        }
    }
    return res;
}

/*!
  This method accepts the QVariantList \a geoData, structured as described in
  \l {Importing GeoJSON}, and returns a string containing the same data in a
  readable form.
*/
QString toString(const QVariantList &geoData) {
    if (geoData.isEmpty())
        return QString();
    return printQvariant(geoData.first(), 0);
}

}  // namespace QGeoJson

QT_END_NAMESPACE
