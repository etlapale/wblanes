#include "wblanes.h"
#include "../necomi/necomi/necomi.h"

#include <QtGui>

namespace wblanes {

void WBLanes::onAreaSelected(double x, double y, double width, double height)
{
  qDebug() << "area of" << width << "×" << height << "selected";
  if (img == nullptr) {
    qWarning() << "no image loaded, aborting selection analysis";
    return;
  }

  // Get image source URL
  auto source = img->property("source").toUrl();
  if (! source.isLocalFile()) {
    qWarning() << "image is not local, aborting selection analysis";
    return;
  }

  // Load the image data from file
  QImage img(source.toLocalFile());
  qDebug() << "image of " << img.width() << "×" << img.height()
	   << "loaded, format is" << img.format();

  if (img.format != QImage::Format_RGB32
      && img.format != QImage::Format_ARGB32) {
    qWarning() << "invalid image format, aborting selection analysis";
    return;
  }

  
}

} // namespace wblanes
