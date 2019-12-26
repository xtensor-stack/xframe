.. Copyright (c) 2018, Johan Mabille, Sylvain Corlay, Wolf Vollprecht
   and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Computation
===========

`xframe` is actually more than a multi-dimensional data frame library; like `xtensor`_,
it is an expression engine that allows numerical computation on any object implementing
the variable or the frame interfaces.

Expressions
-----------

Assume ``x``, ``y`` and ``z`` are variables with *compatible coordinates* (we'll come back
to that later), the return type of an expression such as ``x + exp(y) * sin(z)`` is 
**not a variable**. The result is a variable expression which offers the same interface
as ``xvariable`` but does not hold any value. Such expressions can be plugged into others
to build more cmoplex expressions:

.. code::

    auto f = x + exp(y) * sin(z);
    auto f2 = x + exp(w) * cos(f);

The expression engines avoids the evaluation of intermediate results and their storage in
temporary variables, so you can achieve the same performance as if you had written a loop.
Such a loop is quite more complicated than an array loop since labels and dimension names
are involved in the assignment mechanism.

Since a variable expression provides the same API as ``xvariable``, all the indexing and
selection operations are available:

.. code::

    auto v = (x + exp(y) * sin(z)).select({{"city", "Paris"}, {"group", "a"}});
    auto view = select(x + exp(y) * sin(z), {{"city", xf::keep("Paris")}, {"group", xf::drop({"b", "d", "h"})}});

Lazy evaluation
---------------

An expression such as ``x + exp(y) * sin(z)`` does not hold the result. **Values are only computed
upon access or when the expression is assigned to a variable**. this allows to operate symbolically
on very large data sets and only compute the result for the indices of interest:

.. code::

    // Assuming x and y are variables each holding ~1M values
    auto f = cos(x) + sin(y);
    
    double first_res = f.locate("a", "London");
    double second_res = f.locate("b", "Pekin");
    // Only two values have been computed

That means if you use the same expression in two assign statements, the computation of the expression
will be done twice. Depending on the complexity of the cmoputation and the size of the data, it might
be convenient to store the result of the expression in a temporary variable:

.. code::

    variable_type tmp = cos(x) + sin(y);
    variable_type res1 = tmp + 2 * exp(z);
    variable_type res2 = tmp - 3 * exp(w);

Broadcasting by dimension names
-------------------------------

Like `xarray`_, broadcasting in *xframe* is done according to the dimension names rather than
their positions. This way, you do not need to transpose variable or insert dimensions of length
1 to to get array operations to work, as commonly done in `xtensor`_ with ``xt::reshape`` or
``xt::newaxis``.

This can be illustrated with the following examples. First, consider two one-dimensional variable
aligned along different dimensions:

.. code::

    auto v1 = variable_type(data_type({1., 2.}), {{"x", xf::axis(1, 3)}});
    auto v2 = variable_type(data_type({3., 7.}), {{"y", xf::axis(2, 5)}});

We can apply mathematical operations to these variables, their dimension are expanded automatically:

.. code::

    variable_type res = v1 + v2;
    std::cout << res << std::endl;
    // Output:
    // {{ 4.  8.}
    //  { 5.  9.}}
    // Coordinates:
    // x: (1, 3, )
    // y: (2, 5, )
    
Contrary to `xarray`_ , dimensions are not reordered to the order in which they first appeared:

.. code::

    auto v3 = variable_type(data_type({{1., 2.}, {3., 4.}}),
                            {{"y", xf::axis({2, 5})}, {"x", xf::axis({1, 4})}});
    variable_type res2 = v1 + v3;
    std::cout << res2 << std::endl;
    // Output:
    // {{ 2.  4. }
    //  { 4.  6. }}
    // Coordinates:
    // y: (2, 5, )
    // x: (1, 3, )

This allows many optimizations in the assignment mechanism.

Automatic alignment
-------------------

`xframe` enforces alignment between coordinate labels on objects involved in operations.
The default result of an operation is by the *intersection* of the coordinate labels:

.. code::

    auto v4 = variable_type(data_type({1., 2., 3.}), {{"x", xf::axis({1, 3, 5})}});
    auto v5 = variable_type(data_type({4., 7., 12.}), {{"x", xf::axis({1, 5, 7})}});

    variable_type res3 = v4 + v5;
    std::cout << res3 << std::endl;
    // Output:
    // { 5.  10. }
    // Coordinates:
    // x: (1, 5,)

Operations are slower when variables are not aligned, so it might be useful to
explicitly align variables involved in loops of performance critical code.

Operators and functions
-----------------------

`xframe` provides all the basic operators and mathematical functions:

- arithmetic operators: ``+``, ``-``, ``*``, ``/``
- logical operators: ``&&``, ``||``, ``!``
- comparison operators: ``==``, ``!=``, ``<``, ``<=``, ``>``, ``>=``
- basic functions: ``abs``, ``remainder``, ``fma``, ...
- exponential functions: ``exp``, ``expm1``, ``log``, ``log1p``, ...
- power functions: ``pow``, ``sqrt``, ``cbrt``, ...
- trigonometric functions: ``sin``, ``cos``, ``tan``, ...
- hyperbolic functions: ``sinh``, ``cosh``, ``tanh``, ...
- error and gamma functions: ``erf``, ``erfc``, ``tgamma``, ``lgamma``, ...

Actually, any function operating on ``xtensor`` expressions can work with
``xvariable`` expressions without any additional work; the only constraint
is to accept and return expressions:

.. code::

    template <class E1, class E2>
    inline auto distance(const xexpression<E1>& e1, const xexpression<E2>& e2)
    {
        const E1& de1 = e1.derived_cast();
        const E2& de2 = e2.derived_cast();
        // 
        return sqrt(de1 * de1 + de2 * de2);
    }

This function can work with both tnesor and variable expressions, performing
broadcasting according to the rules of `xtensor` or `xframe` depending on
its argument type:

.. code::

    xt::xarray<double> a1 = {1., 2. };
    xt::xarray<double> a2 = {{1., 3.}, {4., 7.}};

    // Broadcasting is applied according to xtensor rules,
    // that is by dimension order
    xt::xarray<double> ares = distance(a1, a2);

    // Broadcasting is applied according to xframe rules,
    // that is by dimension name
    variable_type vres = distance(v1, v3);

Missing values
--------------

Contrary to `pandas`_ or `xarray`_, `xframe` does not use particular values
for representing missing values. Instead, it makes use of the dedicated type
``xtl::xoptional`` which gathers the value and a flag to specify whether the
value is missing or not:

.. code::

    data_type d = {1., 2., 3. };
    d(1).has_value() = false;

    auto v = variable_type(d, {{"x", xf::axis({1, 3, 4})}});
    std::cout << v << std::endl;
    // Output:
    // { 1.  N/A  3. }
    // Coordinates:
    // x: (1, 3, 4,)

.. _pandas: https://pandas.pydata.org
.. _xarray: https://xarray.pydata.org
.. _xtensor: https://github.com/xtensor-stack/xtensor

