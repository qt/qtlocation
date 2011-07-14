/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QDECLARATIVEQGEOSERVICEPROVIDER_H
#define QDECLARATIVEQGEOSERVICEPROVIDER_H

#include <qgeocoordinate.h>
#include <QtQuick1/qdeclarativeitem.h>

#include <QMap>
#include <QString>
#include <QVariant>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoServiceProviderParameter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    QDeclarativeGeoServiceProviderParameter(QObject *parent = 0);
    ~QDeclarativeGeoServiceProviderParameter();

    void setName(const QString &name);
    QString name() const;

    void setValue(const QVariant &value);
    QVariant value() const;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void valueChanged(const QVariant &value);

private:
    QString name_;
    QVariant value_;
};

class QDeclarativeGeoServiceProvider : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QStringList availableServiceProviders READ availableServiceProviders CONSTANT)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> parameters READ parameters)
    Q_PROPERTY(bool supportsGeocoding READ supportsGeocoding NOTIFY supportsGeocodingChanged)
    Q_PROPERTY(bool supportsReverseGeocoding READ supportsReverseGeocoding NOTIFY supportsReverseGeocodingChanged)
    Q_PROPERTY(bool supportsRouting READ supportsRouting NOTIFY supportsRoutingChanged)
    Q_PROPERTY(bool supportsMapping READ supportsMapping NOTIFY supportsMappingChanged)
    Q_CLASSINFO("DefaultProperty", "parameters")
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    QDeclarativeGeoServiceProvider(QObject *parent = 0);
    ~QDeclarativeGeoServiceProvider();

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    void setName(const QString &name);
    QString name() const;

    QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> parameters();
    QMap<QString, QVariant> parameterMap() const;

    QStringList availableServiceProviders();
    bool supportsGeocoding() const;
    bool supportsReverseGeocoding() const;
    bool supportsRouting() const;
    bool supportsMapping() const;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void supportsGeocodingChanged();
    void supportsReverseGeocodingChanged();
    void supportsRoutingChanged();
    void supportsMappingChanged();

private:
    static void parameter_append(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, QDeclarativeGeoServiceProviderParameter *mapObject);
    static int parameter_count(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop);
    static QDeclarativeGeoServiceProviderParameter* parameter_at(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, int index);
    static void parameter_clear(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop);
    void updateSupportStatus();
    void setSupportsGeocoding(bool supports);
    void setSupportsReverseGeocoding(bool supports);
    void setSupportsRouting(bool supports);
    void setSupportsMapping(bool supports);

    QString name_;
    QList<QDeclarativeGeoServiceProviderParameter*> parameters_;
    bool supportsGeocoding_;
    bool supportsReverseGeocoding_;
    bool supportsRouting_;
    bool supportsMapping_;
    bool complete_;
    Q_DISABLE_COPY(QDeclarativeGeoServiceProvider)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoServiceProviderParameter));
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoServiceProvider));

#endif
