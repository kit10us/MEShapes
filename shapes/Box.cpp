// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <shapes/Box.h>
#include <me/render/RenderMethod.h>
#include <me/render/TextureMode.h>
#include <me/render/BufferUsage.h>
#include <me/render/VertexUtil.h>
#include <me/exception/NotImplemented.h>
#include <me/exception/FailedToCreate.h>
#include <unify/String.h>
#include <unify/Size3.h>
#include <unify/TexArea.h>
#include <unify/Angle.h>

using namespace shapes;
using namespace me;
using namespace render;

const std::string DefaultBufferUsage = "Default";

Box::Box()
{
}

Box::~Box()
{
}

void Box::Create( me::render::PrimitiveList & primitiveList, const unify::Parameters & parameters ) const
{
	using namespace unify;

	try
	{
		V3< float > inf;
		V3< float > sup;
		if( parameters.Exists( "inf,sup" ) )
		{
			inf = parameters.Get< V3< float > >( "inf" );
			sup = parameters.Get< V3< float > >( "sup" );
		}
		else
		{
			unify::Size3< float > size( parameters.Get( "size3", Size3< float >( 1.0f, 1.0f, 1.0f ) ) );
			// Divide the dimensions to center the Box
			size.width *= 0.5f;
			size.height *= 0.5f;
			size.depth *= 0.5f;
			inf = V3< float >( -size.width, -size.height, -size.depth );
			sup = V3< float >( size.width, size.height, size.depth );
		}

		Color diffuse = parameters.Get( "diffuse", ColorWhite() );
		Color specular = parameters.Get( "specular", ColorWhite() );
		int textureMode = parameters.Get( "texturemode", TextureMode::Correct );
		V3< float > center = parameters.Get( "center", V3< float >( 0, 0, 0 ) );
		Effect::ptr effect = parameters.Get< Effect::ptr >( "effect" );
		VertexDeclaration::ptr vd = effect->GetVertexShader()->GetVertexDeclaration();
		BufferUsage::TYPE bufferUsage = BufferUsage::FromString( parameters.Get( "bufferusage", DefaultBufferUsage ) );

		const unsigned int verticesPerSide = 4;
		const unsigned int facesPerSide = 2;
		const unsigned int indicesPerSide = 6;
		const unsigned int numberOfSides = 6;
		const unsigned int totalVertices = verticesPerSide * numberOfSides;
		const unsigned int totalIndices = indicesPerSide * numberOfSides;
		const unsigned int totalTriangles = facesPerSide * numberOfSides;

		char * verticesFinal = new char[totalVertices * vd->GetSizeInBytes( 0 )];
		DataLock lock( verticesFinal, vd->GetSizeInBytes( 0 ), totalVertices, DataLockAccess::ReadWrite, 0 );
		VertexBufferParameters vbParameters{ vd, { { totalVertices, verticesFinal } }, bufferUsage };

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

		BufferSet & set = primitiveList.AddBufferSet();
		set.SetEffect( effect );

		set.AddMethod( RenderMethod::CreateTriangleListIndexed( totalVertices, totalIndices, 0, 0 ) );

		// Set the TEMP vertices...
		V vertices[8];

		vertices[0].pos = V3< float >( inf.x, sup.y, sup.z );
		vertices[0].normal.Normalize( vertices[0].pos );
		vertices[0].coords = TexCoords( 0.0f, 0.0f );
		vertices[0].specular = specular;

		vertices[1].pos = V3< float >( sup.x, sup.y, sup.z );
		vertices[1].normal.Normalize( vertices[1].pos );
		vertices[1].coords = TexCoords( 0.0f, 1.0f );
		vertices[1].specular = specular;

		vertices[2].pos = V3< float >( inf.x, sup.y, inf.z );
		vertices[2].normal.Normalize( vertices[2].pos );
		vertices[2].coords = TexCoords( 1.0f, 0.0f );
		vertices[2].specular = specular;

		vertices[3].pos = V3< float >( sup.x, sup.y, inf.z );
		vertices[3].normal.Normalize( vertices[3].pos );
		vertices[3].coords = TexCoords( 1.0f, 1.0f );
		vertices[3].specular = specular;

		vertices[4].pos = V3< float >( inf.x, inf.y, sup.z );
		vertices[4].normal.Normalize( vertices[4].pos );
		vertices[4].coords = TexCoords( 0.0f, 0.0f );
		vertices[4].specular = specular;

		vertices[5].pos = V3< float >( sup.x, inf.y, sup.z );
		vertices[5].normal.Normalize( vertices[5].pos );
		vertices[5].coords = TexCoords( 0.0f, 1.0f );
		vertices[5].specular = specular;

		vertices[6].pos = V3< float >( inf.x, inf.y, inf.z );
		vertices[6].normal.Normalize( vertices[6].pos );
		vertices[6].coords = TexCoords( 1.0f, 0.0f );
		vertices[6].specular = specular;

		vertices[7].pos = V3< float >( sup.x, inf.y, inf.z );
		vertices[7].normal.Normalize( vertices[7].pos );
		vertices[7].coords = TexCoords( 1.0f, 1.0f );
		vertices[7].specular = specular;

		// Translate all points for center.
		for( unsigned int i = 0; i < 8; ++i )
		{
			vertices[i].pos += center;
			vbParameters.bbox += vertices[i].pos;
		}

		// Allow per-vertex diffuse...
		std::vector< Color > diffuses = parameters.Get< std::vector< Color > >( "diffuses", std::vector< unify::Color >() );
		if( diffuses.size() == 8 )
		{
			vertices[0].diffuse = diffuses[0];
			vertices[1].diffuse = diffuses[1];
			vertices[2].diffuse = diffuses[2];
			vertices[3].diffuse = diffuses[3];
			vertices[4].diffuse = diffuses[4];
			vertices[5].diffuse = diffuses[5];
			vertices[6].diffuse = diffuses[6];
			vertices[7].diffuse = diffuses[7];
		}
		// Else, assume 1 (correcting later where necessary)...
		else
		{
			vertices[0].diffuse = diffuse;
			vertices[1].diffuse = diffuse;
			vertices[2].diffuse = diffuse;
			vertices[3].diffuse = diffuse;
			vertices[4].diffuse = diffuse;
			vertices[5].diffuse = diffuse;
			vertices[6].diffuse = diffuse;
			vertices[7].diffuse = diffuse;
		}

		// Final positions...
		WriteVertex( *vd, lock, {  0,  9, 16 }, *vFormat, (void*)&vertices[0] );
		WriteVertex( *vd, lock, {  1,  8, 21 }, *vFormat, (void*)&vertices[1] );
		WriteVertex( *vd, lock, {  2, 12, 17 }, *vFormat, (void*)&vertices[2] );
		WriteVertex( *vd, lock, {  3, 13, 20 }, *vFormat, (void*)&vertices[3] );
		WriteVertex( *vd, lock, {  4, 11, 18 }, *vFormat, (void*)&vertices[4] );
		WriteVertex( *vd, lock, {  5, 10, 23 }, *vFormat, (void*)&vertices[5] );
		WriteVertex( *vd, lock, {  6, 14, 19 }, *vFormat, (void*)&vertices[6] );
		WriteVertex( *vd, lock, {  7, 15, 22 }, *vFormat, (void*)&vertices[7] );

		// Set the vertices texture coords...
		switch( textureMode )
		{
		case TextureMode::Correct:
		{
			int v, h;
			for( v = 0; v < 4; v++ )
			{
				for( h = 0; h < 4; h++ )
				{
					WriteVertex( *vd, lock, h + (v * 4), texE, vertices[h].coords );
				}
			}

			WriteVertex( *vd, lock, 16, texE, TexCoords( 0, 0 ) );
			WriteVertex( *vd, lock, 17, texE, TexCoords( 0, 1 ) );
			WriteVertex( *vd, lock, 18, texE, TexCoords( 1, 0 ) );
			WriteVertex( *vd, lock, 19, texE, TexCoords( 1, 1 ) );

			WriteVertex( *vd, lock, 20, texE, TexCoords( 0, 0 ) );
			WriteVertex( *vd, lock, 21, texE, TexCoords( 0, 1 ) );
			WriteVertex( *vd, lock, 22, texE, TexCoords( 1, 0 ) );
			WriteVertex( *vd, lock, 23, texE, TexCoords( 1, 1 ) );
		} break;

		case TextureMode::Wrapped:
		{
			float l1 = 0, l2 = 0.33f;
			float m1 = 0.33f, m2 = 0.66f;
			float r1 = 0.66f, r2 = 1;

			float t1 = 0, t2 = 0.5f;
			float b1 = 0.5f, b2 = 1;

			// Left Side..
			WriteVertex( *vd, lock, 12, texE, TexCoords( l1, b1 ) );
			WriteVertex( *vd, lock, 13, texE, TexCoords( l1, b2 ) );
			WriteVertex( *vd, lock, 14, texE, TexCoords( l2, b1 ) );
			WriteVertex( *vd, lock, 15, texE, TexCoords( l2, b2 ) );

			// Front...
			WriteVertex( *vd, lock, 0, texE, TexCoords( m1, b1 ) );
			WriteVertex( *vd, lock, 1, texE, TexCoords( m1, b2 ) );
			WriteVertex( *vd, lock, 2, texE, TexCoords( m2, b1 ) );
			WriteVertex( *vd, lock, 3, texE, TexCoords( m2, b2 ) );

			// Right Side...
			WriteVertex( *vd, lock, 4, texE, TexCoords( r1, b1 ) );
			WriteVertex( *vd, lock, 5, texE, TexCoords( r1, b2 ) );
			WriteVertex( *vd, lock, 6, texE, TexCoords( r2, b1 ) );
			WriteVertex( *vd, lock, 7, texE, TexCoords( r2, b2 ) );

			// Rear...
			WriteVertex( *vd, lock, 8, texE, TexCoords( l1, t1 ) );
			WriteVertex( *vd, lock, 9, texE, TexCoords( l1, t2 ) );
			WriteVertex( *vd, lock, 10, texE, TexCoords( l2, t1 ) );
			WriteVertex( *vd, lock, 11, texE, TexCoords( l2, t2 ) );

			// Top...
			WriteVertex( *vd, lock, 16, texE, TexCoords( m1, t1 ) );
			WriteVertex( *vd, lock, 17, texE, TexCoords( m1, t2 ) );
			WriteVertex( *vd, lock, 18, texE, TexCoords( m2, t1 ) );
			WriteVertex( *vd, lock, 19, texE, TexCoords( m2, t2 ) );

			// Bottom...
			WriteVertex( *vd, lock, 20, texE, TexCoords( r1, t1 ) );
			WriteVertex( *vd, lock, 21, texE, TexCoords( r1, t2 ) );
			WriteVertex( *vd, lock, 22, texE, TexCoords( r2, t1 ) );
			WriteVertex( *vd, lock, 23, texE, TexCoords( r2, t2 ) );
		} break;
		}

		// If we have a faceUV vector...
		if( parameters.Exists( "faceUV" ) )
		{
			std::vector< TexArea > faceUV = parameters.Get< std::vector< TexArea > >( "faceUV" );

			// Loop as long as it takes to get 6 faces
			size_t face = 0;
			while( face != 6 )
			{
				for( auto texArea : faceUV )
				{	   
					WriteVertex( *vd, lock, face * 4 + 0, texE, texArea.UL() );
					WriteVertex( *vd, lock, face * 4 + 1, texE, texArea.UR() );
					WriteVertex( *vd, lock, face * 4 + 2, texE, texArea.DL() );
					WriteVertex( *vd, lock, face * 4 + 3, texE, texArea.DR() );
					face++;
				}
			}
		}

		// Allow per-face diffuse...
		if( diffuses.size() == 6 )
		{
			// Front...
			WriteVertex( *vd, lock, { 0, 1, 2, 3 }, diffuseE, diffuses[1] );

			// Right Side...
			WriteVertex( *vd, lock, { 4, 5, 6, 7 }, diffuseE, diffuses[2] );

			// Rear...
			WriteVertex( *vd, lock, { 8, 9, 10, 11 }, diffuseE, diffuses[3] );

			// Left Side..
			WriteVertex( *vd, lock, { 12, 13, 14, 15 }, diffuseE, diffuses[0] );

			// Top...
			WriteVertex( *vd, lock, { 16, 17, 18, 19 }, diffuseE, diffuses[4] );

			// Bottom...
			WriteVertex( *vd, lock, { 20, 21, 22, 23 }, diffuseE, diffuses[5] );
		}

		set.AddVertexBuffer( vbParameters );

		delete[] verticesFinal;

		// Set the Indices..
		Index32 indices[36] =
		{
			// Top	 (+y)
			0, 1, 2,
			1, 3, 2,

			// Bottom (-y)
			4, 5, 6,
			5, 7, 6,

			// Front  (-z)
			8, 9, 10,
			9, 11, 10,
				
			// Back   (-x)
			12, 13, 14,
			13, 15, 14,

			// Left  (+x)
			16, 17, 18,
			17, 19, 18,

			// Back (-y)
			20, 21, 22,
			21, 23, 22
		};

		set.AddIndexBuffer( { { { totalIndices, &indices[0] } }, bufferUsage } );
	}
	catch( std::exception & ex )
	{
		throw me::exception::FailedToCreate( "Failed to create create shape " + parameters.Get< std::string >( "type" ) + ",  \"" + ex.what() + "\"" );
	}
}
