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

#include "searchdialog.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QVBoxLayout>

SearchDialog::SearchDialog(QWidget *parent) :
    QDialog(parent)
{
    QFormLayout *formLayout = new QFormLayout;
    QVBoxLayout *vbox = new QVBoxLayout;

    searchTermEdit = new QLineEdit;
    formLayout->addRow("Search for", searchTermEdit);

    whereCombo = new QComboBox;
    whereCombo->addItem(tr("Nearby (<10km)"), 10000);
    whereCombo->addItem(tr("Within 30 mins drive of me (<25km)"), 25000);
    whereCombo->addItem(tr("Within 100km of me"), 100000);
    whereCombo->addItem(tr("Anywhere in the world"), -1);
    whereCombo->setCurrentIndex(1);
    formLayout->addRow(tr("Where"), whereCombo);

    QDialogButtonBox *bb = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                QDialogButtonBox::Cancel,
                                                Qt::Horizontal);
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    vbox->addLayout(formLayout);
    vbox->addWidget(bb);
    setLayout(vbox);
    setWindowTitle("Search for location");
}

qreal SearchDialog::radius() const
{
    const int i = whereCombo->currentIndex();
    return whereCombo->itemData(i).toReal();
}

SearchDialog::~SearchDialog()
{
}

QString SearchDialog::searchTerms() const
{
    return searchTermEdit->text();
}
