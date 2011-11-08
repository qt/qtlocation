/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEPLACEICON_P_H
#define QDECLARATIVEPLACEICON_P_H

#include "qdeclarativegeoserviceprovider_p.h"

#include <qplaceicon.h>
#include <QtDeclarative/qdeclarative.h>

#include <QObject>

QT_BEGIN_NAMESPACE

class QDeclarativePlaceIcon : public QObject
{
    Q_OBJECT
    Q_ENUMS(IconFlag)
    Q_FLAGS(IconFlags)

    Q_PROPERTY(QPlaceIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(QUrl fullUrl READ fullUrl WRITE setFullUrl NOTIFY fullUrlChanged)
    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)

public:
    enum IconFlag {
        Normal = QPlaceIcon::Normal,
        Disabled = QPlaceIcon::Disabled,
        Active = QPlaceIcon::Active,
        Selected = QPlaceIcon::Selected,

        Map = QPlaceIcon::Map,
        List = QPlaceIcon::List
    };

    Q_DECLARE_FLAGS(IconFlags, IconFlag)

    explicit QDeclarativePlaceIcon(QObject* parent = 0);
    QDeclarativePlaceIcon(const QPlaceIcon &src, QDeclarativeGeoServiceProvider *plugin, QObject* parent = 0);
    ~QDeclarativePlaceIcon();

    QPlaceIcon icon() const;
    void setIcon(const QPlaceIcon &src);

    Q_INVOKABLE QUrl url(const QSize &size = QSize(), QDeclarativePlaceIcon::IconFlags flags = Normal) const;

    QUrl fullUrl() const;
    void setFullUrl(const QUrl &fullUrl);

    QUrl baseUrl() const;
    void setBaseUrl(const QUrl &baseUrl);

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider *plugin() const;

signals:
    void baseUrlChanged();
    void fullUrlChanged();
    void pluginChanged();

private:
    QPlaceManager *manager() const;
    QUrl m_baseUrl;
    QUrl m_fullUrl;
    QDeclarativeGeoServiceProvider *m_plugin;
};

QT_END_NAMESPACE

Q_DECLARE_OPERATORS_FOR_FLAGS(QT_PREPEND_NAMESPACE(QDeclarativePlaceIcon::IconFlags))

#endif

