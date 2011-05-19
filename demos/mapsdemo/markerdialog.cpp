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

#include "markerdialog.h"

#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>

MarkerDialog::MarkerDialog(Marker *marker) :
    marker(marker)
{
    QVBoxLayout *vbox = new QVBoxLayout;

    QFormLayout *fm = new QFormLayout;

    nameEdit = new QLineEdit;
    nameEdit->setText(marker->name());
    connect(marker, SIGNAL(nameChanged(QString)),
            nameEdit, SLOT(setText(QString)));
    fm->addRow("Name", nameEdit);

    addressLabel = new QLabel;
    setAddressLabel(marker->address());
    connect(marker, SIGNAL(addressChanged(QGeoAddress)),
            this, SLOT(setAddressLabel(QGeoAddress)));
    fm->addRow("Address", addressLabel);

    lonSpin = new QDoubleSpinBox;
    lonSpin->setMinimum(-180.0);
    lonSpin->setMaximum(180.0);
    lonSpin->setDecimals(7);
    lonSpin->setValue(marker->coordinate().longitude());
    fm->addRow("Longitude", lonSpin);

    latSpin = new QDoubleSpinBox;
    latSpin->setMinimum(-90.0);
    latSpin->setMaximum(90.0);
    latSpin->setDecimals(7);
    latSpin->setValue(marker->coordinate().latitude());
    fm->addRow("Latitude", latSpin);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Save |
                                                QDialogButtonBox::Close,
                                                Qt::Horizontal);
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    vbox->addLayout(fm);
    vbox->addWidget(bb);
    setLayout(vbox);
    setWindowTitle("Edit marker");
}

void MarkerDialog::updateMarker()
{
    marker->setName(nameEdit->text());
    QGeoCoordinate c(latSpin->value(), lonSpin->value());
    marker->setCoordinate(c);
}

void MarkerDialog::setAddressLabel(QGeoAddress address)
{
    QString addressFormat = tr("$street\n$city, $state $postcode\n$country");
    addressFormat.replace("$street", address.street());
    addressFormat.replace("$city", address.city());
    addressFormat.replace("$county", address.county());
    addressFormat.replace("$state", address.state());
    addressFormat.replace("$postcode", address.postcode());
    addressFormat.replace("$district", address.district());
    addressFormat.replace("$country", address.country());

    addressLabel->setText(addressFormat);
}

MarkerDialog::~MarkerDialog()
{
}
