/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPLACEREVIEW_H
#define QPLACEREVIEW_H

#include <QtLocation/QPlaceContent>

#include <QtCore/QDateTime>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

#if QT_DEPRECATED_SINCE(6, 0)
class Q_LOCATION_EXPORT QPlaceReview : public QPlaceContent
{
public:
    QT_DEPRECATED_VERSION_X_6_0("Use QPlaceContent") QPlaceReview();
    QT_DEPRECATED_VERSION_X_6_0("Use QPlaceContent") QPlaceReview(const QPlaceContent &other);

    QT_DEPRECATED_VERSION_X_6_0("Use value()") QDateTime dateTime() const
    { return value(QPlaceContent::ReviewDateTime).value<QDateTime>(); }
    QT_DEPRECATED_VERSION_X_6_0("Use setValue()") void setDateTime(const QDateTime &dateTime)
    { setValue(QPlaceContent::ReviewDateTime, QVariant::fromValue(dateTime)); }
    QT_DEPRECATED_VERSION_X_6_0("Use value()") QString text() const
    { return value(QPlaceContent::ReviewText).value<QString>(); }
    QT_DEPRECATED_VERSION_X_6_0("Use setValue()") void setText(const QString &text)
    { setValue(QPlaceContent::ReviewText, QVariant::fromValue(text)); }
    QT_DEPRECATED_VERSION_X_6_0("Use value()") QString language() const
    { return value(QPlaceContent::ReviewLanguage).value<QString>(); }
    QT_DEPRECATED_VERSION_X_6_0("Use setValue()") void setLanguage(const QString &language)
    { setValue(QPlaceContent::ReviewLanguage, QVariant::fromValue(language)); }
    QT_DEPRECATED_VERSION_X_6_0("Use value()") qreal rating() const
    { return value(QPlaceContent::ReviewRating).value<qreal>(); }
    QT_DEPRECATED_VERSION_X_6_0("Use setValue()") void setRating(qreal rating)
    { setValue(QPlaceContent::ReviewRating, QVariant::fromValue(rating)); }
    QT_DEPRECATED_VERSION_X_6_0("Use value()") QString reviewId() const
    { return value(QPlaceContent::ReviewId).value<QString>(); }
    QT_DEPRECATED_VERSION_X_6_0("Use setValue()") void setReviewId(const QString &identifier)
    { setValue(QPlaceContent::ReviewId, QVariant::fromValue(identifier)); }
    QT_DEPRECATED_VERSION_X_6_0("Use value()") QString title() const
    { return value(QPlaceContent::ReviewTitle).value<QString>(); }
    QT_DEPRECATED_VERSION_X_6_0("Use setValue()") void setTitle(const QString &title)
    { setValue(QPlaceContent::ReviewTitle, QVariant::fromValue(title)); }
};
#endif

QT_END_NAMESPACE

#endif // QPLACEREVIEW_H
