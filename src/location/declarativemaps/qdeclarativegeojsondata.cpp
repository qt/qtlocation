// Copyright (C) 2019 Julian Sherollari <jdotsh@gmail.com>
// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qdeclarativegeojsondata_p.h"

#include <QtLocation/private/qgeojson_p.h>

QT_BEGIN_NAMESPACE

namespace extractor
{
    static bool hasProperties(QQuickItem *item)
    {
        QVariant props = item->property("props");
        return !props.isNull();
    }

    static QVariant toVariant(QObject *mapItem)
    {
        if (QDeclarativeGeoMapItemView *miv = qobject_cast<QDeclarativeGeoMapItemView *>(mapItem)) {
            return toVariant(miv);
        } else if (QDeclarativePolylineMapItem *polyline = qobject_cast<QDeclarativePolylineMapItem *>(mapItem)) {
            return toVariant(polyline);
        } else if (QDeclarativePolygonMapItem *polygon = qobject_cast<QDeclarativePolygonMapItem *>(mapItem)) {
            return toVariant(polygon);
        } else if (QDeclarativeCircleMapItem *circle = qobject_cast<QDeclarativeCircleMapItem *>(mapItem)) {
            return toVariant(circle); // If GeoJSON Point type is visualized in other ways, handle those types here instead.
        } else if (QDeclarativeRectangleMapItem *rectangle = qobject_cast<QDeclarativeRectangleMapItem *>(mapItem)) {
            return toVariant(rectangle); // For the self-drawn rectangles. Will be exported as Polygons
        }
        return QVariant();
    }

    static QVariantMap toVariant(QDeclarativePolygonMapItem *mapPolygon)
    {
        QVariantMap ls;
        ls[QStringLiteral("type")] = QStringLiteral("Polygon");
        ls[QStringLiteral("data")] = QVariant::fromValue(mapPolygon->geoShape());
        if (hasProperties(mapPolygon))
            ls[QStringLiteral("properties")] = mapPolygon->property("props").toMap();
        return ls;
    }

    static QVariantMap toVariant(QDeclarativePolylineMapItem *mapPolyline)
    {
        QVariantMap ls;
        ls[QStringLiteral("type")] = QStringLiteral("LineString");
        ls[QStringLiteral("data")] = QVariant::fromValue(mapPolyline->geoShape());
        if (hasProperties(mapPolyline))
            ls[QStringLiteral("properties")] = mapPolyline->property("props").toMap();
        return ls;
    }

    static QVariantMap toVariant(QDeclarativeCircleMapItem *mapCircle)
    {
        QVariantMap pt;
        pt[QStringLiteral("type")] = QStringLiteral("Point");
        pt[QStringLiteral("data")] = QVariant::fromValue(mapCircle->geoShape());
        QVariantMap propMap = mapCircle->property("props").toMap();
        propMap[QStringLiteral("radius")] = mapCircle->radius();
        pt[QStringLiteral("properties")] = propMap;
        return pt;
    }

    static QVariantMap toVariant(QDeclarativeRectangleMapItem *mapRectangle)
    {
        QVariantMap pt;
        pt[QStringLiteral("type")] = QStringLiteral("Polygon");
        QGeoRectangle rectanlge = mapRectangle->geoShape();
        QGeoPolygon poly;
        poly.addCoordinate(rectanlge.topLeft());
        poly.addCoordinate(rectanlge.topRight());
        poly.addCoordinate(rectanlge.bottomRight());
        poly.addCoordinate(rectanlge.bottomLeft());
        pt[QStringLiteral("data")] = QVariant::fromValue(poly);
        if (hasProperties(mapRectangle))
            pt[QStringLiteral("properties")] = mapRectangle->property("props").toMap();
        return pt;
    }
};


/*!
    \qmltype GeoJsonData
    \instantiates QDeclarativeGeoJsonData
    \inqmlmodule QtLocation
    \ingroup qml-QtLocation5-maps
    \since QtLocation 6.7

    \brief A model to represent, load and save GeoJSON documents.


    The GeoJsonData element reads and writes GeoJson documents
    (see the \l {https://en.wikipedia.org/wiki/GeoJSON}
    {Wikipedia page}, \l {https://tools.ietf.org/html/rfc7946} {RFC}) from
    the \l {sourceUrl}.
    The respective data can be accessed as \l {QVariant} via the \l {model} property.
    The \l {QVariant} representation can be used with delegates to visualize
    the data or to make small modifications, e.g. adding new items with the
    \l {addItem} function.

    New data can be stored with the \l {save} and \l {saveAs} functions.

    \sa QGeoJson
*/


QDeclarativeGeoJsonData::QDeclarativeGeoJsonData(QObject *parent)
    : QObject(parent)
{

}

QDeclarativeGeoJsonData::~QDeclarativeGeoJsonData()
{

}

/*!
    \qmlproperty QVariant QtLocation::GeoJsonData::model

    A \l QVariant representation of the GeoJSON document that
    can be used to display the contents with delegates.

    \since 6.7
*/
QVariant QDeclarativeGeoJsonData::model() const
{
    return m_content;
}

void QDeclarativeGeoJsonData::setModel(const QVariant &model)
{
    m_content = model;
    emit modelChanged();
}


/*!
    \qmlproperty QUrl QtLocation::GeoJsonData::sourceUrl

    The URL from which the GeoJSON document is read. Setting this
    property will change the \l model to represent the respective document.

    \since 6.7
*/


QUrl QDeclarativeGeoJsonData::sourceUrl() const
{
    return m_url;
}

/*!
    \qmlmethod void QtLocation::GeoJsonData::clear()

    Delete all items in the \l model of the GeoJsonData.
*/
void QDeclarativeGeoJsonData::clear()
{
    m_content = QVariantList();
    emit modelChanged();
}

/*!
    \qmlmethod bool QtLocation::GeoJsonData::addItem(Item item)

    Add the \a item to the \l model of the GeoJsonData.

    Returns \c true if the file was read successfully, \c false otherwise.
*/
void QDeclarativeGeoJsonData::addItem(QQuickItem *item)
{
    QVariant entry;
    if (auto *polyline = qobject_cast<QDeclarativePolylineMapItem *>(item))
        entry = extractor::toVariant(polyline);
    else if (auto *polygon = qobject_cast<QDeclarativePolygonMapItem *>(item))
        entry = extractor::toVariant(polygon);
    else if (auto *circle = qobject_cast<QDeclarativeCircleMapItem *>(item))
        entry = extractor::toVariant(circle);
    else if (auto *rectangle = qobject_cast<QDeclarativeRectangleMapItem *>(item))
        entry = extractor::toVariant(rectangle);
    else
        return;

    QVariantList geoJson = m_content.toList();
    if (!geoJson.isEmpty()){
        QVariantList geoData = (geoJson[0].toMap()[QStringLiteral("type")] == QStringLiteral("FeatureCollection")) ? geoJson[0].toMap()[QStringLiteral("data")].toList() : geoJson;
        geoData.append(entry);
        geoJson[0] = QVariantMap{{QStringLiteral("type"), QStringLiteral("FeatureCollection")}, {QStringLiteral("data"), geoData}};
    }
    else {
        geoJson.append(entry);
    }
    m_content = geoJson;
    emit modelChanged();
}

/*!
    \qmlmethod bool QtLocation::GeoJsonData::open()

    Reload the content of the file at \l sourceUrl.

    Returns \c true if the file was read successfully, \c false otherwise.
*/
bool QDeclarativeGeoJsonData::open()
{
    return openUrl(m_url);
}

/*!
    \qmlmethod bool QtLocation::GeoJsonData::openUrl(Url url)

    Open the GeoJson file at \a url and load its content. The property
    \l sourceUrl will be set to \a url if the file is read successfully.

    Returns \c true if the file was read successfully, \c false otherwise.
*/
bool QDeclarativeGeoJsonData::openUrl(const QUrl &url)
{
    // Reading GeoJSON file
    QFile loadFile(url.toLocalFile());
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Error while opening the file: " << url;
        qWarning() << loadFile.errorString();
        return false;
    }

    // Load the GeoJSON file using Qt's API
    QJsonParseError err;
    QJsonDocument loadDoc(QJsonDocument::fromJson(loadFile.readAll(), &err));
    if (err.error) {
         qWarning() << "Parsing while importing the JSON document:\n" << err.errorString();
         return false;
    }

    // Import geographic data to a QVariantList
    m_content = QGeoJson::importGeoJson(loadDoc);
    if (m_url != url) {
        m_url = url;
        emit sourceUrlChanged();
    }
    emit modelChanged();
    return true;
}

/*!
    \qmlmethod bool QtLocation::GeoJsonData::saveAs(Url url)

    The current \l model of the GeoJsonData object is saved under \a url.
    The \l sourceUrl property will be set to \a url.

    Returns \c true if the file was saved successfully, \c false otherwise.
*/
bool QDeclarativeGeoJsonData::saveAs(const QUrl &url)
{
    if (m_url != url){
        m_url = url;
        emit sourceUrlChanged();
    }
    return save();
}

/*!
    \qmlmethod bool QtLocation::GeoJsonData::save()

    The current \l model of the GeoJsonData object is saved under
    \l sourceUrl.

    Returns \c true if the file was saved successfully, \c false otherwise.
*/
bool QDeclarativeGeoJsonData::save()
{
    return dumpGeoJSON(m_content.toList(), m_url);
}

/*!
    \qmlmethod void QtLocation::GeoJsonData::setModelToMapContents(MapView mapItemView)

    All mapItems of \a mapItemView are added to the \l model of the GeoJsonData
    object.
    Previously stored mapItems are deleted from the \l model.

    Returns \c true if mapItemView is set successfully, \c false otherwise.

    \sa addItem
*/
void QDeclarativeGeoJsonData::setModelToMapContents(QDeclarativeGeoMap *map)
{
    m_content = toVariant(map);
    emit modelChanged();
}

/*! \internal
    A helper function to convert the children of a map to a \l QVariantList that
    represents the items and that can be exported to a Json file.

    Returns a \l QVariantList containing all data of the mapItems.
*/
QVariantList QDeclarativeGeoJsonData::toVariant(QDeclarativeGeoMap *map)
{
    QVariantList res;
    const QList<QObject*> mapChildren = map->children();
    for (QObject *child : mapChildren) {
        QVariant mapItemAsVariant = extractor::toVariant(child);
        if (mapItemAsVariant.isValid())
            res.append(mapItemAsVariant);
    }
    return res;
}

/*! \internal
    A helper function to dump the VariantList \a geoJson into the file at \a url.

    Returns \c true if the file was saved successfully, \c false otherwise.
*/
bool QDeclarativeGeoJsonData::dumpGeoJSON(const QVariantList &geoJson, const QUrl &url)
{
    QJsonDocument json = QGeoJson::exportGeoJson(geoJson);
    QFile jsonFile(url.toLocalFile());
    if (!jsonFile.open(QIODevice::WriteOnly))
        return false;
    jsonFile.write(json.toJson());
    jsonFile.close();
    return true;
}

/*! \internal
    A helper function to write a human interpretable representation of
    \a geoJson into the file at \a url.

    Returns \c true if the file was saved successfully, \c false otherwise.
*/
bool QDeclarativeGeoJsonData::writeDebug(const QVariantList &geoJson, const QUrl &url)
{
    QString prettyPrint = QGeoJson::toString(geoJson);
    QFile debugFile(url.toLocalFile());
    if (!debugFile.open(QIODevice::WriteOnly))
        return false;
    debugFile.write(prettyPrint.toUtf8());
    debugFile.close();
    return true;
}

/*! \internal
    A helper function to generate a human interpretable representation of
    \a geoJson.

    Returns a string with the human interpretable representation of \a geoJson.
*/
QString QDeclarativeGeoJsonData::toString(const QVariantList &geoJson)
{
    return QGeoJson::toString(geoJson);
}

QT_END_NAMESPACE
