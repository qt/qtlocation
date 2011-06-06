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
***************************************************************************/

#ifndef QDECLARATIVELANDMARKFILTERS_P_H
#define QDECLARATIVELANDMARKFILTERS_P_H

#include <qlandmarkintersectionfilter.h>
#include <qlandmarkproximityfilter.h>
#include <qdeclarativecoordinate_p.h>
#include <qlandmarkunionfilter.h>
#include <qlandmarknamefilter.h>
#include <qlandmarkcategoryfilter.h>
#include <qlandmarkboxfilter.h>
#include <qlandmarkfilter.h>
#include <qdeclarativelandmarkcategory_p.h>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeListProperty>
#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeLandmarkFilterBase : public QObject
{
    Q_OBJECT

public:
    explicit QDeclarativeLandmarkFilterBase(QObject* parent = 0)  : QObject(parent) {}
    virtual ~QDeclarativeLandmarkFilterBase() {}

    // Returns a C++ intersection filter of all set filters
    virtual QLandmarkFilter* filter() = 0;

signals:
    // This signal is more of internal importance, not part of public interface
    void filterContentChanged();

};

class QDeclarativeLandmarkCompoundFilter: public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    explicit QDeclarativeLandmarkCompoundFilter(QObject* parent = 0);
    virtual ~QDeclarativeLandmarkCompoundFilter() {}
    // 'READ' accessor for the filters, basically this is also a 'WRITE' accessor
    // as per QDeclarativeListProperty's design.
    QDeclarativeListProperty<QDeclarativeLandmarkFilterBase> filters();

    static void filters_append(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop, QDeclarativeLandmarkFilterBase* landmark);
    static int filters_count(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop);
    static QDeclarativeLandmarkFilterBase* filters_at(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop, int index);
    static void filters_clear(QDeclarativeListProperty<QDeclarativeLandmarkFilterBase>* prop);

protected:
    template <class T> bool appendFilters(T* compoundFilter);

private:
    QList<QDeclarativeLandmarkFilterBase*> m_filters;
};


class QDeclarativeLandmarkCategoryFilter : public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeLandmarkCategory* category READ category WRITE setCategory NOTIFY categoryChanged)

public:
    explicit QDeclarativeLandmarkCategoryFilter(QObject* parent = 0);
    ~QDeclarativeLandmarkCategoryFilter();

    QDeclarativeLandmarkCategory* category() const;
    void setCategory(QDeclarativeLandmarkCategory* category);
    // From QDeclarativeLandmarkFilterBase
    virtual QLandmarkFilter* filter();

signals:
    void categoryChanged();

private:
    QDeclarativeLandmarkCategory* m_category;
    QLandmarkCategoryFilter m_filter;
};


class QDeclarativeLandmarkBoxFilter : public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate* topLeft READ topLeft WRITE setTopLeft NOTIFY topLeftChanged)
    Q_PROPERTY(QDeclarativeCoordinate* bottomRight READ bottomRight WRITE setBottomRight NOTIFY bottomRightChanged)

public:
    explicit QDeclarativeLandmarkBoxFilter(QObject* parent = 0);
    ~QDeclarativeLandmarkBoxFilter();

    QDeclarativeCoordinate* topLeft() const;
    void setTopLeft(QDeclarativeCoordinate* coordinate);

    QDeclarativeCoordinate* bottomRight() const;
    void setBottomRight(QDeclarativeCoordinate* coordinate);

    // From QDeclarativeLandmarkFilterBase
    virtual QLandmarkFilter* filter();

signals:
    void topLeftChanged();
    void bottomRightChanged();

private:

    QDeclarativeCoordinate* m_topLeft;
    QDeclarativeCoordinate* m_bottomRight;
    QLandmarkBoxFilter m_filter;
};

class QDeclarativeLandmarkNameFilter : public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit QDeclarativeLandmarkNameFilter(QObject* parent = 0);
    ~QDeclarativeLandmarkNameFilter();

    QString name() const;
    void setName(const QString& name);
    // From QDeclarativeLandmarkFilterBase
    virtual QLandmarkFilter* filter();

signals:
    void nameChanged();

private:
    QString m_name;
    QLandmarkNameFilter m_filter;
};


class QDeclarativeLandmarkProximityFilter : public QDeclarativeLandmarkFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeCoordinate* center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)

public:
    explicit QDeclarativeLandmarkProximityFilter(QObject* parent = 0);
    ~QDeclarativeLandmarkProximityFilter();

    double radius() const;
    void setRadius(const double radius);
    QDeclarativeCoordinate* center() const;
    void setCenter(QDeclarativeCoordinate* coordinate);
    // From QDeclarativeLandmarkFilterBase
    virtual QLandmarkFilter* filter();

signals:
    void radiusChanged();
    void centerChanged();

private:
    double m_radius;
    QDeclarativeCoordinate* m_coordinate;
    QLandmarkProximityFilter m_filter;
};


class QDeclarativeLandmarkUnionFilter : public QDeclarativeLandmarkCompoundFilter
{
    Q_OBJECT

public:
    explicit QDeclarativeLandmarkUnionFilter(QObject* parent = 0);
    QLandmarkFilter* filter();

private:
    QLandmarkUnionFilter m_filter;
};

class QDeclarativeLandmarkIntersectionFilter : public QDeclarativeLandmarkCompoundFilter
{
    Q_OBJECT

public:
    explicit QDeclarativeLandmarkIntersectionFilter(QObject* parent = 0);
    virtual QLandmarkFilter* filter();

private:
    QLandmarkIntersectionFilter m_filter;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkNameFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkCategoryFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkBoxFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkProximityFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkUnionFilter));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeLandmarkIntersectionFilter));

#endif // QDECLARATIVELANDMARKFILTERS_P_H
