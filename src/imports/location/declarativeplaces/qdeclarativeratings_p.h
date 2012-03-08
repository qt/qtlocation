/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVERATINGS_P_H
#define QDECLARATIVERATINGS_P_H

#include <qplaceratings.h>
#include <QtQml/qqml.h>

#include <QObject>

QT_BEGIN_NAMESPACE

class QDeclarativeRatings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPlaceRatings ratings READ ratings WRITE setRatings)
    Q_PROPERTY(qreal average READ average WRITE setAverage NOTIFY averageChanged)
    Q_PROPERTY(qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit QDeclarativeRatings(QObject* parent = 0);
    explicit QDeclarativeRatings(const QPlaceRatings &src, QObject* parent = 0);
    ~QDeclarativeRatings();

    QPlaceRatings ratings() const;
    void setRatings(const QPlaceRatings &src);

    qreal average() const;
    void setAverage(qreal average);

    qreal maximum() const;
    void setMaximum(qreal max);

    int count() const;
    void setCount(int count);

signals:
    void averageChanged();
    void maximumChanged();
    void countChanged();

private:
    QPlaceRatings m_ratings;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeRatings));

#endif // QDECLARATIVERATING_P_H
