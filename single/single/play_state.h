#pragma once

#include "game_state.h"


// тайлов в секунду
#define MoveSpeed_TilePerSec 2.0f

#define AnimSpeed 0.25f
static const int FrameCount = 4;

enum TDir: ui32 { Down, Up, Rigth, Left };
enum TCharState { 
	GoingDownOut,  // начали движение вниз (ещё не покинули тайл)
	GointDownIn,   // продолжаем двигаться вниз (вошли на новый тайл)
	GoingUpOut,
	GointUpIn,
	GoingRigthOut,
	GoingRigthIn,
	GoingLeftOut,
	GoingLeftIn,
	Stay };

struct TCharacter
{
	int x = 0;	// pos in tile
	int y = 0;
	float xoff = 0.0f; // offset in tile
	float yoff = 0.0f; 
	float Frame = 0.0f;
	TDir Dir = TDir::Down;
	int AnimLine = 0;
	void SetPos(int x, int y) { this->x = x; this->y = y; }
//	void SetPos(float x, float y) { this->x = x; this->y = y; }
	void SetDir(TDir direction) { Dir = direction; }	
	TCharState State = TCharState::Stay;
	void MoveFrame(float dist) {
		Frame += dist;
		if( Frame >= FrameCount) {
			Frame -= FrameCount;
		}
	}
	
};

namespace std {
	template <>
	struct std::hash<sf::Vector2i> {
		size_t operator()( const sf::Vector2i& v ) const
		{
			TCoord2Int c( v.x, v.y );
			return std::hash<unsigned int>()( c.ui );
		}
	};
}
struct TPlayState : IGameState, noncopyable
{
	TPlayState::TPlayState();
	TPlayState::~TPlayState() override;

	void TPlayState::PoolEvent(sf::Event &) override;
	void TPlayState::Draw() override;
	void TPlayState::OnResize() override;
	void TPlayState::Simulate() override;

	void TPlayState::LoadAndPlay(const wchar_t* FileName);
	void TPlayState::BeginPlay();
	void TPlayState::UpdateView();
	void TPlayState::UpdateMapTarget();

	void TPlayState::SimulateMoving();
	void TPlayState::MoveAnimieFrame();

	void TPlayState::OnChangePlayrPos();
	void TPlayState::OnTileReached();

	static void TPlayState::RaiseDBException( TPlayState * This, const std::wstring& ermsg );

private:
	
	sf::Texture mCharacterTexture;
	void *mCharacterTextureBuf = nullptr;
	sf::Sprite mCharacterSprite;

	// карта
	TTileMap mTileMap;
	sf::Sprite mTileSprite;
	sf::RenderTexture *mMapTarget = 0; // сюда будем отрисовывать тайлы карты
	sf::Sprite mTargetSprite;
	bool mInvalidMapTarget = true;

	// view
	sf::View mView;
	sf::Vector2i mViewOffsetInTiles = { 0, 0 };
	sf::Vector2i mViewSizeInTiles = { 0, 0 };

	TCharacter mCharacter;

	int mCandysFound = 0;
	
	typedef std::unordered_set< sf::Vector2i > TCandys;
	typedef TCandys::iterator TCandysItr;
	TCandys mCandys;
	sf::Sprite mCandySprite;

	sf::Clock mKeyDelayClock;
	unsigned mSounNyamNo = 0;

	bool mLevelComplited = false;

};

