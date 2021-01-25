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

#ifndef QDECLARATIVEPLACEUSER_P_H
#define QDECLARATIVEPLACEUSER_P_H

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
#include <QtCore/QObject>
#include <QtQml/qqml.h>
#include <QtLocation/QPlaceUser>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QDeclarativePlaceUser : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPlaceUser user READ user WRITE setUser)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit QDeclarativePlaceUser(QObject *parent = 0);
    explicit QDeclarativePlaceUser(const QPlaceUser &src, QObject *parent = 0);
    ~QDeclarativePlaceUser();

    QPlaceUser user() const;
    void setUser(const QPlaceUser &src);

    QString userId() const;
    void setUserId(const QString &id);

    QString name() const;
    void setName(const QString &name);

Q_SIGNALS:
    void userIdChanged();
    void nameChanged();

private:
    QPlaceUser m_user;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePlaceUser)

#endif
