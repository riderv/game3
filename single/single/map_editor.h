#pragma once

#include "tile_map.h"

// Решил таки скрыть TMapEditorState::mTileMap в привате,
// а доступ через этот интерфейс.
// По хорошему к каждому объектному-члену-класса так надо закрывать доступ,
// но -- муууторно :), особенно при отсутствии "свойств" в С++,
// потому только когда вижу явную необходимость - чтобы
// др. часть кода не зависила от реализации,
// так как реализация с высокой вероятностью изменится.

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

	// когда в редакторе карты жмём...
	static void DoOnSave(TMapEditorState *This); // F5
	static void DoOnLoad(TMapEditorState *This); // F6
	static void OnExit(TMapEditorState *This);   // F1 - хотел F12 но она с какого то перехватывается толи студией, толи ещё чем.
	static void OnDefTileBrush(TMapEditorState *This); // тильдой
	static void OnStoneBrush(TMapEditorState *This);	// далее цифрами
	static void OnGroundBrush( TMapEditorState *s );
	static void OnWaterBrush( TMapEditorState *s );

	static void RaiseDBException( TMapEditorState *This, const std::wstring &erm ); // класс враппер sqlite будет вызывать в случае ошибки, чтобы пользователь sqlite-а сам бросил нужное ему исключение (чтобы catch блоки не разрастались)
	
	void OnMouseClick(int x, int y);
	void LoadMap(const wchar_t* FileName);	// когда в главном меню выбираем LoadMap

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
