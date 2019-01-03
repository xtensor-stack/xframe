.. Copyright (c) 2018, Johan Mabille, Sylvain Corlay, Wolf Vollprecht
   and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Indexing and selecting data
===========================

In this section, we consider the following variable:

.. code::

    using fstring = xtl::xfixed_string<55>;
    using data_type = xt::xoptional_assembly<xt::xarray<double>, xt::xarray<bool>>;
    using coordinate_type = xf::xcoordinate<fstring>;
    using dimension_type = xf::xdimension<fstring>;
    using variable_type = xvariable<coordinate_type, data_type>;

    data_type d = xt::eval(xt::random::rand({6, 3}, 15., 25.));
    variable_type v(std::move(d),
                    {
                        {"group", xf::axis({"a", "b", "d", "e", "g", "h"})},
                        {"city",  xf::axis({"London", "Paris", "Brussels"})}
                    });

`xframe` provides flexible indexing methods for data selection, similar to the
ones of `xarray`_. These methods are summarized in the following table:

.. |br| raw:: html

   <br />

+------------------+--------------+---------------------------------------------------+
| Dimension lookup | Index lookup | ``xvariable`` syntax                              |
+==================+==============+===================================================+
| Positional       | By integer   | ``v(2, 1)``                                       |
+------------------+--------------+---------------------------------------------------+
| Positional       | By label     | ``v.locate("d", "Paris")``                        |
+------------------+--------------+---------------------------------------------------+
| By name          | By integer   | ``v.iselect({{"group", 2}, {"city", 1}})``        |
+------------------+--------------+---------------------------------------------------+
| By name          | By label     | ``v.select({{"group", "d"}, {"city", "Paris"}})`` |
+------------------+--------------+---------------------------------------------------+

Positional indexing
-------------------

The most basic way to access elements of an ``xvariable`` is to use ``operator()``, like
you would do with an ``xtensor``:

.. code::

    std::cout << v(2, 1) << std::endl;

Contrary to Python, it is not possible to have different return types for a same method
in C++. Multi selection is done with free functions that return views on the variable,
therefore a change in the view will reflect in the underlying variable:

.. code::

    #include "xvariable_view.hpp"

    auto view1 = xf::ilocate(v, xf::irange(0, 5, 2), xf::irange(1, 3));
    std::cout << view1 << std::endl;

    // Output:
    // {{ 23.3501  24.6887}
    //  { 21.3489  24.9646}
    //  { 22.9811  17.9703}}
    // Coordinates:
    // group: (a, d, g,)
    // city: (Paris, Brussels,)

    view1(0, 1) = 0.;
    std::cout << v(2, 2) << std::endl;
    // Outputs 0.

Here ``irange`` returns a range slice from `xtensor`, so any multi selection in `xtensor`
is also supported in `xframe`.

``xvariable`` also supports label-based indexing, with the ``locate`` method for single
point selection, and ``locate`` free function for multi selection:

.. code::

    std::cout << v.locate("d", "Paris") << std::endl;
    auto view2 = xf::locate(v, xf::range("a", "h", 2), xf::range("Paris", "Brussels"));
    std::cout << view2 << std::endl;
    // Same output as previous code

Be aware of the difference between ``range`` and ``irange`` parameters: for the former one,
accepting labels, the last value is *included* while for the latter one, accepting integral
indices, the las value is *excluded*.

`xframe` provides label-based slices similar to those of `xtensor`, so label-based
multi selection is really similar to positional multi selection.

Indxing with dimension names
----------------------------

With the dimension names, we do not have to rely on the dimension order. We can use them
explicitely to select data; Like positional indexing, `xframe` provides methods and
free functions depending on the kind of selection you want to do:

.. code::

    // Dimension by name, index by position
    std::cout << v.iselect({{"city", 1}, {"group", 2}}) << std::endl;
    auto view3 = xf::iselect(v, {{"city", xf::irange(1, 3)}, {"group", xf::irange(0, 5, 2)}});
    
    // Dimension by name, index by label
    std::cout << v.select({{"city", "Paris"}, {"group", "d"}}) << std::endl;
    auto view4 = xf::select(v, {{"city", xr::range("Paris", "Brussels")}, {"group", xf::range("a", "h", 2)}});

Contrary to `xarray`_, `xframe` does noe provide a selection operator accepting a map
argument.

Keeping and dropping labels
---------------------------

``drop`` and ``keep`` functions return slices that can be used to create a view with
the listed labels along the specified dimensions dropped or kept:

.. code::

    auto view5 = xf::select(v, {{"city", xf::drop("London")}, {"group", xf::keep("a", "d", "g")}});
    // view5 is equivalent to view4

This is different form `xarray`_  where the ``xarray.DataArray.drop`` method returns a new object.
To achieve the same with `xframe`, simply assign the view to a new ``xvariable`` object:

.. code::

    variable_type v2 = view5;

Masking views
-------------

Masking views allow to select data points based on conditions expresses on labels. These conditions
can be arbitrary complicated boolean expressions. Contrary to other views which are generally a
subset of the original data, a masking view has the same shape as its underlying ``xvariable``.

Masking views are created with the ``where`` function:

.. code::

    data_type d2 = {{{ 1.,  2., 3. },
                     { 4.,  5., 6. },
                     { 7.,  8., 9. }},
                    {{ 1.3, 1.5, 1.},
                     { 2., 2.3, 2.4},
                     { 3.1, 3.8, 3.}},
                    {{ 8.5, 8.2, 8.6},
                     { 7.5, 8.6, 9.7},
                     { 4.5, 4.4, 4.3}}};

    auto v3 = variable_type(
        d2,
        {
            {"x", xf::axis(3)},
            {"y", xf::axis(3, 6, 1)},
            {"z", xf::axis(3)},
        }
    );

    auto masked_pressure = xf::where(
        v3,
        not_equal(v3.axis<int>("x"), 2) && v3.axis<int>("z") < 2
    );
    std::cout << v3 << std::endl;

This code prints the whole variable, with values not selected printed as "masked".
When assigning to a masked view, masked values are not changed. Like other views,
a masking view is a proxy on its underlying variable, no copy is made, so changing
an unmasked value actually changes the corresponding value in the undnerlying variable.



.. _xarray: https://xarray.pydata.org
