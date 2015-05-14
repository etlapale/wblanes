#! /usr/bin/env python
# -*- coding: utf-8; -*-

from __future__ import print_function
import sys

from numpy import *
from scipy.optimize import curve_fit
from matplotlib.pyplot import *
from matplotlib.patches import Rectangle

import rpy2.robjects.lib.ggplot2 as ggplot2
import rpy2.robjects as robjects
from rpy2.robjects.packages import importr

class LaneSelector(object):
    def __init__(self, cb):
        self.ax = gca()
        self.rect = Rectangle((0,0), 1, 1, alpha=0.2, color='red')
        self.x0 = None
        self.y0 = None
        self.x1 = None
        self.y1 = None
        self.pressed = False
        self.cb = cb
        self.ax.add_patch(self.rect)
        self.ax.figure.canvas.mpl_connect('button_press_event', self.on_press)
        self.ax.figure.canvas.mpl_connect('button_release_event', self.on_release)
        self.ax.figure.canvas.mpl_connect('motion_notify_event', self.on_motion)

    def on_press(self, event):
        self.x0 = event.xdata
        self.y0 = event.ydata
        self.pressed = True

    def on_release(self, event):
        self.pressed = False
        if self.x0 != self.x1 and self.y0 != self.y1:
            self.cb(self.x0, self.y0, self.x1, self.y1)

    def on_motion(self, event):
        if self.pressed:
            self.x1 = event.xdata
            self.y1 = event.ydata
            self.rect.set_width(self.x1 - self.x0)
            self.rect.set_height(self.y1 - self.y0)
            self.rect.set_xy((self.x0, self.y0))
            self.ax.figure.canvas.draw()
            

def gaussian_sum(x, mu1, sg1, a1, mu2, sg2, a2, off):
    return off \
        + a1 * exp(-(x - float(mu1))**2/(2*float(sg1)**2)) \
        + a2 * exp(-(x - float(mu2))**2/(2*float(sg2)**2))
            
if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('usage: %s image'%sys.argv[0], file=sys.stderr)
        exit(1)

    rbase = importr('base')
    rstats = importr('stats')
        
    # Load the input image
    img = imread(sys.argv[1])

    # Compute the lanes
    def compute(x0, y0, x1, y1):
        # Selected square
        sel = 255 - average(img[y0:y1,x0:x1],2)
        # Average across x’s
        ysel = average(sel,1)
        line = ysel
        xs = mgrid[0:line.shape[0]]

        # Pass the data to R
        rxs = robjects.FloatVector(xs)
        rys = robjects.FloatVector(line)
        rdf = robjects.DataFrame({'x': rxs, 'y': rys})
        robjects.globalenv['xs'] = rxs
        robjects.globalenv['df'] = rdf
        #print(rys.r_repr())

        # Fir an R model
        robjects.r('''fit <- nls(y ~ (off + c1 * exp(-(x-mu1)**2/(2*sg1**2))
                                    + c2 * exp(-(x-mu2)**2/(2*sg2**2))),
                               data=df,
                               start=list(off = 90, c1=120, mu1=30, sg1=10,
                                   c2=120, mu2=60, sg2=10),
                               algorithm='port')''')

        # Get fit results
        robjects.r('''k <- coef(fit)
                      fitdat <- data.frame(x=xs)
                      fitdat$y <- predict(fit, newdata=fitdat)
                      ## Independent Gaussians
                      fitg1 <- data.frame(x=xs)
                      fitg1$y <- k[['off']] + k[['c1']] * exp(-(xs-k[['mu1']])**2/(2*k[['sg1']]**2))
                      fitg2 <- data.frame(x=xs)
                      fitg2$y <- k[['off']] + k[['c2']] * exp(-(xs-k[['mu2']])**2/(2*k[['sg2']]**2))''')
        
        # Plot R fits
        fitdat = robjects.globalenv['fitdat']
        fitg1 = robjects.globalenv['fitg1']
        fitg2 = robjects.globalenv['fitg2']
        pp = ggplot2.ggplot(rdf) \
             + ggplot2.aes_string(x='x', y='y') \
             + ggplot2.geom_point() \
             + ggplot2.geom_smooth(data=fitdat, stat="identity", size=1.5) \
             + ggplot2.geom_smooth(data=fitg1, stat="identity") \
             + ggplot2.geom_smooth(data=fitg2, stat="identity")
        pp.plot()

        # Compute the ratio of the gaussian integrals
        ratio = robjects.r('''k[['c1']]*k[['sg1']]/(k[['c1']]*k[['sg1']]+k[['c2']]*k[['sg2']])''')
        
        title('ratio='+str(ratio))
        show()
    
    # Show the image
    figure('image')
    imshow(img)

    # Create a selector
    sel = LaneSelector(compute)
    
    # Event loop
    show()
