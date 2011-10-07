/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVECATEGORY_P_H
#define QDECLARATIVECATEGORY_P_H

#include <QtDeclarative/qdeclarative.h>
#include <QObject>

#include <qplacecategory.h>

QT_BEGIN_NAMESPACE

class QDeclarativeCategory : public QObject
{
    Q_OBJECT

    Q_ENUMS(Visibility)

    Q_PROPERTY(QString categoryId READ categoryId WRITE setCategoryId NOTIFY categoryIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Visibility visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)

public:
    explicit QDeclarativeCategory(QObject* parent = 0);
    explicit QDeclarativeCategory(const QPlaceCategory &category, QObject* parent = 0);
    ~QDeclarativeCategory();

    enum Visibility {
        UnspecifiedVisibility = QtLocation::UnspecifiedVisibility,
        DeviceVisibility = QtLocation::DeviceVisibility,
        PrivateVisibility = QtLocation::PrivateVisibility,
        PublicVisibility = QtLocation::PublicVisibility
    };

    QPlaceCategory category() const;
    void setCategory(const QPlaceCategory &category);

    QString categoryId() const;
    void setCategoryId(const QString &catID);
    QString name() const;
    void setName(const QString &name);

    Visibility visibility() const;
    void setVisibility(Visibility visibility);

signals:
    void categoryIdChanged();
    void nameChanged();
    void visibilityChanged();

private:
    QPlaceCategory m_category;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeCategory));

#endif // QDECLARATIVECATEGORY_P_H
