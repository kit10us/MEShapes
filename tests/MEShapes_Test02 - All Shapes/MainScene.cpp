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

//#include <me/sculpter/SculpterFactory.h>

#include <algorithm>
#include <math.h>

using namespace me;
using namespace render;

MainScene::MainScene( me::game::Game * gameInstance )
	:Scene( gameInstance, "main" )
{
}

void MainScene::OnStart()
{
	AddResources( unify::Path( "resources/Standard.me_res" ) );
	auto colorEffect = GetAsset< Effect >( "ColorSimple" );
	auto textureEffect = GetAsset< Effect>( "TextureSimple" );
	auto textureAndColorEffect = GetAsset< Effect>( "TextureAndColor" );
	auto fourColorsEffect = GetAsset< Effect >( "FourColors" );
	auto colorAmbientEffect = GetAsset< Effect >( "ColorAmbient" );
	auto textureAmbientEffect = GetAsset< Effect >( "TextureAmbient" );
	auto colorInstancedAmbientEffect = GetAsset< Effect >( "ColorAmbientInstanced" );

	// Add an object to act as a camera...
	object::Object * camera = GetObjectAllocator()->NewObject( "camera" );
	camera->GetFrame().SetPosition( unify::V3< float >( 0, 5, -17 ) );
	camera->GetFrame().LookAt( unify::V3< float >( 0, 0, 0 ) );

	// Add a camera component to the camera object
	auto * cameraComponent = new object::component::CameraComponent();
	cameraComponent->SetProjection( unify::MatrixPerspectiveFovLH( 3.141592653589f / 4.0f, 800 / 600, 1, 1000 ) );
	camera->AddComponent( object::component::IObjectComponent::ptr( cameraComponent ) );

	// Create shapes...
	auto shapeCreator = GetManager< Geometry >()->GetFactory( "me_shape" );
	
	auto createObject = [&]( unify::Parameters parameters, int depth )->me::object::Object*
	{
		const unify::V3< float > startPos{ -2.5f, 2.5f, -1.25f + (2.5f * depth) };
		const int itemsPerRow = 4;
		const int itemsPerColumn = 4;
		const int itemsPerPage = itemsPerRow * itemsPerColumn;
		const unify::V3< float > changePerRow{ 0.0f, -2.5f, 0.0f };
		const unify::V3< float > changePerColumn{ 2.5f, 0.0f, 0.0f };
		const unify::V3< float > changePerPage{ 0.0f, 0.0f, 2.5f };

		static int objectIndex[2] = {};
		int page = objectIndex[depth] / itemsPerPage;
		int row = (objectIndex[depth] % itemsPerPage) / itemsPerRow;
		int column = (objectIndex[depth] % itemsPerPage) % itemsPerRow;

		std::string objectName = "object " + unify::Cast< std::string >( objectIndex[depth]++ );
		auto object = GetObjectAllocator()->NewObject( objectName );

		unify::V3< float > pos = startPos + unify::V3< float >{
			(changePerRow * (float)row) + (changePerColumn * (float)column) + (changePerPage * (float)page) };
		object->GetFrame().SetPosition( pos );

		AddGeometryComponent( object, shapeCreator->Produce( parameters ) );
		//object->GetFrame().SetPosition( unify::V3< float >( 0, 0, 0 ) );
		object->AddComponent( object::component::IObjectComponent::ptr( new object::component::BBoxRendererComponent( GetOS(), colorEffect ) ) );

		return object;
	};

	for( int depth = 0; depth < 2; depth++ )
	{
		using namespace unify;
		auto & effect = depth == 0 ? colorEffect : fourColorsEffect;

		{
			Parameters parameters{
				{ "type", (std::string)"box" },
				{ "effect", effect },
				{ "size3", Size3< float >( 1.0f, 1.0f, 1.0f ) },
				{ "diffuses", std::vector< Color >{
						ColorRed(), ColorGreen(), ColorBlue(), ColorYellow(), ColorCyan(), ColorMagenta() 
					}
				}
			};

			auto object = createObject( parameters, depth );
		}
		{
			Parameters parameters {
				{ "type", (std::string)"pointfield" },
				{ "effect", effect },
				{ "majorradius", 0.5f },
				{ "minorradius", 0.5f },
				{ "count", 1000 },
				{ "diffuse", ColorRed() }
			};
			auto object = createObject( parameters, depth );
		}

		{
			Parameters parameters {
				{ "type", (std::string)"pointring" },
				{ "effect", effect },
				{ "majorradius", 0.5f },
				{ "minorradius", 0.25f },
				{ "count", 1000 },
				{ "diffuse", ColorGreen() }
			};
			auto object = createObject( parameters, depth );
		}

		{
			Parameters parameters {
				{ "type", (std::string)"dashring" },
				{ "effect", effect },
				{ "majorradius", 1.0f },
				{ "minorradius", 0.9f },
				{ "size1", 0.5f },
				{ "count", (size_t)12 },
				{ "diffuse", ColorBlue() } 
			};
			auto object = createObject( parameters, depth );
		}

		{
			Parameters parameters {
				{ "type", (std::string)"pyramid" },
				{ "effect", colorEffect },
				{ "size3", Size3< float >{ 1.0f, 1.0f, 1.0f } },
				{ "diffuse", ColorRed( 255 / 2 ) }
			};
			auto object = createObject( parameters, depth );
		}

		{
			Parameters parameters {
				{ "type", (std::string)"circle" },
				{ "effect", effect },
				{ "radius", 1.0f },
				{ "diffuse", ColorGreen( 255 / 2 ) }
			};
			auto object = createObject( parameters, depth );
		}

		{
			Parameters parameters {
				{ "type", (std::string)"sphere" },
				{ "effect", effect },
				{ "radius", 0.5f },
				{ "diffuse", ColorBlue( 255 / 2 ) }
			};
			auto object = createObject( parameters, depth );
		}

		{
			Parameters parameters {
				{ "type", (std::string)"cylinder" },
				{ "effect", effect },
				{ "radius", 0.25f },
				{ "height", 1.0f },
				{ "diffuse", ColorCyan( 255 / 2 ) }
			};
			auto object = createObject( parameters, depth );
		}

		{
			Parameters parameters {
				{ "type", (std::string)"tube" },
				{ "effect", effect },
				{ "majorradius", 0.5f },
				{ "minorradius", 0.25f },
				{ "height", 1.0f },
				{ "diffuse", ColorMagenta( 255 / 2 ) }
			};
			auto object = createObject( parameters, depth );
		}

		{
			Parameters parameters {
				{ "type", (std::string)"plane" },
				{ "effect", effect },
				{ "size2", Size< float >{ 1.0f, 1.0f } },
				{ "diffuse", ColorRed( 255 ) }
			};
			auto object = createObject( parameters, depth );
		}

		{
			Parameters parameters {
				{ "type", (std::string)"cone" },
				{ "effect", effect },
				{ "radius", 0.25f },
				{ "height", 1.0f },
				{ "diffuse", ColorGreen( 255 ) }
			};
			auto object = createObject( parameters, depth );
		}
	}
}

void MainScene::OnUpdate( const UpdateParams & params )
{
	// Use of camera controls to simplify camera movement...
	object::Object * camera = FindObject( "camera" );

	camera->GetFrame().Orbit( unify::V3< float >( 0, 0, 0 ), unify::V2< float >( 1, 0 ), unify::AngleInRadians( params.GetDelta().GetSeconds() ) );
	camera->GetFrame().LookAt( unify::V3< float >( 0, 0, 0 ), unify::V3< float >( 0, 1, 0 ) );
}
