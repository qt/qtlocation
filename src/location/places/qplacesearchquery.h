#ifndef QPLACESEARCHQUERY_H
#define QPLACESEARCHQUERY_H

#include <QSharedDataPointer>
#include "qmobilityglobal.h"
#include "qplacecategory.h"
#include "qplacequery.h"
#include "qplacegeoboundingbox.h"

#include <QString>

QTM_BEGIN_NAMESPACE

class QPlaceSearchQueryPrivate;

class Q_LOCATION_EXPORT QPlaceSearchQuery : public QPlaceQuery
{
public:
    enum RelevanceHint {
        DistanceHint,
        RatingHint,
        AlphabetHint
    };

    QPlaceSearchQuery();
    QPlaceSearchQuery(const QPlaceSearchQuery &other);

    virtual ~QPlaceSearchQuery();

    QPlaceSearchQuery &operator=(const QPlaceSearchQuery &other);

    bool operator==(const QPlaceSearchQuery &other) const;
    bool operator!=(const QPlaceSearchQuery &other) const {
        return !(other == *this);
    }

    QString searchTerm() const;
    void setSearchTerm(const QString &term);

    QList<QPlaceCategory> categories();
    void setCategory(const QPlaceCategory &category);

    QPlaceGeoCoordinate searchCenter() const;
    void setSearchCenter(const QPlaceGeoCoordinate &center);

    QPlaceGeoBoundingBox boundingBox() const;
    void setBoundingBox(const QPlaceGeoBoundingBox &boundingBox);

    uint didYouMeanSuggestionNumber() const;
    void setDidYouMeanSuggestionNumber(const uint &number);

    void clear();

private:
    QSharedDataPointer<QPlaceSearchQueryPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACESEARCHQUERY_H
