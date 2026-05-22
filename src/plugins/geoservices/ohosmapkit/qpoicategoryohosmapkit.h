// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QPOICATEGORYOHOSMAPKIT_H
#define QPOICATEGORYOHOSMAPKIT_H

#include <QtCore/qstringlist.h>
#include <QtCore/qstring.h>

namespace QPoiCategoryOhosMapKit {

QString tryGetParentCategoryIdOrEmpty(const QString &categoryId);
QStringList tryGetChildCategoryIdsOrEmpty(const QString &categoryId);

}

#endif // QPOICATEGORYOHOSMAPKIT_H
