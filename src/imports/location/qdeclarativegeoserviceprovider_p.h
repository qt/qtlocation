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

#ifndef QDECLARATIVEQGEOSERVICEPROVIDER_H
#define QDECLARATIVEQGEOSERVICEPROVIDER_H

#include <qgeocoordinate.h>
#include <QtDeclarative/qdeclarativeitem.h>

#include <QMap>
#include <QString>
#include <QVariant>

QTM_BEGIN_NAMESPACE

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

class QDeclarativeGeoServiceProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> parameters READ parameters)

    Q_CLASSINFO("DefaultProperty", "parameters")

public:
    QDeclarativeGeoServiceProvider(QObject *parent = 0);
    ~QDeclarativeGeoServiceProvider();

    void setName(const QString &name);
    QString name() const;

    QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> parameters();

    QMap<QString, QVariant> parameterMap() const;

Q_SIGNALS:
    void nameChanged(const QString &name);

private:
    static void parameter_append(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, QDeclarativeGeoServiceProviderParameter *mapObject);
    static int parameter_count(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop);
    static QDeclarativeGeoServiceProviderParameter* parameter_at(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, int index);
    static void parameter_clear(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop);

    QString name_;
    QList<QDeclarativeGeoServiceProviderParameter*> parameters_;

    Q_DISABLE_COPY(QDeclarativeGeoServiceProvider)
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoServiceProviderParameter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoServiceProvider));

#endif
