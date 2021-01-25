/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QDECLARATIVEPERIOD_P_H
#define QDECLARATIVEPERIOD_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qplaceperiod.h>
#include <QtQml/qqml.h>

#include <QObject>

QT_BEGIN_NAMESPACE

class QDeclarativePeriod : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDate startDate READ startDate WRITE setStartDate NOTIFY startDateChanged)
    Q_PROPERTY(QTime startTime READ startTime WRITE setStartTime NOTIFY startTimeChanged)
    Q_PROPERTY(QDate endDate READ endDate WRITE setEndDate NOTIFY endDateChanged)
    Q_PROPERTY(QTime endTime READ endTime WRITE setEndTime NOTIFY endTimeChanged)

public:
    explicit QDeclarativePeriod(QObject *parent = 0);
    explicit QDeclarativePeriod(const QPlacePeriod &period, QObject *parent = 0);
    ~QDeclarativePeriod();

    QPlacePeriod period() const;
    void setPeriod(const QPlacePeriod &period);

    QDate startDate() const;
    void setStartDate(const QDate &data);
    QTime startTime() const;
    void setStartTime(const QTime &data);
    QDate endDate() const;
    void setEndDate(const QDate &data);
    QTime endTime() const;
    void setEndTime(const QTime &data);

Q_SIGNALS:
    void startDateChanged();
    void startTimeChanged();
    void endDateChanged();
    void endTimeChanged();

private:
    QPlacePeriod m_period;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativePeriod));

#endif // QDECLARATIVEPERIOD_P_H
