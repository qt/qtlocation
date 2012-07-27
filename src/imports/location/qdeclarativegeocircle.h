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
***************************************************************************/

#ifndef QDECLARATIVEGEOCIRCLE_H
#define QDECLARATIVEGEOCIRCLE_H

#include "qdeclarativegeoshape.h"
#include "qdeclarativecoordinate_p.h"

#include <QtQml/qqml.h>
#include <QtLocation/QGeoCircle>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoCircle : public QDeclarativeGeoShape
{
    Q_OBJECT

    Q_PROPERTY(QGeoCircle circle READ circle WRITE setCircle)
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)

public:
    explicit QDeclarativeGeoCircle(QObject *parent = 0);
    explicit QDeclarativeGeoCircle(const QGeoCircle &circle, QObject *parent = 0);
    void setCircle(const QGeoCircle &circle);
    QGeoCircle circle() const;
    QGeoShape shape() const;

    Q_INVOKABLE bool contains(const QGeoCoordinate &coordinate);

    QGeoCoordinate center();
    void setCenter(const QGeoCoordinate &coordinate);

    qreal radius() const;
    void setRadius(qreal radius);

Q_SIGNALS:
    void centerChanged();
    void radiusChanged();

private:
    QGeoCircle m_circle;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeGeoCircle)

#endif
