.. Copyright (c) 2018, Johan Mabille, Sylvain Corlay, Wolf Vollprecht
   and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Indexing and selecting data
===========================

In this section, we consider the following variable:

.. code::

    using coordinate_type = xf::xcoordinate<xf::fstring>;
    using dimension_type = xf::xdimension<xf::fstring>;
    using variable_type = xf::xvariable<double, coordinate_type>;

    data_type d = xt::eval(xt::random::rand({6, 3}, 15., 25.));
    variable_type v(std::move(d),
                    {
                        {"group", xf::axis({"a", "b", "d", "e", "g", "h"})},
                        {"city",  xf::axis({"London", "Paris", "Brussels"})}
                    });

Printing this variable in a Jupyter Notebook gives:

+-------+---------+---------+----------+
|       | London  |  Paris  | Brussels |
+=======+=========+=========+==========+
| **a** | 16.3548 | 23.3501 | 24.6887  |
+-------+---------+---------+----------+
| **b** | 17.2103 | 18.0817 | 20.4722  |
+-------+---------+---------+----------+
| **d** | 16.8838 | 24.9288 | 24.9646  |
+-------+---------+---------+----------+
| **e** | 24.6769 | 22.2584 | 24.8111  |
+-------+---------+---------+----------+
| **g** | 16.0986 | 22.9811 | 17.9703  |
+-------+---------+---------+----------+
| **h** | 15.0478 | 16.1246 | 21.3976  |
+-------+---------+---------+----------+

`xframe` provides flexible indexing methods for data selection, similar to the
ones of `xarray`_. These methods are summarized in the following table:

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
in C++. Multi selection is done with free functions that return views on the variable:,

.. code::

    #include "xvariable_view.hpp"

    auto view1 = xf::ilocate(v, xf::irange(0, 5, 2), xf::irange(1, 3));
    std::cout << view1 << std::endl;

+-------+---------+----------+
|       |  Paris  | Brussels |
+=======+=========+==========+
| **a** | 23.3501 | 24.6887  |
+-------+---------+----------+
| **d** | 24.9288 | 24.9646  |
+-------+---------+----------+
| **g** | 22.9811 | 17.9703  |
+-------+---------+----------+

Therefore a change in the view will reflect in the underlying variable:

.. code::

    view1(0, 1) = 0.;
    std::cout << v(2, 2) << std::endl;
    // Outputs 0.

In the code creating the view, ``irange`` returns a range slice from `xtensor`, so any multi
selection in `xtensor` is also supported in `xframe`.

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
    auto view4 = xf::select(v, {{"city", xf::range("Paris", "Brussels")}, {"group", xf::range("a", "h", 2)}});
    // view3 and view4 gives the same output as view2 and view1

Contrary to `xarray`_, `xframe` does not provide a selection operator accepting a map
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

    data_type d2 = {{ 1.,  2., 3. },
                    { 4.,  5., 6. },
                    { 7.,  8., 9. }};

    auto v3 = variable_type(
        d2,
        {
            {"x", xf::axis(3)},
            {"y", xf::axis(3)},
        }
    );

    auto view6 = xf::where(
        v3,
        not_equal(v3.axis<int>("x"), 2) && v3.axis<int>("y") < 2
    );
    std::cout << view6 << std::endl;

In a Jupyter Notebookn, this outputs the following:

+-------+--------+--------+--------+
|       |    0   |    1   |    2   |
+=======+========+========+========+
| **0** |    1   |    2   | masked |
+-------+--------+--------+--------+
| **1** |    4   |    5   | masked |
+-------+--------+--------+--------+
| **2** | masked | masked | masked |
+-------+--------+--------+--------+

When assigning to a masked view, masked values are not changed. Like other views,
a masking view is a proxy on its underlying variable, no copy is made, so changing
an unmasked value actually changes the corresponding value in the undnerlying variable.

Assigning values with indexing
------------------------------

Data selection in variables return either references or views; therefore, contrary to
`xarray`_, it is *possible* to assign values to a subset of a variable with any of the
indexing method:

.. code::

    // The next four lines are equivalent, they change a single value of v:
    v(2, 1) = 2.5;
    v.locate("d", "Paris") = 2.5;
    v.iselect({{"city", 1}, {"group", 2}}) = 2.5;
    v.select({{"city", "Paris"}, {"group", "d"}}) = 2.5;

    data_type d3 = {{0.,  1.},
                    {2.,  3.},
                    {4.,  5.}};

    auto v4 = variable_type(
        d3,
        {
            {"group", xf::axis({"a", "d", "g"})},
            {"city", xf::axis({"Paris", "Brussels"})}
        }
    );

    // The next four lines are equivalent, they change a subset of v
    xf::ilocate(v, xf::irange(0, 5, 2), xf::irange(1, 3)) = v4;
    xf::locate(v, xf::range("a", "h", 2), xf::range("Paris", "Brussels")) = v4;
    xf::iselect(v, {{"city", xf::irange(1, 3)}, {"group", xf::irange(0, 5, 2)}}) = v4;
    xf::select(v, {{"city", xr::range("Paris", "Brussels")}, {"group", xf::range("a", "h", 2)}}) = v4;

Printing ``v`` after the assign gives

+-------+---------+---------+----------+
|       | London  |  Paris  | Brussels |
+=======+=========+=========+==========+
| **a** | 16.3548 |    0    |    1     |
+-------+---------+---------+----------+
| **b** | 17.2103 | 18.0817 | 20.4722  |
+-------+---------+---------+----------+
| **d** | 16.8838 |    2    |    3     |
+-------+---------+---------+----------+
| **e** | 24.6769 | 22.2584 | 24.8111  |
+-------+---------+---------+----------+
| **g** | 16.0986 |    4    |    5     |
+-------+---------+---------+----------+
| **h** | 15.0478 | 16.1246 | 21.3976  |
+-------+---------+---------+----------+

Reindexing views
----------------

Reindexing views give variables new set of coordinates to corresponding dimensions. Like other views,
no copy is involved. Asking for values corresponding to new labels not found in the original set of
coordinates returns missing values. In the next example, we reindex the ``city`` dimension:

.. code::

    auto view7 = xf::reindex(v, {{"city", xf::axis({"London", "New York", "Brussels"})}});

+-------+---------+----------+----------+
|       | London  | New York | Brussels |
+=======+=========+==========+==========+
| **a** | 16.3548 |   N/A    | 24.6887  |
+-------+---------+----------+----------+
| **b** | 17.2103 |   N/A    | 20.4722  |
+-------+---------+----------+----------+
| **d** | 16.8838 |   N/A    | 24.9646  |
+-------+---------+----------+----------+
| **e** | 24.6769 |   N/A    | 24.8111  |
+-------+---------+----------+----------+
| **g** | 16.0986 |   N/A    | 17.9703  |
+-------+---------+----------+----------+
| **h** | 15.0478 |   N/A    | 21.3976  |
+-------+---------+----------+----------+

Like `xarray`_, `xframe` provides the useful ``reindex_like`` shortcut which allows to reindex a
variable given the set of coordinates of another variable:

.. code::

    auto v5 = variable_type(
        d,
        {
            {"group", xf::axis({"a", "b", "d", "e", "g", "h"})},
            {"city", xf::axis({"London", "New York", "Brussels"})}
        }
    );

    auto view8 = xf::reindex_like(v, v5);
    // view8 is equivalent to view7

A reindexing view is a read-only view, it is not possible to change its value with indexing.
This allows memory optimizations, the view does not have to store the missing values, it can
return a proxy to a static-allocated missing value.

The ``align`` function allows to reindex many variables with more flexible options:

.. code::

    auto t1 = xf::align<join::inner>(v, v5);
    std::cout << std::get<0>(t1) << std::endl;
    std::cout << std::get<1>(t1) << std::endl;

The last lines print the same output:

+-------+---------+----------+
|       | London  | Brussels |
+=======+=========+==========+
| **a** | 16.3548 | 24.6887  |
+-------+---------+----------+
| **b** | 17.2103 | 20.4722  |
+-------+---------+----------+
| **d** | 16.8838 | 24.9646  |
+-------+---------+----------+
| **e** | 24.6769 | 24.8111  |
+-------+---------+----------+
| **g** | 16.0986 | 17.9703  |
+-------+---------+----------+
| **h** | 15.0478 | 21.3976  |
+-------+---------+----------+

In the following, the variables are aligned w.r.t the union of the coordinates instead
of their intersection:

.. code::

    auto t2 = xf::align<join::outer>(v, v5);
    std::cout << std::get<0>(t2) << std::endl;
    std::cout << std::get<1>(t2) << std::endl;

The first outuput is

+-------+---------+---------+----------+----------+
|       | London  |  Paris  | Brussels | New York |
+=======+=========+=========+==========+==========+
| **a** | 16.3548 | 23.3501 | 24.6887  |   N/A    |
+-------+---------+---------+----------+----------+
| **b** | 17.2103 | 18.0817 | 20.4722  |   N/A    |
+-------+---------+---------+----------+----------+
| **d** | 16.8838 | 24.9288 | 24.9646  |   N/A    |
+-------+---------+---------+----------+----------+
| **e** | 24.6769 | 22.2584 | 24.8111  |   N/A    |
+-------+---------+---------+----------+----------+
| **g** | 16.0986 | 22.9811 | 17.9703  |   N/A    |
+-------+---------+---------+----------+----------+
| **h** | 15.0478 | 16.1246 | 21.3976  |   N/A    |
+-------+---------+---------+----------+----------+

While the second have ``N/A`` in the ``Paris`` column.

.. _xarray: https://xarray.pydata.org
