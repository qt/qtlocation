/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QPLACE_H
#define QPLACE_H

#include <QtCore/QSharedDataPointer>
#include <QtPositioning/QGeoAddress>
#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoCoordinate>
#include <QtPositioning/QGeoLocation>
#include <QtLocation/QPlaceCategory>
#include <QtLocation/QPlaceContent>
#include <QtLocation/QPlaceRatings>
#include <QtLocation/QPlaceReview>
#include <QtLocation/QPlaceAttribute>
#include <QtLocation/QPlaceContactDetail>

QT_BEGIN_NAMESPACE

class QString;
class QPlaceIcon;
class QPlacePrivate;

class Q_LOCATION_EXPORT QPlace
{
public:
    QPlace();
    QPlace(const QPlace &other);
    ~QPlace();

    QPlace &operator=(const QPlace &other);

    bool operator==(const QPlace &other) const;
    bool operator!=(const QPlace &other) const;

    QList<QPlaceCategory> categories() const;
    void setCategory(const QPlaceCategory &category);
    void setCategories(const QList<QPlaceCategory> &categories);
    QGeoLocation location() const;
    void setLocation(const QGeoLocation &location);
    QPlaceRatings ratings() const;
    void setRatings(const QPlaceRatings &ratings);
    QPlaceSupplier supplier() const;
    void setSupplier(const QPlaceSupplier &supplier);

    QString attribution() const;
    void setAttribution(const QString &attribution);

    QPlaceIcon icon() const;
    void setIcon(const QPlaceIcon &icon);

    QPlaceContent::Collection content(QPlaceContent::Type type) const;
    void setContent(QPlaceContent::Type type, const QPlaceContent::Collection &content);
    void insertContent(QPlaceContent::Type type, const QPlaceContent::Collection &content);

    int totalContentCount(QPlaceContent::Type type) const;
    void setTotalContentCount(QPlaceContent::Type type, int total);

    QString name() const;
    void setName(const QString &name);
    QString placeId() const;
    void setPlaceId(const QString &identifier);

    QString primaryPhone() const;
    QString primaryFax() const;
    QString primaryEmail() const;
    QUrl primaryWebsite() const;

    bool detailsFetched() const;
    void setDetailsFetched(bool fetched);

    QStringList extendedAttributeTypes() const;
    QPlaceAttribute extendedAttribute(const QString &attributeType) const;
    void setExtendedAttribute(const QString &attributeType, const QPlaceAttribute &attribute);
    void removeExtendedAttribute(const QString &attributeType);

    QStringList contactTypes() const;
    QList<QPlaceContactDetail> contactDetails(const QString &contactType) const;
    void setContactDetails(const QString &contactType, QList<QPlaceContactDetail> details);
    void appendContactDetail(const QString &contactType, const QPlaceContactDetail &detail);
    void removeContactDetails(const QString &contactType);

    QLocation::Visibility visibility() const;
    void setVisibility(QLocation::Visibility visibility);

    bool isEmpty() const;

protected:
    QPlace(const QSharedDataPointer<QPlacePrivate> &dd);
    QSharedDataPointer<QPlacePrivate> &d();

private:
    QSharedDataPointer<QPlacePrivate> d_ptr;

    inline QPlacePrivate *d_func();
    inline const QPlacePrivate *d_func() const;
    friend class QDeclarativePlace;
};

Q_DECLARE_TYPEINFO(QPlace, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlace)

#endif
