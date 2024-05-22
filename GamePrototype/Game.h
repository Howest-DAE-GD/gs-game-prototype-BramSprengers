#pragma once
#include "BaseGame.h"
#include "vector2f.h"
#include "texture.h"
#include <vector>

enum GameState
{
	start,
	play,
	death
};

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

	struct BusSawEnem
	{
		Point2f pos;
		Vector2f vel{ 0,0 };
		Vector2f dir{ 0,0 };
		float angle{ 0 };
		bool gotHit{ false };
		float size{ 25 };
	};

	struct SawEnem
	{
		Point2f pos;
		Vector2f vel{ 0,0 };
		Vector2f dir{ 0,0 };
		bool LookAtPlayer{ false };
		bool gotHit{ false };
		float angle{ 0 };
		std::vector<Point2f> box{};
		float size{ 75 };
	};

	struct FistEnem
	{
		Point2f pos;
		Vector2f vel{ 0,0 };
		Vector2f dir{ 0,0 };
		bool gotHit{ false };
		bool isBoosting{ false };
		bool isPoweringup{ false };
		float cooldownTimer{};
		float poweringupTimer{};
		float cooldown{ 2.f };
		float poweringup{ 5.f };
		float angle{ 0 };
		float size{ 100 };
	};
	
	Point2f m_Pos{1000,1000};
	Vector2f m_Vel{};
	Vector2f m_Dir{};

	float m_Axel{50};
	float m_MaxSpeed{5};
	float m_DropOff{30.f};
	float m_DashTimer{};

	Vector2f m_Boost{};
	float m_Boosting{ 5000.f };
	float m_BoostDropOff{ 10.f };

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

	float m_oselator{};

	bool m_switch{};

	int m_Wave{ 1 };

	int m_HighScore{1};

	int m_FontSize{ 18 * 2 };
	Texture* m_pText{};
	Texture* m_pScore{};
	Texture* m_pTitle{};
	Texture* m_pStart{};

	std::vector<BusSawEnem> m_BusSaw{};
	std::vector<SawEnem> m_SawEnem{};
	std::vector<FistEnem> m_FistEnem{};

	Rectf m_Floor{250,250,2000,2000};
	Rectf m_Hole{ 0,0,2500,2500 };

	GameState m_State{start};
	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	void UpdateAngel(float elepsedSec);
	void UpdateSpeed(float elepsedSec);
	void Slash(float elepsedSec);
	void UpdateBuzSawEnem(float elepsedSec);
	void UpdateSawEnem(float elepsedSec);
	void UpdateFistEnem(float elepsedSec);

	void Light(const Point2f& target, Color4f baseColor, Color4f falloff) const;
	void Light(float x, float y, Color4f baseColor, Color4f falloff) const;

	void UpdateText();
};