/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#ifndef QDECLARATIVELOCATIONTESTMODEL_H
#define QDECLARATIVELOCATIONTESTMODEL_H

#include <QtQuick/QQuickItem>
#include <QAbstractListModel>
#include <QTimer>
#include <QDebug>
#include <QList>
#include <qdeclarativecoordinate_p.h>

class DataObject: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate* coordinate READ coordinate CONSTANT)

public:
    DataObject() : coordinate_(0) {}
    ~DataObject() {delete coordinate_;}

    QDeclarativeCoordinate* coordinate_;
    QDeclarativeCoordinate* coordinate() const {return coordinate_;}
};

class QDeclarativeLocationTestModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(int datacount READ datacount WRITE setDatacount NOTIFY datacountChanged)
    Q_PROPERTY(int delay READ delay WRITE setDelay NOTIFY delayChanged)
    Q_PROPERTY(bool crazyMode READ crazyMode WRITE setCrazyMode NOTIFY crazyModeChanged)
    Q_PROPERTY(int crazyLevel READ crazyLevel WRITE setCrazyLevel NOTIFY crazyLevelChanged)
    Q_PROPERTY(QString datatype READ datatype WRITE setDatatype NOTIFY datatypeChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    QDeclarativeLocationTestModel(QObject* parent = 0);
    ~QDeclarativeLocationTestModel();

    enum Roles {
        TestDataRole = Qt::UserRole + 500
    };

    // from QDeclarativeParserStatus
    virtual void componentComplete();
    virtual void classBegin() {}

    // From QAbstractListModel
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    int datacount() const;
    void setDatacount(int datacount);

    int delay() const;
    void setDelay(int delay);

    int crazyLevel() const;
    void setCrazyLevel(int level);

    bool crazyMode() const;
    void setCrazyMode(bool mode);

    QString datatype() const;
    void setDatatype(QString datatype);

    //Q_INVOKABLE void clear();
    Q_INVOKABLE void reset();
    //Q_INVOKABLE void reset();

signals:
    void countChanged();
    void datacountChanged();
    void datatypeChanged();
    void delayChanged();
    void modelChanged();
    void crazyLevelChanged();
    void crazyModeChanged();

private slots:
    void repopulate();
    void timerFired();

private:
    void scheduleRepopulation();

private:
    int delay_;
    int datacount_;
    bool componentCompleted_;
    QString datatype_;
    QTimer timer_;
    QList<DataObject*> dataobjects_;
    int crazyLevel_;
    bool crazyMode_;
};

#endif
