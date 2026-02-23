#include "stdafx.h"
#include "MyGame.h"

CMyGame::CMyGame(void) : 
	m_oval(50, 550, 30, CColor::Red(), CColor::Black(), 0),
	m_obstacle(400, 0, 1200, 300, CColor::DarkGreen(), CColor::Black(), 0)
{
}

CMyGame::~CMyGame(void)
{
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	Uint32 t = GetTime();

	// UNCOMMENT THE LINE BELOW TO START
	// Your goal is to get the oval bouncing on the obstacle
	
	// if (m_oval.HitTest(&m_obstacle))		// collision detection
	
	
	
	
	
	m_oval.Accelerate(0, -10);				// apply gravitational acceleration

	m_oval.Update(t);
}

void CMyGame::OnDraw(CGraphics* g)
{
	m_obstacle.Draw(g);
	m_oval.Draw(g);

	*g << centre << "Make the oval sprite BOUNCE" << endl;
	*g << bottom << "Use F2 to restart" << endl;
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	m_obstacle.SetRotation(rand() % 40 - 20);
	m_oval.SetPosition(50, 550);
	m_oval.SetVelocity(360, 0);
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
