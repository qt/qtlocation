/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
****************************************************************************/

#ifndef QPLACESEARCHREQUEST_P_H
#define QPLACESEARCHREQUEST_P_H

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

#include "qplacesearchrequest.h"
#include "qgeocoordinate.h"
#include "qgeoshape.h"

#include <QtCore/QSharedData>
#include <QtCore/QList>
#include <QtLocation/private/qlocationglobal_p.h>
#include <QtCore/QVariant>
#include <QtLocation/QGeoRoute>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QPlaceSearchRequestPrivate : public QSharedData
{
public:
    QPlaceSearchRequestPrivate();
    QPlaceSearchRequestPrivate(const QPlaceSearchRequestPrivate &other);
    ~QPlaceSearchRequestPrivate();

    QPlaceSearchRequestPrivate &operator=(const QPlaceSearchRequestPrivate &other);
    bool operator==(const QPlaceSearchRequestPrivate &other) const;

    void clear();
    static const QPlaceSearchRequestPrivate *get(const QPlaceSearchRequest &request);
    static QPlaceSearchRequestPrivate *get(QPlaceSearchRequest &request);

    QString searchTerm;
    QList<QPlaceCategory> categories;
    QGeoShape searchArea;
    QString recommendationId;
    QLocation::VisibilityScope visibilityScope;
    QPlaceSearchRequest::RelevanceHint relevanceHint;
    QGeoRoute routeSearchArea;
    int limit;
    QVariant searchContext;
    bool related = false;
    int page = 0;
};

QT_END_NAMESPACE

#endif // QPLACESEARCHREQUEST_P_H
