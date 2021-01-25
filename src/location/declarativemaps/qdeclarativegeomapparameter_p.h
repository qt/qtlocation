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

#ifndef QDECLARATIVEGEOMAPPARAMETER_P_H
#define QDECLARATIVEGEOMAPPARAMETER_P_H

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

#include <QtLocation/private/qlocationglobal_p.h>
#include <QtLocation/private/qgeomapparameter_p.h>
#include <QQmlParserStatus>
#include <qqml.h>

QT_BEGIN_NAMESPACE

class Q_LOCATION_PRIVATE_EXPORT QDeclarativeGeoMapParameter : public QGeoMapParameter, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QDeclarativeGeoMapParameter(QObject *parent = 0);
    virtual ~QDeclarativeGeoMapParameter();

    bool isComponentComplete() const;

Q_SIGNALS:
    void completed(QDeclarativeGeoMapParameter *);

protected:
    int initialPropertyCount() const;
    // QQmlParserStatus implementation
    void classBegin() override;
    void componentComplete() override;

private slots:
    void onPropertyUpdated(int index);

private:
    int m_initialPropertyCount;
    bool m_complete;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeGeoMapParameter)

#endif // QDECLARATIVEGEOMAPPARAMETER_P_H
