#pragma once
#include "BaseGame.h"
#include "vector2f.h"
#include <vector>

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

	struct Enem
	{
		Point2f pos;
		Vector2f vel{ 0,0 };
		Vector2f dir{ 0,0 };
		float angle{ 0 };
		bool gotHit{ false };
	};

	
	Point2f m_Pos{200,200};
	Vector2f m_Vel{};
	Vector2f m_Dir{};

	float m_Axel{50};
	float m_MaxSpeed{5};
	float m_DropOff{30.f};
	float m_DashTimer{};

	float m_Boost{};
	float m_Boosting{ 50.f };
	float m_BoostDropOff{ 5.f };
	float m_Sprint{};

	const float m_Ofset{ 10 };
	const float m_BladeSize{ 100 };

	float m_OldAngle{};
	float m_NewAngle{};

	const float m_Size{ 25 };
	const float m_EnemSize{ 25 };

	bool m_SpaceHold{};

	std::vector<Point2f> m_Bottom{ Point2f(0,0),Point2f(0,0) };
	std::vector<Point2f> m_Top{ Point2f(0,0),Point2f(0,0) };
	std::vector<Point2f> m_Slash{};

	Point2f m_MouseP{};
	Vector2f m_MouseV{};

	float m_BottomDis{};
	float m_TopDis{};

	float m_stunTimer{};

	std::vector<Enem> m_enemey{};

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	void UpdateAngel(float elepsedSec);
	void UpdateSpeed(float elepsedSec);
	void Slash(float elepsedSec);
	void UpdateEnem(float elepsedSec);
};