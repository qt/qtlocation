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

#include "qgeoroutingmanager.h"
#include "qgeoroutingmanager_p.h"
#include "qgeoroutingmanagerengine.h"

#include <QLocale>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoRoutingManager

    \brief The QGeoRoutingManager class provides support for geographic routing
    operations.


    \inmodule QtLocation
    \since 1.1

    \ingroup maps-routing

    The calculateRoute() and updateRoute() methods function QGeoRouteReply
    objects, which manage these operations and report on the result of the
    operations and any errors which may have occurred.

    The calculateRoute() function is used to find a route (or routes) that
    follows a set of waypoints and matches various other criteria.  The
    QGeoRouteRequest class is used to specify this information.

    If supportsRouteUpdates() returns true then the QGeoRoutingManager
    supports updating route information based on position updates.  This
    will cause the travel time and distance estimates to be updated, and
    any QGeoRouteSegments already traversed to be removed from the route.

    The updates can be triggered with the updateRoute() function, which makes
    use of the QGeoPositionInfo instances emitted as position updates by
    QGeoPositionInfoSource.

    Instances of QGeoRoutingManager can be accessed with
    QGeoServiceProvider::routingManager().

    A small example of the usage of QGeoRoutingManager and QGeoRouteRequests
    follows:

    \code
class MyRouteHandler : public QObject
{
    Q_OBJECT
public:
    MyRouteHandler(QGeoRoutingManager *routingManager,
                   const QGeoCoordinate &origin,
                   const QGeoCoordinate &destination) {

        QGeoRouteRequest request(origin, destination);

        // The request defaults to the fastest route by car, which is
        // equivalent to:
        // request.setTravelMode(QGeoRouteRequest::CarTravel);
        // request.setRouteOptimization(QGeoRouteRequest::FastestRoute);

        request.setAvoidFeatureTypes(QGeoRouteRequest::AvoidTolls);
        request.setAvoidFeatureTypes(QGeoRouteRequest::AvoidMotorPoolLanes);

        QGeoRouteRequest::AvoidFeaturesTypes avoidableFeatures = routingManager->supportedAvoidFeatureTypes();

        if (!(avoidableFeatures & request.avoidFeatureTypes())) {
            // ... inform the user that the routing manager does not
            // provide support for avoiding tolls and/or motor pool lanes ...
            return;
        }

        QGeoRouteReply *reply = routingManager->calculateRoute(request);

        if (reply->isFinished()) {
            if (reply->error() == QGeoRouteReply::NoError) {
                routeCalculated(reply);
            } else {
                routeError(reply, reply->error(), reply->errorString());
            }
            return;
        }

        connect(routingManager,
                SIGNAL(finished(QGeoRouteReply*)),
                this,
                SLOT(routeCalculated(QGeoRouteReply*)));

        connect(routingManager,
                SIGNAL(error(QGeoRouteReply*,QGeoRouteReply::Error,QString)),
                this,
                SLOT(routeError(QGeoRouteReply*,QGeoRouteReply::Error,QString)));
    }

private slots:
    void routeCalculated(QGeoRouteReply *reply)
    {
        // A route request can ask for several alternative routes ...
        if (reply->routes().size() != 0) {

            // ... but by default it will only get a single route
            QGeoRoute route = reply->routes().at(0);

            //... now we have to make use of the route ...
        }

        reply->deleteLater();
    }

    void routeError(QGeoRouteReply *reply, QGeoRouteReply:Error error, const QString &errorString)
    {
        // ... inform the user that an error has occurred ...
        reply->deleteLater();
    }
};
    \endcode
*/

/*!
    Constructs a new manager with the specified \a parent and with the
    implementation provided by \a engine.

    This constructor is used internally by QGeoServiceProviderFactory. Regular
    users should acquire instances of QGeoRoutingManager with
    QGeoServiceProvider::routingManager();
*/
QGeoRoutingManager::QGeoRoutingManager(QGeoRoutingManagerEngine *engine, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoRoutingManagerPrivate())
{
    d_ptr->engine = engine;
    if (d_ptr->engine) {
        d_ptr->engine->setParent(this);

        connect(d_ptr->engine,
                SIGNAL(finished(QGeoRouteReply*)),
                this,
                SIGNAL(finished(QGeoRouteReply*)));

        connect(d_ptr->engine,
                SIGNAL(error(QGeoRouteReply*, QGeoRouteReply::Error, QString)),
                this,
                SIGNAL(error(QGeoRouteReply*, QGeoRouteReply::Error, QString)));
    } else {
        qFatal("The routing manager engine that was set for this routing manager was NULL.");
    }
}

/*!
    Destroys this manager.
*/
QGeoRoutingManager::~QGeoRoutingManager()
{
    delete d_ptr;
}

/*!
    Returns the name of the engine which implements the behaviour of this
    routing manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
QString QGeoRoutingManager::managerName() const
{
    return d_ptr->engine->managerName();
}

/*!
    Returns the version of the engine which implements the behaviour of this
    routin manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
int QGeoRoutingManager::managerVersion() const
{
    return d_ptr->engine->managerVersion();
}

/*!
    Begins the calculation of the route specified by \a request.

    A QGeoRouteReply object will be returned, which can be used to manage the
    routing operation and to return the results of the operation.

    This manager and the returned QGeoRouteReply object will emit signals
    indicating if the operation completes or if errors occur.

    Once the operation has completed, QGeoRouteReply::routes can be used to
    retrieve the calculated route or routes.

    If \a request includes features which are not supported by this manager, as
    reported by the methods in this manager, then a
    QGeoRouteReply::UnsupportedOptionError will occur.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoRoutingManager::finished(),
    QGeoRoutingManager::error(), QGeoRouteReply::finished() or
    QGeoRouteReply::error() with deleteLater().
*/
QGeoRouteReply* QGeoRoutingManager::calculateRoute(const QGeoRouteRequest& request)
{
    return d_ptr->engine->calculateRoute(request);
}

/*!
    Begins the process of updating \a route based on the current position \a
    position.

    A QGeoRouteReply object will be returned, which can be used to manage the
    routing operation and to return the results of the operation.

    This manager and the returned QGeoRouteReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsRouteUpdates() returns false an
    QGeoRouteReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeoRouteReply::routes can be used to
    retrieve the updated route.

    The returned route could be entirely different to the original route,
    especially if \a position is far away from the initial route.
    Otherwise the route will be similar, although the remaining time and
    distance will be updated and any segments of the original route which
    have been traversed will be removed.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoRoutingManager::finished(),
    QGeoRoutingManager::error(), QGeoRouteReply::finished() or
    QGeoRouteReply::error() with deleteLater().
*/
QGeoRouteReply* QGeoRoutingManager::updateRoute(const QGeoRoute &route, const QGeoCoordinate &position)
{
    return d_ptr->engine->updateRoute(route, position);
}

/*!
    Returns whether this manager supports updating routes.
*/
bool QGeoRoutingManager::supportsRouteUpdates() const
{
    return d_ptr->engine->supportsRouteUpdates();
}

/*!
    Returns whether this manager supports request for alternative routes.
*/
bool QGeoRoutingManager::supportsAlternativeRoutes() const
{
    return d_ptr->engine->supportsAlternativeRoutes();
}

/*!
    Returns whether this engine supports the exclusion of areas from routes.
*/
bool QGeoRoutingManager::supportsExcludeAreas() const
{
    return d_ptr->engine->supportsExcludeAreas();
}

/*!
    Returns the travel modes supported by this manager.
*/
QGeoRouteRequest::TravelModes QGeoRoutingManager::supportedTravelModes() const
{
    return d_ptr->engine->supportedTravelModes();
}

/*!
    Returns the types of features that this manager can take into account
    during route planning.
*/
QGeoRouteRequest::FeatureTypes QGeoRoutingManager::supportedFeatureTypes() const
{
    return d_ptr->engine->supportedFeatureTypes();
}

/*!
    Returns the weightings which this manager can apply to different features
    during route planning.
*/
QGeoRouteRequest::FeatureWeights QGeoRoutingManager::supportedFeatureWeights() const
{
    return d_ptr->engine->supportedFeatureWeights();
}

/*!
    Returns the route optimizations supported by this manager.
*/
QGeoRouteRequest::RouteOptimizations QGeoRoutingManager::supportedRouteOptimizations() const
{
    return d_ptr->engine->supportedRouteOptimizations();
}

/*!
    Returns the levels of detail for routing segments which can be requested
    with this manager.
*/
QGeoRouteRequest::SegmentDetails QGeoRoutingManager::supportedSegmentDetails() const
{
    return d_ptr->engine->supportedSegmentDetails();
}

/*!
    Returns the levels of detail for navigation maneuvers which can be
    requested by this manager.
*/
QGeoRouteRequest::ManeuverDetails QGeoRoutingManager::supportedManeuverDetails() const
{
    return d_ptr->engine->supportedManeuverDetails();
}

/*!
    Sets the locale to be used by the this manager to \a locale.

    If this routing manager supports returning addresses and instructions
    in different languages, they will be returned in the language of \a locale.

    The locale used defaults to the system locale if this is not set.
*/
void QGeoRoutingManager::setLocale(const QLocale &locale)
{
    d_ptr->engine->setLocale(locale);
}

/*!
    Returns the locale used to hint to this routing manager about what
    language to use for addresses and instructions.
*/
QLocale QGeoRoutingManager::locale() const
{
    return d_ptr->engine->locale();
}

/*!
\fn void QGeoRoutingManager::finished(QGeoRouteReply* reply)

This signal is emitted when \a reply has finished processing.

If reply::error() equals QGeoRouteReply::NoError then the processing
finished successfully.

This signal and QGeoRouteReply::finished() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/

/*!
\fn void QGeoRoutingManager::error(QGeoRouteReply* reply, QGeoRouteReply::Error error, QString errorString)

This signal is emitted when an error has been detected in the processing of
\a reply.  The QGeoRoutingManager::finished() signal will probably follow.

The error will be described by the error code \a error.  If \a errorString is
not empty it will contain a textual description of the error.

This signal and QGeoRouteReply::error() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeoRoutingManagerPrivate::QGeoRoutingManagerPrivate()
    : engine(0) {}

QGeoRoutingManagerPrivate::~QGeoRoutingManagerPrivate()
{
    if (engine)
        delete engine;
}

#include "moc_qgeoroutingmanager.cpp"

QTM_END_NAMESPACE
