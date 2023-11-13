// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#ifndef QPLACEUSER_P_H
#define QPLACEUSER_P_H

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

#include <QSharedData>
#include <QString>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class QPlaceUserPrivate : public QSharedData
{
public:
    bool operator==(const QPlaceUserPrivate &other) const;

    QString userId;
    QString name;
};

QT_END_NAMESPACE

#endif
