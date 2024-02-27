#include "Bird.h"

#include <iostream>

#include "AnimSpriteComponent.h"
#include "Game.h"

Bird::Bird ( Game* game )
	:Actor ( game )
	, mVelocity ( 0.f )
	, mJumpCooldown ( 0.f )
{
	// Create an animated sprite component
	AnimSpriteComponent* asc = new AnimSpriteComponent ( this );
	std::vector<SDL_Texture*> anims = {
		game->GetTexture ( "Assets/Bird1.png" ),
		game->GetTexture ( "Assets/Bird2.png" ),
		game->GetTexture ( "Assets/Bird3.png" ),
	};
	asc->SetAnimTextures ( anims );
	asc->SetAnimFPS ( 10.f );

	gWing = Mix_LoadWAV ( "Assets/wing.wav" );
}

Bird::~Bird ( ) {
	Mix_FreeChunk ( gWing );
}

void Bird::UpdateActor ( float deltaTime )
{
	// Update position based on speeds and delta time
	Vector2 pos = GetPosition ( );

	mVelocity += 500.f * deltaTime;

	const float nextPos = pos.y + mVelocity * deltaTime;

	if ( nextPos >= 0.f )
		pos.y = nextPos;

	mJumpCooldown -= deltaTime;

	this->SetRotation ( -std::max ( std::min ( mVelocity * 0.002f, 1.f ), -1.f ) );

	// keep on screen
	if ( pos.y >= this->GetGame ( )->getHeight ( ) ) pos.y = this->GetGame ( )->getHeight ( );

	SetPosition ( pos );
}

void Bird::ProcessKeyboard ( const uint8_t* state )
{
	if ( state[ SDL_SCANCODE_SPACE ] && mJumpCooldown <= 0.f ) {
		mVelocity = -240.f;

		mJumpCooldown = 0.2f;

		Mix_PlayChannel ( -1, gWing, 0 );
	}
}