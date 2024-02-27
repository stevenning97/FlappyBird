#include "PipeComponent.h"

#include <ctime>
#include <iostream>

#include "Actor.h"
#include "Bird.h"
#include "Game.h"

PipeComponent::PipeComponent ( class Actor* owner, Vector2 gap, int drawOrder )
	:SpriteComponent ( owner, drawOrder )
	, mRepeatBaseAmount ( 0 )
	, mGap ( gap.x, gap.y )
	, mScrollSpeed ( 0.0f )
	, mPointCooldown ( 0 ),
	mScore ( 0 ) {
	gHit = Mix_LoadWAV ( "Assets/hit.wav" );
	gPoint = Mix_LoadWAV ( "Assets/point.wav" );

	gFont = TTF_OpenFont ( "Assets/flappy-font.ttf", 50 );

	SDL_Surface* textSurface = TTF_RenderText_Solid ( gFont, "0", { 255, 255, 255, 255 } );
	mScoreTexture.mTexture = SDL_CreateTextureFromSurface ( this->mOwner->GetGame ( )->getRenderer ( ), textSurface );
	mScoreTexture.mSize.x = static_cast<float>(textSurface->w);
	mScoreTexture.mSize.y = static_cast<float>(textSurface->h);

	SDL_FreeSurface ( textSurface );
}

PipeComponent::~PipeComponent ( ) {
	Mix_FreeChunk ( gHit );
	Mix_FreeChunk ( gPoint );
}

void PipeComponent::Update ( float deltaTime )
{
	if ( mPaused ) return;

	// is bird dead?
	if ( mOwner->GetGame ( )->getBird ( )->GetState ( ) == Actor::EPaused )
		mPaused = true;

	// update base
	mBaseTexture.mPosition.x += mScrollSpeed * deltaTime;
	if ( -mBaseTexture.mPosition.x >= mBaseTexture.mSize.x )
		mBaseTexture.mPosition.x = 0;

	auto intersectsRect = [ ] ( const Vector2 & point, Vector2 & pos, Vector2 & size ) {
		const auto closestX = std::max ( pos.x, std::min ( point.x, pos.x + size.x ) );
		const auto closestY = std::max ( pos.y, std::min ( point.y, pos.y + size.y ) );

		const auto distance = std::sqrt ( ( closestX - point.x ) * ( closestX - point.x ) + ( closestY - point.y ) * ( closestY - point.y ) );

		return distance <= 10.f;
	};

	mPointCooldown -= deltaTime;

	// update pipes
	for ( auto & pipe : mPipes ) {
		pipe.x += mScrollSpeed * deltaTime;

		if ( pipe.x + mPipeTexture.mSize.x <= 0.f )
			pipe = Vector2 ( static_cast<float>(mPipes.size(  ) - 1) * ( mPipeTexture.mSize.x + mGap.x ) + mScreenSize.x - mPipeTexture.mSize.x,
				rand ( ) % static_cast< int >( mScreenSize.y - ( mBaseTexture.mSize.y + 150 ) ) + mPipeTexture.mSize.y * 0.5f );

		// did we hit something
		if ( intersectsRect ( mOwner->GetGame ( )->getBird ( )->GetPosition ( ), pipe, mPipeTexture.mSize ) ||
			intersectsRect ( mOwner->GetGame ( )->getBird ( )->GetPosition ( ), pipe - Vector2 ( 0, mGap.y + mPipeTexture.mSize.y ), mPipeTexture.mSize ) ) {
			this->mOwner->GetGame ( )->getBird ( )->SetState ( Actor::EPaused );
			Mix_PlayChannel ( -1, gHit, 0 );
		}
		else if ( intersectsRect(mOwner->GetGame(  )->getBird(  )->GetPosition(  ), pipe - Vector2(0, mGap.y), Vector2(mPipeTexture.mSize.x, mGap.y) ) && mPointCooldown <= 0.f ) { // we got a point
			Mix_PlayChannel ( -1, gPoint, 0 );
			++mScore;
			mPointCooldown = 2.f;

			updateScoreCounter ( );
		}
	}
}

void PipeComponent::updateScoreCounter ( ) {
	// increase on counter
	SDL_DestroyTexture ( mScoreTexture.mTexture );
	SDL_Surface* textSurface = TTF_RenderText_Solid ( gFont, std::to_string ( mScore ).c_str ( ), { 255, 255, 255, 255 } );
	mScoreTexture.mTexture = SDL_CreateTextureFromSurface ( this->mOwner->GetGame ( )->getRenderer ( ), textSurface );
	mScoreTexture.mSize.x = static_cast< float >( textSurface->w );
	mScoreTexture.mSize.y = static_cast< float >( textSurface->h );

	SDL_FreeSurface ( textSurface );
}

void PipeComponent::Draw ( SDL_Renderer* renderer )
{
	// draw pipes
	SDL_Rect p;
	p.w = static_cast<int>(mPipeTexture.mSize.x);
	p.h = static_cast<int>(mPipeTexture.mSize.y);
	for ( const auto & pipe : mPipes ) {
		p.x = static_cast<int>(pipe.x);
		p.y = static_cast<int>(pipe.y);

		// Draw this background
		SDL_RenderCopy ( renderer,
			mPipeTexture.mTexture,
			nullptr,
			&p
		);

		p.y -= mPipeTexture.mSize.y + mGap.y;

		// Draw this background
		SDL_RenderCopyEx ( renderer,
			mPipeTexture.mTexture,
			nullptr,
			&p,
			0,
			nullptr,
			SDL_FLIP_VERTICAL
		);
	}

	// draw base
	SDL_Rect b;
	b.x = static_cast<int>(mBaseTexture.mPosition.x);
	b.y = static_cast<int>(mBaseTexture.mPosition.y);
	b.w = static_cast<int>(mBaseTexture.mSize.x);
	b.h = static_cast<int>(mBaseTexture.mSize.y);

	for ( int i = 0; i < mRepeatBaseAmount; i++ ) {
		b.x += i * b.w;

		// Draw this background
		SDL_RenderCopy ( renderer,
			mBaseTexture.mTexture,
			nullptr,
			&b
		);
	}

	// draw score
	SDL_Rect s;
	s.x = static_cast< int >( mScreenSize.x * 0.5f - mScoreTexture.mSize.x * 0.5f );
	s.y = 30;
	s.w = static_cast< int >( mScoreTexture.mSize.x );
	s.h = static_cast< int >( mScoreTexture.mSize.y );

	// draw points
	SDL_RenderCopy ( renderer,
		mScoreTexture.mTexture,
		nullptr,
		&s
	);
}

void PipeComponent::SetPipeTexture ( SDL_Texture* texture ) {
	mPipeTexture.mTexture = texture;
	srand ( time( nullptr ) );

	SDL_Point point;
	SDL_QueryTexture ( texture, nullptr, nullptr, &point.x, &point.y );

	mPipeTexture.mSize.x = static_cast< float >( point.x );
	mPipeTexture.mSize.y = static_cast< float >( point.y );

	// scuffed, but how many pipes do we add?
	const int amountOfPipes = static_cast< int >( std::ceil ( mScreenSize.x / ( mPipeTexture.mSize.x + mGap.x ) ) + 1 );
	for ( int i = 0; i < amountOfPipes; i++ )
		mPipes.emplace_back ( i * ( mPipeTexture.mSize.x + mGap.x ) + mScreenSize.x, rand ( ) % static_cast< int >( mScreenSize.y - ( mBaseTexture.mSize.y + 150 ) ) + mPipeTexture.mSize.y * 0.5f );
}

void PipeComponent::SetBaseTexture ( SDL_Texture * texture ) {
	mBaseTexture.mTexture = texture;

	SDL_Point point;
	SDL_QueryTexture ( texture, nullptr, nullptr, &point.x, &point.y );

	mBaseTexture.mSize.x = static_cast< float >( point.x );
	mBaseTexture.mSize.y = static_cast< float >( point.y );

	// set right pos
	mBaseTexture.mPosition.x = 0;
	mBaseTexture.mPosition.y = mScreenSize.y - mBaseTexture.mSize.y;

	// set the amount of times we need to repeat
	mRepeatBaseAmount = static_cast<int>( std::ceil( mScreenSize.x / mBaseTexture.mSize.x) + 1 );
}