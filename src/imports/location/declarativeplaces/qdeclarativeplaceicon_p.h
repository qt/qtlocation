/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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

#ifndef QDECLARATIVEPLACEICON_P_H
#define QDECLARATIVEPLACEICON_P_H

#include "qdeclarativegeoserviceprovider_p.h"

#include <qplaceicon.h>
#include <QtQml/qqml.h>
#include <QtQml/QQmlPropertyMap>

#include <QObject>

QT_BEGIN_NAMESPACE

class QQmlPropertyMap;

class QDeclarativePlaceIcon : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPlaceIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(QObject *parameters READ parameters NOTIFY parametersChanged)
    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)

public:
    explicit QDeclarativePlaceIcon(QObject *parent = 0);
    QDeclarativePlaceIcon(const QPlaceIcon &src, QDeclarativeGeoServiceProvider *plugin, QObject *parent = 0);
    ~QDeclarativePlaceIcon();

    QPlaceIcon icon() const;
    void setIcon(const QPlaceIcon &src);

    Q_INVOKABLE QUrl url(const QSize &size = QSize()) const;

    QQmlPropertyMap *parameters() const;

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider *plugin() const;

Q_SIGNALS:
    void pluginChanged();
    void parametersChanged(); //in practice is never emitted since parameters cannot be re-assigned
                              //the declaration is needed to avoid warnings about non-notifyable properties

private Q_SLOTS:
    void pluginReady();

private:
    QPlaceManager *manager() const;
    void initParameters(const QVariantMap &parameterMap);
    QDeclarativeGeoServiceProvider *m_plugin;
    QQmlPropertyMap *m_parameters;
};

QT_END_NAMESPACE

#endif
