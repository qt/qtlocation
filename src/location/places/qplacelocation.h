#ifndef QPLACELOCATION_H
#define QPLACELOCATION_H

#include <QSharedDataPointer>
#include <QString>
#include <QVariant>
#include <QList>
#include "qmobilityglobal.h"
#include "qplacealternativevalue.h"
#include "qplaceaddress.h"
#include "qplacegeocoordinate.h"
#include "qplacegeoboundingbox.h"

QTM_BEGIN_NAMESPACE

class QPlaceLocationPrivate;

class Q_LOCATION_EXPORT QPlaceLocation
{
public:
    QPlaceLocation();
    QPlaceLocation(const QPlaceLocation &other);

    virtual ~QPlaceLocation();

    QPlaceLocation &operator=(const QPlaceLocation &other);

    bool operator==(const QPlaceLocation &other) const;
    bool operator!=(const QPlaceLocation &other) const {
        return !(other == *this);
    }

    QVariantHash additionalData() const;
    void setAdditionalData(const QVariantHash &data);
    QPlaceAddress address() const;
    void setAddress(const QPlaceAddress &address);
    QList<QPlaceAlternativeValue> alternativeLabels() const;
    void setAlternativeLabels(const QList<QPlaceAlternativeValue> &labels);
    QPlaceGeoCoordinate displayPosition() const;
    void setDisplayPosition(const QPlaceGeoCoordinate &position);
    QList<QPlaceGeoCoordinate> navigationPositions() const;
    void setNavigationPositions(const QList<QPlaceGeoCoordinate> &positions);
    QString label() const;
    void setLabel(const QString &label);
    QString locationId() const;
    void setLocationId(const QString &locationId);
    int locationScore() const;
    void setLocationScore(const int &score);
    QPlaceGeoBoundingBox mapView() const;
    void setMapView(const QPlaceGeoBoundingBox &coordinate);

private:
    QSharedDataPointer<QPlaceLocationPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACELOCATION_H
