#pragma once

#include <QtCore>

#include <RInside.h>

namespace wblanes {

class WBLanes : public QObject
{
  Q_OBJECT

public:
  QObject* img;
public slots:
  void onAreaSelected(double x, double y, double width, double height);
};

} // namespace wblanes
