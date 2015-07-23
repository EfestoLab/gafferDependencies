
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

#ifndef APPLESEED_FOUNDATION_IMAGE_GENERICIMAGEFILEWRITER_H
#define APPLESEED_FOUNDATION_IMAGE_GENERICIMAGEFILEWRITER_H

// appleseed.foundation headers.
#include "foundation/image/iimagefilewriter.h"
#include "foundation/image/imageattributes.h"

// appleseed.main headers.
#include "main/dllsymbol.h"

// Forward declarations.
namespace foundation    { class ICanvas; }

namespace foundation
{

//
// Generic image file writer.
//

class APPLESEED_DLLSYMBOL GenericImageFileWriter
  : public IImageFileWriter
{
  public:
    // Write an image file.
    virtual void write(
        const char*             filename,
        const ICanvas&          image,
        const ImageAttributes&  image_attributes = ImageAttributes());
};

}       // namespace foundation

#endif  // !APPLESEED_FOUNDATION_IMAGE_GENERICIMAGEFILEWRITER_H