// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <shapes/Tube.h>
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

const float PI = 3.14159265358979f;
const float PI2 = 6.28318530717959f;
const std::string DefaultBufferUsage = "Default";

Tube::Tube()
{
}

Tube::~Tube()
{
}

void Tube::Create(PrimitiveList & primitiveList, const unify::Parameters & parameters ) const
{
	using namespace unify;
	
	try
	{
		float outer = parameters.Get( "majorradius", 0.5f );
		float inner = parameters.Get( "minorradius", 0.3f );
		unsigned int segments = parameters.Get< unsigned int >( "segments", 12 );
		float height = parameters.Get( "height", 1.0f );
		Color diffuse = parameters.Get( "diffuse", unify::ColorWhite() );
		Color specular = parameters.Get( "specular", unify::ColorWhite() );
		V3< float > center = parameters.Get( "center", unify::V3< float >( 0, 0, 0 ) );
		Effect::ptr effect = parameters.Get< Effect::ptr >( "effect" );
		VertexDeclaration::ptr vd = effect->GetVertexShader()->GetVertexDeclaration();
		BufferUsage::TYPE bufferUsage = BufferUsage::FromString( parameters.Get( "bufferusage", DefaultBufferUsage ) );

		if( segments < 3 ) segments = 3;

		size_t vertexCount = RenderMethod::VertexCountInATriangleStrip( segments * 2 ) * 4;

		// Height is distance from origin/center.
		height *= 0.5f;

		// a segment is made up of two triangles... segments * 2 = NumTriangles
		BufferSet & set = primitiveList.AddBufferSet();
		set.SetEffect( effect );

		std::shared_ptr< unsigned char > vertices( new unsigned char[vd->GetSizeInBytes( 0 ) * vertexCount] );
		DataLock lock( vertices.get(), vd->GetSizeInBytes( 0 ), vertexCount, DataLockAccess::ReadWrite, 0 );

		VertexBufferParameters vbParameters{ vd, { { vertexCount, vertices.get() } }, bufferUsage };

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

		unsigned int trianglesPerSide = segments * 2;
		unsigned int verticesPerSide = segments * 2 + 2;

		// Method 1 - Triangle Strip (Top)
		set.AddMethod( RenderMethod::CreateTriangleStrip( 0 * verticesPerSide, trianglesPerSide ) );

		// Method 2 - Triangle Strip (Bottom)
		set.AddMethod( RenderMethod::CreateTriangleStrip( 1 * verticesPerSide, trianglesPerSide ) );

		// Method 3 - Triangle Strip (Outside)
		set.AddMethod( RenderMethod::CreateTriangleStrip( 2 * verticesPerSide, trianglesPerSide ) );

		// Method 4 - Triangle Strip (Inside)
		set.AddMethod( RenderMethod::CreateTriangleStrip( 3 * verticesPerSide, trianglesPerSide ) );

		V vertex;
		float radiansChange = PI2 / segments;
		float ratioT = inner / outer;
		for( unsigned int v = 0; v < (segments + 1); v++ )
		{
			float radians = radiansChange * v;

			unify::V2< float > coord( sin( radians ), cos( radians ) );

			// Method 1 - Triangle Strip (Top)
			// Outside edge
			vertex.pos = unify::V3< float >( coord.x * outer, height, coord.y * outer );
			vertex.pos += center;
			vertex.normal = unify::V3< float >( 0, 1, 0 );
			vertex.coords = unify::TexCoords( 0.5f + (coord.x * 0.5f), 0.5f + (coord.y * -0.5f) );
			vertex.diffuse = diffuse;
			vertex.specular = specular;
			WriteVertex( *vd, lock, (0 * verticesPerSide) + (v * 2), *vFormat, &vertex );
			vbParameters.bbox += vertex.pos;


			// Inside edge
			vertex.pos = unify::V3< float >( coord.x * inner, height, coord.y * inner );
			vertex.pos += center;
			vertex.normal = unify::V3< float >( 0, 1, 0 );
			vertex.coords = unify::TexCoords( 0.5f + (coord.x * 0.5f * ratioT), 0.5f + (coord.y * -0.5f * ratioT) );
			vertex.diffuse = diffuse;
			vertex.specular = specular;
			WriteVertex( *vd, lock, (0 * verticesPerSide) + (v * 2) + 1, *vFormat, &vertex );
			vbParameters.bbox += vertex.pos;

			// Method 2 - Triangle Strip (Bottom)
			// Outside edge
			vertex.pos = unify::V3< float >( coord.x * outer, -height, coord.y * outer );
			vertex.pos += center;
			vertex.normal = unify::V3< float >( 0, -1, 0 );
			vertex.coords = unify::TexCoords( 0.5f + (coord.x * 0.5f), 0.5f + (coord.y * -0.5f) );
			vertex.diffuse = diffuse;
			vertex.specular = specular;
			WriteVertex( *vd, lock, (1 * verticesPerSide) + (v * 2), *vFormat, &vertex );
			vbParameters.bbox += vertex.pos;

			// Inside edge
			vertex.pos = unify::V3< float >( coord.x * inner, -height, coord.y * inner );
			vertex.pos += center;
			vertex.normal = unify::V3< float >( 0, -1, 0 );
			vertex.coords = unify::TexCoords( 0.5f + (coord.x * 0.5f * ratioT), 0.5f + (coord.y * -0.5f * ratioT) );
			vertex.diffuse = diffuse;
			vertex.specular = specular;
			WriteVertex( *vd, lock, (1 * verticesPerSide) + (v * 2) + 1, *vFormat, &vertex );
			vbParameters.bbox += vertex.pos;

			// Method 3 - Triangle Strip (Outside)
			// Top
			vertex.pos = unify::V3< float >( coord.x * outer, height, coord.y * outer );
			vertex.pos += center;
			vertex.normal = unify::V3< float >( coord.x, 0, coord.y );
			vertex.coords = unify::TexCoords( (1.0f / segments) * v, 0 );
			vertex.diffuse = diffuse;
			vertex.specular = specular;
			WriteVertex( *vd, lock, (2 * verticesPerSide) + (v * 2), *vFormat, &vertex );
			vbParameters.bbox += vertex.pos;

			// Bottom
			vertex.pos = unify::V3< float >( coord.x * outer, -height, coord.y * outer );
			vertex.pos += center;
			vertex.normal = unify::V3< float >( coord.x, 0, coord.y );
			vertex.coords = unify::TexCoords( (1.0f / segments) * v, 1 );
			vertex.diffuse = diffuse;
			vertex.specular = specular;
			WriteVertex( *vd, lock, (2 * verticesPerSide) + (v * 2) + 1, *vFormat, &vertex );
			vbParameters.bbox += vertex.pos;

			// Method 4 - Triangle Strip (Inside)
			// Top
			vertex.pos = unify::V3< float >( coord.x * inner, height, coord.y * inner );
			vertex.pos += center;
			vertex.normal = unify::V3< float >( coord.x, 0, coord.y );
			vertex.coords = unify::TexCoords( (1.0f / segments) * v, 0 );
			vertex.diffuse = diffuse;
			vertex.specular = specular;
			WriteVertex( *vd, lock, (3 * verticesPerSide) + (v * 2), *vFormat, &vertex );
			vbParameters.bbox += vertex.pos;

			// Bottom
			vertex.pos = unify::V3< float >( coord.x * inner, -height, coord.y * inner );
			vertex.pos += center;
			vertex.normal = unify::V3< float >( coord.x, 0, coord.y );
			vertex.coords = unify::TexCoords( (1.0f / segments) * v, 1 );
			vertex.diffuse = diffuse;
			vertex.specular = specular;
			WriteVertex( *vd, lock, (3 * verticesPerSide) + (v * 2) + 1, *vFormat, &vertex );
			vbParameters.bbox += vertex.pos;
		}

		set.AddVertexBuffer( vbParameters );
	}
	catch( std::exception & ex )
	{
		throw me::exception::FailedToCreate( "Failed to create create shape " + parameters.Get< std::string >( "type" ) + ",  \"" + ex.what() + "\"" );
	}
}
