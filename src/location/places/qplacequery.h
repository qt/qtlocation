#ifndef QPLACEQUERY_H
#define QPLACEQUERY_H

#include <QSharedDataPointer>
#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

class QPlaceQueryPrivate;

class Q_LOCATION_EXPORT QPlaceQuery
{
public:
    QPlaceQuery();
    QPlaceQuery(const QPlaceQuery &other);

    virtual ~QPlaceQuery();

    QPlaceQuery &operator=(const QPlaceQuery &other);

    bool operator==(const QPlaceQuery &other) const;
    bool operator!=(const QPlaceQuery &other) const {
        return !(other == *this);
    }

    int offset() const;
    void setOffset(int offset);
    int limit() const;
    void setLimit(int limit);

    virtual void clear();

private:
    QSharedDataPointer<QPlaceQueryPrivate> d;
};

QTM_END_NAMESPACE

#endif
