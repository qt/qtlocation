/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** $QT_END_LICENSE$
**
***************************************************************************/

#ifndef QDECLARATIVEGEOADDRESS_P_H
#define QDECLARATIVEGEOADDRESS_P_H

#include <qgeoaddress.h>
#include <QtCore>
#include <QtDeclarative/qdeclarative.h>

QT_BEGIN_NAMESPACE

class QDeclarativeGeoAddress : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QGeoAddress address READ address WRITE setAddress)
    Q_PROPERTY(QString country READ country WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(QString countryCode READ countryCode WRITE setCountryCode NOTIFY countryCodeChanged)
    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString county READ county WRITE setCounty NOTIFY countyChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString district READ district WRITE setDistrict NOTIFY districtChanged)
    Q_PROPERTY(QString street READ street WRITE setStreet NOTIFY streetChanged)
    Q_PROPERTY(QString postalCode READ postalCode WRITE setPostalCode NOTIFY postalCodeChanged)

public:
    explicit QDeclarativeGeoAddress(QObject* parent = 0);
    QDeclarativeGeoAddress(const QGeoAddress& address, QObject* parent = 0);
    QGeoAddress address() const;
    void setAddress(const QGeoAddress& address);

    QString country() const;
    void setCountry(const QString& country);
    QString countryCode() const;
    void setCountryCode(const QString& countryCode);
    QString state() const;
    void setState(const QString& state);
    QString county() const;
    void setCounty(const QString& county);
    QString city() const;
    void setCity(const QString& city);
    QString district() const;
    void setDistrict(const QString& district);
    QString street() const;
    void setStreet(const QString& street);
    QString postalCode() const;
    void setPostalCode(const QString& postalCode);

signals:
    void countryChanged();
    void countryCodeChanged();
    void stateChanged();
    void countyChanged();
    void cityChanged();
    void districtChanged();
    void streetChanged();
    void postalCodeChanged();

private:
    QGeoAddress m_address;
};

QT_END_NAMESPACE
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoAddress));

#endif // QDECLARATIVEGEOADDRESS_P_H
