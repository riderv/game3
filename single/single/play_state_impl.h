#pragma once


#include "play_state.h"


TPlayState::TPlayState()
{
	mTileMap.mTilesetTexture = GameState.mTilesetTexture;

	std::wstring ffn = GetExePatch() + L"/res/sil_anim.png";
	auto res = MapFile( ffn );
	if( !mCharacterTexture.loadFromMemory( res.first, res.second ) )
		throw TException( "Loading /res/sil_anim.png falied" );

	mCharacterSprite.setTexture( mCharacterTexture );
	mCandySprite.setTexture( GameState.mTilesetTexture );
	mCandySprite.setTextureRect(sf::IntRect(0, TilePxSize, TilePxSize, TilePxSize));


}

TPlayState::~TPlayState()
{
	if( mCharacterTextureBuf )
		UnmapViewOfFile( mCharacterTextureBuf ), mCharacterTextureBuf = 0;
	
}

void TPlayState::PoolEvent( sf::Event & E )
{/*
	switch (E.type)
	{
	case sf::Event::KeyPressed:
		switch (E.key.code) {
		case sf::Keyboard::Left:
		case sf::Keyboard::Right:
		case sf::Keyboard::Up:
		case sf::Keyboard::Down:
			mCharacter.State = TCharacter::TState::Going;
		}
		break;
	case sf::Event::KeyReleased:
		switch (E.key.code) {
		case sf::Keyboard::Left:
		case sf::Keyboard::Right:
		case sf::Keyboard::Up:
		case sf::Keyboard::Down:
			mCharacter.State = TCharacter::TState::Stay;
		}
		break;
	}*/
}

void TPlayState::Draw()
{
	Win.setView( mView );

	if( mInvalidMapTarget )
		UpdateMapTarget();

	
	Win.draw( mTargetSprite );

	mCharacterSprite.setTextureRect(sf::IntRect(
		int(mCharacter.Frame)*TilePxSize ,
		(mCharacter.AnimLine) *CharPxHeigth,
		TilePxSize,
		CharPxHeigth
	));
	mCharacterSprite.setPosition(
		(mCharacter.x - mViewOffsetInTiles.x ) * TilePxSize + mCharacter.xoff,
		(mCharacter.y - mViewOffsetInTiles.y - 1) * TilePxSize + mCharacter.yoff
		);
	Win.draw(mCharacterSprite);
}

void TPlayState::OnResize()
{
	UpdateView();
}

void TPlayState::OnChangePlayrPos()
{
	auto res = mCandys.find( { mCharacter.x,mCharacter.y } );
	if( res != mCandys.end() )
	{
		mCandysFound++;
		mCandys.erase( res );
		mInvalidMapTarget = true;
		GameState.mSounds.at( mSounNyamNo++ ).mSound.play();
		if( mSounNyamNo >= GameState.mSounds.size() )
			mSounNyamNo = 0;
		if( mCandys.size() == 0 ) {
			mLevelComplited = true;
		}
	}

	if( mCharacter.x > mViewOffsetInTiles.x + mViewSizeInTiles.x - 1 ) {
		mViewOffsetInTiles.x += mViewSizeInTiles.x;
		mInvalidMapTarget = true;
	}else
	if( mCharacter.x < mViewOffsetInTiles.x ) {
		mViewOffsetInTiles.x -= mViewSizeInTiles.x;
		if( mViewOffsetInTiles.x < 0 )
			mViewOffsetInTiles.x = 0;
		mInvalidMapTarget = true;
	}else
	if( mCharacter.y > mViewOffsetInTiles.y + mViewSizeInTiles.y - 1 ) {
		mViewOffsetInTiles.y += mViewSizeInTiles.y;
		mInvalidMapTarget = true;
	}else
	if( mCharacter.y < mViewOffsetInTiles.y ) {
		mViewOffsetInTiles.y -= mViewSizeInTiles.y;
		if( mViewOffsetInTiles.y < 0 )
			mViewOffsetInTiles.y = 0;
		mInvalidMapTarget = true;
	}

}

void TPlayState::OnTileReached()
{
	if( mCandys.size() == 0 )
	{
		if( IDYES == MessageBoxA( 0, "’очешь ещЄ?", "јй молодца!", MB_ICONQUESTION | MB_YESNO ) )
			BeginPlay();
		else
			GameState.GotoMainMenu();
	}
}

// TODO: переделать на вектора
void TPlayState::SimulateMoving()
{
	float DeltaDist = GameState.DTs * (MoveSpeed_TilePerSec*TilePxSize);
	while (1) {
		switch (mCharacter.State)
		{
		//////////////////////////
		case TCharState::Stay:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
				if( mTileMap.TypeAt(mCharacter.x + 1, mCharacter.y) == TTileType::Ground )
					mCharacter.State = TCharState::GoingRigthOut;
				mCharacter.Dir = TDir::Rigth;
				return;
			}else
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
				if (mTileMap.TypeAt( mCharacter.x - 1, mCharacter.y ) == TTileType::Ground)
					mCharacter.State = TCharState::GoingLeftOut;
				mCharacter.Dir = TDir::Left;
				return;
			}else
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				if (mTileMap.TypeAt( mCharacter.x, mCharacter.y + 1 ) == TTileType::Ground)
					mCharacter.State = TCharState::GoingDownOut;
				mCharacter.Dir = TDir::Down;
				return;
			}else
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				if (mTileMap.TypeAt( mCharacter.x, mCharacter.y - 1 ) == TTileType::Ground)
					mCharacter.State = TCharState::GoingUpOut;
				mCharacter.Dir = TDir::Up;
				return;
			}
			return;
		//////////////////////////
		case TCharState::GoingRigthOut:
			mCharacter.xoff += DeltaDist;
			if (mCharacter.xoff > TilePxSize / 2) {
				mCharacter.xoff -= TilePxSize;
				mCharacter.x++;
				mCharacter.State = TCharState::GoingRigthIn;
				OnChangePlayrPos();
				return;
			}
			return;
		//////////////////////////
		case TCharState::GoingRigthIn:
			mCharacter.xoff += DeltaDist;
			if (mCharacter.xoff >= 0.0f) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
				&& mTileMap.TypeAt(mCharacter.x+1, mCharacter.y) == TTileType::Ground) {
					mCharacter.State = TCharState::GoingRigthOut;
					OnTileReached();
					return;
				}
				mCharacter.xoff = 0.0f;
				mCharacter.State = TCharState::Stay;
				OnTileReached();
				return;
			}
			return;
		//////////////////////////
		case TCharState::GoingLeftOut:
			mCharacter.xoff -= DeltaDist;
			if (mCharacter.xoff < -(TilePxSize / 2)) {
				mCharacter.xoff += TilePxSize;
				mCharacter.x--;
				mCharacter.State = TCharState::GoingLeftIn;
				OnChangePlayrPos();
				return;
			}
			return;
		//////////////////////////
		case TCharState::GoingLeftIn:
			mCharacter.xoff -= DeltaDist;
			if (mCharacter.xoff <= 0.0f) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
				&& mTileMap.TypeAt( mCharacter.x-1, mCharacter.y ) == TTileType::Ground) {
					mCharacter.State = TCharState::GoingLeftOut;
					OnTileReached();
					return;
				}
				mCharacter.xoff = 0.0f;
				mCharacter.State = TCharState::Stay;
				OnTileReached();
				return;
			}
			return;
		//////////////////////////
		case TCharState::GoingUpOut:
			mCharacter.yoff -= DeltaDist;
			if (mCharacter.yoff < -(TilePxSize / 2)) {
				mCharacter.yoff += TilePxSize;
				mCharacter.y--;
				mCharacter.State = TCharState::GointUpIn;
				OnChangePlayrPos();
				return;
			}
			return;
		//////////////////////////
		case TCharState::GointUpIn:
			mCharacter.yoff -= DeltaDist;
			if (mCharacter.yoff <= 0.0f) {
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
				&& mTileMap.TypeAt( mCharacter.x, mCharacter.y-1 ) == TTileType::Ground) {
					mCharacter.State = TCharState::GoingUpOut;
					OnTileReached();
					return;
				}
				mCharacter.yoff = 0.0f;
				mCharacter.State = TCharState::Stay;
				OnTileReached();
				return;
			}
			return;
		//////////////////////////
		case TCharState::GoingDownOut:
			mCharacter.yoff += DeltaDist;
			if (mCharacter.yoff >= TilePxSize/2 ) {
				mCharacter.yoff -= TilePxSize;
				mCharacter.y++;
				mCharacter.State = TCharState::GointDownIn;
				OnChangePlayrPos();
				return;
			}
			return;
		//////////////////////////
		case TCharState::GointDownIn:
			mCharacter.yoff += DeltaDist;
			if (mCharacter.yoff >= 0.0f) {
				if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
				&& mTileMap.TypeAt( mCharacter.x, mCharacter.y+1 ) == TTileType::Ground) {
					mCharacter.State = TCharState::GoingDownOut;
					OnTileReached();
					return;
				}
				mCharacter.yoff = 0.0f;
				mCharacter.State = TCharState::Stay;
				OnTileReached();
				return;
			}
			return;

		}//switch
		break; // loop
	}
}
void TPlayState::MoveAnimieFrame()
{
	float DeltaDist = GameState.DTs * (MoveSpeed_TilePerSec*TilePxSize);
	if( TCharState::Stay == mCharacter.State )
	{
		if( TDir::Down == mCharacter.Dir ) {
			mCharacter.AnimLine = 0;
			mCharacter.MoveFrame( DeltaDist/4 * AnimSpeed );
		}
		else {
			mCharacter.Frame = 0.0f;
			mCharacter.AnimLine = mCharacter.Dir + 1;
		}
	}
	else {
		mCharacter.AnimLine = mCharacter.Dir + 1;
		mCharacter.MoveFrame( DeltaDist * AnimSpeed );
	}
}
void TPlayState::Simulate()
{
	SimulateMoving();
	MoveAnimieFrame();
}
void TPlayState::UpdateView() // TODO: не нравитс€ копипаст из редактора. Ќужно как то обобщить.
{
	sf::Vector2u ws = Win.getSize();

	// mView должен быть пропорционален экрану
	// но в два раза крупнее (впикселах).
	// 50 ѕикселов отступ вправо дл€ меню.
	mView.reset(sf::FloatRect(0.0f, 0.0f, float(ws.x) / 2, float(ws.y) / 2));
	//mView.setViewport({ 1.0f / ws.x * 50, 0.0f, 1.0f, 1.0f });
	//mView.setViewport( { 0.0f, 0.0f, 1.0f, 1.0f } );
	// записать сколько тайлов влезает во View и отрисовывать только влезающие
	auto vs = mView.getSize();
	mViewSizeInTiles.x = int(vs.x / TilePxSize);
	mViewSizeInTiles.y = int(vs.y / TilePxSize);
	
	if( mMapTarget )
		delete mMapTarget;
	auto Targ = new sf::RenderTexture();
	assert( Targ->create( int(vs.x), int(vs.y) ));
	mTargetSprite.setTexture( Targ->getTexture() );
	mTargetSprite.setTextureRect( { 0,0, int(vs.x), int(vs.y) } );
	mTargetSprite.setOrigin( 0.0f, 0.0f );
	mTargetSprite.setPosition( 0.0f, 0.0f );
	mMapTarget = Targ;


	UpdateMapTarget();
}
void TPlayState::UpdateMapTarget()
{
	//mMapTarget->setView( mView );
	mMapTarget->clear();

	//sf::RectangleShape top( { (float)mMapTarget->getSize().x, 1.0f } );
	//top.setFillColor( sf::Color::Red );
	//mMapTarget->draw( top );

	//sf::RectangleShape bottom( { (float)mMapTarget->getSize().x, 1.0f } );
	//bottom.setPosition( 0.0f, float( mMapTarget->getSize().y ) - 1.0f );
	//mMapTarget->draw( bottom );
	//// перебор тайлов влезающих в экран	

	//int tx = 1;
	//int ty = 1;
	//TTileType Type = mTileMap.TypeAt( tx, ty );
	//if( Type == TTileType::Unknown )
	//{
	//	__debugbreak(); return;
	//}
	//sf::IntRect r( int( Type )*TilePxSize, 0, TilePxSize, TilePxSize );
	//mTileSprite.setTextureRect( r );
	//sf::Vector2f pos_in_pixels( float( tx * TilePxSize ), float( ty * TilePxSize ) );
	////pos_in_pixels.y *= -1;
	//pos_in_pixels.y = mMapTarget->getSize().y - pos_in_pixels.y - TilePxSize;

	//mTileSprite.setPosition( pos_in_pixels );
	//mMapTarget->draw( mTileSprite );
	//mInvalidMapTarget = false;
	//return;

	for( int tx = 0; tx < mViewSizeInTiles.x; ++tx )
	{
		for( int ty = 0; ty < mViewSizeInTiles.y; ++ty )
		{
			int xof = tx + mViewOffsetInTiles.x;
			int yof = ty + mViewOffsetInTiles.y;

			TTileType Type = mTileMap.TypeAt( xof, yof );
			if( Type == TTileType::Unknown )
				continue;
			sf::IntRect r( int( Type )*TilePxSize, 0, TilePxSize, TilePxSize );
			mTileSprite.setTextureRect( r );
			sf::Vector2f pos_in_pixels( float( tx * TilePxSize ), float( ty * TilePxSize ) );
			pos_in_pixels.y = mMapTarget->getSize().y - pos_in_pixels.y - TilePxSize;
			mTileSprite.setPosition( pos_in_pixels );
			mMapTarget->draw( mTileSprite );

			auto res = mCandys.find( { xof,yof } );
			if( res != mCandys.end() )
			{
				mCandySprite.setPosition( pos_in_pixels );
				mMapTarget->draw( mCandySprite );
			}
		}
	}
	mInvalidMapTarget = false;
}

void TPlayState::RaiseDBException( TPlayState * This, const std::wstring& ermsg )
{
	throw TException( ermsg );
}

void TPlayState::LoadAndPlay(const wchar_t* FileName)
{
	try {
		SQLite::TDB db;
		db.SetExceptionRaiser( this, &TPlayState::RaiseDBException );
		db.Open(FileName);
		mTileMap.Load(db);
		mTileMap.mParam.FileName = FileName;
		mTileMap.mTilesetTexture = GameState.mTilesetTexture;
		mTileSprite.setTexture(GameState.mTilesetTexture);
		mCandySprite.setTexture( GameState.mTilesetTexture );
		BeginPlay();

		UpdateView();
		return;
	}
	catch (const TException& e)
	{
		MessageBoxW(0, e.msg.c_str(), L"Error loading map.", MB_ICONERROR);
		Log( L"TPlayState::LoadAndPlay catch exception: " + e.msg );
	}
	mTileMap = TTileMap();
	GameState.GotoMainMenu();
}

void TPlayState::BeginPlay()
{
	// ищем куда поставить персонажа
	int x = 0;	int y = 0;
	bool found = false;
	for(;;)
	{
		for( int xx = x; xx >= 0; xx-- ) {
			if( mTileMap.TypeAt( xx, y ) == TTileType::Ground ) {
				x = xx;
				goto FoundLabel; // ќќќќ  акой ужас! GOTO ай-€й-€й-€й ))) // 				
			}
		}
		for( int yy = y-1; yy >= 0; yy-- ) {
			if( mTileMap.TypeAt( x, yy ) == TTileType::Ground ) {
				y = yy;
				goto FoundLabel;
			}
		}

		if( x < (mTileMap.mParam.w - 1) )
			++x;
		if( y < (mTileMap.mParam.h - 1) )
			++y;

		if( x == (mTileMap.mParam.w - 1)
		&&  y == (mTileMap.mParam.h - 1) )
			break;
	}
	throw TException( "TPlayState::BeginPlay: ground tile for character not found.");
FoundLabel:
	mCharacter.SetPos(x, y);
	mCharacter.xoff = 0.0f;
	mCharacter.yoff = 0.0f;
	mCharacter.SetDir(TDir::Down);

	mCandysFound = 0;
	mCandys.clear();
	
	int GroundCount = mTileMap.CountOf( TTileType::Ground );
	int CandyCount = int(float(GroundCount) / 10.0f);
	mCandys.reserve( CandyCount );
	sf::Vector2i CandyPos;
	for( int i = 0; i < CandyCount; ++i )
	{
		do {
			CandyPos.x = rand() % mTileMap.mParam.w;
			CandyPos.y = rand() % mTileMap.mParam.h;
		} while( mTileMap.TypeAt( CandyPos.x, CandyPos.y ) != TTileType::Ground );
		mCandys.insert( CandyPos );
	}
	mLevelComplited = false;
	UpdateView();
}