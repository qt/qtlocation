/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QDOUBLEVECTOR2D_P_H
#define QDOUBLEVECTOR2D_P_H

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

#include <QVector2D>

#include <QtCore/qmetatype.h>

QT_BEGIN_NAMESPACE

class QDoubleVector3D;

class QDoubleVector2D
{
public:
    QDoubleVector2D();
    explicit QDoubleVector2D(const QVector2D &vector);
    QDoubleVector2D(double xpos, double ypos);
    explicit QDoubleVector2D(const QDoubleVector3D &vector);

    operator QVector2D() const;

    bool isNull() const;

    double x() const;
    double y() const;

    void setX(double x);
    void setY(double y);

    double length() const;
    double lengthSquared() const;

    QDoubleVector2D normalized() const;
    void normalize();

    QDoubleVector2D &operator+=(const QDoubleVector2D &vector);
    QDoubleVector2D &operator-=(const QDoubleVector2D &vector);
    QDoubleVector2D &operator*=(double factor);
    QDoubleVector2D &operator*=(const QDoubleVector2D &vector);
    QDoubleVector2D &operator/=(double divisor);

    static double dotProduct(const QDoubleVector2D &v1, const QDoubleVector2D &v2);

    friend inline bool operator==(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend inline bool operator!=(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend inline const QDoubleVector2D operator+(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend inline const QDoubleVector2D operator-(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend inline const QDoubleVector2D operator*(double factor, const QDoubleVector2D &vector);
    friend inline const QDoubleVector2D operator*(const QDoubleVector2D &vector, double factor);
    friend inline const QDoubleVector2D operator*(const QDoubleVector2D &v1, const QDoubleVector2D &v2);
    friend inline const QDoubleVector2D operator-(const QDoubleVector2D &vector);
    friend inline const QDoubleVector2D operator/(const QDoubleVector2D &vector, double divisor);

    friend inline bool qFuzzyCompare(const QDoubleVector2D &v1, const QDoubleVector2D &v2);

    QDoubleVector3D toVector3D() const;

private:
    double xp, yp;

    friend class QDoubleVector3D;
};

Q_DECLARE_TYPEINFO(QDoubleVector2D, Q_MOVABLE_TYPE);

inline QDoubleVector2D::QDoubleVector2D() : xp(0.0), yp(0.0) {}

inline QDoubleVector2D::QDoubleVector2D(const QVector2D &vector) : xp(vector.x()), yp(vector.y()) {}

inline QDoubleVector2D::QDoubleVector2D(double xpos, double ypos) : xp(xpos), yp(ypos) {}

inline QDoubleVector2D::operator QVector2D() const
{
    return QVector2D(xp, yp);
}

inline bool QDoubleVector2D::isNull() const
{
    return qIsNull(xp) && qIsNull(yp);
}

inline double QDoubleVector2D::x() const { return double(xp); }
inline double QDoubleVector2D::y() const { return double(yp); }

inline void QDoubleVector2D::setX(double aX) { xp = aX; }
inline void QDoubleVector2D::setY(double aY) { yp = aY; }

inline QDoubleVector2D &QDoubleVector2D::operator+=(const QDoubleVector2D &vector)
{
    xp += vector.xp;
    yp += vector.yp;
    return *this;
}

inline QDoubleVector2D &QDoubleVector2D::operator-=(const QDoubleVector2D &vector)
{
    xp -= vector.xp;
    yp -= vector.yp;
    return *this;
}

inline QDoubleVector2D &QDoubleVector2D::operator*=(double factor)
{
    xp *= factor;
    yp *= factor;
    return *this;
}

inline QDoubleVector2D &QDoubleVector2D::operator*=(const QDoubleVector2D &vector)
{
    xp *= vector.xp;
    yp *= vector.yp;
    return *this;
}

inline QDoubleVector2D &QDoubleVector2D::operator/=(double divisor)
{
    xp /= divisor;
    yp /= divisor;
    return *this;
}

inline bool operator==(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return v1.xp == v2.xp && v1.yp == v2.yp;
}

inline bool operator!=(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return v1.xp != v2.xp || v1.yp != v2.yp;
}

inline const QDoubleVector2D operator+(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return QDoubleVector2D(v1.xp + v2.xp, v1.yp + v2.yp);
}

inline const QDoubleVector2D operator-(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return QDoubleVector2D(v1.xp - v2.xp, v1.yp - v2.yp);
}

inline const QDoubleVector2D operator*(double factor, const QDoubleVector2D &vector)
{
    return QDoubleVector2D(vector.xp * factor, vector.yp * factor);
}

inline const QDoubleVector2D operator*(const QDoubleVector2D &vector, double factor)
{
    return QDoubleVector2D(vector.xp * factor, vector.yp * factor);
}

inline const QDoubleVector2D operator*(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return QDoubleVector2D(v1.xp * v2.xp, v1.yp * v2.yp);
}

inline const QDoubleVector2D operator-(const QDoubleVector2D &vector)
{
    return QDoubleVector2D(-vector.xp, -vector.yp);
}

inline const QDoubleVector2D operator/(const QDoubleVector2D &vector, double divisor)
{
    return QDoubleVector2D(vector.xp / divisor, vector.yp / divisor);
}

inline bool qFuzzyCompare(const QDoubleVector2D &v1, const QDoubleVector2D &v2)
{
    return qFuzzyCompare(v1.xp, v2.xp) && qFuzzyCompare(v1.yp, v2.yp);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QDoubleVector2D &vector);
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &, const QDoubleVector2D &);
QDataStream &operator>>(QDataStream &, QDoubleVector2D &);
#endif

QT_END_NAMESPACE

#endif
