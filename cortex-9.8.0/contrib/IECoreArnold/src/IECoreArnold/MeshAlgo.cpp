//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2011-2016, Image Engine Design Inc. All rights reserved.
//  Copyright (c) 2012, John Haddon. All rights reserved.
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

#include "ai.h"

#include "IECore/Exception.h"
#include "IECore/MessageHandler.h"
#include "IECore/MeshPrimitive.h"

#include "IECoreArnold/NodeAlgo.h"
#include "IECoreArnold/ShapeAlgo.h"
#include "IECoreArnold/MeshAlgo.h"
#include "IECoreArnold/ParameterAlgo.h"

using namespace std;
using namespace IECore;
using namespace IECoreArnold;

//////////////////////////////////////////////////////////////////////////
// Internal utilities
//////////////////////////////////////////////////////////////////////////

namespace
{

AtArray *faceVaryingIndices( const IECore::MeshPrimitive *mesh )
{
	vector<int> ids;
	ids.resize( mesh->variableSize( PrimitiveVariable::FaceVarying ) );
	for( size_t i=0, e=ids.size(); i < e; i++ )
	{
		ids[i] = i;
	}
	return AiArrayConvert( ids.size(), 1, AI_TYPE_INT, (void *)&(ids[0]) );
}

AtNode *convertCommon( const IECore::MeshPrimitive *mesh )
{

	// make the result mesh and add topology

	AtNode *result = AiNode( "polymesh" );

	const std::vector<int> verticesPerFace = mesh->verticesPerFace()->readable();
	AiNodeSetArray(
		result,
		"nsides",
		AiArrayConvert( verticesPerFace.size(), 1, AI_TYPE_INT, (void *)&( verticesPerFace[0] ) )
	);

	const std::vector<int> vertexIds = mesh->vertexIds()->readable();
	AiNodeSetArray(
		result,
		"vidxs",
		AiArrayConvert( vertexIds.size(), 1, AI_TYPE_INT, (void *)&( vertexIds[0] ) )
	);

	// set subdivision

	if( mesh->interpolation()=="catmullClark" )
	{
		AiNodeSetStr( result, "subdiv_type", "catclark" );
		AiNodeSetBool( result, "smoothing", true );
	}

	// add uvs

	const FloatVectorData *s = mesh->variableData<FloatVectorData>( "s" );
	const FloatVectorData *t = mesh->variableData<FloatVectorData>( "t" );
	if( s && t )
	{
		PrimitiveVariable::Interpolation sInterpolation = mesh->variables.find( "s" )->second.interpolation;
		PrimitiveVariable::Interpolation tInterpolation = mesh->variables.find( "t" )->second.interpolation;
		if( sInterpolation == tInterpolation )
		{
			if( sInterpolation == PrimitiveVariable::Varying || sInterpolation == PrimitiveVariable::Vertex || sInterpolation == PrimitiveVariable::FaceVarying )
			{
				// interleave the uvs and set them
				vector<float> st;
				st.reserve( s->readable().size() * 2 );
				for( vector<float>::const_iterator sIt = s->readable().begin(), tIt = t->readable().begin(), eIt = s->readable().end(); sIt != eIt; sIt++, tIt++ )
				{
					st.push_back( *sIt );
					st.push_back( *tIt );
				}
				AiNodeSetArray(
					result,
					"uvlist",
					AiArrayConvert( s->readable().size(), 1, AI_TYPE_POINT2, (void *)&(st[0]) )
				);
				// build uv indices
				if( sInterpolation == PrimitiveVariable::FaceVarying )
				{
					AiNodeSetArray(
						result,
						"uvidxs",
						faceVaryingIndices( mesh )
					);
				}
				else
				{
					AiNodeSetArray(
						result,
						"uvidxs",
						AiArrayConvert( vertexIds.size(), 1, AI_TYPE_INT, (void *)&( vertexIds[0] ) )
					);
				}
			}
			else
			{
				msg( Msg::Warning, "ToArnoldMeshConverter::doConversion", "Variables s and t have unsupported interpolation type - not generating uvs." );
			}
		}
		else
		{
			msg( Msg::Warning, "ToArnoldMeshConverter::doConversion", "Variables s and t have different interpolation - not generating uvs." );
		}
	}
	else if( s || t )
	{
		msg( Msg::Warning, "ToArnoldMeshConverter::doConversion", "Only one of s and t available - not generating uvs." );
	}

	// add arbitrary user parameters

	const char *ignore[] = { "P", "s", "t", "N", 0 };
	ShapeAlgo::convertPrimitiveVariables( mesh, result, ignore );

	return result;
}

const V3fVectorData *normal( const IECore::MeshPrimitive *mesh, PrimitiveVariable::Interpolation &interpolation )
{
	PrimitiveVariableMap::const_iterator it = mesh->variables.find( "N" );
	if( it == mesh->variables.end() )
	{
		return NULL;
	}

	const V3fVectorData *n = runTimeCast<const V3fVectorData>( it->second.data.get() );
	if( !n )
	{
		msg( Msg::Warning, "MeshAlgo", boost::format( "Variable \"N\" has unsupported type \"%s\" (expected V3fVectorData)." ) % it->second.data->typeName() );
		return NULL;
	}

	const PrimitiveVariable::Interpolation thisInterpolation = it->second.interpolation;
	if( interpolation != PrimitiveVariable::Invalid && thisInterpolation != interpolation )
	{
		msg( Msg::Warning, "MeshAlgo", "Variable \"N\" has inconsistent interpolation types - not generating normals." );
		return NULL;
	}

	if( thisInterpolation != PrimitiveVariable::Varying && thisInterpolation != PrimitiveVariable::Vertex && thisInterpolation != PrimitiveVariable::FaceVarying )
	{
		msg( Msg::Warning, "MeshAlgo", "Variable \"N\" has unsupported interpolation type - not generating normals." );
		return NULL;
	}

	interpolation = thisInterpolation;
	return n;
}

void convertNormalIndices( const IECore::MeshPrimitive *mesh, AtNode *node, PrimitiveVariable::Interpolation interpolation )
{
	if( interpolation == PrimitiveVariable::FaceVarying )
	{
		AiNodeSetArray(
			node,
			"nidxs",
			faceVaryingIndices( mesh )
		);
	}
	else
	{
		const std::vector<int> vertexIds = mesh->vertexIds()->readable();
		AiNodeSetArray(
			node,
			"nidxs",
			AiArrayConvert( vertexIds.size(), 1, AI_TYPE_INT, (void *)&( vertexIds[0] ) )
		);
	}
}

NodeAlgo::ConverterDescription<MeshPrimitive> g_description( MeshAlgo::convert, MeshAlgo::convert );

} // namespace

//////////////////////////////////////////////////////////////////////////
// Implementation of public API
//////////////////////////////////////////////////////////////////////////

AtNode *MeshAlgo::convert( const IECore::MeshPrimitive *mesh )
{
	AtNode *result = convertCommon( mesh );

	ShapeAlgo::convertP( mesh, result, "vlist" );

	// add normals

	PrimitiveVariable::Interpolation nInterpolation = PrimitiveVariable::Invalid;
	if( const V3fVectorData *n = normal( mesh, nInterpolation ) )
	{
		AiNodeSetArray(
			result,
			"nlist",
			AiArrayConvert( n->readable().size(), 1, AI_TYPE_VECTOR, &n->readable().front() )
		);
		convertNormalIndices( mesh, result, nInterpolation );
		AiNodeSetBool( result, "smoothing", true );
	}

	return result;
}

AtNode *MeshAlgo::convert( const std::vector<const IECore::MeshPrimitive *> &samples, const std::vector<float> &sampleTimes )
{
	AtNode *result = convertCommon( samples.front() );

	std::vector<const IECore::Primitive *> primitiveSamples( samples.begin(), samples.end() );
	ShapeAlgo::convertP( primitiveSamples, result, "vlist" );

	// add normals

	vector<const Data *> nSamples;
	nSamples.reserve( samples.size() );
	PrimitiveVariable::Interpolation nInterpolation = PrimitiveVariable::Invalid;
	for( vector<const MeshPrimitive *>::const_iterator it = samples.begin(), eIt = samples.end(); it != eIt; ++it )
	{
		if( const V3fVectorData *n = normal( *it, nInterpolation ) )
		{
			nSamples.push_back( n );
		}
		else
		{
			break;
		}
	}

	if( nSamples.size() == samples.size() )
	{
		AiNodeSetArray(
			result,
			"nlist",
			ParameterAlgo::dataToArray( nSamples, AI_TYPE_VECTOR )
		);
		convertNormalIndices( samples.front(), result, nInterpolation );
		AiNodeSetBool( result, "smoothing", true );
	}

	// add time sampling

	AiNodeSetArray( result, "deform_time_samples", AiArrayConvert( sampleTimes.size(), 1, AI_TYPE_FLOAT, &sampleTimes.front() ) );

	return result;
}

