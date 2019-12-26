.. Copyright (c) 2018, Johan Mabille, Sylvain Corlay and Wolf Vollprecht
   and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.


.. raw:: html

   <style>
   .rst-content .section>img {
       width: 30px;
       margin-bottom: 0;
       margin-top: 0;
       margin-right: 15px;
       margin-left: 15px;
       float: left;
   }
   </style>

Installation
============

Although ``xframe`` is a header-only library, we provide standardized means to
install it, with package managers or with cmake.

Besides the xframe headers, all these methods place the ``cmake`` project
configuration file in the right location so that third-party projects can use
cmake's ``find_package`` to locate xframe headers.

.. image:: conda.svg

Using the conda package
-----------------------

A package for xframe is available on the conda package manager.

.. code::

    conda install -c conda-forge xframe

.. image:: cmake.svg

From source with cmake
----------------------

You can also install ``xframe`` from source with cmake. This requires that you
have the xtensor_ library installed on your system. On Unix platforms,
from the source directory:

.. code::

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=path_to_prefix ..
    make install

On Windows platforms, from the source directory:

.. code::

    mkdir build
    cd build
    cmake -G "NMake Makefiles" -DCMAKE_INSTALL_PREFIX=path_to_prefix ..
    nmake
    nmake install

``path_to_prefix`` is the absolute path to the folder where cmake searches for
dependencies and installs libraries. ``xframe`` installation from cmake assumes
this folder contains ``include`` and ``lib`` subfolders.

Including xframe in your project
--------------------------------

The different packages of ``xframe`` are built with cmake, so whatever the
installation mode you choose, you can add ``xframe`` to your project using cmake:

.. code::

    find_package(xframe REQUIRED)
    target_include_directories(your_target PUBLIC ${xframe_INCLUDE_DIRS})
    target_link_libraries(your_target PUBLIC xframe)

.. _xtensor: https://github.com/xtensor-stack/xtensor
