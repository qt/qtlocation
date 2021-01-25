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

#ifndef ERROR_MESSAGES_H
#define ERROR_MESSAGES_H

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


#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

extern const char CONTEXT_NAME[];
extern const char PLUGIN_PROPERTY_NOT_SET[];
extern const char PLUGIN_ERROR[];
extern const char PLUGIN_PROVIDER_ERROR[];
extern const char PLUGIN_NOT_VALID[];
extern const char CATEGORIES_NOT_INITIALIZED[];
extern const char UNABLE_TO_MAKE_REQUEST[];
extern const char INDEX_OUT_OF_RANGE[];

QT_END_NAMESPACE

#endif // ERROR_MESSAGES_H
