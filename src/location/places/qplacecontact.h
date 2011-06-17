/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QPLACECONTACT_H
#define QPLACECONTACT_H

#include <QSharedDataPointer>
#include <QString>
#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

class QPlaceContactPrivate;

class Q_LOCATION_EXPORT QPlaceContact
{
public:
    enum ContactType {
        Phone,
        Email,
        URL,
        Fax,
        IM,
        Undefined
    };

    QPlaceContact();
    QPlaceContact(const QPlaceContact &other);

    virtual ~QPlaceContact();

    QPlaceContact &operator=(const QPlaceContact &other);

    bool operator==(const QPlaceContact &other) const;
    bool operator!=(const QPlaceContact &other) const {
        return !(other == *this);
    }

    QString description() const;
    void setDescription(const QString& data);
    ContactType type() const;
    void setType(const ContactType &data);
    QString value() const;
    void setValue(const QString& data);

private:
    QSharedDataPointer<QPlaceContactPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACECONTACT_H
