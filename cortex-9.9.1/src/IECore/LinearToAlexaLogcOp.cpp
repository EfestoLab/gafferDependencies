//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2012, Image Engine Design Inc. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
//     * Neither the name of Image Engine Design nor the names of any
//       other contributors to this software may be used to endorse or
//       promote products derived from this software without specific prior
//       written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////

#include "IECore/LinearToAlexaLogcOp.h"
#include "IECore/TypeTraits.h"
#include "IECore/DespatchTypedData.h"
#include "IECore/CompoundParameter.h"
#include "IECore/LinearToAlexaLogcDataConversion.h"

using namespace IECore;
using namespace std;
using namespace boost;

IE_CORE_DEFINERUNTIMETYPED( LinearToAlexaLogcOp );

ColorSpaceTransformOp::ColorSpaceDescription<LinearToAlexaLogcOp> LinearToAlexaLogcOp::g_colorSpaceDescription( "linear", "alexaLogC" );

LinearToAlexaLogcOp::LinearToAlexaLogcOp()
	:	ChannelOp( "Applies linear to Alexa V3 Log C conversion on ImagePrimitive channels." )
{
}

LinearToAlexaLogcOp::~LinearToAlexaLogcOp()
{
}

struct LinearToAlexaLogcOp::Converter
{
	typedef void ReturnType;

	template<typename T>
	ReturnType operator()( T * data )
	{
		typedef typename T::ValueType Container;
		typedef typename Container::value_type V;
		typedef typename Container::iterator It;
		LinearToAlexaLogcDataConversion< V, V > converter;
		It end = data->writable().end();
		for ( It it = data->writable().begin(); it != end; it++ )
		{
			*it = converter( *it );
		}
	}
};

void LinearToAlexaLogcOp::modifyChannels( const Imath::Box2i &displayWindow, const Imath::Box2i &dataWindow, ChannelVector &channels )
{
	LinearToAlexaLogcOp::Converter converter;
	for ( ChannelVector::iterator it = channels.begin(); it != channels.end(); it++ )
	{
		despatchTypedData<LinearToAlexaLogcOp::Converter, TypeTraits::IsFloatVectorTypedData>( it->get(), converter );
	}
}