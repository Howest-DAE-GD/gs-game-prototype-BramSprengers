#pragma once
#include "BaseGame.h"
#include "vector2f.h"

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	Point2f m_Pos{0,0};
	Vector2f m_Vel{};
	Vector2f m_Dir{};
	float m_Axel{50};
	float m_MaxSpeed{5};
	float m_DropOff{30.f};

	float m_Boost{};
	float m_Boosting{ 25 };
	float m_BoostDropOff{ 15.f };
	float m_Sprint{};

	float m_Angle{};

	const float m_Size{ 25 };


	std::vector<Point2f> m_Bottom{ Point2f(0,0),Point2f(0,0) };
	std::vector<Point2f> m_Top{ Point2f(0,0),Point2f(0,0) };

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	void UpdateSpeed(float elepsedSec);
	void Slash(float elepsedSec);
};