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

#include "navigatedialog.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QMetaObject>

Q_DECLARE_METATYPE(QGeoRouteRequest::TravelMode)

NavigateDialog::NavigateDialog(QWidget *parent) :
    QDialog(parent)
{
    QFormLayout *formLayout = new QFormLayout;
    QVBoxLayout *vbox = new QVBoxLayout;

    addressEdit = new QLineEdit;
    formLayout->addRow("Address", addressEdit);

    modeCombo = new QComboBox;
    modeCombo->addItem("Car", qVariantFromValue(QGeoRouteRequest::CarTravel));
    modeCombo->addItem("Pedestrian", qVariantFromValue(QGeoRouteRequest::PedestrianTravel));
    modeCombo->addItem("Bicycle", qVariantFromValue(QGeoRouteRequest::BicycleTravel));
    modeCombo->addItem("Public Transit", qVariantFromValue(QGeoRouteRequest::PublicTransitTravel));
    formLayout->addRow("Mode", modeCombo);

    QDialogButtonBox *bb = new QDialogButtonBox;
    bb->addButton(QDialogButtonBox::Ok);
    bb->addButton(QDialogButtonBox::Cancel);
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    vbox->addLayout(formLayout);
    vbox->addWidget(bb);

    setLayout(vbox);
    setWindowTitle("Directions to address");
}

NavigateDialog::~NavigateDialog()
{
}

QString NavigateDialog::destinationAddress() const
{
    return addressEdit->text();
}

QGeoRouteRequest::TravelModes NavigateDialog::travelMode() const
{
    QVariant v = modeCombo->itemData(modeCombo->currentIndex());
    QGeoRouteRequest::TravelModes mode;
    mode |= qVariantValue<QGeoRouteRequest::TravelMode>(v);
    return mode;
}
