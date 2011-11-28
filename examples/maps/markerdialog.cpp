/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "markerdialog.h"

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

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
    QString addressFormat = tr("$street\n$city, $state $postalcode\n$country");
    addressFormat.replace("$street", address.street());
    addressFormat.replace("$city", address.city());
    addressFormat.replace("$county", address.county());
    addressFormat.replace("$state", address.state());
    addressFormat.replace("$postalcode", address.postalCode());
    addressFormat.replace("$district", address.district());
    addressFormat.replace("$country", address.country());

    addressLabel->setText(addressFormat);
}

MarkerDialog::~MarkerDialog()
{
}
