#include "pch.h"
#include "Game.h"
#include "utils.h"
#include <iostream>
#include <string>
#include "Camera.h"

Game::Game( const Window& window ) 
	:BaseGame{ window }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{

	m_pScore = new Texture(std::string("HIGHSCORE: " + std::to_string(m_HighScore)), "upheavtt.ttf", 18 * 3, Color4f(1, 1, 1, 1));

	m_pSurvived = new Texture(std::string("waves survived: " + std::to_string(m_Wave)), "upheavtt.ttf", 18 * 3, Color4f(1, 1, 1, 1));

	m_pText = new Texture(std::string("wave: " + std::to_string(m_Wave)), "upheavtt.ttf", m_FontSize, Color4f(1, 1, 1, 1));

	m_pTitle = new Texture(std::string("SLASH & BASH" + std::to_string(m_HighScore)), "upheavtt.ttf", 18 * 5, Color4f(1, 1, 1, 1));

	m_pStart = new Texture(std::string("press ENTER to start."), "upheavtt.ttf", 18 * 3, Color4f(1, 1, 1, 1));

	m_pHintButton = new Texture(std::string("when in game press P to open/close the hint menu."), "upheavtt.ttf", 18 * 3, Color4f(1, 1, 1, 1));

	m_pMoevement = new Texture(std::string("w/a/s/d (z/q/s/d) to move."), "upheavtt.ttf", 18 * 3, Color4f(1, 1, 1, 1));
	m_pSlashing = new Texture(std::string("move the mous to slash."), "upheavtt.ttf", 18 * 3, Color4f(1, 1, 1, 1));
	m_pDodge = new Texture(std::string("space to dodge."), "upheavtt.ttf", 18 * 3, Color4f(1, 1, 1, 1));

	m_pFool = new Texture(std::string("you fool i am a polerbear!!!"), "upheavtt.ttf", 18, Color4f(1, 1, 1, 1));

    m_Top[0] = Point2f(sin(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y);
    m_Bottom[0] = Point2f(sin(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.y);

    m_Top[1] = Point2f(sin(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y);
    m_Bottom[1] = Point2f(sin(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.y);
    
	int enemySize{ 1 };

	for (int i = 0; i < enemySize; i++)
	{
		BusSawEnem test{ Point2f(float(rand() % int(m_Floor.width - m_Floor.left) + m_Floor.left), float(rand() % int(m_Floor.height - m_Floor.bottom) + m_Floor.bottom)) };
		m_BusSaw.push_back(test);
	}

	//for (int i = 0; i < enemySize; i++)
	//{
	//	FistEnem test{ Point2f(float(rand() % int(m_Floor.width - m_Floor.left) + m_Floor.left), float(rand() % int(m_Floor.height - m_Floor.bottom) + m_Floor.bottom)) };
	//	m_FistEnem.push_back(test);
	//}
}

void Game::Cleanup( )
{
	delete m_pText;
	m_pText = nullptr;
	delete m_pScore;
	m_pScore = nullptr;
	delete m_pStart;
	m_pStart = nullptr;
	delete m_pTitle;
	m_pText = nullptr;
	delete m_pMoevement;
	m_pMoevement = nullptr;
	delete m_pSlashing;
	m_pSlashing = nullptr;
	delete m_pDodge;
	m_pDodge = nullptr;
	delete m_pSurvived;
	m_pSurvived = nullptr;
	delete m_pHintButton;
	m_pHintButton = nullptr;
	delete m_pFool;
	m_pFool = nullptr;
}

void Game::Update( float elapsedSec )
{
	const Uint8* keys{ SDL_GetKeyboardState(nullptr) };

	if(m_State == play)
	{
		if (!m_controllerImputing)
		{
			m_Dir.x = 0;
			m_Dir.y = 0;
		}

		if (keys[SDL_SCANCODE_W]) m_Dir.y = 1;

		if (keys[SDL_SCANCODE_S]) m_Dir.y = -1;

		if (keys[SDL_SCANCODE_D]) m_Dir.x = 1;

		if (keys[SDL_SCANCODE_A]) m_Dir.x = -1;

		if (keys[SDL_SCANCODE_SPACE])
		{
			if (!m_SpaceHold && m_DashTimer <= 0)
			{
				m_Boost = -Vector2f((GetViewPort().width * 0.5f) - m_MouseP.x, (GetViewPort().height * 0.5f) - m_MouseP.y).Normalized() * m_Boosting;
				m_DashTimer = 1;
			}
			m_SpaceHold = true;
		}
		else m_SpaceHold = false;

		if (keys[SDL_SCANCODE_LSHIFT])
		{
			if (!m_SpaceHold && m_DashTimer <= 0)
			{
				m_Boost = m_Dir * m_Boosting;
				m_DashTimer = 1;
			}
			m_SpaceHold = true;
		}
		else m_SpaceHold = false;



		if (m_DashTimer > 0) m_DashTimer -= elapsedSec;

		float speed{ 5 };

		if (m_switch) m_oselator += elapsedSec * speed;
		else m_oselator -= elapsedSec * speed;

		if (m_oselator > 1.5f) m_switch = false;
		if (m_oselator < -1.f) m_switch = true;

		UpdateAngel(elapsedSec);
		UpdateSpeed(elapsedSec);

		Slash(elapsedSec);
		UpdateBuzSawEnem(elapsedSec);
		UpdateSawEnem(elapsedSec);
		UpdateFistEnem(elapsedSec);

		for (int i = 0; i < m_BusSaw.size(); i++)
		{
			if (!utils::IsPointInRect(m_BusSaw[i].pos, m_Floor))
			{
				m_BusSaw.erase(m_BusSaw.begin() + i);
			}
		}

		for (int i = 0; i < m_SawEnem.size(); i++)
		{
			if (!utils::IsPointInRect(m_SawEnem[i].pos, m_Floor))
			{
				m_SawEnem.erase(m_SawEnem.begin() + i);
			}
		}

		for (int i = 0; i < m_FistEnem.size(); i++)
		{
			if (!utils::IsPointInRect(m_FistEnem[i].pos, m_Floor))
			{
				m_FistEnem.erase(m_FistEnem.begin() + i);
			}
		}

		if (!utils::IsPointInRect(m_Pos, m_Floor))
		{
			m_SawEnem.clear();
			m_BusSaw.clear();
			m_FistEnem.clear();
			
			m_Pos = Point2f(m_Floor.left + m_Floor.width * 0.5f, m_Floor.bottom + m_Floor.height * 0.5f);
			m_Vel = Vector2f(0, 0);
			m_Boost = Vector2f(0, 0);

			m_Top.clear();
			m_Bottom.clear();

			m_Top.push_back(Point2f(sin(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y));
			m_Bottom.push_back(Point2f(sin(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.y));

			m_Top.push_back(Point2f(sin(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y));
			m_Bottom.push_back(Point2f(sin(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.y));

			UpdateText();

			if(m_HighScore <= m_Wave) 
			{
				m_HighScore = m_Wave;
				delete m_pScore;
				m_pScore = new Texture(std::string("HIGHSCORE: " + std::to_string(m_HighScore)), "upheavtt.ttf", 18 * 3, Color4f(1, 1, 1, 1));
			}

			delete m_pSurvived;
			m_pSurvived = new Texture(std::string("waves survived: " + std::to_string(m_Wave)), "upheavtt.ttf", 18 * 3, Color4f(1, 1, 1, 1));

			m_Wave = 0;

			m_State = death;
		}

		if (m_SawEnem.size() == 0 && m_BusSaw.size() == 0 && m_FistEnem.size() == 0)
		{
			++m_Wave;
			std::cout << m_Wave << std::endl;
			for (int i = 0; i < ceilf(m_Wave / 2.f); i++)
			{
				BusSawEnem enem{ Point2f(float(rand() % int(m_Floor.width - m_Floor.left) + m_Floor.left), float(rand() % int(m_Floor.height - m_Floor.bottom) + m_Floor.bottom)) };
				m_BusSaw.push_back(enem);
				UpdateBuzSawEnem(elapsedSec);
			}

			for (int j = 0; j < floorf(m_Wave / 3.f); j++)
			{
				SawEnem saw{ Point2f(float(rand() % int(m_Floor.width - m_Floor.left) + m_Floor.left), float(rand() % int(m_Floor.height - m_Floor.bottom) + m_Floor.bottom)) };
				m_SawEnem.push_back(saw);
				UpdateSawEnem(elapsedSec);
			}

			for (int j = 0; j < floorf(m_Wave / 5.f); j++)
			{
				FistEnem fist{ Point2f(float(rand() % int(m_Floor.width - m_Floor.left) + m_Floor.left), float(rand() % int(m_Floor.height - m_Floor.bottom) + m_Floor.bottom)) };
				m_FistEnem.push_back(fist);
				UpdateFistEnem(elapsedSec);
			}

			UpdateText();
		}
	}	
}

void Game::Draw( ) const
{
	ClearBackground( );

	switch (m_State)
	{
	case start:

	{
		m_pTitle->Draw(Point2f(GetViewPort().width * 0.5f - m_pTitle->GetWidth() * 0.5f, GetViewPort().height * 0.5f + m_pTitle->GetHeight() * 2));
		m_pStart->Draw(Point2f(GetViewPort().width * 0.5f - m_pStart->GetWidth() * 0.5f, GetViewPort().height * 0.5f - m_pStart->GetHeight() * 2));
		m_pHintButton->Draw(Point2f(GetViewPort().width * 0.5f - m_pHintButton->GetWidth() * 0.5f, GetViewPort().height * 0.5f - m_pStart->GetHeight() * 2 - m_pHintButton->GetHeight() * 2));
	}

		break;
	case hint:
	case play:

	{
		Camera camera(GetViewPort().width, GetViewPort().height);

		camera.Aim(m_Hole.width, m_Hole.height, m_Pos);

		utils::SetColor(Color4f(0.f, 0.f, 0.f, 1.f));
		utils::FillRect(m_Hole);

		utils::SetColor(Color4f(0.15f, 0.125f, 0.125f, 1.f));

		Light(m_Floor.left + m_Floor.width * 0.5f, m_Floor.bottom + m_Floor.height * 0.5f, Color4f(0.65f, 0.5f, 0.6f, 0.75f), Color4f(0.75f,0.5f,0.55f,0));

		Light(m_Pos, Color4f(0.75f, 0.75f, 0.75f, 0.25f), Color4f(0.65f, 0.f, 0.75f, 0.75f));

		for (int i = 0; i < m_BusSaw.size() && i <= 3; i++)
		{
			Light(m_BusSaw[i].pos, Color4f(0.75f, 0.25f, 0.25f, 0.3f), Color4f(0.55f, 0.55f, 0.55f, 0.8f));
		}

		for (int i = 0; i < m_SawEnem.size() && i <= 3; i++)
		{
			Light(m_SawEnem[i].pos, Color4f(0.25f, 0.55f, 0.75f, 0.3f), Color4f(0.55f, 0.55f, 0.55f, 0.8f));
		}

		for (int i = 0; i < m_FistEnem.size() && i <= 3; i++)
		{
			Light(m_FistEnem[i].pos, Color4f(1.f - m_FistEnem[i].poweringupTimer / m_FistEnem[i].poweringup, float(m_FistEnem[i].isBoosting), 0.f, 0.3f - (m_FistEnem[i].poweringupTimer / m_FistEnem[i].poweringup) * 0.5f), Color4f(0.55f, 0.55f, 0.55f, 0.8f));
		}

		const float angOfset{ 140.f / 180.f * M_PI };

		const Point2f p1{ m_Pos.x + sinf(m_NewAngle) * m_Size,m_Pos.y + cosf(m_NewAngle) * m_Size };
		const Point2f p2{ m_Pos.x + sinf(m_NewAngle + angOfset) * m_Size, m_Pos.y + cosf(m_NewAngle + angOfset) * m_Size };
		const Point2f p3{ m_Pos.x + sinf(m_NewAngle - angOfset) * m_Size, m_Pos.y + cosf(m_NewAngle - angOfset) * m_Size };

		const Point2f eyeL{ p2 + Vector2f(p1 - p2) * 0.7f };
		const Point2f eyeR{ p3 + Vector2f(p1 - p3) * 0.7f };
		const Point2f tail{ p3 + Vector2f(p2 - p3) * 0.5f };

		if(m_Poler == "poler")
		{
			utils::SetColor(Color4f(1 - m_DashTimer * 0.5f, 1 - m_DashTimer * 0.5f, 1 - m_DashTimer * 0.5f, 1.f));
			utils::FillTriangle(p1, p2, p3);
			utils::FillEllipse(tail, 15, 15);

			utils::SetColor(Color4f(0,0,0,1));
			utils::FillEllipse(eyeL,5,5);
			utils::FillEllipse(eyeR, 5, 5);
		}
		else
		{
			utils::SetColor(Color4f(1 - m_DashTimer, 0, 1.f, 1.f));
			utils::FillTriangle(p1, p2, p3);
		}


		//utils::DrawLine(m_Pos, Point2f(GetViewPort().width * 0.5f, GetViewPort().height * 0.5f), 
			//Vector2f(m_Pos - Point2f(GetViewPort().width * 0.5f , GetViewPort().height * 0.5f)).Length() * 0.01f);

		utils::SetColor(Color4f(0, 1, 0, 1));
		//utils::DrawLine(m_Pos, m_Pos + m_Vel * 25, 2);
		//for (int i{}; i < m_BusSaw.size(); ++i) utils::DrawLine(m_BusSaw[i].pos, m_BusSaw[i].pos + m_BusSaw[i].vel * 25);

		utils::SetColor(Color4f(1, 0, 0, 1));
		//utils::DrawLine(m_Pos, m_Pos + m_Dir.Normalized() * 25, 2);

		

		for (int i = 0; i < ((m_Slash.size()) / 2); i++)
		{
			utils::SetColor(Color4f(1, 1, 1, i / float(m_Slash.size())));
			std::vector<Point2f> box{ m_Slash[i],m_Slash[i + 1], m_Slash[m_Slash.size() - i - 2],m_Slash[m_Slash.size() - i - 1] };
			utils::FillPolygon(box);
		}

		//utils::DrawPolygon(m_Slash,false);
		Color4f gray{ 0.25f,0.25f,0.25f,1 };
		Color4f lightGray{ 0.75f,0.75f,0.75f,1 };

		const float boxSize{ 50 };
		int tieth{ 3 };

		for (int i{}; i < m_BusSaw.size(); ++i)
		{
			utils::SetColor(Color4f(gray));
			for (int j = 0; j < tieth; j++)
			{
				const float angleIn{ 90.f / tieth * j };
				glPushMatrix();
				glTranslatef(m_BusSaw[i].pos.x, m_BusSaw[i].pos.y, 0);
				glRotatef(m_BusSaw[i].angle / float(M_PI) * 180 / 2 + angleIn, 0, 0, 1);
				glTranslatef(-boxSize * 0.5f, -boxSize * 0.5f, 0);
				utils::FillRect(0, 0, boxSize, boxSize);
				glPopMatrix();
			}

			utils::SetColor(Color4f(lightGray));
			utils::FillEllipse(m_BusSaw[i].pos, m_BusSaw[i].size, m_BusSaw[i].size);
		}
		tieth = 4;
		for (int i = 0; i < m_SawEnem.size(); i++)
		{
			utils::SetColor(Color4f(gray));
			const float ofset{ m_SawEnem[i].size / tieth };

			for (int j = 0; j < tieth; j++)
			{

				Point2f lp1{ m_SawEnem[i].box[0] + Vector2f(m_SawEnem[i].box[3] - m_SawEnem[i].box[0]) / float(tieth) * float(j) };
				Point2f lp2{ m_SawEnem[i].box[0] + Vector2f(m_SawEnem[i].box[3] - m_SawEnem[i].box[0]) / float(tieth) * (j + 1.f) };
				Point2f lp3{ m_SawEnem[i].box[0] + Vector2f(m_SawEnem[i].box[3] - m_SawEnem[i].box[0]) / float(tieth) * (j + 0.25f) - Vector2f(m_SawEnem[i].box[1] - m_SawEnem[i].box[0]).Normalized() * 15.f };

				Point2f rp1{ m_SawEnem[i].box[1] + Vector2f(m_SawEnem[i].box[2] - m_SawEnem[i].box[1]) / float(tieth) * float(j) };
				Point2f rp2{ m_SawEnem[i].box[1] + Vector2f(m_SawEnem[i].box[2] - m_SawEnem[i].box[1]) / float(tieth) * (j + 1.f) };
				Point2f rp3{ m_SawEnem[i].box[1] + Vector2f(m_SawEnem[i].box[2] - m_SawEnem[i].box[1]) / float(tieth) * (j + 0.25f) - Vector2f(m_SawEnem[i].box[0] - m_SawEnem[i].box[1]).Normalized() * 15.f };


				utils::FillTriangle(lp1, lp2, lp3);
				utils::FillTriangle(rp1, rp2, rp3);
			}

			utils::SetColor(lightGray);
			utils::FillPolygon(m_SawEnem[i].box);
			//utils::DrawEllipse(m_SawEnem[i].pos, m_SawEnem[i].size, m_SawEnem[i].size);
		}

		for (int i = 0; i < m_FistEnem.size(); i++)
		{
			glPushMatrix();

			glTranslatef(m_FistEnem[i].pos.x, m_FistEnem[i].pos.y, 0);
			glRotatef(m_FistEnem[i].angle / M_PI * 180, 0, 0, 1);
			glTranslatef(-m_FistEnem[i].size * 0.5f, -m_FistEnem[i].size * 0.5f, 0);

			Rectf box{ 0, 0, m_FistEnem[i].size, m_FistEnem[i].size };
			//Rectf boost(m_FistEnem[i].size * 0.125f, m_FistEnem[i].size, m_FistEnem[i].size * 0.75f, m_FistEnem[i].size * 0.15f);

			Rectf indexf{	m_FistEnem[i].size * 0.25f * 0, m_FistEnem[i].size * 0.35f - m_FistEnem[i].size * 0.55f, m_FistEnem[i].size * 0.25f, m_FistEnem[i].size * 0.45f };
			Rectf fuckf{	m_FistEnem[i].size * 0.25f * 1, m_FistEnem[i].size * 0.35f - m_FistEnem[i].size * 0.60f, m_FistEnem[i].size * 0.25f, m_FistEnem[i].size * 0.50f };
			Rectf ringf{	m_FistEnem[i].size * 0.25f * 2, m_FistEnem[i].size * 0.35f - m_FistEnem[i].size * 0.50f, m_FistEnem[i].size * 0.25f, m_FistEnem[i].size * 0.40f };
			Rectf pingkf{	m_FistEnem[i].size * 0.25f * 3, m_FistEnem[i].size * 0.35f - m_FistEnem[i].size * 0.45f, m_FistEnem[i].size * 0.25f, m_FistEnem[i].size * 0.35f };

			utils::SetColor(gray);
			utils::FillRect(box);

			utils::SetColor(Color4f(1.f - m_FistEnem[i].poweringupTimer / m_FistEnem[i].poweringup, float(m_FistEnem[i].isBoosting), 0.f, 1.f));

			utils::FillRect(indexf);
			utils::FillRect(fuckf);
			utils::FillRect(ringf);
			utils::FillRect(pingkf);

			utils::SetColor(gray);

			const float lineWidth{ 3 };

			utils::DrawRect(indexf, lineWidth);
			utils::DrawRect(fuckf , lineWidth);
			utils::DrawRect(ringf , lineWidth);
			utils::DrawRect(pingkf, lineWidth);

			//utils::FillRect(boost);

			glPopMatrix();

			//utils::DrawLine(m_FistEnem[i].pos, m_FistEnem[i].pos + m_FistEnem[i].dir * 100);
		}

		camera.Reset();

		utils::SetColor(Color4f(0, 0, 0, 0.5f));

		float margin{ 15 };

		utils::FillRect(0, GetViewPort().height - m_pText->GetHeight() - margin * 2, m_pText->GetWidth() + margin * 2, GetViewPort().height);
		m_pText->Draw(Point2f(margin, GetViewPort().height - m_pText->GetHeight() - margin));
	}

	if (m_State == hint)
	{
		utils::SetColor(Color4f(0, 0, 0, 0.5f));
		utils::FillRect(GetViewPort());

		m_pMoevement->Draw(Point2f(GetViewPort().width * 0.5f - m_pMoevement->GetWidth() * 0.5f, GetViewPort().height * 0.5f + m_pMoevement->GetHeight() * 2));
		m_pSlashing->Draw(Point2f(GetViewPort().width * 0.5f - m_pSlashing->GetWidth() * 0.5f, GetViewPort().height * 0.5f));
		m_pDodge->Draw(Point2f(GetViewPort().width * 0.5f - m_pDodge->GetWidth() * 0.5f, GetViewPort().height * 0.5f - m_pDodge->GetHeight() * 2));

		m_pHintButton->Draw(Point2f(GetViewPort().width * 0.5f - m_pHintButton->GetWidth() * 0.5f, GetViewPort().height * 0.5f - m_pDodge->GetHeight() * 2 - m_pHintButton->GetHeight() * 4));

		if (m_Poler == "poler")
		{
			m_pFool->Draw();
		}
	}

		break;
	case death:

		m_pScore->Draw(Point2f(GetViewPort().width * 0.5f - m_pScore->GetWidth() * 0.5f, GetViewPort().height * 0.5f + m_pScore->GetHeight() * 2));
		m_pSurvived->Draw(Point2f(GetViewPort().width * 0.5f - m_pSurvived->GetWidth() * 0.5f, GetViewPort().height * 0.5f));

		m_pStart->Draw(Point2f(GetViewPort().width * 0.5f - m_pStart->GetWidth() * 0.5f, GetViewPort().height * 0.5f - m_pStart->GetHeight() * 4));

		break;
	default:
		break;
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	if (e.keysym.sym == SDLK_p)
	{
		if (m_State == hint)
		{
			m_State = play;
		}
		else
		{
			m_State = hint;
		}
	}


	if (e.keysym.sym == SDLK_RETURN)
	{
		m_State = play;
	}


	if(m_Poler != "poler")
	{
		if (e.keysym.sym == SDLK_p && m_Poler == "")
		{
			m_Poler += 'p';
		}
		else if (e.keysym.sym == SDLK_o && m_Poler == "p")
		{
			m_Poler += 'o';
		}
		else if (e.keysym.sym == SDLK_l && m_Poler == "po")
		{
			m_Poler += 'l';
		}
		else if (e.keysym.sym == SDLK_e && m_Poler == "pol")
		{
			m_Poler += 'e';
		}
		else if (e.keysym.sym == SDLK_r && m_Poler == "pole")
		{
			m_Poler += 'r';
		}
		else m_Poler = "";
	}

	std::cout << m_Poler << std::endl;

	if (e.keysym.sym == SDLK_KP_PLUS)
	{
		BusSawEnem test{ Point2f(float(rand() % int(m_Floor.width - m_Floor.left) + m_Floor.left), float(rand() % int(m_Floor.height - m_Floor.bottom) + m_Floor.bottom)) };
		m_BusSaw.push_back(test);
	}

	if (e.keysym.sym == SDLK_KP_MINUS)
	{
		if(m_BusSaw.size() > 0) m_BusSaw.pop_back();
	}

	if (e.keysym.sym == SDLK_KP_9)
	{
		FistEnem test{ Point2f(float(rand() % int(m_Floor.width - m_Floor.left) + m_Floor.left), float(rand() % int(m_Floor.height - m_Floor.bottom) + m_Floor.bottom)) };
		m_FistEnem.push_back(test);
	}

	if (e.keysym.sym == SDLK_KP_6)
	{
		if (m_FistEnem.size() > 0) m_FistEnem.pop_back();
	}

	if (e.keysym.sym == SDLK_KP_8)
	{
		SawEnem test{ Point2f(float(rand() % int(m_Floor.width - m_Floor.left) + m_Floor.left), float(rand() % int(m_Floor.height - m_Floor.bottom) + m_Floor.bottom)) };
		m_SawEnem.push_back(test);
	}

	if (e.keysym.sym == SDLK_KP_5)
	{
		if (m_SawEnem.size() > 0) m_SawEnem.pop_back();
	}
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{

}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	if (m_stunTimer <= 0)
	{
		m_MouseP = Point2f(float(e.x),float(e.y));
	}
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
}

void Game::HandleControllerButton(const SDL_Event& e, const SDL_EventType& events)
{
	//SDL_GameController* controller = SDL_GameControllerFromInstanceID(e.caxis.which);

	//SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_START);


	if(events == SDL_KEYDOWN)
	{
		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_START)
		{
			if (m_State != play) m_State = play;
			else m_State = hint;
		}

		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_BACK)
		{
			if (m_State != hint) m_State = hint;
			else m_State = play;
		}

		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
		{
				if (!m_SpaceHold && m_DashTimer <= 0)
				{
					m_Boost = -Vector2f((GetViewPort().width * 0.5f) - m_MouseP.x, (GetViewPort().height * 0.5f) - m_MouseP.y).Normalized() * m_Boosting;
					m_DashTimer = 1;
				}
				m_SpaceHold = true;
		} 
		else m_SpaceHold = false;

		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
		{
			if (!m_SpaceHold && m_DashTimer <= 0)
			{
				m_Boost = m_Dir.Normalized() * m_Boosting;
				m_DashTimer = 1;
			}
			m_SpaceHold = true;
		}
		else m_SpaceHold = false;
	}
}

void Game::HandleControlerLeftStick(const SDL_Event& e)
{
	SDL_GameController* controller = SDL_GameControllerFromInstanceID(e.caxis.which);

	//m_controllerImputing = false;

	int rx{};
	if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
	{
		rx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
		//if (abs(rx) > AXIS_DEADZONE)
		{
			m_Dir.x = rx / m_maxX;
		}
		if (abs(rx) > AXIS_DEADZONE) m_controllerImputing = true;
		//std::cout << "LEFT Axis Motion: X=" << m_Dir.x << std::endl;
	}


	int ry{};
	if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
	{
		ry = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
		//if (abs(ry) > AXIS_DEADZONE)
		{
			m_Dir.y = -ry / m_maxY;
		}
		if (abs(ry) > AXIS_DEADZONE) m_controllerImputing = true;
		//std::cout << "LEFT Axis Motion: Y=" << y << std::endl;
	}

	if (m_Dir.x < 0.1f && m_Dir.x > -0.1f) m_Dir.x = 0;
	if (m_Dir.y < 0.1f && m_Dir.y > -0.1f) m_Dir.y = 0;

	std::cout << "LEFT Axis Motion: X=" << m_Dir.x << std::endl;
	std::cout << "LEFT Axis Motion: Y=" << m_Dir.y << std::endl;

	//m_Dir = m_Dir.Normalized();
}

void Game::HandleControlerRightStick(const SDL_Event& e)
{
	SDL_GameController* controller = SDL_GameControllerFromInstanceID(e.caxis.which);

	int lx{};
	if (e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
	{
		lx = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
		if (abs(lx) > AXIS_DEADZONE * 2)
		{
			m_MouseP.x = lx;
		}
		std::cout << "Right Axis Motion: X=" << lx << std::endl;
	}

	int ly{};
	if (e.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
	{
		ly = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
		if (abs(ly) > AXIS_DEADZONE * 2)
		{
			m_MouseP.y = -ly;
		}
		//std::cout << "Right Axis Motion: Y=" << y << std::endl;
	}
}

void Game::HandleControlerTrigger(const SDL_Event& e)
{
	SDL_GameController* controller = SDL_GameControllerFromInstanceID(e.caxis.which);

	int lt{};
	if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
	{
		lt = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
		if (abs(lt) > 32760)
		{
				if (!m_SpaceHold && m_DashTimer <= 0)
				{
					m_Boost = m_Dir.Normalized() * m_Boosting;
					m_DashTimer = 1;
				}
				m_SpaceHold = true;
		}
		else m_SpaceHold = false;
	}

	int rt{};
	if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
	{
		rt = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
		if (abs(rt) > 32760)
		{
			if (!m_SpaceHold && m_DashTimer <= 0)
			
			{
					m_Boost = -Vector2f((GetViewPort().width * 0.5f) - m_MouseP.x, (GetViewPort().height * 0.5f) - m_MouseP.y).Normalized() * m_Boosting;
					m_DashTimer = 1;
			}
			m_SpaceHold = true;
		}
	}
	else m_SpaceHold = false;
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.15f, 0.15f, 0.15f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::UpdateAngel(float elepsedSec)
{
	m_MouseV = Vector2f(m_MouseP - Point2f(GetViewPort().width * 0.5f, GetViewPort().height * 0.5f)).Normalized();

	Vector2f up{ cosf(m_OldAngle),-sinf(m_OldAngle) };

	m_NewAngle = (m_MouseV.AngleWith(Vector2f(0,1)));

	float speed{ 15 };
	float angleUpdate{ m_MouseV.DotProduct(up) };

	//std::cout << "old angle: " << m_OldAngle << ", new angle: " << m_NewAngle << ", angle update: " << angleUpdate << std::endl;

	m_OldAngle += angleUpdate * speed * elepsedSec;
}

void Game::UpdateSpeed(float elepsedSec)
{
	const float pushDropoff{ 10 };

	if (m_stunTimer <= 0)
	{
		Vector2f nDir{ m_Dir };
		//if (m_Dir.x != 0 || m_Dir.y != 0) nDir = m_Dir.Normalized();

		if (m_Boost.Length() > m_Axel) 
		{
			m_Boost -= m_Boost * m_BoostDropOff * elepsedSec;
			m_Pos += m_Boost * elepsedSec;
		}
		else 
		{
			m_Boost = m_Boost * 0;

			m_Vel += nDir * m_Axel * elepsedSec;
			if (m_Vel.Length() > m_MaxSpeed) m_Vel = m_Vel.Normalized() * m_MaxSpeed;
			m_Vel += nDir * elepsedSec;
			if ((m_Dir.x == 0 && m_Dir.y == 0)) m_Vel -= m_Vel * m_DropOff * elepsedSec;

			m_Pos += m_Vel;
		}
	}
	else 
	{
		m_stunTimer -= elepsedSec;

		m_Vel -= m_Vel.Normalized() * pushDropoff * elepsedSec;

		if (m_Vel.Length() <= 0.1f) m_stunTimer = 0;

		m_Pos += m_Vel;
	}
}

void Game::Slash(float elepsedSec)
{
    m_Slash.clear();
	m_TopDis = 0;
	m_BottomDis = 0;

    float speed{ 50.f };
    float dis{ 0.01f };
	float maxSpeed{ 1000.f };

                //adding and moving a new point to the list
                if (Vector2f(m_Top[m_Top.size() - 2] - m_Top[m_Top.size() - 1]).Length() >= dis)
                {
                   // std::cout << "adding\n";
                    m_Bottom.push_back(m_Bottom[m_Bottom.size() - 1]);
                    m_Top.push_back(m_Top[m_Top.size() - 1]);
                }

                    for (int i = 0; i < m_Bottom.size() - 1; i++) // calculation of the distance between all points at the bottom
                    {
                        m_BottomDis += Vector2f(m_Bottom[i + 1] - m_Bottom[i]).Length();
                    }

					if (((speed + m_Bottom.size()) * Vector2f(m_Bottom[1] - m_Bottom[0]).Length()) <= maxSpeed)
					{
						m_Bottom[0] += Vector2f(m_Bottom[1] - m_Bottom[0]).Normalized() * ((speed + m_Bottom.size()) * Vector2f(m_Bottom[1] - m_Bottom[0]).Length()) * elepsedSec;
						std::cout << (speed + m_Bottom.size()) * Vector2f(m_Bottom[1] - m_Bottom[0]).Length() << std::endl;
					}
					else 
					{
						m_Bottom[0] += Vector2f(m_Bottom[1] - m_Bottom[0]).Normalized() * maxSpeed * elepsedSec;
						std::cout << maxSpeed << std::endl;
					}

					

                    for (int i = 0; i < m_Top.size() - 1; i++) // calculation of the distance between all points at the top
                    {
                        m_TopDis += Vector2f(m_Top[i + 1] - m_Top[i]).Length();
                    }

					if (((speed + m_Top.size()) * Vector2f(m_Top[1] - m_Top[0]).Length()) <= maxSpeed) 
					{
						m_Top[0] += Vector2f(m_Top[1] - m_Top[0]).Normalized() * ((speed + m_Top.size()) * Vector2f(m_Top[1] - m_Top[0]).Length()) * elepsedSec;
					}
					else 
					{
						m_Top[0] += Vector2f(m_Top[1] - m_Top[0]).Normalized() * maxSpeed * elepsedSec;
					}

                //remoeving the last one if the list is bigger than 2
                if (m_Bottom.size() > 2)
                {
                    //std::cout << "deleting\n";
                    if (Vector2f(m_Top[0] - m_Top[1]).Length() <= m_TopDis * 0.125f )
                    {
                        m_Bottom.erase(m_Bottom.begin());
                        m_Top.erase(m_Top.begin());
                    }
                }

                m_Top[m_Top.size() - 1] = Point2f(sin(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y);
                m_Bottom[m_Bottom.size() - 1] = Point2f(sin(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.y);

                for (int i{}; i < m_Bottom.size(); i++)
                {
                    m_Slash.push_back(m_Bottom[i]);
                }
                for (int i{ int(m_Top.size() -1) }; i >= 0; i--)
                {
                    m_Slash.push_back(m_Top[i]);
                }

				m_Slash.push_back(m_Bottom[0]);

                //std::cout << Vector2f(m_Bottom[m_Bottom.size() - 1] - m_Bottom[m_Bottom.size() - 2]) << std::endl;
}

void Game::UpdateBuzSawEnem(float elepsedSec)
{
    const float speed{ 10.f };
	const float maxSpeed{ 3.f };
	const float pushback{ 20.f };
	const float maxPushSpeed{ 15.f };
	const float speedDropOff{ 15.f };

    for (int i = 0; i < m_BusSaw.size(); i++)
    {
        m_BusSaw[i].angle += 5.f * elepsedSec;
		Vector2f dirToPlayer{ m_Pos - m_BusSaw[i].pos };
		float multyplier{};

		if(!m_BusSaw[i].gotHit)
		{
			dirToPlayer = dirToPlayer.Normalized();

			m_BusSaw[i].dir = Vector2f(sinf(m_BusSaw[i].angle), cosf(m_BusSaw[i].angle));
			multyplier = (dirToPlayer.DotProduct(m_BusSaw[i].dir.Normalized()) + 1) ;

			m_BusSaw[i].vel += m_BusSaw[i].dir * speed * multyplier * elepsedSec;

			if (m_BusSaw[i].vel.Length() >= maxSpeed) m_BusSaw[i].vel = m_BusSaw[i].vel.Normalized() * maxSpeed;
		}
		else
		{
			m_BusSaw[i].vel -= m_BusSaw[i].vel.Normalized() * speedDropOff * elepsedSec;

			std::cout << m_BusSaw[i].vel.Length() << std::endl;

			if (m_BusSaw[i].vel.Length() <= 1)
			{
				m_BusSaw[i].gotHit = false;
			}
		}

		m_BusSaw[i].pos += m_BusSaw[i].vel;
		//std::cout << multyplier << std::endl;

		Circlef enemHitbox{ m_BusSaw[i].pos,m_BusSaw[i].size};
		Circlef playerHitbox{ m_Pos,m_Size };

		if (utils::IsOverlapping(enemHitbox, playerHitbox) && m_Boost.Length() <= 1)
		{
			m_Vel = dirToPlayer * pushback * m_BusSaw[i].vel.Length() * 10 * elepsedSec;

			if (m_Vel.Length() >= maxPushSpeed) m_Vel = m_BusSaw[i].vel.Normalized() * (maxPushSpeed);

			m_stunTimer = 0.5f;
		}

		if (utils::IsOverlapping(m_Slash,enemHitbox) && m_stunTimer <= 0)
		{
			m_Vel = dirToPlayer * maxPushSpeed * elepsedSec;
			m_BusSaw[i].vel = Vector2f(sinf(m_OldAngle),cosf(m_OldAngle)) * (m_TopDis + m_BottomDis) * 0.5f * pushback * elepsedSec;

			if (m_BusSaw[i].vel.Length() >= maxPushSpeed) m_BusSaw[i].vel = m_BusSaw[i].vel.Normalized() * maxPushSpeed;

			m_BusSaw[i].gotHit = true;
		}

		for (int j = 0; j < m_BusSaw.size(); j++)
		{
			if (i != j)
			{
				Circlef otherEnemHitbox{ m_BusSaw[j].pos,m_BusSaw[j].size };
				if (utils::IsOverlapping(otherEnemHitbox, enemHitbox))
				{
					const Vector2f pushDir{ m_BusSaw[j].pos - m_BusSaw[i].pos };

					m_BusSaw[i].vel = -pushDir * pushback * 0.5f * elepsedSec;
					m_BusSaw[j].vel = pushDir * pushback * 0.5f * elepsedSec;

					m_BusSaw[i].gotHit = true;
					m_BusSaw[j].gotHit = true;
				}
			}
		}

		for (int j = 0; j < m_SawEnem.size(); j++)
		{
			//Circlef otherEnemHitbox{ m_SawEnem[j].pos, 50.f };
			if (utils::IsOverlapping(m_SawEnem[j].box, enemHitbox))
			{
				const Vector2f pushDir{ m_SawEnem[j].pos - m_BusSaw[i].pos };

				m_BusSaw[i].vel = -pushDir * pushback * 0.75f * elepsedSec;
				m_SawEnem[j].vel = pushDir * pushback * 0.25f * elepsedSec;

				m_BusSaw[i].gotHit = true;
				m_SawEnem[j].gotHit = true;
			}
		}
    }
}

void Game::UpdateSawEnem(float elepsedSec)
{
	//const float size{ 50 };
	const float squereAngle{ (360.f / 4.f) / 180.f * float(M_PI) };
	const float turnSpeed{ 0.5f };
	const float vel{ 5.f };
	const float maxSpeed{ 2.f };
	const float speedDropOff{ 7.f };
	const float pushback{ 15.f };
	const float maxPushSpeed{ 5.f };

	for (int i = 0; i < m_SawEnem.size(); i++)
	{
		m_SawEnem[i].box.clear();

		Vector2f dirToPlayer{ m_Pos - m_SawEnem[i].pos };
		const float angleOfset{ 0 };

		Vector2f AngleWithPayer{ Vector2f(m_SawEnem[i].pos - m_Pos).Normalized() };

		Vector2f up{ cosf(m_SawEnem[i].angle),-sinf(m_SawEnem[i].angle) };

		m_SawEnem[i].angle += AngleWithPayer.DotProduct(up) * turnSpeed * elepsedSec;

		for (int j{}; j < 4; ++j) 
		{
			m_SawEnem[i].box.push_back(Point2f(m_SawEnem[i].pos.x + sinf(m_SawEnem[i].angle + squereAngle * j + angleOfset) * m_SawEnem[i].size, m_SawEnem[i].pos.y + cosf(m_SawEnem[i].angle + squereAngle * j + angleOfset) * m_SawEnem[i].size));
		}

		m_SawEnem[i].dir = Vector2f(m_Pos - m_SawEnem[i].pos).Normalized();

		if (!m_SawEnem[i].gotHit)
		{
			m_SawEnem[i].vel += m_SawEnem[i].dir * (vel * m_oselator) * elepsedSec;

			if (m_SawEnem[i].vel.Length() >= maxSpeed) m_SawEnem[i].vel = m_SawEnem[i].vel.Normalized() * maxSpeed;
		}

		else
		{
			m_SawEnem[i].vel -= m_SawEnem[i].vel.Normalized() * speedDropOff * elepsedSec;
			if (m_SawEnem[i].vel.Length() <= 1)
			{
				m_SawEnem[i].gotHit = false;
			}
		}

		m_SawEnem[i].pos += m_SawEnem[i].vel;

		Circlef playerHitbox{ m_Pos,m_Size };

		if (utils::IsOverlapping(m_SawEnem[i].box, playerHitbox) && m_Boost.Length() <= 1)
		{
			m_Vel = dirToPlayer * pushback * m_SawEnem[i].vel.Length() * 10 * elepsedSec;

			if (m_Vel.Length() >= maxPushSpeed) m_Vel = m_SawEnem[i].vel.Normalized() * (maxPushSpeed);

			m_stunTimer = 0.5f;
		}

		std::vector<Point2f> line1{ m_SawEnem[i].box[0],m_SawEnem[i].box[1] };
		std::vector<Point2f> line2{ m_SawEnem[i].box[2],m_SawEnem[i].box[3] };
		std::vector<Point2f> line3{ m_SawEnem[i].box[0],m_SawEnem[i].box[3] };
		std::vector<Point2f> line4{ m_SawEnem[i].box[1],m_SawEnem[i].box[2] };

		for (int j = 0; j < m_Top.size(); j++)
		{
			utils::HitInfo info{};

			if (utils::Raycast(line1, m_Bottom[j], m_Top[j], info) || utils::Raycast(line2, m_Bottom[j], m_Top[j], info) && !m_SawEnem[i].gotHit) // good hit
			{
				//m_Vel = dirToPlayer * maxPushSpeed * elepsedSec;
				m_SawEnem[i].gotHit = true;
				m_SawEnem[i].vel = -dirToPlayer.Normalized() * pushback * (m_TopDis + m_BottomDis) * 0.5f * elepsedSec;
				if (m_SawEnem[i].vel.Length() >= maxPushSpeed * 2)
				{
					m_SawEnem[i].vel = m_SawEnem[i].vel.Normalized() * maxPushSpeed * 2;
				}
			}
			else if (utils::Raycast(line3, m_Bottom[j], m_Top[j], info) || utils::Raycast(line4, m_Bottom[j], m_Top[j], info) && !m_SawEnem[i].gotHit) // bad hit
			{
				m_stunTimer = 0.25f;
				m_SawEnem[i].vel = -dirToPlayer.Normalized() * pushback * (m_TopDis + m_BottomDis) * 0.25f * elepsedSec;
				m_Vel = dirToPlayer.Normalized() * pushback * (m_BladeSize - info.lambda) * elepsedSec;

				if (m_Vel.Length() >= 0.25f * maxPushSpeed)
				{
					m_Vel = m_Vel.Normalized() * maxPushSpeed * 0.25f;
				}

				if (m_Vel.Length() <= 2.f)
				{
					m_Vel = m_Vel.Normalized() * pushback;
				}
			}
		}
		Circlef enemHitbox{ m_SawEnem[i].pos,m_SawEnem[i].size * 0.5f };
		for (int j = 0; j < m_SawEnem.size(); j++)
		{
			if (i != j)
			{
				Circlef otherEnemHitbox{ m_SawEnem[j].pos,m_SawEnem[j].size * 0.5f };
				if (utils::IsOverlapping(otherEnemHitbox, enemHitbox))
				{
					const Vector2f pushDir{ m_SawEnem[j].pos - m_SawEnem[i].pos };

					m_SawEnem[i].vel = -pushDir * pushback * 0.15f * elepsedSec;
					m_SawEnem[j].vel = pushDir * pushback * 0.15f * elepsedSec;

					m_SawEnem[i].gotHit = true;
					m_SawEnem[j].gotHit = true;
				}
			}
		}
	}
}

void Game::UpdateFistEnem(float elepsedSec)
{
	const float turnSpeed{ 0.75f };
	const float maxSpeed{ 1000.f };
	const float speedDropOff{ 50.f };
	const float pushback{ 20.f };
	const float maxPushSpeed{ 20.f };

	Circlef playerHitbox{ m_Pos,m_Size * 0.5f};

	for (int i = 0; i < m_FistEnem.size(); i++)
	{
		Vector2f dirToPlayer{ m_Pos - m_FistEnem[i].pos };
		Circlef enemHitbox{ m_FistEnem[i].pos, m_FistEnem[i].size * 0.5f };

		if (m_FistEnem[i].isBoosting)
		{
			m_FistEnem[i].pos += m_FistEnem[i].vel * elepsedSec;
			if(!m_FistEnem[i].gotHit) m_FistEnem[i].vel -= m_FistEnem[i].vel.Normalized() * (speedDropOff + (Vector2f(Point2f(m_Floor.left + m_Floor.width * 0.5f, m_Floor.bottom + m_Floor.height * 0.5f) - m_FistEnem[i].pos).Length() * 0.9f)) * elepsedSec;
			else m_FistEnem[i].vel -= m_FistEnem[i].vel.Normalized() * (speedDropOff + (Vector2f(Point2f(m_Floor.left + m_Floor.width * 0.5f, m_Floor.bottom + m_Floor.height * 0.5f) - m_FistEnem[i].pos).Length() * 0.25f)) * elepsedSec;

			if (m_FistEnem[i].vel.Length() <= 5)
			{
				m_FistEnem[i].isBoosting = false;
				m_FistEnem[i].cooldownTimer = m_FistEnem[i].cooldown;
				m_FistEnem[i].poweringupTimer = m_FistEnem[i].poweringup;
				m_FistEnem[i].gotHit = false;
			}

			if (utils::IsOverlapping(enemHitbox, playerHitbox))
			{
				m_stunTimer = 0.5f;
				m_Vel = dirToPlayer.Normalized() * m_FistEnem[i].vel.Length() * elepsedSec * 2.5f;
				m_FistEnem[i].vel = -dirToPlayer.Normalized() * m_FistEnem[i].vel.Length() * 0.75f;

				if (m_Vel.Length() >= maxPushSpeed)
				{
					m_Vel = m_Vel.Normalized() * maxPushSpeed * elepsedSec;
				}

				Vector2f up{ 0,1 };

				m_FistEnem[i].angle = -dirToPlayer.Normalized().AngleWith(up);
			}

			if (utils::IsOverlapping(m_Slash, enemHitbox) && m_Slash.size() >= 6 && !m_FistEnem[i].overlaping)
			{
				m_FistEnem[i].gotHit = true;
				m_Vel = dirToPlayer.Normalized() * m_FistEnem[i].vel.Length() * 1.5f;
				m_FistEnem[i].vel = -dirToPlayer.Normalized() * m_FistEnem[i].vel.Length() * 0.80f;

				Vector2f up{ 0,1 };

				m_FistEnem[i].angle = -dirToPlayer.Normalized().AngleWith(up);
				m_FistEnem[i].overlaping = true;
			}

			if (!utils::IsOverlapping(m_Slash, enemHitbox))
			{
				m_FistEnem[i].overlaping = false;
			}

			for (int j = 0; j < m_BusSaw.size(); j++)
			{
				Circlef otherEnem{ m_BusSaw[j].pos,m_BusSaw[j].size };

				if (utils::IsOverlapping(otherEnem, enemHitbox))
				{
					Vector2f dirWithEnem{ Vector2f(m_FistEnem[i].pos - m_BusSaw[j].pos) };

					m_BusSaw[j].gotHit = true;
					m_BusSaw[j].vel = -dirWithEnem * pushback * 0.5f * elepsedSec;
					std::cout << m_BusSaw[j].vel.Length();

					m_FistEnem[i].vel = dirWithEnem.Normalized() * m_FistEnem[i].vel.Length() * 0.9f;

					m_FistEnem[i].angle = -dirToPlayer.Normalized().AngleWith(dirWithEnem.Normalized());
				}
			}

			for (int j = 0; j < m_FistEnem.size(); j++)
			{
				Circlef otherEnem{ m_FistEnem[j].pos,m_FistEnem[j].size };

				if(j != i)
				{
					if (utils::IsOverlapping(otherEnem, enemHitbox))
					{
						Vector2f dirWithEnem{ Vector2f(m_FistEnem[i].pos - m_FistEnem[j].pos) };

						m_FistEnem[j].vel = -dirWithEnem.Normalized() * m_FistEnem[j].vel.Length() * 0.8f;
						m_FistEnem[j].angle = -dirToPlayer.Normalized().AngleWith(dirWithEnem.Normalized());

						m_FistEnem[i].vel = dirWithEnem.Normalized() * m_FistEnem[i].vel.Length() * 0.8f;
						m_FistEnem[i].angle = -dirToPlayer.Normalized().AngleWith(dirWithEnem.Normalized());
					}
				}
			}

			for (int j = 0; j < m_SawEnem.size(); j++)
			{
				Circlef otherEnem{ m_SawEnem[j].pos,m_SawEnem[j].size };

				if (utils::IsOverlapping(otherEnem, enemHitbox))
				{
					Vector2f dirWithEnem{ Vector2f(m_FistEnem[i].pos - m_SawEnem[j].pos) };

					m_SawEnem[j].gotHit = true;
					m_SawEnem[j].vel = -dirWithEnem * pushback * 0.25f * elepsedSec;
					std::cout << m_SawEnem[j].vel.Length();

					m_FistEnem[i].vel = dirWithEnem.Normalized() * m_FistEnem[i].vel.Length() * 0.8f;

					m_FistEnem[i].angle = -dirToPlayer.Normalized().AngleWith(dirWithEnem.Normalized());
				}
			}
		}

		else 
		{
			if(m_FistEnem[i].isPoweringup)
			{
				if (m_FistEnem[i].poweringupTimer > 0)
				{
					m_MouseV = Vector2f(m_Pos - m_FistEnem[i].pos).Normalized();

					Vector2f up{ cosf(m_FistEnem[i].angle),sinf(m_FistEnem[i].angle)};

					float angleUpdate{ m_MouseV.DotProduct(up) };

					//std::cout << "old angle: " << m_OldAngle << ", new angle: " << m_NewAngle << ", angle update: " << angleUpdate << std::endl;

					m_FistEnem[i].angle += angleUpdate * turnSpeed * elepsedSec;

					m_FistEnem[i].dir = Vector2f(cosf(m_FistEnem[i].angle - M_PI * 0.5f),sinf(m_FistEnem[i].angle - M_PI * 0.5f));

					m_FistEnem[i].poweringupTimer -= elepsedSec;
					//std::cout << m_FistEnem[i].poweringupTimer << std::endl;
				}
				else
				{
					m_FistEnem[i].vel = m_FistEnem[i].dir * maxSpeed;
					m_FistEnem[i].isPoweringup = false;
					m_FistEnem[i].isBoosting = true;
				}

				if (utils::IsOverlapping(m_Slash, enemHitbox) && m_Slash.size() >= 6)
				{
					Vector2f up{ 0,1 };



					m_FistEnem[i].angle += -dirToPlayer.Normalized().AngleWith(up) * 0.5f * elepsedSec;
				}
			}
			else
			{
				if (m_FistEnem[i].cooldownTimer >= 0)
				{
					m_FistEnem[i].cooldownTimer -= elepsedSec;
					//std::cout << m_FistEnem[i].cooldownTimer << std::endl;
				}
				else
				{
					m_FistEnem[i].isPoweringup = true;
					//m_FistEnem[i].poweringupTimer = m_FistEnem[i].poweringup;
				}
			}

			if (utils::IsOverlapping(enemHitbox, playerHitbox))
			{
				m_Pos = m_FistEnem[i].pos + dirToPlayer.Normalized() * (m_FistEnem[i].size * 0.5f + m_Size * 0.5f);
				m_Vel = Vector2f(0,0);
			}

			for (int j = 0; j < m_BusSaw.size(); j++)
			{
				Circlef otherEnem{ m_BusSaw[j].pos,m_BusSaw[j].size };

				if (utils::IsOverlapping(otherEnem, enemHitbox))
				{
					Vector2f dirWithEnem{ Vector2f(m_FistEnem[i].pos - m_BusSaw[j].pos) };

					m_BusSaw[j].gotHit = true;
					m_BusSaw[j].vel = -dirWithEnem * pushback * 0.25f * elepsedSec;
					std::cout << m_BusSaw[j].vel.Length();
				}
			}

			for (int j = 0; j < m_SawEnem.size(); j++)
			{
				Circlef otherEnem{ m_SawEnem[j].pos,m_SawEnem[j].size };

				if (utils::IsOverlapping(otherEnem, enemHitbox))
				{
					Vector2f dirWithEnem{ Vector2f(m_FistEnem[i].pos - m_SawEnem[j].pos) };

					m_SawEnem[j].gotHit = true;
					m_SawEnem[j].vel = -dirWithEnem * pushback * 0.25f * elepsedSec;
					std::cout << m_SawEnem[j].vel.Length();
				}
			}
		}

	}
}

void Game::Light(const Point2f& target, Color4f baseColor, Color4f falloff) const
{
	const int sizeW{ 25 };
	const int sizeH{ 25 };

	const float width{ m_Floor.width / float(sizeW) };
	const float height{ m_Floor.height / float(sizeH) };

	for (int x = 0; x < sizeW; x++)
	{
		for (int y = 0; y < sizeH; y++)
		{
			const Rectf block{ m_Floor.left + width * x, m_Floor.bottom + height * y, width, height };

			utils::SetColor(Color4f(
				  baseColor.r - ((Vector2f(0, (block.bottom - target.y) / m_Floor.height).Length() + Vector2f((block.left - target.x) / m_Floor.width, 0).Length()) * falloff.r)
				, baseColor.g - ((Vector2f(0, (block.bottom - target.y) / m_Floor.height).Length() + Vector2f((block.left - target.x) / m_Floor.width, 0).Length()) * falloff.g)
				, baseColor.b - ((Vector2f(0, (block.bottom - target.y) / m_Floor.height).Length() + Vector2f((block.left - target.x) / m_Floor.width, 0).Length()) * falloff.b)
				, baseColor.a - ((Vector2f(0, (block.bottom - target.y) / m_Floor.height).Length() + Vector2f((block.left - target.x) / m_Floor.width, 0).Length()) * falloff.a)));
			//utils::SetColor(Color4f(float(x + y) / float(sizeW + sizeH) * 0.35f + 0.25f, float(x + y) / float(sizeW + sizeH) * 0.25f + 0.25f, float(x + y) / float(sizeW + sizeH) * 0.25f + 0.25f, 1.f));

			utils::FillRect(block);
		}
	}
}

void Game::Light(float x, float y, Color4f baseColor, Color4f falloff) const
{
	Light(Point2f(x, y), baseColor, falloff);
}

void Game::UpdateText()
{
	delete m_pText;
	m_pText = new Texture(std::string("wave: " + std::to_string(m_Wave)), "upheavtt.ttf", m_FontSize, Color4f(1, 1, 1, 1));
}