// src/main.cc – Program entry point
//
// Copyright © 2015 Émilien Tlapale
// Licensed under the Simplified BSD License.

#include <QtQuick>

#include "wblanes.h"
using namespace wblanes;


int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);
  
  // Load the QML UI description
  auto eng = new QQmlApplicationEngine;
  eng->load(QUrl("qrc:/qml/wblanes.qml"));

  // Show the top level window
  auto win = qobject_cast<QQuickWindow*>(eng->rootObjects().value(0));
  win->show();

  auto wblanes = new WBLanes(argc, argv);
  
  // Associate the QML items to the controller
  wblanes->setImageItem(win->findChild<QObject*>("image"));
  wblanes->setPlotItem(win->findChild<QObject*>("plot"));
  wblanes->setRatioLabelItem(win->findChild<QObject*>("ratioLabel"));

  // Cleanup on exit
  QObject::connect(&app, &QCoreApplication::aboutToQuit, [wblanes]() {
      delete wblanes;
    });

  return app.exec();
}
