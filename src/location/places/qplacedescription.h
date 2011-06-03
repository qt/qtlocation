#ifndef QPLACEDESCRIPTION_H
#define QPLACEDESCRIPTION_H

#include <QSharedDataPointer>
#include <QStringList>
#include "qmobilityglobal.h"
#include "qplacesupplier.h"

QTM_BEGIN_NAMESPACE

class QPlaceDescriptionPrivate;

class Q_LOCATION_EXPORT QPlaceDescription
{
public:
    QPlaceDescription();
    QPlaceDescription(const QPlaceDescription &other);

    virtual ~QPlaceDescription();

    QPlaceDescription &operator=(const QPlaceDescription &other);

    bool operator==(const QPlaceDescription &other) const;
    bool operator!=(const QPlaceDescription &other) const {
        return !(other == *this);
    }

    QString content() const;
    void setContent(const QString &data);
    QString contentTitle() const;
    void setContentTitle(const QString &data);
    QString contentType() const;
    void setContentType(const QString &data);
    QPlaceSupplier supplier() const;
    void setSupplier(const QPlaceSupplier &data);
    QString sourceURL() const;
    void setSourceURL(const QString &data);
    QString language() const;
    void setLanguage(const QString &data);

private:
    QSharedDataPointer<QPlaceDescriptionPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACEDESCRIPTION_H
