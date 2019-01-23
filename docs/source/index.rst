.. Copyright (c) 2018, Johan Mabille, Sylvain Corlay, Wolf Vollprecht
   and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

.. image:: xframe.svg
   :alt: xframe

Multi-dimensional labeled arrays and data frame based on `xtensor`_.

Introduction
------------

`xframe` is a C++ library meant for numerical analysis with multi-dimensional
labeled array expressions (also referred as variable expressions) and data frame
expressions. It is built upon `xtensor`_ and provides similar features:

- an extensible expression system enabling **lazy broadcasting** based on dimension
  names.
- an API following the idioms of the C++ standard library.
- tools to manipulate variable expressions and build upon *xframe*.

The API of `xframe` is inspired by the ones of `pandas`_ and `xarray`_.

`xframe` requires a modern C++ compiler supporting C++14. The following C++
compilers are supported:

- On Windows platforms, Visual C++ 2015 Update 2, or more recent
- On Unix platforms, gcc 4.9 or a recent version of Clang

Licensing
---------

We use a shared copyright model that enables all contributors to maintain the
copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the LICENSE file
for details.


.. toctree::
   :caption: INSTALLATION
   :maxdepth: 1

   installation

.. toctree::
   :caption: USAGE
   :maxdepth: 2

   getting_started
   data_structure
   indexing
   computation

.. toctree::
   :caption: API REFERENCE
   :maxdepth: 2

   api/axis_index
   api/coordinate_index
   api/variable_index

.. toctree::
   :caption: MISCELLANEOUS

   xarray

.. _xtensor: https://github.com/QuantStack/xtensor
.. _pandas: https://pandas.pydata.org
.. _xarray: https://xarray.pydata.org
