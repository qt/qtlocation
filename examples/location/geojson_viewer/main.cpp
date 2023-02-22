// Copyright (C) 2019 Julian Sherollari <jdotsh@gmail.com>
// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QVariantMap>
#include <QQmlContext>
#include <QtLocation/private/qgeojson_p.h>
#include <QGeoCircle>
#include <QGeoPath>
#include <QGeoPolygon>
#include <QtLocation/private/qdeclarativegeomapitemview_p.h>
#include <QtLocation/private/qdeclarativegeomapquickitem_p.h>
#include <QtLocation/private/qdeclarativecirclemapitem_p.h>
#include <QtLocation/private/qdeclarativepolylinemapitem_p.h>
#include <QtLocation/private/qdeclarativepolygonmapitem_p.h>
#include <QtLocation/private/qdeclarativerectanglemapitem_p.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>
#include <QtCore/qobjectdefs.h>
#ifdef Q_OS_ANDROID
#    include <QtCore/private/qandroidextras_p.h>
#endif

using namespace Qt::StringLiterals;

class extractor
{
public:
    extractor();

    static bool hasProperties(QQuickItem *item)
    {
        QVariant props = item->property("props");
        return !props.isNull();
    }

    static bool isFeatureCollection(QQuickItem *item)
    {
        QVariant geoJsonType = item->property("geojsonType");
        return geoJsonType.toString() == QStringLiteral("FeatureCollection");
    }

    static bool isGeoJsonEntry(QQuickItem *item)
    {
        QVariant geoJsonType = item->property("geojsonType");
        return !geoJsonType.toString().isEmpty();
    }

    static QVariantMap toVariant(QDeclarativePolygonMapItem *mapPolygon)
    {
        QVariantMap ls;
        ls["type"] = "Polygon";
        ls["data"] = QVariant::fromValue(mapPolygon->geoShape());
        if (hasProperties(mapPolygon))
            ls["properties"] = mapPolygon->property("props").toMap();
        return ls;
    }
    static QVariantMap toVariant(QDeclarativePolylineMapItem *mapPolyline)
    {
        QVariantMap ls;
        ls["type"] = "LineString";
        ls["data"] = QVariant::fromValue(mapPolyline->geoShape());
        if (hasProperties(mapPolyline))
            ls["properties"] = mapPolyline->property("props").toMap();
        return ls;
    }
    //! [Extractor Example Circle]
    static QVariantMap toVariant(QDeclarativeCircleMapItem *mapCircle)
    {
        QVariantMap pt;
        pt["type"] = "Point";
        pt["data"] = QVariant::fromValue(mapCircle->geoShape());
        QVariantMap propMap = mapCircle->property("props").toMap();
        propMap["radius"] = mapCircle->radius();
        pt["properties"] = propMap;
        return pt;
    }
    //! [Extractor Example Circle]
    static QVariantMap toVariant(QDeclarativeRectangleMapItem *mapRectangle)
    {
        QVariantMap pt;
        pt["type"] = "Polygon";
        QGeoRectangle rectanlge = mapRectangle->geoShape();
        QGeoPolygon poly;
        poly.addCoordinate(rectanlge.topLeft());
        poly.addCoordinate(rectanlge.topRight());
        poly.addCoordinate(rectanlge.bottomRight());
        poly.addCoordinate(rectanlge.bottomLeft());
        pt["data"] = QVariant::fromValue(poly);
        if (hasProperties(mapRectangle))
            pt["properties"] = mapRectangle->property("props").toMap();
        return pt;
    }

    static QVariantMap toVariant(QDeclarativeGeoMapItemView *mapItemView)
    {
        // bool featureCollecton = isFeatureCollection(mapItemView);

        // If not a feature collection, this must be a geometry collection,
        // or a multilinestring/multipoint/multipolygon.
        // To disambiguate, one could check for heterogeneity.
        // For simplicity, in this example, we expect the property "geojsonType" to be injected in the mapItemView
        // by the delegate, and to be correct.

        QString nodeType = mapItemView->property("geojsonType").toString();
        QVariantMap root;
        if (!nodeType.isEmpty())  // Empty nodeType can happen only for the root MIV
            root["type"] = nodeType;
        if (hasProperties(mapItemView)) // Features are converted to regular types w properties.
            root["properties"] = mapItemView->property("props").toMap();

        QVariantList features;
        const QList<QQuickItem *> &quickChildren = mapItemView->childItems();
        for (auto kid : quickChildren) {
            QVariant entry;
            if (QDeclarativeGeoMapItemView *miv = qobject_cast<QDeclarativeGeoMapItemView *>(kid)) {
                // Handle nested miv
                entry = toVariant(miv);
            } else if (QDeclarativePolylineMapItem *polyline = qobject_cast<QDeclarativePolylineMapItem *>(kid)) {
                entry = toVariant(polyline);
            } else if (QDeclarativePolygonMapItem *polygon = qobject_cast<QDeclarativePolygonMapItem *>(kid)) {
                entry = toVariant(polygon);
            } else if (QDeclarativeCircleMapItem *circle = qobject_cast<QDeclarativeCircleMapItem *>(kid)) {
                entry = toVariant(circle); // If GeoJSON Point type is visualized in other ways, handle those types here instead.
            } else if (QDeclarativeRectangleMapItem *rectangle = qobject_cast<QDeclarativeRectangleMapItem *>(kid)) {
                entry = toVariant(rectangle); // For the self-drawn rectangles. Will be exported as Polygons
            }
            features.append(entry);
        }
        if (nodeType.isEmpty()) {
            if (features.isEmpty())
                return root;
            else if (features.size() == 1)
                return features.first().toMap();
            else
                root["type"] = "FeatureCollection";
        }
        root["data"] = features;
        return root;
    }
};

//! [GeoJsoner]
class GeoJsoner: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model MEMBER m_importedGeoJson NOTIFY modelChanged)
//! [GeoJsoner]

public:
    GeoJsoner(QObject *parent = nullptr) : QObject(parent)
    {

    }

public slots:
    //! [clear]
    Q_INVOKABLE void clear()
    {
        m_importedGeoJson = QVariantList();
        emit modelChanged();
    }
    //! [clear]

    //! [add item]
    Q_INVOKABLE void addItem(QQuickItem *item)
    {
        QVariant entry;
        if (QDeclarativePolylineMapItem *polyline = qobject_cast<QDeclarativePolylineMapItem *>(item)) {
            entry = extractor::toVariant(polyline);
        } else if (QDeclarativePolygonMapItem *polygon = qobject_cast<QDeclarativePolygonMapItem *>(item)) {
            entry = extractor::toVariant(polygon);
        } else if (QDeclarativeCircleMapItem *circle = qobject_cast<QDeclarativeCircleMapItem *>(item)) {
            entry = extractor::toVariant(circle);
        } else if (QDeclarativeRectangleMapItem *rectangle = qobject_cast<QDeclarativeRectangleMapItem *>(item)) {
            entry = extractor::toVariant(rectangle);
        } else {
            return;
        }
        QVariantList geoJson = m_importedGeoJson.toList();
        if (!geoJson.isEmpty()){
            QVariantList geoData = (geoJson[0].toMap()["type"] == "FeatureCollection") ? geoJson[0].toMap()["data"].toList() : geoJson;
            geoData.append(entry);
            geoJson[0] = QVariantMap{{"type", "FeatureCollection"}, {"data", geoData}};
        }
        else {
            geoJson.append(entry);
        }
        m_importedGeoJson = geoJson;
        emit modelChanged();
    }
    //! [add item]

    Q_INVOKABLE bool load(QUrl url)
    {
        // Reading GeoJSON file
        QFile loadFile(url.toLocalFile());
        if (!loadFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Error while opening the file: " << url;
            qWarning() << loadFile.error() <<  loadFile.errorString();
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
        //! [Conversion QVariantList]
        QVariantList modelList = QGeoJson::importGeoJson(loadDoc);
        m_importedGeoJson =  modelList;
        emit modelChanged();
        //! [Conversion QVariantList]
        return true;
    }

    // Used by the MapItemView Extractor to identify a Feature
    //! [Conversion QVariantList From Items]
    Q_INVOKABLE QVariantList toVariant(QDeclarativeGeoMapItemView *mapItemView)
    {
        QVariantList res;
        QDeclarativeGeoMapItemView *root = mapItemView;
        QVariantMap miv = extractor::toVariant(root);
        if (!miv.isEmpty())
            res.append(miv);
        return res;
    }
    //! [Conversion QVariantList From Items]

    //! [Write QVariantList to Json]
    Q_INVOKABLE void dumpGeoJSON(QVariantList geoJson, QUrl url)
    {
        QJsonDocument json = QGeoJson::exportGeoJson(geoJson);
        QFile jsonFile(url.toLocalFile());
        jsonFile.open(QIODevice::WriteOnly);
        jsonFile.write(json.toJson());
        jsonFile.close();
    }
    //! [Write QVariantList to Json]

    Q_INVOKABLE void writeDebug(QVariantList geoJson, QUrl url)
    {
        QString prettyPrint = QGeoJson::toString(geoJson);
        QFile debugFile(url.toLocalFile());
        debugFile.open(QIODevice::WriteOnly);
        debugFile.write(prettyPrint.toUtf8());
        debugFile.close();
    }

    Q_INVOKABLE void print(QDeclarativeGeoMapItemView *view)
    {
        QVariantList list;
        list.append(extractor::toVariant(view));
        QString prettyPrint =
                QGeoJson::toString(list);
        qDebug().noquote() << prettyPrint;
    }

signals:
    void modelChanged();

public:
    QVariant m_importedGeoJson;
};

#include "main.moc"

#ifdef Q_OS_ANDROID
// Request permissions because we're using QStandardPaths::writableLocation()
bool requestStoragePermissions()
{
    const QString permission = "android.permission.WRITE_EXTERNAL_STORAGE"_L1;
    auto checkFuture = QtAndroidPrivate::checkPermission(permission);
    if (checkFuture.result() == QtAndroidPrivate::Denied) {
        auto requestFuture = QtAndroidPrivate::requestPermission(permission);
        if (requestFuture.result() != QtAndroidPrivate::Authorized) {
            qWarning() << "Couldn't get permission: " << permission;
            return false;
        }
    }

    return true;
}
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

#ifdef Q_OS_ANDROID
    if (!requestStoragePermissions())
        return -1;
#endif

    QQmlApplicationEngine engine;
    QUrl absoluteFilePath = argc > 1 ?
                    QUrl(QStringLiteral("file://") + QFileInfo(argv[1]).absoluteFilePath()) :
                    QUrl();
    engine.rootContext()->setContextProperty("dataPath", QUrl(QStringLiteral("file://")
                                                              + qPrintable(QT_STRINGIFY(SRC_PATH))
                                                              + QStringLiteral("/data")));
    //! [QMLEngine]
    qmlRegisterType<GeoJsoner>("Qt.GeoJson", 1, 0, "GeoJsoner");
    //! [QMLEngine]

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;
    if (!absoluteFilePath.isEmpty()) {
        GeoJsoner *geoJsoner = engine.rootObjects().first()->findChild<GeoJsoner*>();
        QMetaObject::invokeMethod(geoJsoner, "load", Qt::QueuedConnection, Q_ARG(QUrl, absoluteFilePath));
    }

    return app.exec();
}
