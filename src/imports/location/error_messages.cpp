/***************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
#include "error_messages.h"

QT_BEGIN_NAMESPACE

const char CONTEXT_NAME[] = "QtLocationQML";

const char PLUGIN_DOESNOT_SUPPORT_ROUTING[] = QT_TRANSLATE_NOOP("QtLocationQML", "Error: Plugin does not support routing.\nError message: %1");
const char PLUGIN_DOESNOT_SUPPORT_MAPPING[] = QT_TRANSLATE_NOOP("QtLocationQML", "Error: Plugin does not support mapping.\nError message: %1");
const char PLUGIN_DOESNOT_SUPPORT_GEOCODING[] = QT_TRANSLATE_NOOP("QtLocationQML", "Error: Plugin does not support (reverse) geocoding.\nError message: %1");

const char NOT_SUPPORTED_BY[] = QT_TRANSLATE_NOOP("QtLocationQML", "%1 not supported by %2 plugin.");
const char PLUGIN_PROPERTY_NOT_SET[] = QT_TRANSLATE_NOOP("QtLocationQML", "Plugin property is not set.");
const char PLUGIN_ERROR[] = QT_TRANSLATE_NOOP("QtLocationQML", "Plugin Error (%1): %2");
const char PLUGIN_PROVIDER_ERROR[] = QT_TRANSLATE_NOOP("QtLocationQML", "Plugin Error (%1): Could not instantiate provider %1");
const char PLUGIN_NOT_VALID[] = QT_TRANSLATE_NOOP("QtLocationQML", "Plugin is not valid");
const char PLUGIN_NOT_ASSIGNED_TO_PLACE[] = QT_TRANSLATE_NOOP("QtLocationQML", "Plugin is not assigned to place.");
const char PLUGIN_NOT_ASSIGNED_TO_PLACE_ICON[] = QT_TRANSLATE_NOOP("QtLocationQML", "Plugin is not assigned to place icon.");
const char CATEGORIES_NOT_INITIALIZED[] = QT_TRANSLATE_NOOP("QtLocationQML", "Unable to initialize categories");
const char NOT_INSTANTIABLE_BY_DEVELOPER[] = QT_TRANSLATE_NOOP("QtLocationQML", "%1 is not intended instantiable by developer.");

const char UNSUPPORTED_BOUND_TYPE[] = QT_TRANSLATE_NOOP("QtLocationQML", "Unsupported bound type (Box and Circle supported)");
const char UNSUPPORTED_QUERY_TYPE[] = QT_TRANSLATE_NOOP("QtLocationQML", "Unsupported query type for geocode model (coordinate, string and Address supported).");
const char PLUGIN_SET_ONCE[] = QT_TRANSLATE_NOOP("QtLocationQML", "Plugin is a write-once property, and cannot be set again.");

const char PINCH_ROTATE_GESTURE_ACTIVATED[] = QT_TRANSLATE_NOOP("QtLocationQML", "Pinchrotation gesture activated. Note that it is experimental feature.");
const char PINCH_TILT_GESTURE_ACTIVATED[] = QT_TRANSLATE_NOOP("QtLocationQML", "Pinchtilt gesture activated. Note that it is experimental feature.");

const char INDEX_INVALID[] = QT_TRANSLATE_NOOP("QtLocationQML", "Error, invalid index for get(): %1");
const char INDEX_OUT_OF_RANGE[] = QT_TRANSLATE_NOOP("QtLocationQML", "Error, too big or small index in get(): %1");

const char MODEL_INDEX_INVALID[] = QT_TRANSLATE_NOOP("QtLocationQML", "Error in indexing route model's data (invalid index).");
const char MODEL_INDEX_OUT_OF_RANGE[] = QT_TRANSLATE_NOOP("QtLocationQML", "Fatal error in indexing route model's data (index overflow).");

const char GEOCODE_PLUGIN_NOT_SET[] = QT_TRANSLATE_NOOP("QtLocationQML", "Cannot geocode, plugin not set.");
const char GEOCODE_MGR_NOT_SET[] = QT_TRANSLATE_NOOP("QtLocationQML", "Cannot geocode, geocode manager not set.");
const char GEOCODE_QUERY_NOT_SET[] = QT_TRANSLATE_NOOP("QtLocationQML", "Cannot geocode, valid query not set.");

const char ROUTE_PLUGIN_NOT_SET[] = QT_TRANSLATE_NOOP("QtLocationQML", "Cannot route, plugin not set.");
const char ROUTE_MGR_NOT_SET[] = QT_TRANSLATE_NOOP("QtLocationQML", "Cannot route, route manager not set.");
const char ROUTE_QUERY_NOT_SET[] = QT_TRANSLATE_NOOP("QtLocationQML", "Cannot route, valid query not set.");
const char ROUTE_WAYPOINTS_NOT_SET[] = QT_TRANSLATE_NOOP("QtLocationQML", "Not enough waypoints for routing.");

const char CANNOT_REMOVE_AREA[] = QT_TRANSLATE_NOOP("QtLocationQML", "Cannot remove nonexistent area.");
const char CANNOT_ADD_INVALID_WAYPOINT[] = QT_TRANSLATE_NOOP("QtLocationQML", "Not adding invalid waypoint.");
const char CANNOT_REMOVE_WAYPOINT[] = QT_TRANSLATE_NOOP("QtLocationQML", "Cannot remove nonexistent waypoint.");
const char COORD_NOT_BELONG_TO[] = QT_TRANSLATE_NOOP("QtLocationQML", "Coordinate does not belong to %1");
const char MISSED_NMEA_FILE[] = QT_TRANSLATE_NOOP("QtLocationQML", "Nmea file not found.");
const char UNABLE_TO_MAKE_REQUEST[]= QT_TRANSLATE_NOOP("QtLocationQML", "Unable to create request");

QT_END_NAMESPACE
