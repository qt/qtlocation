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

#ifndef QDECLARATIVESUPPLIER_P_H
#define QDECLARATIVESUPPLIER_P_H

#include <QObject>
#include <QtCore/QUrl>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/QDeclarativeParserStatus>
#include <qplacesupplier.h>

#include "qdeclarativeplaceicon_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeSupplier : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QPlaceSupplier supplier READ supplier WRITE setSupplier)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    Q_PROPERTY(QString supplierId READ supplierId WRITE setSupplierId NOTIFY supplierIdChanged);
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QDeclarativePlaceIcon* icon READ icon WRITE setIcon NOTIFY iconChanged)

    Q_INTERFACES(QDeclarativeParserStatus)

public:
    explicit QDeclarativeSupplier(QObject* parent = 0);
    explicit QDeclarativeSupplier(const QPlaceSupplier &src, QDeclarativeGeoServiceProvider *plugin, QObject* parent = 0);
    ~QDeclarativeSupplier();

    // From QDeclarativeParserStatus
    void classBegin() { }
    void componentComplete();

    QPlaceSupplier supplier();
    void setSupplier(const QPlaceSupplier &src, QDeclarativeGeoServiceProvider *plugin = 0);

    QString name() const;
    void setName(const QString &data);
    QString supplierId() const;
    void setSupplierId(const QString &data);
    QUrl url() const;
    void setUrl(const QUrl &data);

    QDeclarativePlaceIcon *icon() const;
    void setIcon(QDeclarativePlaceIcon *icon);

signals:
    void nameChanged();
    void supplierIdChanged();
    void urlChanged();
    void iconChanged();

private:
    QPlaceSupplier m_src;
    QDeclarativePlaceIcon *m_icon;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeSupplier));

#endif // QDECLARATIVESUPPLIER_P_H
