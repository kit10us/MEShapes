// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <MainScene.h>
#include <me/scene/SceneManager.h>
#include <me/object/Object.h>
#include <me/render/Mesh.h>
#include <me/factory/PixelShaderFactories.h>
#include <me/factory/VertexShaderFactory.h>
#include <me/object/component/BBoxRendererComponent.h>
#include <me/scene/component/AutoBBoxSceneComponent.h>
#include <me/object/component/CameraComponent.h>
#include <me/sculpter/SculpterFactory.h>
#include <me/sculpter/IShapeCreator.h>
#include <me/sculpter/IVertexBuilder.h>
#include <me/sculpter/IFaceBuilder.h>

#include <algorithm>
#include <math.h>

using namespace me;
using namespace render;

class Cube : public sculpter::IShapeCreator
{
public:
	Cube() {}
	void Create( me::render::PrimitiveList & primitiveList, me::render::Effect::ptr effect )
	{
		class CubeVertexBuilder : public sculpter::IVertexBuilder
		{
		public:
			sculpter::VertexOutput Build( sculpter::SheetStats sheetStats, sculpter::VertexInput vertexInput ) override
			{
				using namespace unify;
				sculpter::VertexOutput output {};

				V3< float > inf;
				V3< float > sup;
				if( sheetStats.parameters.Exists( "inf,sup" ) )
				{
					inf = sheetStats.parameters.Get< V3< float > >( "inf" );
					sup = sheetStats.parameters.Get< V3< float > >( "sup" );
				}
				else
				{
					unify::Size3< float > size( sheetStats.parameters.Get( "size3", Size3< float >( 1.0f, 1.0f, 1.0f ) ) );
					// Divide the dimensions to center the cube
					size.width *= 0.5f;
					size.height *= 0.5f;
					size.depth *= 0.5f;
					inf = V3< float >( -size.width, -size.height, -size.depth );
					sup = V3< float >( size.width, size.height, size.depth );
				}

				Color diffuse = sheetStats.parameters.Get( "diffuse", ColorWhite() );
				Color specular = sheetStats.parameters.Get( "specular", ColorWhite() );
			
				V3< float > center = sheetStats.parameters.Get( "center", V3< float >( 0, 0, 0 ) );

				switch( vertexInput.index )
				{
				case 0:			
					output.vertex.pos = V3< float >( inf.x, sup.y, sup.z );
					output.vertex.coords = TexCoords( 0.0f, 0.0f );
					break;

				case 1:
					output.vertex.pos = V3< float >( sup.x, sup.y, sup.z );
					output.vertex.coords = TexCoords( 0.0f, 1.0f );
					break;

				case 2:
					output.vertex.pos = V3< float >( inf.x, sup.y, inf.z );
					output.vertex.coords = TexCoords( 1.0f, 0.0f );
					break;

				case 3:
					output.vertex.pos = V3< float >( sup.x, sup.y, inf.z );
					output.vertex.coords = TexCoords( 1.0f, 1.0f );
					break;

				case 4:
					output.vertex.pos = V3< float >( inf.x, inf.y, sup.z );
					output.vertex.coords = TexCoords( 0.0f, 0.0f );
					break;

				case 5:
					output.vertex.pos = V3< float >( sup.x, inf.y, sup.z );
					output.vertex.coords = TexCoords( 0.0f, 1.0f );
					break;

				case 6:
					output.vertex.pos = V3< float >( inf.x, inf.y, inf.z );
					output.vertex.coords = TexCoords( 1.0f, 0.0f );
					break;

				case 7:
					output.vertex.pos = V3< float >( sup.x, inf.y, inf.z );
					output.vertex.coords = TexCoords( 1.0f, 1.0f );
					break;
				}

				output.vertex.pos += center;
				output.vertex.normal.Normalize( output.vertex.pos );
				output.vertex.diffuse = diffuse;

				return output;
			};
		};

		class CubeFaceBuilder : public sculpter::IFaceBuilder
		{
		public:
			typedef std::shared_ptr< IFaceBuilder > ptr;

			virtual ~CubeFaceBuilder() {};


			sculpter::FaceOutput Build( sculpter::SheetStats sheetStats, sculpter::FaceInput input )
			{
				sculpter::FaceOutput output {};

				switch( input.index )
				{
				case 0:
					output.indices.push_back( 0 );
					output.indices.push_back( 1 );
					output.indices.push_back( 2 );
					output.indices.push_back( 1 );
					output.indices.push_back( 3 );
					output.indices.push_back( 2 );
					break;

				case 1:
					output.indices.push_back( 2 );
					output.indices.push_back( 3 );
					output.indices.push_back( 6 );
					output.indices.push_back( 3 );
					output.indices.push_back( 7 );
					output.indices.push_back( 6 );
					break;

				case 2:
					output.indices.push_back( 0 );
					output.indices.push_back( 2 );
					output.indices.push_back( 4 );
					output.indices.push_back( 2 );
					output.indices.push_back( 6 );
					output.indices.push_back( 4 );
					break;

				case 3:
					output.indices.push_back( 3 );
					output.indices.push_back( 1 );
					output.indices.push_back( 7 );
					output.indices.push_back( 1 );
					output.indices.push_back( 5 );
					output.indices.push_back( 7 );
					break;

				case 4:
					output.indices.push_back( 5 );
					output.indices.push_back( 4 );
					output.indices.push_back( 6 );
					output.indices.push_back( 5 );
					output.indices.push_back( 7 );
					output.indices.push_back( 6 );
					break;

				case 5:
					output.indices.push_back( 1 );
					output.indices.push_back( 0 );
					output.indices.push_back( 5 );
					output.indices.push_back( 0 );
					output.indices.push_back( 4 );
					output.indices.push_back( 5 );
					break;
				}

				return output;
			}
		};
	
		sculpter::SheetStats sheetStats;
		sheetStats.parameters.Set( "size3", unify::Size3< float >( 2, 2, 2 ) );
		sheetStats.parameters.Set( "diffuse", unify::ColorGreen() );
		sheetStats.parameters.Set( "specular", unify::ColorWhite() );
		sheetStats.totalVertices = 8;
		sheetStats.totalFaces = 6;
		sheetStats.totalTriangles = 12;

		sculpter::Sculpter sculpter;
		sculpter.AddSheet( 
			sheetStats, 
			sculpter::IVertexBuilder::ptr( new CubeVertexBuilder() ),
			sculpter::IFaceBuilder::ptr( new CubeFaceBuilder() )
			);
		sculpter.Build( primitiveList, effect );
	}
};




void BuildSphere( me::render::PrimitiveList & primitiveList, me::render::Effect::ptr effect )
{
	class SphereVertexBuilder : public sculpter::IVertexBuilder
	{
	public:
		sculpter::VertexOutput Build( sculpter::SheetStats sheetStats, sculpter::VertexInput vertexInput ) override
		{
			using namespace unify;
			sculpter::VertexOutput output{};

		}
	};

}




MainScene::MainScene( me::game::Game * gameInstance )
	:Scene( gameInstance, "Main" )
{	
}

void MainScene::OnStart()
{
	AddResources( unify::Path( "resources/Standard.me_res" ) );
	AddResources( unify::Path( "resources/TestTextures.me_res" ) );
	auto colorEffect = GetAsset< Effect >( "ColorSimple" );
	auto textureEffect = GetAsset< Effect>( "TextureSimple" );
	auto textureAndColorEffect = GetAsset< Effect>( "TextureAndColor" );
	auto fourColorsEffect = GetAsset< Effect >( "FourColors" );
	auto colorAmbientEffect = GetAsset< Effect >( "ColorAmbient" );
	auto textureAmbientEffect = GetAsset< Effect >( "TextureAmbient" );
	auto colorInstancedAmbientEffect = GetAsset< Effect >( "ColorAmbientInstanced" );

	// Create our textured effect with an image.
	auto textureImageEffect = textureEffect->Duplicate();
	textureImageEffect->SetTexture( 0, GetAsset< ITexture >( "4" ) );

	// Add a camera...
	object::Object * camera = GetObjectAllocator()->NewObject( "camera" );
	camera->AddComponent( object::component::IObjectComponent::ptr( new object::component::CameraComponent() ) );
	auto * cameraComponent = unify::polymorphic_downcast< object::component::CameraComponent * >( camera->GetComponent( "camera" ).get() );
	cameraComponent->SetProjection( unify::MatrixPerspectiveFovLH( 3.141592653589f / 4.0f, 800/600, 1, 1000 ) );
	camera->GetFrame().SetPosition( unify::V3< float >( 0, 5, -17 ) );
	camera->GetFrame().LookAt( unify::V3< float >( 0, 0, 0 ) );


	auto shapeCreator = GetManager< Geometry >()->GetFactory( "me_shape" );

	{
		unify::Parameters parameters {
			{ "type", (std::string)"box" },
			{ "effect", colorEffect },
			{ "size3", unify::Size3< float >( 9, 9, 9 ) },
			{ "diffuses",  std::vector< unify::Color >{ 
				unify::ColorRed(), unify::ColorGreen(), unify::ColorBlue(), unify::ColorYellow(), unify::ColorCyan(), unify::ColorMagenta() 
			} },
		};

		auto object = GetObjectAllocator()->NewObject( "cube" );
		AddGeometryComponent( object, shapeCreator->Produce( parameters ) );
		object->GetFrame().SetPosition( unify::V3< float >( 0, 0, 0 ) );
		object->AddComponent( object::component::IObjectComponent::ptr( new object::component::BBoxRendererComponent( GetOS(), colorEffect ) ) );
	}
}

void MainScene::OnUpdate( const UpdateParams & params )
{
	// Use of camera controls to simplify camera movement...
	object::Object * camera = FindObject( "camera" );
	
	camera->GetFrame().Orbit( unify::V3< float >( 0, 0, 0 ), unify::V2< float >( 1, 0 ), unify::AngleInRadians( params.GetDelta().GetSeconds() ) );
	//camera->GetFrame().Orbit( unify::V3< float >( 0, 0, 0 ), unify::Quaternion( unify::V3< float >( 0, 1, 0 ), unify::AngleInRadians( renderInfo.GetDelta() ) ) );
	
	camera->GetFrame().LookAt( unify::V3< float >( 0, 0, 0 ), unify::V3< float >( 0, 1, 0 ) );
}
