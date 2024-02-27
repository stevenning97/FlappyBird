#pragma once
#include <SDL/SDL_mixer.h>

#include "Actor.h"
class Bird : public Actor
{
public:
	Bird ( class Game* game );
	~Bird ( ) override;
	void UpdateActor ( float deltaTime ) override;
	void ProcessKeyboard ( const uint8_t* state );
	float GetVelocity ( ) const { return mVelocity; }
private:
	float mVelocity; // x velocity never changes, this is only y velocity
	float mJumpCooldown;
	Mix_Chunk* gWing;
};