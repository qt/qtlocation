#ifndef QPLACEBUSINESSFEATURE_H
#define QPLACEBUSINESSFEATURE_H

#include <QSharedDataPointer>
#include <QString>
#include "qplaceglobal.h"

namespace QT_PLACES_NAMESPACE {

class QPlaceBusinessFeaturePrivate;

class Q_PLACES_EXPORT QPlaceBusinessFeature
{
public:
    QPlaceBusinessFeature();
    QPlaceBusinessFeature(const QPlaceBusinessFeature &other);

    virtual ~QPlaceBusinessFeature();

    QPlaceBusinessFeature &operator=(const QPlaceBusinessFeature &other);

    bool operator==(const QPlaceBusinessFeature &other) const;
    bool operator!=(const QPlaceBusinessFeature &other) const {
        return !(other == *this);
    }

    QString key() const;
    void setKey(const QString &data);
    QString label() const;
    void setLabel(const QString &data);
    QString value() const;
    void setValue(const QString &data);

private:
    QSharedDataPointer<QPlaceBusinessFeaturePrivate> d;
};

} // QT_PLACES_NAMESPACE

#endif // QPLACEBUSINESSFEATURE_H
