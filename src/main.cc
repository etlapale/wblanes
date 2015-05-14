// src/main.cc – Program entry point
//
// Copyright © 2015 Émilien Tlapale
// Licensed under the Simplified BSD License.

#include <QtQuick>

int main(int argc, char* argv[])
{
  QGuiApplication app(argc, argv);

  // Load the QML UI description
  auto eng = new QQmlApplicationEngine;
  eng->load(QUrl::fromLocalFile("qml/wblanes.qml"));

  // Show the top level window
  auto win = qobject_cast<QQuickWindow*>(eng->rootObjects().value(0));
  win->show();

  return app.exec();
}
