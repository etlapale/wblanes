#pragma once

#include <QtCore>

#include <RInside.h>

namespace wblanes {

class WBLanes : public QObject
{
  Q_OBJECT
  
public:
  WBLanes(int argc, char* argv[]);
  void setImageItem(QObject* item);
  void setPlotItem(QObject* item);
  void setRatioLabelItem(QObject* item);
				 
public slots:
  void onAreaSelected(double x, double y, double width, double height);
  
protected:
  RInside m_R;
  
  QObject* m_img;
  QObject* m_plot;
  QObject* m_ratio_label;

  void plotSvg(const QString& path);
};

} // namespace wblanes
