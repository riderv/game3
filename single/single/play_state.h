#pragma once

#include "game_state.h"


// ������ � �������
#define MoveSpeed_TilePerSec 2.0f

#define AnimSpeed 0.25f
static const int FrameCount = 4;

enum class TDir: i32 { Down, Up, Rigth, Left };
enum class TCharState { 
	GoingDownOut,  // ������ �������� ���� (��� �� �������� ����)
	GointDownIn,   // ���������� ��������� ���� (����� �� ����� ����)
	GoingUpOut,
	GointUpIn,
	GoingRigthOut,
	GoingRigthIn,
	GoingLeftOut,
	GoingLeftIn,
	Stay };

struct TCharacter
{
	int x = 0;	// pos in tile-map
	int y = 0;
	float xoff = 0.0f; // offset in tile
	float yoff = 0.0f; 
	float Frame = 0.0f;
	TDir Dir = TDir::Down;
	int AnimLine = 0;

	void SetPos(int x, int y) { this->x = x; this->y = y; }
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
			TCoord2UInt c( v.x, v.y );
			return std::hash<unsigned int>()( c.ui );
		}
	};
}
struct TPlayState : IGameState, noncopyable
{
	TPlayState();
	~TPlayState() override;

	void PoolEvent(sf::Event &) override;
	void Draw() override;
	void OnResize() override;
	void Simulate() override;

	void LoadAndPlay(const wchar_t* FileName);
	void BeginPlay();
	void UpdateView();
	void UpdateMapTarget();

	void SimulateMoving();
	void MoveAnimieFrame();

	void OnChangePlayrPos();
	void OnTileReached();

	static void RaiseDBException( TPlayState * This, const std::wstring& ermsg );

private:
	
	sf::Vector2u mPrevWindowSize = { 0,0 };
	sf::Texture mCharacterTexture;
	void *mCharacterTextureBuf = nullptr;
	sf::Sprite mCharacterSprite;

	// �����
	TTileMap mTileMap;
	sf::RenderTexture *mMapTargetTexture = 0; // ���� ����� ������������ ����� ����� (���� ���������� ��������)
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

