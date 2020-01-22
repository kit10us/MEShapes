// MEShapes.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <me/sculpter/SculpterFactory.h>
#include <me/render/IRenderer.h>
#include <me/render/Geometry.h>
#include <me/game/Game.h>

#include <shapes/BeveledBox.h>
#include <shapes/Circle.h>
#include <shapes/Cone.h>
#include <shapes/Box.h>
#include <shapes/Cylinder.h>
#include <shapes/DashRing.h>
#include <shapes/Plane.h>
#include <shapes/PointField.h>
#include <shapes/PointRing.h>
#include <shapes/Pyramid.h>
#include <shapes/Sphere.h>
#include <shapes/Tube.h>

using namespace shapes;
using namespace me;
using namespace render;

void Deleter( me::sculpter::IShapeCreator * creator )
{
	delete creator;
}

extern "C" __declspec(dllexport) bool MELoader( me::game::IGame * gameInstance, const qxml::Element * element );

__declspec(dllexport) bool MELoader( me::game::IGame * gameBase, const qxml::Element * element )
{
	using namespace me;

	auto gameInstance = dynamic_cast< game::Game * >(gameBase);

	// Add sculpter creators.
	auto geometryManager = unify::polymorphic_downcast< rm::ResourceManager< me::render::Geometry > * >( gameInstance->GetManager< Geometry >( ) );
	auto shapeFactory = dynamic_cast< me::sculpter::SculpterFactory *>( geometryManager->GetFactory( "me_shape" ) );
	shapeFactory->AddShapeCreator( "box", me::sculpter::IShapeCreator::ptr( new shapes::Box(), Deleter ) );
	shapeFactory->AddShapeCreator( "beveledbox", me::sculpter::IShapeCreator::ptr( new shapes::BeveledBox(), Deleter ) );
	shapeFactory->AddShapeCreator( "Circle", me::sculpter::IShapeCreator::ptr( new shapes::Circle(), Deleter ) );
	shapeFactory->AddShapeCreator( "Cone", me::sculpter::IShapeCreator::ptr( new shapes::Cone(), Deleter ) );
	shapeFactory->AddShapeCreator( "Cylinder", me::sculpter::IShapeCreator::ptr( new shapes::Cylinder(), Deleter ) );
	shapeFactory->AddShapeCreator( "DashRing", me::sculpter::IShapeCreator::ptr( new shapes::DashRing(), Deleter ) );
	shapeFactory->AddShapeCreator( "Plane", me::sculpter::IShapeCreator::ptr( new shapes::Plane(), Deleter ) );
	shapeFactory->AddShapeCreator( "PointField", me::sculpter::IShapeCreator::ptr( new shapes::PointField(), Deleter ) );
	shapeFactory->AddShapeCreator( "PointRing", me::sculpter::IShapeCreator::ptr( new shapes::PointRing(), Deleter ) );
	shapeFactory->AddShapeCreator( "Pyramid", me::sculpter::IShapeCreator::ptr( new shapes::Pyramid(), Deleter ) );
	shapeFactory->AddShapeCreator( "Sphere", me::sculpter::IShapeCreator::ptr( new shapes::Sphere(), Deleter ) );
	shapeFactory->AddShapeCreator( "Tube", me::sculpter::IShapeCreator::ptr( new shapes::Tube(), Deleter ) );

	//gameInstance->GetManager< Geometry >()->AddFactory( "me_shape", GeometryFactory::ptr( factory ) );

	return true;
}

