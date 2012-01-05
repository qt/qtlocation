/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "navigator.h"

#include <QtWidgets/QCoreApplication>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QAction>
#include <QtWidgets/QVBoxLayout>

MainWindow::MainWindow() :
    serviceProvider(0),
    markerManager(0),
    positionSource(0),
    lastNavigator(0),
    tracking(true),
    firstUpdate(true)
{
    // our actual maps widget is the centre of the mainwindow
    mapsWidget = new MapsWidget;
    setCentralWidget(mapsWidget);

    // set up the menus
    QMenuBar *mbar = new QMenuBar(this);
    mbar->addAction("Quit", qApp, SLOT(quit()));
    mbar->addAction("My Location", this, SLOT(goToMyLocation()));

    QMenu *searchMenu = new QMenu("Search");
    mbar->addMenu(searchMenu);

    searchMenu->addAction("For address or name", this, SLOT(showSearchDialog()));

    QMenu *navigateMenu = new QMenu("Directions");
    mbar->addMenu(navigateMenu);

    navigateMenu->addAction("From here to address", this, SLOT(showNavigateDialog()));

    setMenuBar(mbar);
    setWindowTitle("Maps Demo");

    // now begin the process of opening the network link
    netConfigManager = new QNetworkConfigurationManager;
    connect(netConfigManager, SIGNAL(updateCompleted()),
            this, SLOT(openNetworkSession()));
    netConfigManager->updateConfigurations();
}

void MainWindow::openNetworkSession()
{
    // use the default network configuration and make sure that
    // the link is open
    session = new QNetworkSession(netConfigManager->defaultConfiguration());
    if (session->isOpen()) {
        initialize();
    } else {
        connect(session, SIGNAL(opened()),
                this, SLOT(initialize()));
        session->open();
    }
}

MainWindow::~MainWindow()
{
    delete mapsWidget;
    if (serviceProvider)
        delete serviceProvider;
    if (markerManager)
        delete markerManager;
}

void MainWindow::goToMyLocation()
{
    mapsWidget->animatedPanTo(markerManager->myLocation());
    mapsWidget->map()->setFocus();
    tracking = true;
}

void MainWindow::initialize()
{
    if (serviceProvider)
        delete serviceProvider;

    // check we have a valid default provider

    QList<QString> providers = QGeoServiceProvider::availableServiceProviders();
    if (providers.size() < 1) {
        QMessageBox::information(this, tr("Maps Demo"),
                                 tr("No service providers are available"));
        QCoreApplication::quit();
        return;
    }

    foreach (QString provider, providers) {
        serviceProvider = new QGeoServiceProvider(provider);
        if (serviceProvider->mappingManager() &&
                serviceProvider->geocodingManager() &&
                serviceProvider->routingManager())
            break;
    }

    if (serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("Maps Demo"),
                                 tr("Error loading geoservice plugin"));
        QCoreApplication::quit();
        return;
    }

    if (!serviceProvider->mappingManager() ||
            !serviceProvider->geocodingManager() ||
            !serviceProvider->routingManager()) {
        QMessageBox::information(this, tr("Maps Demo"),
                                 tr("No geoservice found with mapping/search/routing"));
        QCoreApplication::quit();
        return;
    }

    // start initialising things (maps, searching, routing)

    mapsWidget->initialize(serviceProvider->mappingManager());

    if (markerManager)
        delete markerManager;
    markerManager = new MarkerManager(serviceProvider->geocodingManager());
    mapsWidget->setMarkerManager(markerManager);

    connect(markerManager, SIGNAL(searchError(QGeocodeReply::Error,QString)),
            this, SLOT(showErrorMessage(QGeocodeReply::Error,QString)));
    connect(mapsWidget, SIGNAL(markerClicked(Marker*)),
            this, SLOT(showMarkerDialog(Marker*)));
    connect(mapsWidget, SIGNAL(mapPanned()),
            this, SLOT(disableTracking()));

    if (positionSource)
        delete positionSource;

    // set up position feeds (eg GPS)

    positionSource = QGeoPositionInfoSource::createDefaultSource(this);

    if (!positionSource) {
        mapsWidget->statusBar()->showText("Could not open GPS", 5000);
        mapsWidget->setMyLocation(QGeoCoordinate(-27.5796, 153.1));
        //mapsWidget->setMyLocation(QGeoCoordinate(21.1813, -86.8455));
    } else {
        positionSource->setUpdateInterval(1000);
        connect(positionSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(updateMyPosition(QGeoPositionInfo)));
        positionSource->startUpdates();
        mapsWidget->statusBar()->showText("Opening GPS...");
    }
}

void MainWindow::disableTracking()
{
    tracking = false;
}

void MainWindow::updateMyPosition(QGeoPositionInfo info)
{
    if (mapsWidget) {
        mapsWidget->setMyLocation(info.coordinate(), false);
        if (tracking)
            mapsWidget->animatedPanTo(info.coordinate());
        if (firstUpdate)
            mapsWidget->statusBar()->showText("Receiving from GPS");
    }

    firstUpdate = false;
}

void MainWindow::showNavigateDialog()
{
    NavigateDialog nd;
    if (nd.exec() == QDialog::Accepted) {
        if (markerManager) {
            QGeoRouteRequest req;

            req.setTravelModes(nd.travelMode());

            // tell the old navigator instance to delete itself
            // so that its map objects will disappear
            if (lastNavigator)
                lastNavigator->deleteLater();

            Navigator *nvg = new Navigator(serviceProvider->routingManager(),
                                           serviceProvider->geocodingManager(),
                                           mapsWidget, nd.destinationAddress(),
                                           req);

            lastNavigator = nvg;

            connect(nvg, SIGNAL(searchError(QGeocodeReply::Error,QString)),
                    this, SLOT(showErrorMessage(QGeocodeReply::Error,QString)));
            connect(nvg, SIGNAL(routingError(QGeoRouteReply::Error,QString)),
                    this, SLOT(showErrorMessage(QGeoRouteReply::Error,QString)));

            mapsWidget->statusBar()->setText("Routing...");
            mapsWidget->statusBar()->show();

            nvg->start();

            connect(nvg, SIGNAL(finished()),
                    mapsWidget->statusBar(), SLOT(hide()));

            mapsWidget->map()->setFocus();
        }
    }
}

void MainWindow::showSearchDialog()
{
    SearchDialog sd;
    if (sd.exec() == QDialog::Accepted) {
        if (markerManager) {
            markerManager->removeSearchMarkers();
            markerManager->search(sd.searchTerms(), sd.radius());
            mapsWidget->map()->setFocus();
        }
    }
}

void MainWindow::showErrorMessage(QGeocodeReply::Error err, QString msg)
{
    Q_UNUSED(err)
    QMessageBox::critical(this, tr("Error"), msg);
    mapsWidget->statusBar()->hide();
    mapsWidget->map()->setFocus();
}

void MainWindow::showErrorMessage(QGeoRouteReply::Error err, QString msg)
{
    Q_UNUSED(err)
    QMessageBox::critical(this, tr("Error"), msg);
    mapsWidget->statusBar()->hide();
    mapsWidget->map()->setFocus();
}

void MainWindow::showMarkerDialog(Marker *marker)
{
    MarkerDialog md(marker);
    if (md.exec() == QDialog::Accepted) {
        md.updateMarker();
        mapsWidget->map()->setFocus();
    }
}
