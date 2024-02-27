#pragma once
#include "SpriteComponent.h"
#include <vector>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>

#include "Math.h"
class PipeComponent : public SpriteComponent
{
public:
	// Set draw order to default to lower (so it's in the background)
	PipeComponent ( class Actor* owner, Vector2 gap, int drawOrder = 10 );
	~PipeComponent ( ) override;
	// Update/draw overriden from parent
	void Update ( float deltaTime ) override;
	void Draw ( SDL_Renderer* renderer ) override;
	// Set the textures used for the background
	void SetPipeTexture ( SDL_Texture * texture );
	void SetBaseTexture ( SDL_Texture * texture );
	// Get/set screen size and scroll speed
	void SetScreenSize ( const Vector2& size ) { mScreenSize = size; }
	void SetScrollSpeed ( float speed ) { mScrollSpeed = speed; }
	float GetScrollSpeed ( ) const { return mScrollSpeed; }

	void setPaused ( bool paused ) { this->mPaused = paused; }
	bool getPaused ( ) { return this->mPaused; }

	void setScore ( int score ) { this->mScore = score; }
	int GetScore ( ) { return this->mScore; }

	void updateScoreCounter ( );

	std::vector<Vector2>& getPipes ( ) { return this->mPipes; }
private:
	// Struct to encapsulate each image and its offset
	struct Texture
	{
		SDL_Texture* mTexture;
		Vector2 mPosition;
		Vector2 mSize;
	};

	TTF_Font* gFont;

	Mix_Chunk* gPoint;
	Mix_Chunk* gHit;

	Texture mPipeTexture;
	Texture mBaseTexture;
	Texture mScoreTexture;
	int mRepeatBaseAmount;

	std::vector<Vector2> mPipes;

	Vector2 mScreenSize;
	Vector2 mGap;
	float mScrollSpeed;
	float mPointCooldown;

	int mScore;

	bool mPaused;
};