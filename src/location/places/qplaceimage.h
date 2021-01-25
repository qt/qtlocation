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

#ifndef QPLACEIMAGE_H
#define QPLACEIMAGE_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>
#include <QtLocation/QPlaceSupplier>
#include <QtLocation/QPlaceContent>

QT_BEGIN_NAMESPACE

class QPlaceImagePrivate;
class QUrl;

class Q_LOCATION_EXPORT QPlaceImage : public QPlaceContent
{
public:
    QPlaceImage();
#ifdef Q_QDOC
    QPlaceImage(const QPlaceContent &other);
#else
    Q_DECLARE_CONTENT_COPY_CTOR(QPlaceImage)
#endif

    virtual ~QPlaceImage();

    QUrl url() const;
    void setUrl(const QUrl &url);

    QString imageId() const;
    void setImageId(const QString &identifier);

    QString mimeType() const;
    void setMimeType(const QString &data);

private:
    Q_DECLARE_CONTENT_D_FUNC(QPlaceImage)
};

QT_END_NAMESPACE

#endif
