// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved
#pragma once

#include <me/sculpter/IShapeCreator.h>

namespace shapes
{
	class Sphere : public me::sculpter::IShapeCreator
	{
	public:
		Sphere();
		~Sphere() override;

		void Create( me::render::PrimitiveList & primitiveList, const unify::Parameters & parameters ) const override;
	};
}