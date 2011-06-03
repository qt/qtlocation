#ifndef QPLACE_H
#define QPLACE_H

#include <QSharedDataPointer>
#include <QString>
#include <QVariant>
#include <QList>
#include "qmobilityglobal.h"
#include "qplacealternativevalue.h"
#include "qplacebusinessinformation.h"
#include "qplacecategory.h"
#include "qplacecontact.h"
#include "qplacedescription.h"
#include "qplacelocation.h"
#include "qplacerating.h"
#include "qplacepaginationlist.h"
#include "qplacemediaobject.h"
#include "qplacereview.h"

QTM_BEGIN_NAMESPACE

class QPlacePrivate;

class Q_LOCATION_EXPORT QPlace
{
public:
    QPlace();
    QPlace(const QPlace &other);

    virtual ~QPlace();

    QPlace &operator=(const QPlace &other);

    bool operator==(const QPlace &other) const;
    bool operator!=(const QPlace &other) const {
        return !(other == *this);
    }

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QList<QPlaceAlternativeValue> alternativeNames() const;
    void setAlternativeNames(const QList<QPlaceAlternativeValue> &alternativeNames);
    double placeScore() const;
    void setPlaceScore(const double &data);
    QPlaceBusinessInformation businessInformation() const;
    void setBusinessInformation(const QPlaceBusinessInformation &business);
    QList<QPlaceCategory> categories() const;
    void setCategories(const QList<QPlaceCategory> &categories);
    QList<QPlaceContact> contacts() const;
    void setContacts(const QList<QPlaceContact> &contacts);
    QList<QPlaceDescription> descriptions() const;
    void setDescriptions(const QList<QPlaceDescription> &descriptions);
    QPlaceLocation location() const;
    void setLocation(const QPlaceLocation &location);
    QList<QPlaceLocation> alternativeLocations() const;
    void setAlternativeLocations(const QList<QPlaceLocation> &locations);
    QPlaceRating rating() const;
    void setRating(const QPlaceRating &rating);
    QList<QPlaceSupplier> suppliers() const;
    void setSuppliers(const QList<QPlaceSupplier> &data);
    QStringList feeds() const;
    void setFeeds(const QStringList &feeds);
    QPlacePaginationList<QPlaceMediaObject> media() const;
    void setMedia(const QPlacePaginationList<QPlaceMediaObject> &media);
    uint mediaCount() const;
    void setMediaCount(const uint &data);
    QString name() const;
    void setName(const QString &name);
    QString placeId() const;
    void setPlaceId(const QString &placeId);
    QPlacePaginationList<QPlaceReview> reviews() const;
    void setReviews(const QPlacePaginationList<QPlaceReview> &reviews);
    uint reviewCount() const;
    void setReviewCount(const uint &data);
    QString shortDescription() const;
    void setShortDescription(const QString &description);
    QStringList tags() const;
    void setTags(const QStringList &tags);

private:
    QSharedDataPointer<QPlacePrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACE_H
