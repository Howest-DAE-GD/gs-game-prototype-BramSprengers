#pragma once
#include "BaseGame.h"
#include "vector2f.h"
#include "texture.h"
#include "BaseGame.h"
#include <vector>

enum GameState
{
	start,
	play,
	hint,
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
	void HandleControllerButton(const SDL_Event& e,const SDL_EventType& events) override;
	void HandleControlerLeftStick(const SDL_Event& e) override;
	void HandleControlerRightStick(const SDL_Event& e) override;
	void HandleControlerTrigger(const SDL_Event& e) override;

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
		bool overlaping{ false };
		float cooldownTimer{};
		float poweringupTimer{ (rand() % 10) / -10.f };
		float cooldown{ 2.f };
		float poweringup{ rand() % 5 + 5.f};
		float angle{ float((rand() % 360) / 180.f * M_PI) };
		float size{ 100 };
	};

	bool m_controllerImputing{ false };

	Point2f m_Pos{1000,1000};
	Vector2f m_Vel{};
	Vector2f m_Dir{};

	int AXIS_DEADZONE{ 2000 };

	float m_maxX{ 32767.f };
	float m_maxY{ 32767.f };

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

	const float m_Size{ 40 };
	const float m_EnemSize{ 25 };

	bool m_SpaceHold{};

	static const int m_LightX{ 50 };
	static const int m_LightY{ m_LightX };

	std::vector<Color4f> m_LightCollor;

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
	Texture* m_pHintButton{};
	Texture* m_pMoevement{};
	Texture* m_pSlashing{};
	Texture* m_pDodge{};
	Texture* m_pSurvived{};
	Texture* m_pFool{};

	Texture* m_pControls{};

	std::string m_Poler{""};

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

	void LightCal(int x, int y, std::vector<Color4f>& light, const Point2f& target,const Color4f& baseColor,const Color4f& falloff);
	void DrawFloor(int x, int y,const std::vector<Color4f>& light) const;
	void ResetLight(int x, int y, std::vector<Color4f>& light, const Point2f& target, Color4f baseColor, Color4f falloff);

	void Light(const Point2f& target,const Color4f& baseColor,const Color4f& falloff) const;
	void Light(float x, float y,const Color4f& baseColor,const Color4f& falloff) const;

	void UpdateText();
};