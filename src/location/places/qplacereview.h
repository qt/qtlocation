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

#ifndef QPLACEREVIEW_H
#define QPLACEREVIEW_H

#include <QtLocation/QPlaceContent>

QT_BEGIN_NAMESPACE

class QDateTime;
class QPlaceReviewPrivate;

class Q_LOCATION_EXPORT QPlaceReview : public QPlaceContent
{
public:
    QPlaceReview();
#ifdef Q_QDOC
    QPlaceReview(const QPlaceContent &other);
#else
    Q_DECLARE_CONTENT_COPY_CTOR(QPlaceReview)
#endif
    virtual ~QPlaceReview();

    QDateTime dateTime() const;
    void setDateTime(const QDateTime &dt);
    QString text() const;
    void setText(const QString &text);
    QString language() const;
    void setLanguage(const QString &data);

    qreal rating() const;
    void setRating(qreal data);
    QString reviewId() const;
    void setReviewId(const QString &identifier);
    QString title() const;
    void setTitle(const QString &data);

private:
    Q_DECLARE_CONTENT_D_FUNC(QPlaceReview)
};

QT_END_NAMESPACE

#endif // QPLACEREVIEW_H
