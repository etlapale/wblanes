#include "wblanes.h"
#include "../necomi/necomi/necomi.h"

#include <QtGui>

namespace wblanes {

WBLanes::WBLanes(int argc, char* argv[])
  : m_R(argc, argv)
{
}

void WBLanes::onAreaSelected(double rx, double ry,
			     double rwidth, double rheight)
{
  auto x = static_cast<std::size_t>(rx);
  auto y = static_cast<std::size_t>(ry);
  auto width = static_cast<std::size_t>(rwidth);
  auto height = static_cast<std::size_t>(rheight);
  
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

  if (img.format() != QImage::Format_RGB32
      && img.format() != QImage::Format_ARGB32) {
    qWarning() << "invalid image format, aborting selection analysis";
    return;
  }
  if (img.byteCount() != 4*img.width()*img.height()) {
    qWarning() << "unexpected image byte count, got" << img.byteCount()
	       << "expecting" << 4*img.width()*img.height();
    return;
  }
  
  // Create a Necomi array for the image
  using namespace necomi;
  
  const auto bits = img.constBits();
  StridedArray<const uchar,3> a(bits, [](const uchar*){},
				img.height(), img.width(), 4);

  // Convert the array to inverted grayscale
  std::array<std::size_t,2> dims = {static_cast<std::size_t>(img.height()),
				    static_cast<std::size_t>(img.width())};
  auto dgray = make_delayed<double,2>(dims, [&a](const auto& coords) {
					double b = a(coords[0],coords[1],0);
					double g = a(coords[0],coords[1],1);
					double r = a(coords[0],coords[1],2);
					return 255 - (r+g+b)/3;
				      });
  auto gray = strided_array(dgray);

  // Create a view on the selection
  auto selection = gray.slice((slice(y,height),slice(x,width)));

  // Average to a single line
  auto line = average(selection, 1);
  
  using namespace necomi::streams;
  std::cout << line << std::endl;

  // Pass the data to R
  Rcpp::NumericVector rys(line.dim(0));
  for (auto i = 0UL; i < line.dim(0); i++)
    rys(i) = line(i);
  m_R["ys"] = rys;

  // Run the fit code
  m_R.parseEvalQ("xs <- seq(0, length(ys)-1)\n"
		 "df <- data.frame(x=xs, y=ys)\n"
		 "fit <- nls(y ~ (off + c1 * exp(-(x-mu1)**2/(2*sg1**2))"
		 "                    + c2 * exp(-(x-mu2)**2/(2*sg2**2))),"
		 "           data=df,"
		 "           start=list(off = 90, c1=120, mu1=30, sg1=10,"
		 "                      c2=120, mu2=60, sg2=10),"
		 "           algorithm='port')\n");
  m_R.parseEvalQ("print(summary(fit))");
  
  //m_R["ys"] = vline;
  
  //m_R.parseEvalQ("print(ys); print(class(ys));");
  
  //std::cout << selection.dims() << std::endl
  //<< selection.strides() << std::endl;
  //qDebug() << gray(0,0);
}

} // namespace wblanes

