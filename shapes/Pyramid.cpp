// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <shapes/Pyramid.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToCreate.h>
#include <me/render/VertexUtil.h>
#include <unify/String.h>
#include <unify/Size3.h>
#include <unify/TexArea.h>
#include <unify/Angle.h>

using namespace me;
using namespace render;
using namespace shapes;

const std::string DefaultBufferUsage = "Default";

Pyramid::Pyramid()
{
}

Pyramid::~Pyramid()
{
}

void Pyramid::Create(PrimitiveList & primitiveList, const unify::Parameters & parameters ) const
{
	using namespace unify;

	try
	{
		Size3< float > size( parameters.Get( "size3", unify::Size3< float >( 1.0f, 1.0f, 1.0f ) ) );
		Color diffuse = parameters.Get( "diffuse", unify::ColorWhite() );
		Color specular = parameters.Get( "specular", unify::ColorWhite() );
		int textureMode = parameters.Get( "texturemode", TextureMode::Correct );
		V3< float > center = parameters.Get( "center", V3< float >( 0, 0, 0 ) );
		Effect::ptr effect = parameters.Get< Effect::ptr >( "effect" );
		VertexDeclaration::ptr vd = effect->GetVertexShader()->GetVertexDeclaration();
		BufferUsage::TYPE bufferUsage = BufferUsage::FromString( parameters.Get( "bufferusage", DefaultBufferUsage ) );
		size_t vertexCount = 16;
		size_t indexCount = 18;

		// Divide the dimensions to center the cube
		size *= 0.5f;

		BufferSet & set = primitiveList.AddBufferSet();
		set.SetEffect( effect );

		set.AddMethod( RenderMethod::CreateTriangleListIndexed( vertexCount, (unsigned int)indexCount, 0, 0 ) );

		unsigned short stream = 0;

		VertexElement positionE = CommonVertexElement::Position( stream );
		VertexElement normalE = CommonVertexElement::Normal( stream );
		VertexElement diffuseE = CommonVertexElement::Diffuse( stream );
		VertexElement specularE = CommonVertexElement::Specular( stream );
		VertexElement texE = CommonVertexElement::TexCoords( stream );

		class V
		{
		public:
			unify::V3< float > pos;
			unify::V3< float > normal;
			unify::Color diffuse;
			unify::Color specular;
			unify::TexCoords coords;
		};
		qjson::Object jsonFormat;
		jsonFormat.Add( { "Position", "Float3" } );
		jsonFormat.Add( { "Normal", "Float3" } );
		jsonFormat.Add( { "Diffuse", "Color" } );
		jsonFormat.Add( { "Specular", "Color" } );
		jsonFormat.Add( { "TexCoord", "TexCoord" } );
		VertexDeclaration::ptr vFormat( new VertexDeclaration( jsonFormat ) );

		// Set the vertices from the TEMP vertices...
		std::shared_ptr< unsigned char > verticesRaw( new unsigned char[vd->GetSizeInBytes( 0 ) * vertexCount] );
		DataLock lock( verticesRaw.get(), vd->GetSizeInBytes( 0 ), vertexCount, DataLockAccess::ReadWrite, 0 );

		VertexBufferParameters vbParameters{ vd, { { vertexCount, verticesRaw.get() } }, bufferUsage };

		// Set the TEMP vertices...
		V vertices[5];

		// Top point
		vertices[0].pos = unify::V3< float >( 0, size.height, 0 );
		vertices[0].normal.Normalize( vertices[0].pos );
		vertices[0].coords = unify::TexCoords( 0.5f, 0.5f );
		vertices[0].specular = specular;

		// FL
		vertices[1].pos = unify::V3< float >( -size.width, -size.height, -size.depth );
		vertices[1].normal.Normalize( vertices[1].pos );
		vertices[1].coords = unify::TexCoords( 0.0f, 1.0f );
		vertices[1].specular = specular;

		// FR
		vertices[2].pos = unify::V3< float >( size.width, -size.height, -size.depth );
		vertices[2].normal.Normalize( vertices[2].pos );
		vertices[2].coords = unify::TexCoords( 1, 1 );
		vertices[2].specular = specular;

		// BL
		vertices[3].pos = unify::V3< float >( size.width, -size.height, size.depth );
		vertices[3].normal.Normalize( vertices[3].pos );
		vertices[3].coords = unify::TexCoords( 0, 1 );
		vertices[3].specular = specular;

		// BR
		vertices[4].pos = unify::V3< float >( -size.width, -size.height, size.depth );
		vertices[4].normal.Normalize( vertices[4].pos );
		vertices[4].coords = unify::TexCoords( 1, 1 );
		vertices[4].specular = specular;

		// Translate all points for center.
		for( unsigned int i = 0; i < 5; ++i )
		{
			vertices[i].pos += center;
			vbParameters.bbox += vertices[i].pos;
		}

		// Allow per-vertex diffuse...
		std::vector< unify::Color > diffuses = parameters.Get< std::vector< unify::Color > >( "diffuses", std::vector< unify::Color >() );
		if( diffuses.size() == 5 )
		{
			vertices[0].diffuse = diffuses[0];
			vertices[1].diffuse = diffuses[1];
			vertices[2].diffuse = diffuses[2];
			vertices[3].diffuse = diffuses[3];
			vertices[4].diffuse = diffuses[4];
		}
		// Else, assume 1 (correcting later where necessary)...
		else
		{
			vertices[0].diffuse = diffuse;
			vertices[1].diffuse = diffuse;
			vertices[2].diffuse = diffuse;
			vertices[3].diffuse = diffuse;
			vertices[4].diffuse = diffuse;
		}

		WriteVertex( *vd, lock, { 0, 3, 6, 9 }, *vFormat, &vertices[0] );
		WriteVertex( *vd, lock, { 1, 11, 12 }, *vFormat, &vertices[1] );
		WriteVertex( *vd, lock, { 2, 4, 14 }, *vFormat, &vertices[2] );
		WriteVertex( *vd, lock, { 5, 7, 15 }, *vFormat, &vertices[3] );
		WriteVertex( *vd, lock, { 8, 10, 13 }, *vFormat, &vertices[4] );

		// Set the vertices texture coords...
		switch( textureMode )
		{
		case TextureMode::Correct:
		{
			// Sides
			for( int s = 0; s < 4; s++ )
			{
				WriteVertex( *vd, lock, (s * 3), texE, vertices[0].coords );
				WriteVertex( *vd, lock, (s * 3) + 1, texE, vertices[1].coords );
				WriteVertex( *vd, lock, (s * 3) + 2, texE, vertices[2].coords );
			}

			// Bottom
			WriteVertex( *vd, lock, 12, texE, unify::TexCoords( 0, 0 ) );
			WriteVertex( *vd, lock, 13, texE, unify::TexCoords( 0, 1 ) );
			WriteVertex( *vd, lock, 14, texE, unify::TexCoords( 1, 0 ) );
			WriteVertex( *vd, lock, 15, texE, unify::TexCoords( 1, 1 ) );

		} break;

		case TextureMode::Wrapped:
		{
			float l1 = 0, l2 = 0.33f;
			float m1 = 0.33f, m2 = 0.66f;
			float r1 = 0.66f, r2 = 1;

			float t1 = 0, t2 = 0.5f;
			float b1 = 0.5f, b2 = 1;

			// Left Side...
			WriteVertex( *vd, lock, 9, texE, unify::TexCoords( 0.1667f, b1 ) );
			WriteVertex( *vd, lock, 10, texE, unify::TexCoords( l1, b2 ) );
			WriteVertex( *vd, lock, 11, texE, unify::TexCoords( l2, b2 ) );

			// Front...
			WriteVertex( *vd, lock, 0, texE, unify::TexCoords( 0.5f, b1 ) );
			WriteVertex( *vd, lock, 1, texE, unify::TexCoords( m1, b2 ) );
			WriteVertex( *vd, lock, 2, texE, unify::TexCoords( m2, b2 ) );

			// Right Side...
			WriteVertex( *vd, lock, 3, texE, unify::TexCoords( 0.8333f, b1 ) );
			WriteVertex( *vd, lock, 4, texE, unify::TexCoords( r1, b2 ) );
			WriteVertex( *vd, lock, 5, texE, unify::TexCoords( r2, b2 ) );

			// Rear...
			WriteVertex( *vd, lock, 6, texE, unify::TexCoords( 0.1667f, t1 ) );
			WriteVertex( *vd, lock, 7, texE, unify::TexCoords( l1, t2 ) );
			WriteVertex( *vd, lock, 8, texE, unify::TexCoords( l2, t2 ) );

			// Bottom...
			WriteVertex( *vd, lock, 12, texE, unify::TexCoords( r1, t1 ) );
			WriteVertex( *vd, lock, 13, texE, unify::TexCoords( r1, t2 ) );
			WriteVertex( *vd, lock, 14, texE, unify::TexCoords( r2, t1 ) );
			WriteVertex( *vd, lock, 15, texE, unify::TexCoords( r2, t2 ) );
		}break;
		}

		set.AddVertexBuffer( vbParameters );

		// Set the Indices..
		Index32 indices[18] =
		{
			// Front
			0, 2, 1,

			// R Side
			3, 5, 4,

			// Back
			6, 8, 7,

			// L Side
			9, 11, 10,

			// Bottom
			12, 14, 13,
			14, 15, 13
		};

		set.AddIndexBuffer( { { { indexCount, indices } }, bufferUsage } );
	}
	catch( std::exception & ex )
	{
		throw me::exception::FailedToCreate( "Failed to create create shape " + parameters.Get< std::string >( "type" ) + ",  \"" + ex.what() + "\"" );
	}
}

