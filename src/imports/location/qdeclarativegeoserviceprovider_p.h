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

#ifndef QDECLARATIVEQGEOSERVICEPROVIDER_H
#define QDECLARATIVEQGEOSERVICEPROVIDER_H

#include <qgeocoordinate.h>
#include <QtQuick/QQuickItem>
#include <QtDeclarative/QDeclarativeParserStatus>
#include <QtLocation/QPlaceManager>

#include <QMap>
#include <QString>
#include <QVariant>

QT_BEGIN_NAMESPACE

class QGeoServiceProvider;

class QDeclarativeGeoServiceProviderParameter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    QDeclarativeGeoServiceProviderParameter(QObject *parent = 0);
    ~QDeclarativeGeoServiceProviderParameter();

    void setName(const QString &name);
    QString name() const;

    void setValue(const QVariant &value);
    QVariant value() const;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void valueChanged(const QVariant &value);

private:
    QString name_;
    QVariant value_;
};

class QDeclarativeGeoServiceProvider : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_ENUMS (PlacesFeature)
    Q_ENUMS (PluginFeature)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QStringList availableServiceProviders READ availableServiceProviders CONSTANT)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> parameters READ parameters)
    Q_PROPERTY(PluginFeatures required READ requiredFeatures WRITE setRequiredFeatures NOTIFY requiredFeaturesChanged)
    Q_PROPERTY(PluginFeatures supported READ supportedFeatures NOTIFY supportedFeaturesChanged)
    Q_PROPERTY(PlacesFeatures supportedPlacesFeatures READ supportedPlacesFeatures NOTIFY supportedPlacesFeaturesChanged)
    Q_PROPERTY(QStringList locales READ locales WRITE setLocales NOTIFY localesChanged)

    Q_CLASSINFO("DefaultProperty", "parameters")
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    QDeclarativeGeoServiceProvider(QObject *parent = 0);
    ~QDeclarativeGeoServiceProvider();

    enum PluginFeature {
        NoFeatures                 = 0x00000000,
        GeocodingFeature           = 0x00000001,
        ReverseGeocodingFeature    = 0x00000002,
        RoutingFeature             = 0x00000004,
        MappingFeature             = 0x00000008,
        AnyPlacesFeature           = 0x00000010
    };

    Q_DECLARE_FLAGS(PluginFeatures, PluginFeature)
    Q_FLAGS(PluginFeatures)

    enum PlacesFeature {
        NoPlaceFeatures = QPlaceManager::NoFeatures,
        SavePlaceFeature = QPlaceManager::SavePlaceFeature,
        RemovePlaceFeature = QPlaceManager::RemovePlaceFeature,
        SaveCategoryFeature = QPlaceManager:: SaveCategoryFeature,
        RemoveCategoryFeature = QPlaceManager::RemoveCategoryFeature,
        RecommendationsFeature = QPlaceManager::RecommendationsFeature,
        SearchSuggestionsFeature = QPlaceManager::SearchSuggestionsFeature,
        CorrectionsFeature = QPlaceManager::CorrectionsFeature,
        LocaleFeature = QPlaceManager::LocaleFeature,
        NotificationsFeature = QPlaceManager::NotificationsFeature,
        FavoritesMatchingFeature = QPlaceManager::MatchingFeature
    };

    Q_DECLARE_FLAGS(PlacesFeatures, PlacesFeature)
    Q_FLAGS(PlacesFeatures)

    // From QDeclarativeParserStatus
    virtual void classBegin() {}
    virtual void componentComplete();

    void setName(const QString &name);
    QString name() const;

    QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> parameters();
    QMap<QString, QVariant> parameterMap() const;

    QStringList availableServiceProviders();

    PluginFeatures supportedFeatures() const;

    PluginFeatures requiredFeatures() const;
    void setRequiredFeatures(const PluginFeatures &features);

    PlacesFeatures supportedPlacesFeatures() const;

    QGeoServiceProvider *sharedGeoServiceProvider();

    QStringList locales() const;
    void setLocales(const QStringList &locales);

    bool ready() const;

Q_SIGNALS:
    void nameChanged(const QString &name);
    void localesChanged();
    void supportedFeaturesChanged(const PluginFeatures &features);
    void requiredFeaturesChanged(const PluginFeatures &features);
    void supportedPlacesFeaturesChanged(const PlacesFeatures &features);

private:
    static void parameter_append(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, QDeclarativeGeoServiceProviderParameter *mapObject);
    static int parameter_count(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop);
    static QDeclarativeGeoServiceProviderParameter* parameter_at(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, int index);
    static void parameter_clear(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop);
    void update(bool doEmit = true);

    QGeoServiceProvider *sharedProvider_;
    QString name_;
    QList<QDeclarativeGeoServiceProviderParameter*> parameters_;
    PluginFeatures supported_;
    PluginFeatures required_;
    bool complete_;
    QStringList locales_;
    PlacesFeatures placesFeatures_;
    Q_DISABLE_COPY(QDeclarativeGeoServiceProvider)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoServiceProviderParameter));
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeoServiceProvider));
Q_DECLARE_OPERATORS_FOR_FLAGS(QT_PREPEND_NAMESPACE(QDeclarativeGeoServiceProvider::PlacesFeatures))
Q_DECLARE_OPERATORS_FOR_FLAGS(QT_PREPEND_NAMESPACE(QDeclarativeGeoServiceProvider::PluginFeatures))

#endif
