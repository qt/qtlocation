#ifndef QPLACEALTERNATIVEVALUE_P_H
#define QPLACEALTERNATIVEVALUE_P_H

#include <QString>
#include <QSharedData>

#include "qplacealternativevalue.h"

QTM_BEGIN_NAMESPACE

class QPlaceAlternativeValuePrivate : public QSharedData
{
public:
    QPlaceAlternativeValuePrivate();
    QPlaceAlternativeValuePrivate(const QPlaceAlternativeValuePrivate &other);

    ~QPlaceAlternativeValuePrivate();

    bool operator==(const QPlaceAlternativeValuePrivate &other) const;

    QString key;
    QString language;
    QPlaceAlternativeValue::TextSemantics semantics;
    QPlaceAlternativeValue::NameType type;
    QString value;
};

QTM_END_NAMESPACE

#endif // QPLACEALTERNATIVEVALUE_P_H
