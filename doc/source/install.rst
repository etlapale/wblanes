Installation
============

Although wblanes is in itself a cross-platform software able to run on
various operating system, no binary package is currently available:
you have to build it from source.

Requirements
------------

- Qt5_ for the graphical interface. If your operating system splits
  Qt in multiple package, you need to install at least the Core, Gui,
  Qml and Quick modules, as well as the controls, dialogs and layouts
  QML modules. Make sure you install the image formats plugin if you
  require TIFF support for instance.

- R_, the statistical language, for the non-linear regression and
  plotting. You must install the following packages in R: ggplot2_,
  RInside_ and RCpp_.

Installing packages in R can be done from its interactive prompt.

.. code-block:: console

   $ R
   
   R version 3.1.1 (2014-07-10) -- "Sock it to Me"
   Copyright (C) 2014 The R Foundation for Statistical Computing
   […]

   > install.packages('ggplot2')
   Installing package into ‘/home/neil/R/x86_64-pc-linux-gnu-library/3.1’
   (as ‘lib’ is unspecified)
   --- Please select a CRAN mirror for use in this session ---

At that point a dialog opens asking you to select a mirror to download
the package from. Just select one close to your location and press
OK. Repeat the last command, ``install.package('…')`` for ``RCpp``
and ``RInside``.
  
Additionally, to build and install wblanes, you will need a recent C++
compiler, such as clang_ or GCC_, the build software CMake_, and Git_
to get the sources.  

Building
--------

First, download the sources, either a released version or the Git_
repository:

.. code-block:: console

   $ git clone https://git.atelo.org/etlapale/wblanes.git

Then, move to the source directory and run ``cmake`` to generate a
Makefile. You may want to pass specific options to ``cmake``, for
instance to specify the C++ compiler. Once the Makefiles have been
generated, use ``make`` to compile and link the final program:

.. code-block:: console
   
   $ cd wblanes
   $ cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ .
   $ make

If successful, the last command will generate an executable named
``wblanes`` which you can run to start the program.


.. _clang: http://clang.llvm.org
.. _CMake: http://cmake.org
.. _GCC: http://gcc.gnu.org
.. _ggplot2: http://ggplot2.org
.. _Git: http://git-scm.com
.. _Qt5: http://qt.io
.. _R: http://r-project.org
.. _RCpp: http://dirk.eddelbuettel.com/code/rcpp.html
.. _RInside: http://dirk.eddelbuettel.com/code/rinside.html
