#ifndef QPLACESEARCHRESULT_H
#define QPLACESEARCHRESULT_H

#include "qmobilityglobal.h"

#include <QSharedDataPointer>
#include <QVariant>
#include <QString>
#include "qplace.h"

QTM_BEGIN_NAMESPACE

class QPlaceSearchResultPrivate;

class Q_LOCATION_EXPORT QPlaceSearchResult
{
public:
    QPlaceSearchResult();
    QPlaceSearchResult(const QPlaceSearchResult &other);

    virtual ~QPlaceSearchResult();

    QPlaceSearchResult &operator=(const QPlaceSearchResult &other);

    bool operator==(const QPlaceSearchResult &other) const;
    bool operator!=(const QPlaceSearchResult &other) const {
        return !(other == *this);
    }

    enum LocationMatchType {
        PointAddress,
        Interpolated,
        Undefined
    };

    enum SearchResultType {
        Place,
        DidYouMeanSuggestion,
        Unassigned
    };

    qreal relevance() const;
    void setRelevance(const qreal &relevance);

    qreal distance() const;
    void setDistance(const qreal &distance);

    qreal heading() const;
    void setHeading(const qreal &heading);

    LocationMatchType matchType() const;
    void setMatchType(const LocationMatchType &matchType);

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);

    SearchResultType type() const;
    void setType(const SearchResultType &type);

    QPlace place() const;
    void setPlace(const QPlace &place);

    QString didYouMeanSuggestion() const;
    void setDidYouMeanSuggestion(const QString &didYouMeanSuggestion);

private:
    QSharedDataPointer<QPlaceSearchResultPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACESEARCHRESULT_H
