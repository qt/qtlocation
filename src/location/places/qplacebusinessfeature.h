#ifndef QPLACEBUSINESSFEATURE_H
#define QPLACEBUSINESSFEATURE_H

#include <QSharedDataPointer>
#include <QString>
#include "qmobilityglobal.h"

QT_BEGIN_HEADER

class QPlaceBusinessFeaturePrivate;

class Q_LOCATION_EXPORT QPlaceBusinessFeature
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

QTM_END_NAMESPACE

#endif // QPLACEBUSINESSFEATURE_H
