#ifndef __GAMESTATE_HPP__
#define __GAMESTATE_HPP__

class GameState
{
public:
	GameState () {}

	virtual void OnStart() = 0;
	virtual void OnPlay() = 0;
	virtual void OnPause() = 0;
	virtual void OnResume() = 0;
	virtual void OnExit() = 0;
};

#endif // __GAMESTATE_HPP__