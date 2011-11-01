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

#ifndef QPLACE_P_H
#define QPLACE_P_H

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

#include <QSharedData>
#include <QUrl>

#include "qplace.h"
#include "qgeoaddress.h"
#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "qplacesupplier.h"
#include <QtLocation/QPlaceIcon>

QT_BEGIN_NAMESPACE

class QPlacePrivate : public QSharedData
{
public:
    QPlacePrivate();
    QPlacePrivate(const QPlacePrivate &other);
    ~QPlacePrivate();

    QPlacePrivate &operator= (const QPlacePrivate &other);

    bool operator==(const QPlacePrivate &other) const;

    QList<QPlaceCategory> categories;
    QGeoLocation location;
    QPlaceRating rating;
    QPlaceSupplier supplier;
    QString name;
    QString placeId;
    QString attribution;

    QMap<QPlaceContent::Type, QPlaceContent::Collection> contentCollections;
    QMap<QPlaceContent::Type, int> contentCounts;

    QMap<QString, QPlaceAttribute> extendedAttributes;
    QMap<QString, QList<QPlaceContactDetail> > contacts;

    QtLocation::Visibility visibility;
    QPlaceIcon icon;
    bool detailsFetched;
};

QT_END_NAMESPACE

#endif

