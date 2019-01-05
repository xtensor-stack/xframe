.. Copyright (c) 2018, Johan Mabille, Sylvain Corlay, Wolf Vollprecht
   and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Getting started
===============

This short guide explains how to get started with `xframe` once you have installed it with one of
the methods described in the installation section.

First example
-------------

.. code::

    #include <iostream>
    #include "xtensor/xrandom.hpp"
    #include "xframe/xio.hpp"
    #include "xframe/xvariable.hpp"

    int main(int argc, char* argv[])
    {
        using coordinate_type = xf::xcoordinate<xf::fstring>;
        using variable_type = xf::xvariable<double, coordinate_type>;
        using data_type = variable_type::data_type;

        // Creation of the data
        data_type data = xt::eval(xt::random::rand({6, 3}, 15., 25.));
        data(0, 0).has_value() = false;
        data(2, 1).has_value() = false;

        // Creation of coordinates and dimensions
        auto time_axis = xf::axis({"2018-01-01", "2018-01-02", "2018-01-03", "2018-01-04", "2018-01-05", "2018-01-06"});
        auto city_axis = xf::axis({"London", "Paris", "Brussels"});
        auto coord = xf::coordinate({{"date", time_axis}, {"city", city_axis}});
        auto dim = xf::dimension({"date", "city"});

        // Creation of the variable
        auto var = variable_type(data, coord, dim);
        std::cout << var << std::endl;

        return 0;
    }

This example creates a variable, that is, a tensor data (here random) with labels and dimension names.

Compiling the first example
---------------------------

`xframe` is a header-only library, so there is no library to link with. The only constraint
is that the compiler must be able to find the headers of `xframe` and those of its dependencies,
that is, `xtensor` and `xtl`; this is usually done by having the directory containing the headers
in the include path. With GCC, use the ``-I`` option to achieve this. Assuming the first example
code is located in ``example.cpp``, the compilation command is:

.. code:: bash

    gcc -I /path/to/headers/ example.cpp -o example

When you run the program, it produces the following output (data should be different
since it is randomly generated):

.. code::

    {{    N/A, 23.3501, 24.6887},
     {17.2103, 18.0817, 20.4722},
     {16.8838,     N/A, 24.9646},
     {24.6769, 22.2584, 24.8111},
     {16.0986, 22.9811, 17.9703},
     {15.0478, 16.1246, 21.3976}}
    Coordinates:
    date: (2018-01-01, 2018-01-02, 2018-01-03, 2018-01-04, 2018-01-05, 2018-01-06, )
    city: (London, Paris, Brussels)

Building with cmake
-------------------

A better alternative for building programs using `xframe` is to use `cmake`, especially if you are
developing for several platforms. Assuming the following folder structure:

.. code:: bash

    first_example
       |- src
       |   |- example.cpp
       |- CMakeLists.txt

The following minimal ``CMakeLists.txt`` is enough to build the first example:

.. code:: cmake

    cmake_minimum_required(VERSION 3.1)
    project(first_example)

    find_package(xtl REQUIRED)
    find_package(xframeREQUIRED)

    add_executable(first_example src/example.cpp)
    target_link_libraries(first_example xtensor)

`cmake` has to know where to find the headers, this is done through the ``CMAKE_INSTALL_PREFIX``
variable. Note that ``CMAKE_INSTALL_PREFIX`` is usually the path to a folder containing the following
subfolders: ``include``, ``lib`` and ``bin``, so you don't have to pass any additional option for linking.
Examples of valid values for ``CMAKE_INSTALL_PREFIX`` on Unix platforms are ``/usr/local``, ``/opt``.

The following commands create a directory for building (avoid building in the source folder), builds
the first example with cmake and then runs the program:

.. code:: bash

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=your_prefix ..
    make
    ./first_program

Second example: simplified variable creation
--------------------------------------------

`xframe` provides many shortcuts so coordinates and variables can be created with a concise syntax.
The following example creates the same variable as the previous one:

.. code::

    #include <iostream>
    #include "xtensor/xrandom.hpp"
    #include "xframe/xio.hpp"
    #include "xframe/xvariable.hpp"

    int main(int argc, char* argv[])
    {
        using coordinate_type = xf::xcoordinate<xf::fstring>;
        using variable_type = xf::xvariable<double, coordinate_type>;
        using data_type = variable_type::data_type;

        // Creation of the data
        data_type data = xt::eval(xt::random::rand({6, 3}, 15., 25.));
        data(0, 0).has_value() = false;
        data(2, 1).has_value() = false;

        // Creation of the variable
        auto var = variable_type(
            data,
            {
                {"date", xf::axis({"2018-01-01", "2018-01-02", "2018-01-03", "2018-01-04", "2018-01-05", "2018-01-06"})},
                {"city", xf::axis({"London", "Paris", "Brussels"})}
            }
        );
        std::cout << var << std::endl;

        return 0;
    }

When compiled and run, this produces output similar to the one of the previous example (same coordinate system
but different data due to random generation).

Third example: data access
--------------------------

`xframe` provides different ways to access data in a variable.

.. code::

    #include <iostream>
    #include "xtensor/xrandom.hpp"
    #include "xframe/xio.hpp"
    #include "xframe/xvariable.hpp"

    int main(int argc, char* argv[])
    {

        using coordinate_type = xf::xcoordinate<xf::fstring>;
        using variable_type = xf::xvariable<double, coordinate_type>;
        using data_type = variable_type::data_type;

        // Creation of the data
        data_type data = xt::eval(xt::random::rand({6, 3}, 15., 25.));
        data(0, 0).has_value() = false;
        data(2, 1).has_value() = false;

        // Creation of the variable
        auto var = variable_type(
            data,
            {
                {"date", xf::axis({"2018-01-01", "2018-01-02", "2018-01-03", "2018-01-04", "2018-01-05", "2018-01-06"})},
                {"city", xf::axis({"London", "Paris", "Brussels"})}
            }
        );

        // Data access
        std::cout << "operator() - " << var(3, 0) << std::endl;
        std::cout << "locate     - " << var.locate("2018-01-04", "London") << std::endl;
        std::cout << "iselect    - " << var.iselect({{"date", 3}, {"city", 0}}) << std::endl;
        std::cout << "select     - " << var.select({{"date", "2018-01-04"}, {"city", "London"}}) << std::endl;

        return 0;
    }

Outputs:

.. code::

    operator() - 24.6769
    locate     - 24.6769
    iselect    - 24.6769
    select     - 24.6769

