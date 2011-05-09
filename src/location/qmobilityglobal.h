/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QMOBILITYGLOBAL_H
#define QMOBILITYGLOBAL_H

#include <QtCore/qglobal.h>

// The namespace is hardcoded as moc has issues resolving
// macros which would be a prerequisite for a dynmamic namespace
//#define QTM_NAMESPACE QtMobility
//#define QTM_NAMESPACE

#ifdef QTM_NAMESPACE
# define QTM_PREPEND_NAMESPACE(name) ::QTM_NAMESPACE::name
# define QTM_BEGIN_NAMESPACE namespace QTM_NAMESPACE {
# define QTM_END_NAMESPACE }
# define QTM_USE_NAMESPACE using namespace QTM_NAMESPACE;
#else
# define QTM_PREPEND_NAMESPACE(name) ::name
# define QTM_BEGIN_NAMESPACE
# define QTM_END_NAMESPACE
# define QTM_USE_NAMESPACE
#endif

//in case Qt is in namespace
QT_USE_NAMESPACE

#endif // QMOBILITYGLOBAL_H

