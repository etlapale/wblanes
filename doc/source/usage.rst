Usage
=====

Using :doc:`wblanes <index>` is quite straightforward: first load an
image, then select a rectangle from which two bands shall be
quantified.

.. image:: images/screenshot-empty.png
   :scale: 60%
	
To load an image, press the *Load* button which will open a standard
file selector.

.. image:: images/screenshot-loaded.png
   :scale: 60%

Once your image is displayed in the top portion of the software, use
the mouse to draw a rectangle around the region of interest. When the
selected region is defined, the software will perform a regression
analysis to fit a sum of two Gaussians to the bands.

.. image:: images/screenshot-selection.png
   :scale: 60%

If the fit is successful, a plot and quantitative values will appear
on the lower part of the interface.

.. image:: images/wblanes-fit.png
   :scale: 45%

The status bar indicates the value
of the area under the first curve *A1*, under the second curve *A2*,
and the ratio of the first over the total area in percent (and its
complement under brackets).

.. image:: images/screenshot-statusbar.png
   :scale: 70%

The first curve always represents the upper most band in the
original image.
      
To select a new region of interest, simply drag the existing selection
or draw another rectangle on a different part of the image.
