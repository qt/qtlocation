#ifndef QPLACESEARCHRESULT_P_H
#define QPLACESEARCHRESULT_P_H

#include "qplacesearchresult.h"

#include <QSharedData>

QTM_BEGIN_NAMESPACE

class QPlaceSearchResultPrivate : public QSharedData
{
public:
    QPlaceSearchResultPrivate();
    QPlaceSearchResultPrivate(const QPlaceSearchResultPrivate &other);

    ~QPlaceSearchResultPrivate();

    bool operator==(const QPlaceSearchResultPrivate &other) const;

    qreal relevance;
    qreal distance;
    qreal heading;
    QPlaceSearchResult::LocationMatchType matchType;
    QVariantHash additionalData;
    QPlaceSearchResult::SearchResultType type;
    QPlace place;
    QString dymString;
};

QTM_END_NAMESPACE

#endif // QPLACESEARCHRESULT_P_H
