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

#ifndef QDECLARATIVEPLACEATTRIBUTE_P_H
#define QDECLARATIVEPLACEATTRIBUTE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtLocation/private/qlocationglobal_p.h>
#include <QObject>
#include <QtQml/qqml.h>
#include <QString>

#include <QtLocation/qplaceattribute.h>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePlaceAttribute : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPlaceAttribute attribute READ attribute WRITE setAttribute)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit QDeclarativePlaceAttribute(QObject *parent = 0);
    explicit QDeclarativePlaceAttribute(const QPlaceAttribute &src, QObject *parent = 0);
    ~QDeclarativePlaceAttribute();

    QPlaceAttribute attribute() const;
    void setAttribute(const QPlaceAttribute &place);

    QString text() const;
    void setText(const QString &text);


    QString label() const;
    void setLabel(const QString &label);

Q_SIGNALS:
    void labelChanged();
    void textChanged();

private:
    QPlaceAttribute m_attribute;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePlaceAttribute)

#endif
