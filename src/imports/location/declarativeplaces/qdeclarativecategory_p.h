/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVECATEGORY_P_H
#define QDECLARATIVECATEGORY_P_H

#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeParserStatus>
#include <QObject>

#include <qplacecategory.h>

#include "qdeclarativegeoserviceprovider_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativePlaceIcon;
class QPlaceReply;
class QPlaceManager;

class QDeclarativeCategory : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_ENUMS(Status Visibility)


    Q_PROPERTY(QPlaceCategory category READ category WRITE setCategory)
    Q_PROPERTY(QDeclarativeGeoServiceProvider *plugin READ plugin WRITE setPlugin NOTIFY pluginChanged)
    Q_PROPERTY(QString categoryId READ categoryId WRITE setCategoryId NOTIFY categoryIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Visibility visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
    Q_PROPERTY(QDeclarativePlaceIcon* icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

    Q_INTERFACES(QDeclarativeParserStatus)

public:
    explicit QDeclarativeCategory(QObject* parent = 0);
    QDeclarativeCategory(const QPlaceCategory &category, QDeclarativeGeoServiceProvider *plugin, QObject* parent = 0);
    ~QDeclarativeCategory();

    enum Visibility {
        UnspecifiedVisibility = QtLocation::UnspecifiedVisibility,
        DeviceVisibility = QtLocation::DeviceVisibility,
        PrivateVisibility = QtLocation::PrivateVisibility,
        PublicVisibility = QtLocation::PublicVisibility
    };
    enum Status {Ready, Saving, Removing, Error};

    //From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    void setPlugin(QDeclarativeGeoServiceProvider *plugin);
    QDeclarativeGeoServiceProvider* plugin() const;

    QPlaceCategory category();
    void setCategory(const QPlaceCategory &category);

    QString categoryId() const;
    void setCategoryId(const QString &catID);
    QString name() const;
    void setName(const QString &name);

    Visibility visibility() const;
    void setVisibility(Visibility visibility);

    QDeclarativePlaceIcon *icon() const;
    void setIcon(QDeclarativePlaceIcon *icon);

    Q_INVOKABLE QString errorString() const;

    Status status() const;
    void setStatus(Status status);

    Q_INVOKABLE void save(const QString &parentId = QString());
    Q_INVOKABLE void remove();

signals:
    void pluginChanged();
    void categoryIdChanged();
    void nameChanged();
    void visibilityChanged();
    void iconChanged();
    void statusChanged();

private slots:
    void replyFinished();

private:
    QPlaceManager *manager();

    QPlaceCategory m_category;
    QDeclarativePlaceIcon *m_icon;
    QDeclarativeGeoServiceProvider *m_plugin;
    QPlaceReply *m_reply;
    bool m_complete;
    Status m_status;
    QString m_errorString;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeCategory));

#endif // QDECLARATIVECATEGORY_P_H
