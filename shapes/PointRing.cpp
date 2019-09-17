// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <shapes/PointRing.h>
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


PointRing::PointRing()
{
}

PointRing::~PointRing()
{
}

void PointRing::Create(PrimitiveList & primitiveList, const unify::Parameters & parameters ) const
{
	using namespace unify;

	try
	{
		float majorRadius = parameters.Get( "majorradius", 0.5f );	// Size radius
		float minorRadius = parameters.Get( "minorradius", 1.0f );	// Radius of ring
		unsigned int count = parameters.Get( "count", 100 );
		Color diffuse = parameters.Get( "diffuse", unify::ColorWhite() );
		Color specular = parameters.Get( "specular", unify::ColorWhite() );
		V3< float > center = parameters.Get( "center", unify::V3< float >( 0, 0, 0 ) );
		Effect::ptr effect = parameters.Get< Effect::ptr >( "effect" );
		VertexDeclaration::ptr vd = effect->GetVertexShader()->GetVertexDeclaration();
		BufferUsage::TYPE bufferUsage = BufferUsage::FromString( parameters.Get( "bufferusage", DefaultBufferUsage ) );

		BufferSet & set = primitiveList.AddBufferSet();
		set.SetEffect( effect );

		set.AddMethod( RenderMethod::CreatePointList( 0, count ) );

		V3< float > vec;
		V2< float > vPos2;	// Initial position ( by radius1 )
		V3< float > vPos3;
		V3< float > norm;

		char * vertices = new char[vd->GetSizeInBytes( 0 ) * count];
		DataLock lock( vertices, vd->GetSizeInBytes( 0 ), count, DataLockAccess::ReadWrite, 0 );
		VertexBufferParameters vbParameters{ vd, { { count, vertices } }, bufferUsage };
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

		for( unsigned int v = 0; v < count; v++ )
		{
			// Generate initial position...
			vPos2.x = (float)(rand() % 10000) * 0.0001f;
			vPos2.y = (float)(rand() % 10000) * 0.0001f;
			vPos2.Normalize();
			vPos2 *= majorRadius;

			// Allow inversions...
			if( (rand() % 2) ) vPos2.x *= -1.0f;
			if( (rand() % 2) ) vPos2.y *= -1.0f;

			vec.x = vPos2.x;
			vec.z = vPos2.y;
			vec.y = 0;

			// Generate from Radius2
			vPos3.x = (float)(rand() % 10000) * 0.0001f;
			vPos3.y = (float)(rand() % 10000) * 0.0001f;
			vPos3.z = (float)(rand() % 10000) * 0.0001f;
			vPos3.Normalize();
			vPos3 *= minorRadius;

			// Allow inversions...
			if( (rand() % 2) ) vPos3.x *= -1.0f;
			if( (rand() % 2) ) vPos3.y *= -1.0f;
			if( (rand() % 2) ) vPos3.z *= -1.0f;

			vec += vPos3;

			norm = vec;
			norm.Normalize();

			vec += center;

			WriteVertex( *vd, lock, v, positionE, vec );
			WriteVertex( *vd, lock, v, normalE, norm );
			WriteVertex( *vd, lock, v, diffuseE, diffuse );
			WriteVertex( *vd, lock, v, specularE, specular );   
			vbParameters.bbox += vec;
		}
		set.AddVertexBuffer( vbParameters );
		delete[] vertices;
	} catch( std::exception & ex )
	{
		throw me::exception::FailedToCreate( "Failed to create create shape " + parameters.Get< std::string >( "type" ) + ",  \"" + ex.what() + "\"" );
	}
}
