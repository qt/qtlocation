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

#ifndef QPLACEDESCRIPTION_H
#define QPLACEDESCRIPTION_H

#include <QSharedDataPointer>
#include <QStringList>
#include "qmobilityglobal.h"
#include "qplacesupplier.h"

QTM_BEGIN_NAMESPACE

class QPlaceDescriptionPrivate;

class Q_LOCATION_EXPORT QPlaceDescription
{
public:
    QPlaceDescription();
    QPlaceDescription(const QPlaceDescription &other);

    virtual ~QPlaceDescription();

    QPlaceDescription &operator=(const QPlaceDescription &other);

    bool operator==(const QPlaceDescription &other) const;
    bool operator!=(const QPlaceDescription &other) const {
        return !(other == *this);
    }

    QString content() const;
    void setContent(const QString &data);
    QString contentTitle() const;
    void setContentTitle(const QString &data);
    QString contentType() const;
    void setContentType(const QString &data);
    QPlaceSupplier supplier() const;
    void setSupplier(const QPlaceSupplier &data);
    QString sourceUrl() const;
    void setSourceUrl(const QString &data);
    QString language() const;
    void setLanguage(const QString &data);

private:
    QSharedDataPointer<QPlaceDescriptionPrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACEDESCRIPTION_H
