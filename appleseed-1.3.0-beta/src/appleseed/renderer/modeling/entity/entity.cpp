
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010-2013 Francois Beaune, Jupiter Jazz Limited
// Copyright (c) 2014-2015 Francois Beaune, The appleseedhq Organization
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// Interface header.
#include "entity.h"

using namespace foundation;
using namespace std;

namespace renderer
{

struct Entity::Impl
{
    string m_name;
};

Entity::Entity(
    const UniqueID      class_uid)
  : impl(new Impl())
  , m_class_uid(class_uid)
  , m_parent(0)
  , m_render_layer(~0)
{
}

Entity::Entity(
    const UniqueID      class_uid,
    Entity*             parent)
  : impl(new Impl())
  , m_class_uid(class_uid)
  , m_parent(parent)
  , m_render_layer(~0)
{
}

Entity::Entity(
    const UniqueID      class_uid,
    const ParamArray&   params)
  : impl(new Impl())
  , m_class_uid(class_uid)
  , m_parent(0)
  , m_params(params)
  , m_render_layer(~0)
{
}

Entity::Entity(
    const UniqueID      class_uid,
    Entity*             parent,
    const ParamArray&   params)
  : impl(new Impl())
  , m_class_uid(class_uid)
  , m_parent(parent)
  , m_params(params)
  , m_render_layer(~0)
{
}

Entity::~Entity()
{
    delete impl;
}

void Entity::set_name(const char* name)
{
    assert(name);
    impl->m_name = name;
}

const char* Entity::get_name() const
{
    return impl->m_name.c_str();
}

}   // namespace renderer