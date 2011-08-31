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
    if (!mDb->isConnected()) {
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
    if (id == cleanReqId)
        emit dbCleaned();
}

void JsonDbCleaner::jsonDbError(int id, int code, const QString& data)
{
    qDebug() << Q_FUNC_INFO << " id: " << id << " code: " << code << " data: " << data;
}

void JsonDbCleaner::cleanDb()
{
    cleanReqId = mDb->remove(QString::fromLatin1("[?%1=\"%2\"]").arg(TYPE).arg(PLACE_TYPE));
}


