#ifndef QPLACEMEDIAOBJECT_P_H
#define QPLACEMEDIAOBJECT_P_H

#include <QSharedData>

#include "qplacemediaobject.h"

QTM_BEGIN_NAMESPACE

class QPlaceMediaObjectPrivate : public QSharedData
{
public:
    QPlaceMediaObjectPrivate();
    QPlaceMediaObjectPrivate(const QPlaceMediaObjectPrivate &other);

    ~QPlaceMediaObjectPrivate();

    bool operator==(const QPlaceMediaObjectPrivate &other) const;

    QString url;
    QString thumbnailURL;
    QString id;
    QString metaInfo;
    QString mimeType;
    QPlaceSupplier supplier;
};

QTM_END_NAMESPACE

#endif // QPLACEMEDIAOBJECT_P_H
