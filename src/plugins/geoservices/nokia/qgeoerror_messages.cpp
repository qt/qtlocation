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

#include "qgeoerror_messages.h"

QT_BEGIN_NAMESPACE

const char NOKIA_PLUGIN_CONTEXT_NAME[] = "QtLocationQML";
const char MISSED_CREDENTIALS[] = QT_TRANSLATE_NOOP("QtLocationQML", "Qt Location requires app_id and token parameters.\nPlease register at https://developer.here.com/ to get your personal application credentials.");
const char SAVING_PLACE_NOT_SUPPORTED[] = QT_TRANSLATE_NOOP("QtLocationQML", "Saving places is not supported.");
const char REMOVING_PLACE_NOT_SUPPORTED[] = QT_TRANSLATE_NOOP("QtLocationQML", "Removing places is not supported.");
const char SAVING_CATEGORY_NOT_SUPPORTED[] = QT_TRANSLATE_NOOP("QtLocationQML", "Saving categories is not supported.");
const char REMOVING_CATEGORY_NOT_SUPPORTED[] = QT_TRANSLATE_NOOP("QtLocationQML", "Removing categories is not supported.");
const char PARSE_ERROR[] = QT_TRANSLATE_NOOP("QtLocationQML", "Error parsing response.");
const char NETWORK_ERROR[] = QT_TRANSLATE_NOOP("QtLocationQML", "Network error.");
const char CANCEL_ERROR[] = QT_TRANSLATE_NOOP("QtLocationQML", "Request was canceled.");
const char RESPONSE_NOT_RECOGNIZABLE[] = QT_TRANSLATE_NOOP("QtLocationQML", "The response from the service was not in a recognizable format.");

QT_END_NAMESPACE
