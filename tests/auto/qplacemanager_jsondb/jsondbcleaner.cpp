#include "jsondbcleaner.h"

#include <jsondb-client.h>

#include <QDebug>
#include <QMap>
#include <QString>
#include <QStringList>

#define UUID "_uuid"
#define TYPE "_type"

#define PLACE_TYPE "place"

JsonDbCleaner::JsonDbCleaner(QObject *parent)
    : QObject(parent)
{
    mDb = new JsonDbClient(this);
    if (!mDb->connected()) {
        qWarning() << "JsonDb not connected";
    } else  {
        connect(mDb, SIGNAL(response(int, const QVariant&)),
                this, SLOT(jsonDbResponse(int, const QVariant&)), Qt::QueuedConnection);
        connect(mDb, SIGNAL(error(int, int, const QString&)),
                this, SLOT(jsonDbError(int, int, const QString&)), Qt::QueuedConnection);
    }
}

void JsonDbCleaner::jsonDbResponse(int id, const QVariant& data)
{
    if (cleanReqId.contains(id)) {
        QVariantMap jsonMap  = data.toMap();
        QList<QVariant> dataList = jsonMap.value("data").toList();
        if (!dataList.isEmpty()) {
            bool placesFound = false;
            foreach (const QVariant &var, dataList) {
                QVariantMap placeJson = var.toMap();
                if (placeJson.value(TYPE).toString() == QLatin1String(PLACE_TYPE)) {
                    placesFound = true;
                    QVariantMap removeMap;
                    removeMap.insert(UUID,placeJson.value(UUID).toString());
                    removeReqId.append(mDb->remove(removeMap));
                }
            }

            if (!placesFound)
                emit dbCleaned();
        }
        cleanReqId.removeAll(id);
    }

    if (removeReqId.contains(id)) {
        removeReqId.removeAll(id);
        if (removeReqId.isEmpty())
            emit dbCleaned();
    }
}

void JsonDbCleaner::jsonDbError(int id, int code, const QString& data)
{
    qDebug() << Q_FUNC_INFO << " id: " << id << " code: " << code << " data: " << data;
}

void JsonDbCleaner::cleanDb()
{
    cleanReqId.append(mDb->query(QString("[*]")));
}


