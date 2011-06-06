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

#ifndef QDECLARATIVEGEOMAPOBJECTBORDER_H
#define QDECLARATIVEGEOMAPOBJECTBORDER_H

#include "qmobilityglobal.h"
#include <QtDeclarative/qdeclarative.h>

#include <QObject>
#include <QColor>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoMapObjectBorder : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)

public:
    QDeclarativeGeoMapObjectBorder(QObject *parent = 0);
    ~QDeclarativeGeoMapObjectBorder();

    QColor color() const;
    void setColor(const QColor &color);

    int width() const;
    void setWidth(int width);

Q_SIGNALS:
    void colorChanged(const QColor &color);
    void widthChanged(int width);

private:
    QColor m_color;
    int m_width;
    Q_DISABLE_COPY(QDeclarativeGeoMapObjectBorder)
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapObjectBorder));

#endif
