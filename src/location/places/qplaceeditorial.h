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
****************************************************************************/

#ifndef QPLACEEDITORIAL_H
#define QPLACEEDITORIAL_H

#include <QtLocation/QPlaceContent>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE



class QPlaceEditorialPrivate;

class Q_LOCATION_EXPORT QPlaceEditorial : public QPlaceContent
{
public:
    QPlaceEditorial();
#ifdef Q_QDOC
    QPlaceEditorial::QPlaceEditorial(const QPlaceContent &other)
#else
    Q_DECLARE_CONTENT_COPY_CTOR(QPlaceEditorial)
#endif

    virtual ~QPlaceEditorial();

    QString text() const;
    void setText(const QString &text);
    QString title() const;
    void setTitle(const QString &data);
    QString language() const;
    void setLanguage(const QString &data);

private:
    Q_DECLARE_CONTENT_D_FUNC(QPlaceEditorial)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPLACEEDITORIAL_H
