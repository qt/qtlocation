/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
***************************************************************************/

#ifndef QDECLARATIVEGEOPLACE_P_H
#define QDECLARATIVEGEOPLACE_P_H

#include <qgeoplace.h>
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativegeoaddress_p.h"

#include <QVariant>
#include <QtDeclarative/qdeclarative.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoPlace : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeGeoBoundingBox* viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QDeclarativeGeoAddress* address READ address WRITE setAddress NOTIFY addressChanged)

public:
    explicit QDeclarativeGeoPlace(QObject* parent = 0);
    QDeclarativeGeoPlace(const QGeoPlace& place, QObject* parent);
    virtual ~QDeclarativeGeoPlace();
    void setPlace(const QGeoPlace& place);

    QDeclarativeGeoBoundingBox* viewport();
    void setViewport(QDeclarativeGeoBoundingBox* box);

    QDeclarativeCoordinate* coordinate();
    void setCoordinate(QDeclarativeCoordinate* coordinate);

    QDeclarativeGeoAddress* address();
    void setAddress(QDeclarativeGeoAddress* address);

signals:
    void viewportChanged();
    void coordinateChanged();
    void addressChanged();

private:
    QDeclarativeGeoBoundingBox m_declarativeBox;
    QDeclarativeCoordinate m_declarativeCoordinate;
    QDeclarativeGeoAddress m_declarativeAddress;
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGeoPlace))

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoPlace));

#endif // QDECLARATIVEGEOPLACE_P_H
