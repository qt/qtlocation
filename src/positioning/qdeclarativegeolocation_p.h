/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGEOLOCATION_P_H
#define QDECLARATIVEGEOLOCATION_P_H

#include <QtCore/QObject>
#include <QtPositioning/QGeoLocation>
#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/private/qdeclarativegeoaddress_p.h>

QT_BEGIN_NAMESPACE

class Q_POSITIONING_EXPORT QDeclarativeGeoLocation : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QGeoLocation location READ location WRITE setLocation)
    Q_PROPERTY(QDeclarativeGeoAddress *address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(QGeoCoordinate coordinate READ coordinate WRITE setCoordinate NOTIFY coordinateChanged)
    Q_PROPERTY(QGeoRectangle boundingBox READ boundingBox WRITE setBoundingBox NOTIFY boundingBoxChanged)

public:
    explicit QDeclarativeGeoLocation(QObject *parent = 0);
    explicit QDeclarativeGeoLocation(const QGeoLocation &src, QObject *parent = 0);
    ~QDeclarativeGeoLocation();

    QGeoLocation location() const;
    void setLocation(const QGeoLocation &src);

    QDeclarativeGeoAddress *address() const;
    void setAddress(QDeclarativeGeoAddress *address);
    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate coordinate);

    QGeoRectangle boundingBox() const;
    void setBoundingBox(const QGeoRectangle &boundingBox);

Q_SIGNALS:
    void addressChanged();
    void coordinateChanged();
    void boundingBoxChanged();

private:
    QDeclarativeGeoAddress *m_address;
    QGeoRectangle m_boundingBox;
    QGeoCoordinate m_coordinate;
};

QT_END_NAMESPACE

#endif // QDECLARATIVELOCATION_P_H
