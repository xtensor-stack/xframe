.. Copyright (c) 2018, Johan Mabille, Sylvain Corlay, Wolf Vollprecht
   and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

xcoordinate
===========

Defined in ``xframe/xcoordinate.hpp``

.. doxygenclass:: xf::xcoordinate
   :project: xframe
   :members:

.. doxygenfunction:: coordinate(const std::map<K, xaxis_variant<L, S, MT>>&)
   :project: xframe

.. doxygenfunction:: coordinate(std::map<K, xaxis_variant<L, S, MT>>&&)
   :project: xframe

.. doxygenfunction:: coordinate(xnamed_axis<K, S, MT, L, LT>, xnamed_axis<K1, S, MT, L, LT1>...)
   :project: xframe

.. doxygenfunction:: broadcast_coordinates(xcoordinate<K, L, S, MT>&, const Args&...)
   :project: xframe
