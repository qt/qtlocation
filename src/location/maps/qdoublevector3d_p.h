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

#ifndef QDOUBLEVECTOR3D_P_H
#define QDOUBLEVECTOR3D_P_H

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

#include <QVector3D>

#include <QtCore/qmetatype.h>

QT_BEGIN_NAMESPACE

class QDoubleVector2D;

class QDoubleVector3D
{
public:
    QDoubleVector3D();
    QDoubleVector3D(double xpos, double ypos, double zpos);
    explicit QDoubleVector3D(const QVector3D &vector);
    QDoubleVector3D(const QDoubleVector2D &vector);
    QDoubleVector3D(const QDoubleVector2D &vector, double zpos);

    operator QVector3D() const;

    bool isNull() const;

    double x() const;
    double y() const;
    double z() const;

    void setX(double x);
    void setY(double y);
    void setZ(double z);

    double get(int i) const;
    void set(int i, double value);

    double length() const;
    double lengthSquared() const;

    QDoubleVector3D normalized() const;
    void normalize();

    QDoubleVector3D &operator+=(const QDoubleVector3D &vector);
    QDoubleVector3D &operator-=(const QDoubleVector3D &vector);
    QDoubleVector3D &operator*=(double factor);
    QDoubleVector3D &operator*=(const QDoubleVector3D &vector);
    QDoubleVector3D &operator/=(double divisor);

    static double dotProduct(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    static QDoubleVector3D crossProduct(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    static QDoubleVector3D normal(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    static QDoubleVector3D normal
        (const QDoubleVector3D &v1, const QDoubleVector3D &v2, const QDoubleVector3D &v3);

    double distanceToPlane(const QDoubleVector3D &plane, const QDoubleVector3D &normal) const;
    double distanceToPlane(const QDoubleVector3D &plane1, const QDoubleVector3D &plane2, const QDoubleVector3D &plane3) const;
    double distanceToLine(const QDoubleVector3D &point, const QDoubleVector3D &direction) const;

    friend inline bool operator==(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend inline bool operator!=(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend inline const QDoubleVector3D operator+(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend inline const QDoubleVector3D operator-(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend inline const QDoubleVector3D operator*(double factor, const QDoubleVector3D &vector);
    friend inline const QDoubleVector3D operator*(const QDoubleVector3D &vector, double factor);
    friend const QDoubleVector3D operator*(const QDoubleVector3D &v1, const QDoubleVector3D &v2);
    friend inline const QDoubleVector3D operator-(const QDoubleVector3D &vector);
    friend inline const QDoubleVector3D operator/(const QDoubleVector3D &vector, double divisor);

    friend inline bool qFuzzyCompare(const QDoubleVector3D &v1, const QDoubleVector3D &v2);

    QDoubleVector2D toVector2D() const;

private:
    double xp, yp, zp;

    friend class QDoubleVector2D;
};

Q_DECLARE_TYPEINFO(QDoubleVector3D, Q_MOVABLE_TYPE);

inline QDoubleVector3D::QDoubleVector3D() : xp(0.0), yp(0.0), zp(0.0) {}

inline QDoubleVector3D::QDoubleVector3D(const QVector3D &vector) : xp(vector.x()), yp(vector.y()), zp(vector.z()) {}

inline QDoubleVector3D::QDoubleVector3D(double xpos, double ypos, double zpos) : xp(xpos), yp(ypos), zp(zpos) {}

inline QDoubleVector3D::operator QVector3D() const
{
    return QVector3D(xp, yp, zp);
}

inline bool QDoubleVector3D::isNull() const
{
    return qIsNull(xp) && qIsNull(yp) && qIsNull(zp);
}

inline double QDoubleVector3D::x() const { return xp; }
inline double QDoubleVector3D::y() const { return yp; }
inline double QDoubleVector3D::z() const { return zp; }

inline void QDoubleVector3D::setX(double aX) { xp = aX; }
inline void QDoubleVector3D::setY(double aY) { yp = aY; }
inline void QDoubleVector3D::setZ(double aZ) { zp = aZ; }

inline double QDoubleVector3D::get(int i) const
{
    switch (i) {
    case 0:
        return xp;
    case 1:
        return yp;
    case 2:
        return zp;
    default:
        return 0.0;
    }
}

inline void QDoubleVector3D::set(int i, double value)
{
    switch (i) {
    case 0:
        xp = value;
        break;
    case 1:
        yp = value;
        break;
    case 2:
        zp = value;
        break;
    default:
        break;
    }
}

inline QDoubleVector3D &QDoubleVector3D::operator+=(const QDoubleVector3D &vector)
{
    xp += vector.xp;
    yp += vector.yp;
    zp += vector.zp;
    return *this;
}

inline QDoubleVector3D &QDoubleVector3D::operator-=(const QDoubleVector3D &vector)
{
    xp -= vector.xp;
    yp -= vector.yp;
    zp -= vector.zp;
    return *this;
}

inline QDoubleVector3D &QDoubleVector3D::operator*=(double factor)
{
    xp *= factor;
    yp *= factor;
    zp *= factor;
    return *this;
}

inline QDoubleVector3D &QDoubleVector3D::operator*=(const QDoubleVector3D &vector)
{
    xp *= vector.xp;
    yp *= vector.yp;
    zp *= vector.zp;
    return *this;
}

inline QDoubleVector3D &QDoubleVector3D::operator/=(double divisor)
{
    xp /= divisor;
    yp /= divisor;
    zp /= divisor;
    return *this;
}

inline bool operator==(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return v1.xp == v2.xp && v1.yp == v2.yp && v1.zp == v2.zp;
}

inline bool operator!=(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return v1.xp != v2.xp || v1.yp != v2.yp || v1.zp != v2.zp;
}

inline const QDoubleVector3D operator+(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return QDoubleVector3D(v1.xp + v2.xp, v1.yp + v2.yp, v1.zp + v2.zp);
}

inline const QDoubleVector3D operator-(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return QDoubleVector3D(v1.xp - v2.xp, v1.yp - v2.yp, v1.zp - v2.zp);
}

inline const QDoubleVector3D operator*(double factor, const QDoubleVector3D &vector)
{
    return QDoubleVector3D(vector.xp * factor, vector.yp * factor, vector.zp * factor);
}

inline const QDoubleVector3D operator*(const QDoubleVector3D &vector, double factor)
{
    return QDoubleVector3D(vector.xp * factor, vector.yp * factor, vector.zp * factor);
}

inline const QDoubleVector3D operator*(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return QDoubleVector3D(v1.xp * v2.xp, v1.yp * v2.yp, v1.zp * v2.zp);
}

inline const QDoubleVector3D operator-(const QDoubleVector3D &vector)
{
    return QDoubleVector3D(-vector.xp, -vector.yp, -vector.zp);
}

inline const QDoubleVector3D operator/(const QDoubleVector3D &vector, double divisor)
{
    return QDoubleVector3D(vector.xp / divisor, vector.yp / divisor, vector.zp / divisor);
}

inline bool qFuzzyCompare(const QDoubleVector3D &v1, const QDoubleVector3D &v2)
{
    return qFuzzyCompare(v1.xp, v2.xp) &&
           qFuzzyCompare(v1.yp, v2.yp) &&
           qFuzzyCompare(v1.zp, v2.zp);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QDoubleVector3D &vector);
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &, const QDoubleVector3D &);
QDataStream &operator>>(QDataStream &, QDoubleVector3D &);
#endif

QT_END_NAMESPACE

#endif
