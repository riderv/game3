#pragma once

#include "tile_map.h"

// ����� ���� ������ TMapEditorState::mTileMap � �������,
// � ������ ����� ���� ���������.
// �� �������� � ������� ����������-�����-������ ��� ���� ��������� ������,
// �� -- ��������� :), �������� ��� ���������� "�������" � �++,
// ������ ������ ����� ���� ����� ������������� - �����
// ��. ����� ���� �� �������� �� ����������,
// ��� ��� ���������� � ������� ������������ ���������.

struct ITileMap 
{
	virtual           ~ITileMap() {}
	virtual void      Set(int x, int y, TTileType val) = 0;
	virtual TTileType Get(int x, int y) const = 0;
};

struct TMapEditorState : IGameState, noncopyable
{
		 TMapEditorState();
		 ~TMapEditorState() override;
	void PoolEvent(sf::Event &) override;
	void Simulate() override;
	void Draw() override;
	void OnResize() override;

	void CreateMap(const TMapParams& MapParams);
	void UpdateView();

	// ����� � ��������� ����� ���...
	static void DoOnSave(TMapEditorState *This); // F5
	static void DoOnLoad(TMapEditorState *This); // F6
	static void OnExit(TMapEditorState *This);   // F1 - ����� F12 �� ��� � ������ �� ��������������� ���� �������, ���� ��� ���.
	static void OnDefTileBrush(TMapEditorState *This); // �������
	static void OnStoneBrush(TMapEditorState *This);	// ����� �������
	static void OnGroundBrush( TMapEditorState *s );
	static void OnWaterBrush( TMapEditorState *s );

	static void RaiseDBException( TMapEditorState *This, const std::wstring &erm ); // ����� ������� sqlite ����� �������� � ������ ������, ����� ������������ sqlite-� ��� ������ ������ ��� ���������� (����� catch ����� �� ������������)
	
	void OnMouseClick(int x, int y);
	void LoadMap(const wchar_t* FileName);	// ����� � ������� ���� �������� LoadMap

	ITileMap&		TileMap()		{ return miTileMap; }
	ITileMap const& TileMap() const { return miTileMap; }
	
private:

	TTileMap mTileMap;

	sf::View mView;
	sf::Vector2i mViewOffsetInTiles = { 0, 0 };
	sf::Vector2i mViewSizeInTiles = { 0, 0 };
	sf::Sprite mTileSprite;

	sf::Clock mKeyDelayClock;
	
	TMenu mMenu;
	sf::Sprite mCursorSprite;
	TTileType mCurrentBrush = mTileMap.mParam.DefaultTileType;

	struct ITileMapImpl: ITileMap
	{
		void      ITileMapImpl::Set(int x, int y, TTileType val) override;
		TTileType ITileMapImpl::Get(int x, int y) const override;
	}miTileMap;
	
	sf::Text mLastAction;
};
