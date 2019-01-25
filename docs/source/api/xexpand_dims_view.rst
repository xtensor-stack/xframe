.. Copyright (c) 2018, Johan Mabille, Sylvain Corlay, Wolf Vollprecht
   and Martin Renou

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

xexpand_dims_view
=================

Defined in ``xframe/xexpand_dims_view.hpp``

.. doxygenclass:: xf::xexpand_dims_view
   :project: xframe
   :members:

.. doxygenfunction:: expand_dims(E&&, std::initializer_list<K>)
   :project: xframe

.. doxygenfunction:: expand_dims(E&&, std::initializer_list<std::pair<typename std::decay_t<E>::key_type, std::size_t>>)
   :project: xframe
