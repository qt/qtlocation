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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qplacesuppliersrepository.h"

#include <qplacesupplier.h>

QT_USE_NAMESPACE

QPlaceSuppliersRepository *QPlaceSuppliersRepository::instance()
{
    static QPlaceSuppliersRepository instance;
    return &instance;
}

QPlaceSuppliersRepository::QPlaceSuppliersRepository(QObject *parent)
    : QObject(parent)
{
}

QPlaceSuppliersRepository::~QPlaceSuppliersRepository()
{
    suppliers.clear();
}

QPlaceSupplier QPlaceSuppliersRepository::addSupplier(const QPlaceSupplier &src)
{
    QPlaceSupplier res;
    QPlaceSupplier tmp;

    foreach (tmp, suppliers) {
        if ((!src.supplierId().isEmpty() && src.supplierId() == tmp.supplierId())
                || (!src.name().isEmpty() && src.name() == tmp.name())) {
            copyMissingData(src, tmp);
            res = tmp;
            break;
        }
    }
    if (res.supplierId().isEmpty() && res.name().isEmpty()) {
        res = src;
        suppliers.append(res);
    }
    return res;
}

void QPlaceSuppliersRepository::copyMissingData(const QPlaceSupplier &src,
                                                QPlaceSupplier &target)
{
    if (target.name().isEmpty() && !src.name().isEmpty()) {
        target.setName(src.name());
    }
    if (target.supplierId().isEmpty() && !src.supplierId().isEmpty()) {
        target.setSupplierId(src.supplierId());
    }
    if (target.url().isEmpty() && !src.url().isEmpty()) {
        target.setUrl(src.url());
    }
    if (target.supplierIconUrl().isEmpty() && !src.supplierIconUrl().isEmpty()) {
        target.setSupplierIconUrl(src.supplierIconUrl());
    }
}
