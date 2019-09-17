// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <shapes/Cylinder.h>
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

const float PI = 3.14159265358979f;
const float PI2 = 6.28318530717959f;
const std::string DefaultBufferUsage = "Default";

Cylinder::Cylinder()
{
}

Cylinder::~Cylinder()
{
}

void Cylinder::Create(PrimitiveList & primitiveList, const unify::Parameters & parameters ) const
{
	using namespace unify;

	try
	{
		float radius = parameters.Get( "radius", 0.5f );
		unsigned int segments = parameters.Get< unsigned int >( "segments", 12 );
		float height = parameters.Get( "height", 1.0f );
		Color diffuse = parameters.Get( "diffuse", ColorWhite() );
		Color specular = parameters.Get( "specular", ColorWhite() );
		V3< float > center = parameters.Get( "center", V3< float >( 0, 0, 0 ) );
		TexArea texArea = parameters.Get< TexArea >( "texarea", TexArea( unify::TexCoords( 0, 0 ), TexCoords( 1, 1 ) ) );
		// TODO: support top and bottom texArea.
		Effect::ptr effect = parameters.Get< Effect::ptr >( "effect" );
		VertexDeclaration::ptr vd = effect->GetVertexShader()->GetVertexDeclaration();
		bool caps = parameters.Get( "caps", true );
		BufferUsage::TYPE bufferUsage = BufferUsage::FromString( parameters.Get( "bufferusage", DefaultBufferUsage ) );

		if( segments < 3 ) segments = 3;

		height *= 0.5f;

		size_t vertexCount = (segments + 1) * 2;
		size_t indexCount = 0;
		if( caps )
		{
			vertexCount += (segments + 2) * 2;
			indexCount = segments * 3 * 2;
		}

		BufferSet & set = primitiveList.AddBufferSet();
		set.SetEffect( effect );

		std::shared_ptr< unsigned char > vertices( new unsigned char[vd->GetSizeInBytes( 0 ) * vertexCount] );
		DataLock lock( vertices.get(), vd->GetSizeInBytes( 0 ), vertexCount, DataLockAccess::ReadWrite, 0 );

		std::vector< Index32 > indices( indexCount );
		VertexBufferParameters vbParameters{ vd, { { vertexCount, vertices.get() } }, bufferUsage };

		// Method 1 - Triangle Strip (sides)
		set.AddMethod( RenderMethod::CreateTriangleStrip( 0, segments * 2 ) );

		if( caps )
		{
			set.AddMethod( RenderMethod::CreateTriangleListIndexed( segments + 1, segments * 3, 0, 0 ) );
			set.AddMethod( RenderMethod::CreateTriangleListIndexed( segments + 1, segments * 3, segments * 3, 0 ) );
		}

		unsigned short stream = 0;

		VertexElement positionE = CommonVertexElement::Position( stream );
		VertexElement normalE = CommonVertexElement::Normal( stream );
		VertexElement diffuseE = CommonVertexElement::Diffuse( stream );
		VertexElement specularE = CommonVertexElement::Specular( stream );
		VertexElement texE = CommonVertexElement::TexCoords( stream );
	
		V3< float > pos;
		V3< float > norm;
		float rad = 0;
		float radChange = PI2 / segments;
		TexCoords cChange;
		cChange.u = (texArea.dr.u - texArea.ul.u) / segments;

		// Sides...
		double dRad = 0;
		double dRadChange = PI2 / segments;
		for( unsigned int s = 0; s <= segments; s++ )
		{
			pos = unify::V3< float >( sin( rad ) * radius, -height, cos( rad ) * radius );
			norm = pos;
			norm.Normalize();
			WriteVertex( *vd, lock, (s * 2) + 0, positionE, pos + center );
			WriteVertex( *vd, lock, (s * 2) + 0, normalE, norm );
			WriteVertex( *vd, lock, (s * 2) + 0, texE, TexCoords( cChange.u * s, texArea.dr.v ) );
			WriteVertex( *vd, lock, (s * 2) + 0, diffuseE, diffuse );
			WriteVertex( *vd, lock, (s * 2) + 0, specularE, specular );
			vbParameters.bbox += pos + center;

			pos = unify::V3< float >( sin( rad ) * radius, height, cos( rad ) * radius );
			norm = pos;
			norm.Normalize();
			WriteVertex( *vd, lock, (s * 2) + 1, positionE, pos + center );
			WriteVertex( *vd, lock, (s * 2) + 1, normalE, norm );
			WriteVertex( *vd, lock, (s * 2) + 1, texE, TexCoords( cChange.u * s, texArea.ul.v ) );
			WriteVertex( *vd, lock, (s * 2) + 1, diffuseE, diffuse );
			WriteVertex( *vd, lock, (s * 2) + 1, specularE, specular );
			vbParameters.bbox += pos + center;

			if( caps )
			{
				pos = unify::V3< float >( sin( rad ) * radius, height, cos( rad ) * radius );
				norm = pos;
				norm.Normalize();
				WriteVertex( *vd, lock, (segments * 2 + 2) + s, positionE, pos + center );
				WriteVertex( *vd, lock, (segments * 2 + 2) + s, normalE, norm );
				WriteVertex( *vd, lock, (segments * 2 + 2) + s, texE, TexCoords( 0.5f + (float)(sin( rad ) * 0.5f), 0.5f + (float)(cos( rad ) * -0.5f) ) );
				WriteVertex( *vd, lock, (segments * 2 + 2) + s, diffuseE, diffuse );
				WriteVertex( *vd, lock, (segments * 2 + 2) + s, specularE, specular );
				vbParameters.bbox += pos + center;

				pos = unify::V3< float >( cos( rad ) * radius, -height, sin( rad ) * radius );
				norm = pos;
				norm.Normalize();
				WriteVertex( *vd, lock, (segments * 2 + 2) + (segments + 2) + s, positionE, pos + center );
				WriteVertex( *vd, lock, (segments * 2 + 2) + (segments + 2) + s, normalE, norm );
				WriteVertex( *vd, lock, (segments * 2 + 2) + (segments + 2) + s, texE, TexCoords( 0.5f + (float)(sin( rad ) * 0.5f), 0.5f + (float)(cos( rad ) * -0.5f) ) );
				WriteVertex( *vd, lock, (segments * 2 + 2) + (segments + 2) + s, diffuseE, diffuse );
				WriteVertex( *vd, lock, (segments * 2 + 2) + (segments + 2) + s, specularE, specular );
				vbParameters.bbox += pos + center;
			}
			rad += radChange;
		}

		if( caps )
		{
			for( unsigned int s = 0; s < segments; ++s )
			{
				indices[0 + s * 3] = (segments * 2) + 2 + (s);
				indices[1 + s * 3] = (segments * 2) + 2 + (s)+1;
				indices[2 + s * 3] = (segments * 2) + 2 + segments + 1;

				indices[(segments * 3) + 0 + s * 3] = (segments * 2) + 2 + segments + 2 + (s);
				indices[(segments * 3) + 2 + s * 3] = (segments * 2) + 2 + segments + 2 + (s)+1;
				indices[(segments * 3) + 1 + s * 3] = (segments * 2) + 2 + segments + 2 + segments + 1;
			}

			pos = unify::V3< float >( 0, height, 0 );
			norm = pos;
			norm.Normalize();
			WriteVertex( *vd, lock, segments * 2 + 2 + segments + 1, positionE, pos + center );
			WriteVertex( *vd, lock, segments * 2 + 2 + segments + 1, normalE, norm );
			WriteVertex( *vd, lock, segments * 2 + 2 + segments + 1, texE, TexCoords( 0.5f, 0.5f ) );
			WriteVertex( *vd, lock, segments * 2 + 2 + segments + 1, diffuseE, diffuse );
			WriteVertex( *vd, lock, segments * 2 + 2 + segments + 1, specularE, specular );
			vbParameters.bbox += pos + center;

			pos.y = -height;
			norm = pos;
			norm.Normalize();
			WriteVertex( *vd, lock, segments * 2 + 2 + ((segments + 1) * 2) + 1, positionE, pos + center );
			WriteVertex( *vd, lock, segments * 2 + 2 + ((segments + 1) * 2) + 1, normalE, norm );
			WriteVertex( *vd, lock, segments * 2 + 2 + ((segments + 1) * 2) + 1, texE, TexCoords( 0.5f, 0.5f ) );
			WriteVertex( *vd, lock, segments * 2 + 2 + ((segments + 1) * 2) + 1, diffuseE, diffuse );
			WriteVertex( *vd, lock, segments * 2 + 2 + ((segments + 1) * 2) + 1, specularE, specular );
			vbParameters.bbox += pos + center;
		}

		set.AddVertexBuffer( vbParameters );

		if( indexCount > 0 )
		{
			set.AddIndexBuffer( { { { indexCount, &indices[0] } }, bufferUsage } );
		}
	}
	catch( std::exception & ex )
	{
		throw me::exception::FailedToCreate( "Failed to create create shape " + parameters.Get< std::string >( "type" ) + ",  \"" + ex.what() + "\"" );
	}
}
