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
  eng->load(QUrl::fromLocalFile("qml/wblanes.qml"));

  // Show the top level window
  auto win = qobject_cast<QQuickWindow*>(eng->rootObjects().value(0));
  win->show();

  auto wblanes = new WBLanes;
  
  // Get the image
  auto img = win->findChild<QObject*>("image");
  wblanes->img = img;
  
  QObject::connect(img, SIGNAL(areaSelected(double,double,double,double)),
		   wblanes, SLOT(onAreaSelected(double,double,double,double)));

  return app.exec();
}
