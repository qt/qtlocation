/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOPLACE_P_H
#define QGEOPLACE_P_H

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

#include <QRectF>
#include <QSharedData>

#include "qgeoplace.h"
#include "qgeoaddress.h"
#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

class QGeoPlacePrivate : public QSharedData
{
public:
    enum PlaceType {
        GeoPlaceType,
        LandmarkType
    };

    QGeoPlacePrivate();
    QGeoPlacePrivate(const QGeoPlacePrivate &other);
    virtual ~QGeoPlacePrivate();

    QGeoPlacePrivate& operator= (const QGeoPlacePrivate &other);

    virtual bool operator== (const QGeoPlacePrivate &other) const;

    virtual QGeoPlacePrivate* clone() const { return new QGeoPlacePrivate(*this); }
    PlaceType type;
    QGeoBoundingBox viewport;
    QGeoCoordinate coordinate;
    QGeoAddress address;
};

QTM_END_NAMESPACE

#if defined(Q_CC_MWERKS)
// This results in multiple symbol definition errors on all other compilers
// but not having a definition here results in an attempt to use the unspecialized
// clone (which fails because of the pure virtuals above)
template<> QTM_PREPEND_NAMESPACE(QGeoPlacePrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QGeoPlacePrivate)>::clone()
{
    return d->clone();
}
#else
template<> QTM_PREPEND_NAMESPACE(QGeoPlacePrivate) *QSharedDataPointer<QTM_PREPEND_NAMESPACE(QGeoPlacePrivate)>::clone();
#endif

#endif

