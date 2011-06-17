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

#ifndef QPLACEALTERNATIVEVALUE_H
#define QPLACEALTERNATIVEVALUE_H

#include <QSharedDataPointer>
#include <QString>
#include "qmobilityglobal.h"

QTM_BEGIN_NAMESPACE

class QPlaceAlternativeValuePrivate;

class Q_LOCATION_EXPORT QPlaceAlternativeValue
{
public:
    enum TextSemantics {
        Synonim,
        Exonym,
        Unclassified
    };

    enum NameType {
        BaseName,
        ShortBaseName,
        Abbrevation,
        Unknown
    };

    QPlaceAlternativeValue();
    QPlaceAlternativeValue(const QPlaceAlternativeValue &other);

    virtual ~QPlaceAlternativeValue();

    QPlaceAlternativeValue &operator=(const QPlaceAlternativeValue &other);

    bool operator==(const QPlaceAlternativeValue &other) const;
    bool operator!=(const QPlaceAlternativeValue &other) const {
        return !(other == *this);
    }
    QString key() const;
    void setKey(const QString &data);
    QString language() const;
    void setLanguage(const QString &data);
    TextSemantics semantics() const;
    void setSemantics(const TextSemantics &data);
    NameType type() const;
    void setType(const NameType &data);
    QString value() const;
    void setValue(const QString &data);

private:
    QSharedDataPointer<QPlaceAlternativeValuePrivate> d;
};

QTM_END_NAMESPACE

#endif // QPLACEALTERNATIVEVALUE_H
