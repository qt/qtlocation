/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include "qdeclarativepinchgenerator_p.h"
#include "qdeclarativelocationtestmodel_p.h"
#include <QtDeclarative/qdeclarative.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

class QLocationDeclarativeTestModule: public QDeclarativeExtensionPlugin
{
    Q_OBJECT

public:
    virtual void registerTypes(const char* uri) {
        qDebug() << "registerTypes in new test plugin: " << uri;
        if (QLatin1String(uri) == QLatin1String("QtLocation.test")) {
            // This version numbering is not correct. It is just something to get going
            // until the proper versioning scheme of QML plugins in Qt5 is agreed upon.
            qmlRegisterType<QDeclarativePinchGenerator>(uri, 5, 0, "PinchGenerator");
            qmlRegisterType<QDeclarativeLocationTestModel>(uri, 5, 0, "TestModel");
        } else {
            qDebug() << "Unsupported URI given to load location test QML plugin: " << QLatin1String(uri);
        }
    }
};

#include "locationtest.moc"

QT_END_NAMESPACE

Q_EXPORT_PLUGIN2(declarative_location_test, QT_PREPEND_NAMESPACE(QLocationDeclarativeTestModule))
