// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <shapes/Plane.h>
#include <me/render/VertexUtil.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToCreate.h>
#include <unify/String.h>
#include <unify/Size3.h>
#include <unify/TexArea.h>
#include <unify/Angle.h>

using namespace me;
using namespace render;
using namespace shapes;

const std::string DefaultBufferUsage = "Default";

Plane::Plane()
{
}

Plane::~Plane()
{
}

void Plane::Create(PrimitiveList & primitiveList, const unify::Parameters & parameters ) const
{
	using namespace unify;

	try
	{
		Size< float > size = parameters.Get( "size2", Size< float >( 1.0f, 1.0f ) );
		Color diffuse = parameters.Get( "diffuse", ColorWhite() );
		Color specular = parameters.Get( "specular", ColorWhite() );
		unsigned int segments = parameters.Get< unsigned int >( "segments", 1 );
		V3< float > center = parameters.Get( "center", V3< float >( 0, 0, 0 ) );
		TexArea texArea = parameters.Get( "texarea", TexArea( unify::TexCoords( 0, 0 ), TexCoords( 1, 1 ) ) );
		Effect::ptr effect = parameters.Get< Effect::ptr >( "effect" );
		VertexDeclaration::ptr vd = effect->GetVertexShader()->GetVertexDeclaration();
		BufferUsage::TYPE bufferUsage = BufferUsage::FromString( parameters.Get( "bufferusage", DefaultBufferUsage ) );
		size_t vertexCount = (segments + 1) * (segments + 1);
		size_t indexCount = 6 * segments * segments;

		BufferSet & set = primitiveList.AddBufferSet();
		set.SetEffect( effect );

		set.AddMethod( RenderMethod::CreateTriangleListIndexed( vertexCount, (unsigned int)indexCount, 0, 0 ) );

		unsigned short stream = 0;

		VertexElement positionE = CommonVertexElement::Position( stream );
		VertexElement normalE = CommonVertexElement::Normal( stream );
		VertexElement diffuseE = CommonVertexElement::Diffuse( stream );
		VertexElement specularE = CommonVertexElement::Specular( stream );
		VertexElement texE = CommonVertexElement::TexCoords( stream );

		struct V
		{
			V3< float > pos;
			V3< float > normal;
			Color diffuse;
			Color specular;
			TexCoords coords;
		};
		qjson::Object jsonFormat;
		jsonFormat.Add( { "Position", "Float3" } );
		jsonFormat.Add( { "Normal", "Float3" } );
		jsonFormat.Add( { "Diffuse", "Color" } );
		jsonFormat.Add( { "Specular", "Color" } );
		jsonFormat.Add( { "TexCoord", "TexCoord" } );
		VertexDeclaration::ptr vFormat( new VertexDeclaration( jsonFormat ) );

		std::shared_ptr< unsigned char > vertices( new unsigned char[vd->GetSizeInBytes( 0 ) * vertexCount] );
		DataLock lock( vertices.get(), vd->GetSizeInBytes( 0 ), vertexCount, DataLockAccess::ReadWrite, 0 );

		VertexBufferParameters vbParameters{ vd, { { vertexCount, vertices.get() } }, bufferUsage };

		unify::V3< float > posUL = center - unify::V3< float >( size.width * 0.5f, 0, size.height * 0.5f );
		for( unsigned int v = 0; v < (segments + 1); ++v )
		{
			float factorY = 1.0f / float( segments ) * v;
			for( unsigned int h = 0; h < (segments + 1); ++h )
			{
				float factorX = 1.0f / float( segments ) * h;
				unify::V3< float > pos = posUL + unify::V3< float >( size.width * factorX, 0, size.height * factorY );

				unsigned int index = v * (segments + 1) + h;
				WriteVertex( *vd, lock, index, positionE, pos );
				WriteVertex( *vd, lock, index, normalE, unify::V3< float >( 0, 1, 0 ) );
				WriteVertex( *vd, lock, index, diffuseE, diffuse );
				WriteVertex( *vd, lock, index, specularE, specular );
				WriteVertex( *vd, lock, index, texE, unify::TexCoords( factorX, factorY ) );
				vbParameters.bbox += pos;
			}
			unify::V3< float > pos = center - unify::V3< float >( size.width * 0.5f, 0, size.height * 0.5f );
		}

		set.AddVertexBuffer( vbParameters );

		std::vector< Index32 > indices( 6 * segments * segments );
		for( unsigned int v = 0; v < segments; ++v )
		{
			for( unsigned int h = 0; h < segments; ++h )
			{
				Index32 offset = 6 * (h + (v * segments));
				indices[offset + 0] = 0 + (segments + 1) * v + h;
				indices[offset + 1] = 1 + (segments + 1) * v + h;
				indices[offset + 2] = 0 + (segments + 1) * (v + 1) + h;
				indices[offset + 3] = 0 + (segments + 1) * (v + 1) + h;
				indices[offset + 4] = 1 + (segments + 1) * v + h;
				indices[offset + 5] = 1 + (segments + 1) * (v + 1) + h;
			}
		}

		set.AddIndexBuffer( { { { indexCount, &indices[0] } }, bufferUsage } );
	} catch( std::exception & ex )
	{
		throw me::exception::FailedToCreate( "Failed to create create shape " + parameters.Get< std::string >( "type" ) + ",  \"" + ex.what() + "\"" );
	}
}
