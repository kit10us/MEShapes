// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved
#pragma once

#include <me/sculpter/IShapeCreator.h>

namespace shapes
{
	class BeveledBox : public me::sculpter::IShapeCreator
	{
	public:
		BeveledBox();
		~BeveledBox() override;

		void Create( me::render::PrimitiveList & primitiveList, const unify::Parameters & parameters ) const override;
	};
}
