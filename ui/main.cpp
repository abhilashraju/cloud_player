#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "device_capabilities_qt.hpp"
#include "core/scloud_player.h"
#include <QQuickView>
#include "sipslistmodel.h"
#include "datasource.h"
using QtPlayer = SCloudPlayer<qt_device_capabilities>;
int main(int argc, char** argv) {
    QGuiApplication app(argc, argv);
    qmlRegisterType<StackNotifier>("SipsControls", 1, 0, "StackNotifier");
    qmlRegisterType<SipsListMdodel>("SipsControls", 1, 0, "CloudItemModel");
    qmlRegisterUncreatableType<ModelItems>("SipsControls",1,0,"ModelItems","Cannot create ModelItems in qml");
    std::unique_ptr<QtPlayer> rootPlayer{ std::make_unique<QtPlayer>()};
    rootPlayer->start(qt_device_capabilities::getRootUrl());
    return app.exec();
}
