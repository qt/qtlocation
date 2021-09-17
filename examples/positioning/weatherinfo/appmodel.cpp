/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "appmodel.h"
#include "openweathermapbackend.h"
#include "weatherapibackend.h"

#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QGeoCircle>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(requestsLog, "wapp.requests")

WeatherData::WeatherData(QObject *parent) :
        QObject(parent)
{
}

WeatherData::WeatherData(const WeatherData &other) :
        QObject(nullptr),
        m_dayOfWeek(other.m_dayOfWeek),
        m_weather(other.m_weather),
        m_weatherDescription(other.m_weatherDescription),
        m_temperature(other.m_temperature)
{
}

WeatherData::WeatherData(const WeatherInfo &other)
    : QObject(nullptr),
      m_dayOfWeek(other.m_dayOfWeek),
      m_weather(other.m_weatherIconId),
      m_weatherDescription(other.m_weatherDescription),
      m_temperature(other.m_temperature)
{
}

QString WeatherData::dayOfWeek() const
{
    return m_dayOfWeek;
}

/*!
 * The icon value is based on OpenWeatherMap.org icon set. For details
 * see http://bugs.openweathermap.org/projects/api/wiki/Weather_Condition_Codes
 *
 * e.g. 01d ->sunny day
 *
 * The icon string will be translated to
 * http://openweathermap.org/img/w/01d.png
 */
QString WeatherData::weatherIcon() const
{
    return m_weather;
}

QString WeatherData::weatherDescription() const
{
    return m_weatherDescription;
}

QString WeatherData::temperature() const
{
    return m_temperature;
}

void WeatherData::setDayOfWeek(const QString &value)
{
    m_dayOfWeek = value;
    emit dataChanged();
}

void WeatherData::setWeatherIcon(const QString &value)
{
    m_weather = value;
    emit dataChanged();
}

void WeatherData::setWeatherDescription(const QString &value)
{
    m_weatherDescription = value;
    emit dataChanged();
}

void WeatherData::setTemperature(const QString &value)
{
    m_temperature = value;
    emit dataChanged();
}

/*
    The class is used as a cache for the weather information.
    It contains a map to cache weather for cities.
    The gps location is cached separately.

    For the coordiante search we do not compare the coordinate directly, but
    check if it's within a circle of 3 km radius (we assume that the weather
    does not really change within that radius).

    The cache returns a pair with empty location and weather data if no data
    is found, or if the data is outdated.
*/
class WeatherDataCache
{
public:
    WeatherDataCache() = default;

    using WeatherDataPair = QPair<QString, QList<WeatherInfo>>;

    WeatherDataPair getWeatherData(const QString &name) const;
    WeatherDataPair getWeatherData(const QGeoCoordinate &coordinate) const;

    void addCacheElement(const LocationInfo &location, const QList<WeatherInfo> &info);

    static bool isCacheResultValid(const WeatherDataPair &result);

private:
    struct CacheItem
    {
        qint64 m_cacheTime;
        QList<WeatherInfo> m_weatherData;
    };

    QMap<QString, CacheItem> m_cityCache;

    QGeoCoordinate m_gpsLocation;
    QString m_gpsName;
    CacheItem m_gpsData;

    static const qint64 kCacheTimeoutInterval = 3600; // 1 hour
    static const int kCircleRadius = 3000; // 3 km
};

WeatherDataCache::WeatherDataPair WeatherDataCache::getWeatherData(const QString &name) const
{
    if (m_cityCache.contains(name)) {
        const qint64 currentTime = QDateTime::currentSecsSinceEpoch();
        const auto &item = m_cityCache.value(name);
        if (currentTime - item.m_cacheTime < kCacheTimeoutInterval)
            return qMakePair(name, item.m_weatherData);
    }
    return qMakePair(QString(), QList<WeatherInfo>());
}

WeatherDataCache::WeatherDataPair WeatherDataCache::getWeatherData(const QGeoCoordinate &coordinate) const
{
    if (m_gpsLocation.isValid() && !m_gpsName.isEmpty()) {
        const QGeoCircle area(m_gpsLocation, kCircleRadius);
        if (area.contains(coordinate)) {
            const qint64 currentTime = QDateTime::currentSecsSinceEpoch();
            if (currentTime - m_gpsData.m_cacheTime < kCacheTimeoutInterval)
                return qMakePair(m_gpsName, m_gpsData.m_weatherData);
        }
    }
    return qMakePair(QString(), QList<WeatherInfo>());
}

void WeatherDataCache::addCacheElement(const LocationInfo &location, const QList<WeatherInfo> &info)
{
    // It it expected that we have valid QGeoCoordinate only when the weather
    // is received based on coordinates.
    const qint64 currentTime = QDateTime::currentSecsSinceEpoch();
    if (location.m_coordinate.isValid()) {
        m_gpsLocation = location.m_coordinate;
        m_gpsName = location.m_name;
        m_gpsData = { currentTime, info };
    } else {
        m_cityCache[location.m_name] = { currentTime, info };
    }
}

bool WeatherDataCache::isCacheResultValid(const WeatherDataCache::WeatherDataPair &result)
{
    return !result.first.isEmpty() && !result.second.isEmpty();
}

class AppModelPrivate
{
public:
    QGeoPositionInfoSource *src = nullptr;
    QGeoCoordinate coord;
    QString city;
    WeatherData now;
    QList<WeatherData*> forecast;
    QQmlListProperty<WeatherData> *fcProp = nullptr;
    bool ready = false;
    bool useGps = true;
    WeatherDataCache m_dataCache;
    ProviderBackend *m_currentBackend = nullptr;
    QList<ProviderBackend*> m_supportedBackends;
    qsizetype m_currentBackendIndex = 0;
};

static void forecastAppend(QQmlListProperty<WeatherData> *prop, WeatherData *val)
{
    Q_UNUSED(val);
    Q_UNUSED(prop);
}

static WeatherData *forecastAt(QQmlListProperty<WeatherData> *prop, qsizetype index)
{
    AppModelPrivate *d = static_cast<AppModelPrivate*>(prop->data);
    return d->forecast.at(index);
}

static qsizetype forecastCount(QQmlListProperty<WeatherData> *prop)
{
    AppModelPrivate *d = static_cast<AppModelPrivate*>(prop->data);
    return d->forecast.size();
}

static void forecastClear(QQmlListProperty<WeatherData> *prop)
{
    static_cast<AppModelPrivate*>(prop->data)->forecast.clear();
}

//! [0]
AppModel::AppModel(QObject *parent) :
        QObject(parent),
        d(new AppModelPrivate)
{
//! [0]
    d->fcProp = new QQmlListProperty<WeatherData>(this, d, forecastAppend,
                                                           forecastCount,
                                                           forecastAt,
                                                           forecastClear);

    d->m_supportedBackends.push_back(new OpenWeatherMapBackend(this));
    d->m_supportedBackends.push_back(new WeatherApiBackend(this));
    registerBackend(0);

//! [1]
    d->src = QGeoPositionInfoSource::createDefaultSource(this);

    if (d->src) {
        d->useGps = true;
        connect(d->src, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdated(QGeoPositionInfo)));
        connect(d->src, SIGNAL(errorOccurred(QGeoPositionInfoSource::Error)),
                this, SLOT(positionError(QGeoPositionInfoSource::Error)));
        d->src->startUpdates();
    } else {
        d->useGps = false;
        d->city = "Brisbane";
        emit cityChanged();
        requestWeatherByCity();
    }
}
//! [1]

AppModel::~AppModel()
{
    if (d->src)
        d->src->stopUpdates();
    if (d->fcProp)
        delete d->fcProp;
    foreach (WeatherData *inf, d->forecast)
        delete inf;
    d->forecast.clear();
    delete d;
}

//! [2]
void AppModel::positionUpdated(QGeoPositionInfo gpsPos)
{
    d->coord = gpsPos.coordinate();

    if (!d->useGps)
        return;

    requestWeatherByCoordinates();
}
//! [2]

void AppModel::positionError(QGeoPositionInfoSource::Error e)
{
    Q_UNUSED(e);
    qWarning() << "Position source error. Falling back to simulation mode.";

    // activate simulation mode
    if (d->useGps) {
        d->useGps = false;
        d->city = "Brisbane";
        emit cityChanged();
        requestWeatherByCity();
    }
}

void AppModel::refreshWeather()
{
    if (d->city.isEmpty()) {
        qCDebug(requestsLog) << "refreshing weather skipped (no city)";
        return;
    }
    qCDebug(requestsLog) << "refreshing weather";
    requestWeatherByCity();
}

void AppModel::handleWeatherData(const LocationInfo &location,
                                 const QList<WeatherInfo> &weatherDetails)
{
    if (applyWeatherData(location.m_name, weatherDetails))
        d->m_dataCache.addCacheElement(location, weatherDetails);
}

void AppModel::switchToNextBackend()
{
    deregisterCurrentBackend();
    registerBackend(d->m_currentBackendIndex + 1);
    if (d->m_currentBackend) {
        // repeat the query
        if (d->useGps)
            requestWeatherByCoordinates();
        else
            requestWeatherByCity();
    } else {
        qWarning("The application has iterated through all of the weather backends, "
                 "and none of them seems to respond now. Please wait until any of the "
                 "backends becomes available again.");
    }
}

bool AppModel::applyWeatherData(const QString &city, const QList<WeatherInfo> &weatherDetails)
{
    // Check that we didn't get outdated weather data. The city should match,
    // if only we do not use GPS.
    if (city != d->city && !d->useGps)
        return false;

    if (city != d->city && d->useGps) {
        d->city = city;
        emit cityChanged();
    }

    // delete previous forecast
    foreach (WeatherData *inf, d->forecast)
        delete inf;
    d->forecast.clear();

    // The first item in the list represents current weather.
    if (!weatherDetails.isEmpty()) {
        d->now.setTemperature(weatherDetails.first().m_temperature);
        d->now.setWeatherIcon(weatherDetails.first().m_weatherIconId);
        d->now.setWeatherDescription(weatherDetails.first().m_weatherDescription);
    }

    // The other items represent weather forecast. The amount of items depends
    // on the provider backend.
    for (qsizetype i = 1; i < weatherDetails.size(); ++i) {
        WeatherData *forecastEntry = new WeatherData(weatherDetails.at(i));
        d->forecast.append(forecastEntry);
    }

    if (!d->ready) {
        d->ready = true;
        emit readyChanged();
    }

    emit weatherChanged();

    return true;
}

void AppModel::requestWeatherByCoordinates()
{
    const auto cacheResult = d->m_dataCache.getWeatherData(d->coord);
    if (WeatherDataCache::isCacheResultValid(cacheResult))
        applyWeatherData(cacheResult.first, cacheResult.second);
    else if (d->m_currentBackend)
        d->m_currentBackend->requestWeatherInfo(d->coord);
}

void AppModel::requestWeatherByCity()
{
    const auto cacheResult = d->m_dataCache.getWeatherData(d->city);
    if (WeatherDataCache::isCacheResultValid(cacheResult))
        applyWeatherData(cacheResult.first, cacheResult.second);
    else if (d->m_currentBackend)
        d->m_currentBackend->requestWeatherInfo(d->city);
}

void AppModel::registerBackend(qsizetype index)
{
    if (index >= 0 && index < d->m_supportedBackends.size()) {
        d->m_currentBackend = d->m_supportedBackends.at(index);
        d->m_currentBackendIndex = index;
        connect(d->m_currentBackend, &ProviderBackend::weatherInformation,
                this, &AppModel::handleWeatherData);
        connect(d->m_currentBackend, &ProviderBackend::errorOccurred,
                this, &AppModel::switchToNextBackend);
    }
}

void AppModel::deregisterCurrentBackend()
{
    if (d->m_currentBackend) {
        disconnect(d->m_currentBackend, &ProviderBackend::weatherInformation,
                   this, &AppModel::handleWeatherData);
        disconnect(d->m_currentBackend, &ProviderBackend::errorOccurred,
                   this, &AppModel::switchToNextBackend);
        d->m_currentBackend = nullptr;
    }
}

bool AppModel::hasValidCity() const
{
    return (!(d->city.isEmpty()) && d->city.size() > 1 && d->city != "");
}

bool AppModel::hasValidWeather() const
{
    return hasValidCity() && (!(d->now.weatherIcon().isEmpty()) &&
                              (d->now.weatherIcon().size() > 1) &&
                              d->now.weatherIcon() != "");
}

WeatherData *AppModel::weather() const
{
    return &(d->now);
}

QQmlListProperty<WeatherData> AppModel::forecast() const
{
    return *(d->fcProp);
}

bool AppModel::ready() const
{
    return d->ready;
}

bool AppModel::hasSource() const
{
    return (d->src != NULL);
}

bool AppModel::useGps() const
{
    return d->useGps;
}

void AppModel::setUseGps(bool value)
{
    d->useGps = value;
    if (value) {
        d->city = "";
        emit cityChanged();
        emit weatherChanged();
        // if we already have a valid GPS position, do not wait until it
        // updates, but query the city immediately
        if (d->coord.isValid())
            requestWeatherByCoordinates();
    }
    emit useGpsChanged();
}

QString AppModel::city() const
{
    return d->city;
}

void AppModel::setCity(const QString &value)
{
    d->city = value;
    emit cityChanged();
    requestWeatherByCity();
}
