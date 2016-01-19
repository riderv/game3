#pragma once


char g_GameStateSpace[sizeof(TGameState)] = { 0 };
TGameState * gpGameState = nullptr;
extern sf::RenderWindow *gpWin = 0;

//----------------------------
//			TGameState
//----------------------------

TGameState::TGameState()
{

}

void TGameState::PoolEvent(sf::Event &Event)
{
	if (Event.type == sf::Event::Closed)
	{
		// do some action (maybe dialog)

		mClosed = true;
	}
	else
	{
		mCurrentState->PoolEvent(Event);
	}
}

void TGameState::GotoNewMapEditor()
{
	if (!mMapEditorState)
	{
		mMapEditorState = new TMapEditorState;
		mMapEditorState->GenerateMap();
	}
	
	mCurrentState = mMapEditorState;
}

//----------------------------
//			TMainMenuState
//----------------------------

TMainMenuState::TMainMenuState()
{
	assert(mFont.loadFromFile("c:/windows/fonts/consola.ttf"));
	for (int i = 0; i < enCount; ++i)
	{
		sf::Text & t = mTxtMenus[i];
		t.setFont(mFont);
		t.setCharacterSize(20);
		t.setPosition( 100.0f, float(100 + i * 30) );
		t.setColor(sf::Color::White);
	}
	mTxtMenus[enCaption].setPosition(100.0f, 100.0f - 30);
	mTxtMenus[enCaption].setString(L"Ρσοεπ γειμ:");
	mTxtMenus[enCaption].setCharacterSize(30);

	mTxtMenus[enGenNewMap].setString("1) Generate and edit new map.")
	mTxtMenus[enLoadMap].setString("2) Load map...");
	mTxtMenus[enLoadLastMap].setString("3) Load last map.");
	
}

void TMainMenuState::PoolEvent(sf::Event & Event)
{
	if (Event.type == sf::Event::KeyReleased)
	{
		switch (Event.key.code)
		{
		case sf::Keyboard::Num1:
			gpGameState->GotoNewMapEditor();
			break;
		case sf::Keyboard::Num2:
			MessageBoxA(0, "Num2 sf::Event::KeyReleased", "Event", 0);
			break;
		case sf::Keyboard::Num3:

			break;
		}
	}
}

void TMainMenuState::Draw()
{
	for (auto t : mTxtMenus)
		gpWin->draw(t);
}

void TMainMenuState::Simulate()
{
}

//------------------------------------------
//			 TMapEditorState
//------------------------------------------

void TMapEditorState::PoolEvent(sf::Event &)
{

}

void TMapEditorState::Simulate()
{

}
void TMapEditorState::Draw()
{

}

void TMapEditorState::GenerateMap()
{

}