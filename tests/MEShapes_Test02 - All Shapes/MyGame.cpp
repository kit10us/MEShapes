// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <MEWinMain.h>
#include <me/game/Game.h>
#include <MainScene.h>

using namespace me;

class MyGame : public game::Game
{
public:
	MyGame()
		: Game( unify::Path( "MEShapes.me_setup" ) )
	{
	}

	void AddScenes( scene::SceneManager * sceneManager ) override
	{
		sceneManager->AddScene( me::scene::IScene::ptr( new MainScene( this ) ) );
	}
} myGame;

RegisterGame( myGame );
