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

#ifndef QGEOBOUNDINGAREA_H
#define QGEOBOUNDINGAREA_H

#include <QtCore/QSharedDataPointer>
#include <QtLocation/QGeoCoordinate>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGeoBoundingAreaPrivate;

class Q_LOCATION_EXPORT QGeoBoundingArea
{
public:
    QGeoBoundingArea();
    QGeoBoundingArea(const QGeoBoundingArea &other);
    ~QGeoBoundingArea();

    enum AreaType {
        UnknownType,
        BoxType,
        CircleType
    };

    AreaType type() const;

    bool isValid() const;
    bool isEmpty() const;
    bool contains(const QGeoCoordinate &coordinate) const;

    bool operator==(const QGeoBoundingArea &other) const;
    bool operator!=(const QGeoBoundingArea &other) const;

    QGeoBoundingArea &operator=(const QGeoBoundingArea &other);

protected:
    QGeoBoundingArea(QGeoBoundingAreaPrivate *d);

    QSharedDataPointer<QGeoBoundingAreaPrivate> d_ptr;

private:
    inline QGeoBoundingAreaPrivate *d_func();
    inline const QGeoBoundingAreaPrivate *d_func() const;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoBoundingArea)

QT_END_HEADER

#endif // QGEOBOUNDINGAREA_H
