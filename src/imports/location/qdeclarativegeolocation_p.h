/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOLOCATION_P_H
#define QDECLARATIVEGEOLOCATION_P_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <QtLocation/QGeoLocation>
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativegeoaddress_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGeoLocation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QGeoLocation location READ location WRITE setLocation)
    Q_PROPERTY(QDeclarativeGeoAddress* address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QDeclarativeGeoBoundingBox *boundingBox READ boundingBox WRITE setBoundingBox NOTIFY boundingBoxChanged)

public:
    explicit QDeclarativeGeoLocation(QObject* parent = 0);
    explicit QDeclarativeGeoLocation(const QGeoLocation &src, QObject* parent = 0);
    ~QDeclarativeGeoLocation();

    QGeoLocation location() ;
    void setLocation(const QGeoLocation &src);

    QDeclarativeGeoAddress *address();
    void setAddress(QDeclarativeGeoAddress *address);
    QDeclarativeCoordinate *coordinate();
    void setCoordinate(QDeclarativeCoordinate *coordinate);

    QDeclarativeGeoBoundingBox *boundingBox();
    void setBoundingBox(QDeclarativeGeoBoundingBox *boundingBox);

Q_SIGNALS:
    void addressChanged();
    void coordinateChanged();
    void boundingBoxChanged();

private:
    QDeclarativeGeoAddress *m_address;
    QDeclarativeCoordinate *m_coordinate;
    QDeclarativeGeoBoundingBox *m_boundingBox;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoLocation));

#endif // QDECLARATIVELOCATION_P_H
