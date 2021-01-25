/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/
#ifndef QCLIPPERUTILS_P_H
#define QCLIPPERUTILS_P_H

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

/*
 * This file is intended to be include only in source files of
 * QtPositioning/QtLocation. It is in QtPositioning to enable manipulation
 * of geo polygons
 */

#include <QtPositioning/private/qpositioningglobal_p.h>
#include <QtCore/QtGlobal>
#include <QtCore/QList>
#include <cmath>
/* clip2tri triangulator includes */
#include <clip2tri.h>
#include <QtPositioning/private/qdoublevector2d_p.h>

QT_BEGIN_NAMESPACE

class Q_POSITIONING_PRIVATE_EXPORT QClipperUtils
{
public:
    static double clipperScaleFactor();

    static QDoubleVector2D  toVector2D(const IntPoint &p);
    static IntPoint         toIntPoint(const QDoubleVector2D &p);

    static QList<QDoubleVector2D> pathToQList(const Path &path);
    static QList<QList<QDoubleVector2D> > pathsToQList(const Paths &paths);

    static Path  qListToPath(const QList<QDoubleVector2D> &list);
    static Paths qListToPaths(const QList<QList<QDoubleVector2D> > &lists);
};

QT_END_NAMESPACE

#endif // QCLIPPERUTILS_P_H
