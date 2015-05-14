#include "wblanes.h"
#include "../necomi/necomi/necomi.h"

#include <QtGui>

namespace wblanes {

WBLanes::WBLanes(int argc, char* argv[])
  : m_R(argc, argv)
  , m_img(nullptr), m_plot(nullptr)
{
  // Load ggplot2
  // TODO: try/catch for missing library (std::runtime_error)
  m_R.parseEvalQ("library(ggplot2)\n");
}

void WBLanes::setImageItem(QObject* item)
{
  m_img = item;
  connect(item, SIGNAL(areaSelected(double,double,double,double)),
	  this, SLOT(onAreaSelected(double,double,double,double)));
}

void WBLanes::setPlotItem(QObject* item)
{
  m_plot = item;
}

void WBLanes::setRatioLabelItem(QObject* item)
{
  m_ratio_label = item;
}

void WBLanes::onAreaSelected(double rx, double ry,
			     double rwidth, double rheight)
{
  if (m_img == nullptr) {
    qWarning() << "no image loaded, aborting selection analysis";
    return;
  }

  // Get image source URL
  auto source = m_img->property("source").toUrl();
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

  // Make sure the selection is inside the image
  qDebug() << "selection: " << rwidth << "×" << rheight << "at" << rx << ry;
  if (rx < 0) {
    rwidth += rx;
    rx = 0;
  }
  if (ry < 0) {
    rheight += ry;
    ry = 0;
  }
  rwidth = std::min(rwidth, img.width() - rx);
  rheight = std::min(rheight, img.height() - ry);
  qDebug() << "selecting: " << rwidth << "×" << rheight << "at" << rx << ry;
  
  auto x = static_cast<std::size_t>(rx);
  auto y = static_cast<std::size_t>(ry);
  auto width = static_cast<std::size_t>(rwidth);
  auto height = static_cast<std::size_t>(rheight);
  
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
  try {
    m_R.parseEvalQ("xs <- seq(0, length(ys)-1)\n"
		   "df <- data.frame(x=xs, y=ys)\n"
		   "fit <- nls(y ~ (off + c1 * exp(-(x-mu1)**2/(2*sg1**2))"
		   "                    + c2 * exp(-(x-mu2)**2/(2*sg2**2))),"
		   "           data=df,"
		   "           start=list(off = 90, c1=120, mu1=30, sg1=10,"
		   "                      c2=120, mu2=60, sg2=10),"
		   "           algorithm='port')\n");
  } catch (std::runtime_error e) {
    // TODO: check reason
    // TODO: clear plot
    qWarning() << "could not fit the data";
    return;
  }
  m_R.parseEvalQ("print(summary(fit))");

  // Generate fitted data to be plotted
  m_R.parseEvalQ("xss <- seq(0, length(ys-1), 0.25)\n"
		 "k <- coef(fit)\n"
		 "fitdat <- data.frame(x=xss)\n"
		 "fitdat$y <- predict(fit, newdata=fitdat)\n"
		 "fitg1 <- data.frame(x=xss)\n"
		 "fitg1$y <- k[['off']] + k[['c1']] * exp(-(xss-k[['mu1']])**2/(2*k[['sg1']]**2))\n"
		 "fitg2 <- data.frame(x=xss)\n"
		 "fitg2$y <- k[['off']] + k[['c2']] * exp(-(xss-k[['mu2']])**2/(2*k[['sg2']]**2))\n");

  // Get the areas and display them
  double area1 = m_R.parseEval("k[['c1']]*abs(k[['sg1']])*sqrt(pi)");
  double area2 = m_R.parseEval("k[['c2']]*abs(k[['sg2']])*sqrt(pi)");
  auto ratio_fmt = QString("A1=%1 A2=%2 A1/(A1+A2)=%3% [%4%]").arg(area1).arg(area2).arg(100.*area1/(area1+area2)).arg(100.*(1.-area1/(area1+area2)));
  qDebug() << ratio_fmt;
  if (m_ratio_label != nullptr)
    m_ratio_label->setProperty("text", ratio_fmt);

  // Generate a new name for the plot
  // TODO: erase previous plotfiles
  auto plotfile = QString("fit-%1.svg").arg(qrand());
  
  // Plot the data in an SVG file
  m_R.parseEvalQ(QString("plt <- ggplot(df, aes(x=x, y=y)) + geom_point()"
			 "         + geom_smooth(data=fitdat, stat='identity', size=1.5)"
			 "         + geom_smooth(data=fitg1, stat='identity')"
			 "         + geom_smooth(data=fitg2, stat='identity')\n"
			 "ggsave(file='%1', plot=plt, width=10, height=8)\n").arg(plotfile).toLocal8Bit().constData());

  // Convert the result to SVG Tiny

  // Display the plot
  plotSvg(plotfile);
}

void WBLanes::plotSvg(const QString& path)
{
  if (m_plot == nullptr) {
    qWarning() << "plot item not found, aborting result display";
    return;
  }

  m_plot->setProperty("source", QUrl());
  m_plot->setProperty("source", QUrl::fromLocalFile(path));
}

} // namespace wblanes

