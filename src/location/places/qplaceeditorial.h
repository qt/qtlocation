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

#ifndef QPLACEEDITORIAL_H
#define QPLACEEDITORIAL_H

#include <QtLocation/QPlaceContent>

QT_BEGIN_NAMESPACE

class QPlaceEditorialPrivate;

class Q_LOCATION_EXPORT QPlaceEditorial : public QPlaceContent
{
public:
    QPlaceEditorial();
#ifdef Q_QDOC
    QPlaceEditorial(const QPlaceContent &other);
#else
    Q_DECLARE_CONTENT_COPY_CTOR(QPlaceEditorial)
#endif

    virtual ~QPlaceEditorial();

    QString text() const;
    void setText(const QString &text);
    QString title() const;
    void setTitle(const QString &data);
    QString language() const;
    void setLanguage(const QString &data);

private:
    Q_DECLARE_CONTENT_D_FUNC(QPlaceEditorial)
};

QT_END_NAMESPACE

#endif // QPLACEEDITORIAL_H
