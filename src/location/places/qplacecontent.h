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
#ifndef QPLACECONTENT_H
#define QPLACECONTENT_H

#include <QtLocation/qlocationglobal.h>

#include <QtCore/QMap>
#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>

QT_BEGIN_NAMESPACE

#define Q_DECLARE_CONTENT_D_FUNC(Class) \
    inline Class##Private *d_func(); \
    inline const Class##Private *d_func() const;\
    friend class Class##Private;

#define Q_DECLARE_CONTENT_COPY_CTOR(Class) \
    Class(const QPlaceContent &other);

class QPlaceUser;
class QPlaceSupplier;
class QPlaceContentPrivate;
class Q_LOCATION_EXPORT QPlaceContent
{
public:
    typedef QMap<int, QPlaceContent> Collection;

    enum Type {
        NoType = 0,
        ImageType,
        ReviewType,
        EditorialType,
        CustomType = 0x0100
    };

    QPlaceContent();
    QPlaceContent(const QPlaceContent &other);
    virtual ~QPlaceContent();

    QPlaceContent &operator=(const QPlaceContent &other);

    bool operator==(const QPlaceContent &other) const;
    bool operator!=(const QPlaceContent &other) const;

    QPlaceContent::Type type() const;

    QPlaceSupplier supplier() const;
    void setSupplier(const QPlaceSupplier &supplier);

    QPlaceUser user() const;
    void setUser(const QPlaceUser &user);

    QString attribution() const;
    void setAttribution(const QString &attribution);

protected:
    explicit QPlaceContent(QPlaceContentPrivate *d);
    QSharedDataPointer<QPlaceContentPrivate> d_ptr;

private:
    inline QPlaceContentPrivate *d_func();
    inline const QPlaceContentPrivate *d_func() const;

    friend class QPlaceContentPrivate;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QPlaceContent)
Q_DECLARE_METATYPE(QPlaceContent::Type)

#endif

