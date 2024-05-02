#include "pch.h"
#include "Game.h"
#include "utils.h"
#include <iostream>

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
    m_Top[0] = Point2f(sin(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y);
    m_Bottom[0] = Point2f(sin(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.y);

    m_Top[1] = Point2f(sin(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y);
    m_Bottom[1] = Point2f(sin(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_OldAngle) * (m_Ofset + m_Size) + m_Pos.y);
    
	const int enemySize{ 5 };

	for (int i = 0; i < enemySize; i++)
	{
		Enem test{ Point2f(float(rand() % int(GetViewPort().width)), float(rand() % int(GetViewPort().height))) };
		m_enemey.push_back(test);
	}
}

void Game::Cleanup( )
{

}

void Game::Update( float elapsedSec )
{
	const Uint8* keys{ SDL_GetKeyboardState(nullptr) };

	m_Dir.x = 0;
	m_Dir.y = 0;
	m_Sprint = 0;

	if (keys[SDL_SCANCODE_W]) m_Dir.y = 1;

	if (keys[SDL_SCANCODE_S]) m_Dir.y = -1;

	if (keys[SDL_SCANCODE_D]) m_Dir.x = 1;

	if (keys[SDL_SCANCODE_A]) m_Dir.x = -1;

    if (keys[SDL_SCANCODE_SPACE])
    {
		if (!m_SpaceHold && m_DashTimer <= 0)
		{
			m_Boost = m_Axel * m_Boosting;
			m_DashTimer = 1;
		}
		m_SpaceHold = true;
    }
    else m_SpaceHold = false;

	if (m_DashTimer > 0) m_DashTimer -= elapsedSec;

	if (keys[SDL_SCANCODE_LSHIFT]) m_Sprint = m_Axel*2;

	UpdateAngel(elapsedSec);
	UpdateSpeed(elapsedSec);
	Slash(elapsedSec);
	UpdateEnem(elapsedSec);
}

void Game::Draw( ) const
{
	ClearBackground( );

	const float angOfset{ 210 };

	const Point2f p1{ m_Pos.x + sinf(m_NewAngle) * m_Size,m_Pos.y + cosf(m_NewAngle) * m_Size };
	const Point2f p2{ m_Pos.x + sinf(m_NewAngle + angOfset) * m_Size, m_Pos.y + cosf(m_NewAngle + angOfset) * m_Size };
	const Point2f p3{ m_Pos.x + sinf(m_NewAngle - angOfset) * m_Size, m_Pos.y + cosf(m_NewAngle - angOfset) * m_Size };

	utils::SetColor(Color4f(1 - m_DashTimer, 0, 1.f, 1.f));
	utils::FillTriangle(p1, p2, p3);
	utils::DrawLine(m_Pos, Point2f(GetViewPort().width * 0.5f, GetViewPort().height * 0.5f), 
		Vector2f(m_Pos - Point2f(GetViewPort().width * 0.5f , GetViewPort().height * 0.5f)).Length() * 0.01f);

	utils::SetColor(Color4f(0, 1, 0, 1));
	utils::DrawLine(m_Pos, m_Pos + m_Vel * 25,2);
	for (int i{}; i < m_enemey.size(); ++i) utils::DrawLine(m_enemey[i].pos, m_enemey[i].pos + m_enemey[i].vel * 25);

	utils::SetColor(Color4f(1, 0, 0, 1));
	utils::DrawLine(m_Pos, m_Pos + m_Dir.Normalized() * 25,2);

    utils::SetColor(Color4f(1, 1, 1, 1));
    utils::DrawPolygon(m_Slash,false);

	utils::SetColor(Color4f(1, 0, 0, 1));

	const float boxSize{ 50 };
	const float enemSize{ 25 };
	const int tieth{ 3 };

	for(int i{}; i < m_enemey.size(); ++i)
	{
		utils::SetColor(Color4f(1, 0, 0, 1));

		for (int j = 0; j < tieth; j++)
		{
			const float angleIn{ 90.f / tieth * j };
			glPushMatrix();
			glTranslatef(m_enemey[i].pos.x, m_enemey[i].pos.y, 0);
			glRotatef(m_enemey[i].angle / M_PI * 180 / 2 + angleIn, 0, 0, 1);
			glTranslatef(-boxSize * 0.5f, -boxSize * 0.5f, 0);
			utils::FillRect(0, 0, boxSize, boxSize);
			glPopMatrix();
		}
		utils::SetColor(Color4f(0.75f, 0, 0, 1));
		utils::FillEllipse(m_enemey[i].pos, enemSize, enemSize);
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{

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

void Game::ClearBackground( ) const
{
	glClearColor( 0.23f, 0.23f, 0.23f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::UpdateAngel(float elepsedSec)
{
	m_MouseV = Vector2f(m_MouseP - m_Pos).Normalized();

	Vector2f up{ cosf(m_OldAngle),-sinf(m_OldAngle) };

	m_NewAngle = (m_MouseV.AngleWith(Vector2f(0,1)));

	float speed{ 15 };
	float angleUpdate{ m_MouseV.DotProduct(up) };

	std::cout << "old angle: " << m_OldAngle << ", new angle: " << m_NewAngle << ", angle update: " << angleUpdate << std::endl;

	m_OldAngle += angleUpdate * speed * elepsedSec;
}

void Game::UpdateSpeed(float elepsedSec)
{
	const float pushDropoff{ 10 };

	if (m_stunTimer <= 0)
	{
		Vector2f nDir{};
		if (m_Dir.x != 0 || m_Dir.y != 0) nDir = m_Dir.Normalized();

		if (m_Boost > m_Axel) m_Boost -= m_Boost * m_BoostDropOff * elepsedSec;
		else m_Boost = 0;

		m_Vel += nDir * m_Axel * elepsedSec;
		if (m_Vel.Length() > m_MaxSpeed) m_Vel = m_Vel.Normalized() * m_MaxSpeed;
		m_Vel += nDir * (m_Sprint + m_Boost) * elepsedSec;
		if ((m_Dir.x == 0 && m_Dir.y == 0)) m_Vel -= m_Vel * m_DropOff * elepsedSec;
	}
	else 
	{
		m_stunTimer -= elepsedSec;

		m_Vel -= m_Vel.Normalized() * pushDropoff * elepsedSec;

		if (m_Vel.Length() <= 0.1f) m_stunTimer = 0;
	}

	m_Pos += m_Vel;
}

void Game::Slash(float elepsedSec)
{
    m_Slash.clear();
	m_TopDis = 0;
	m_BottomDis = 0;

    float speed{ 50.f };
    float dis{ 0.01f };

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

                    m_Bottom[0] += Vector2f(m_Bottom[1] - m_Bottom[0]).Normalized() * ((speed + m_Bottom.size()) * Vector2f(m_Bottom[1] - m_Bottom[0]).Length()) * elepsedSec;

                    for (int i = 0; i < m_Top.size() - 1; i++) // calculation of the distance between all points at the top
                    {
                        m_TopDis += Vector2f(m_Top[i + 1] - m_Top[i]).Length();
                    }

                    m_Top[0] += Vector2f(m_Top[1] - m_Top[0]).Normalized() * ((speed + m_Top.size()) * Vector2f(m_Top[1] - m_Top[0]).Length()) * elepsedSec;

                //remoeving the last one if the list is bigger than 2
                if (m_Bottom.size() > 2)
                {
                    //std::cout << "deleting\n";
                    if (Vector2f(m_Top[0] - m_Top[1]).Length() <= m_TopDis * 0.05f )
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

void Game::UpdateEnem(float elepsedSec)
{
    const float speed{ 10.f };
	const float maxSpeed{ 5.f };
	const float pushback{ 20.f };
	const float maxPushSpeed{ 15.f };
	const float speedDropOff{ 15.f };

    for (int i = 0; i < m_enemey.size(); i++)
    {
        m_enemey[i].angle += 5.f * elepsedSec;
		Vector2f dirToPlayer{ m_Pos - m_enemey[i].pos };
		float multyplier{};

		if(!m_enemey[i].gotHit)
		{
			dirToPlayer = dirToPlayer.Normalized();

			m_enemey[i].dir = Vector2f(sinf(m_enemey[i].angle), cosf(m_enemey[i].angle));
			multyplier = (dirToPlayer.DotProduct(m_enemey[i].dir.Normalized()) + 1) ;

			m_enemey[i].vel += m_enemey[i].dir * speed * multyplier * elepsedSec;

			if (m_enemey[i].vel.Length() >= maxSpeed) m_enemey[i].vel = m_enemey[i].vel.Normalized() * maxSpeed;
		}
		else
		{
			m_enemey[i].vel -= m_enemey[i].vel.Normalized() * speedDropOff * elepsedSec;
			if (m_enemey[i].vel.Length() <= 1)
			{
				m_enemey[i].gotHit = false;
			}
		}

		m_enemey[i].pos += m_enemey[i].vel;
		std::cout << multyplier << std::endl;

		Circlef enemHitbox{ m_enemey[i].pos,m_EnemSize };
		Circlef playerHitbox{ m_Pos,m_Size };

		if (utils::IsOverlapping(enemHitbox, playerHitbox) && m_Boost == 0)
		{
			m_Vel = dirToPlayer * pushback * m_enemey[i].vel.Length() * 10 * elepsedSec;

			if (m_Vel.Length() >= maxPushSpeed) m_Vel = m_enemey[i].vel.Normalized() * (maxPushSpeed);

			m_stunTimer = 0.5f;
		}

		if (utils::IsOverlapping(m_Slash,enemHitbox) && m_stunTimer <= 0)
		{
			m_Vel = dirToPlayer * pushback * elepsedSec;
			m_enemey[i].vel = Vector2f(sinf(m_OldAngle),cosf(m_OldAngle)) * (m_TopDis + m_BottomDis) * pushback * elepsedSec;

			if (m_enemey[i].vel.Length() >= maxPushSpeed) m_enemey[i].vel = m_enemey[i].vel.Normalized() * maxPushSpeed;

			m_enemey[i].gotHit = true;
		}

		for (int j = 0; j < m_enemey.size(); j++)
		{
			if (i != j)
			{
				Circlef otherEnemHitbox{ m_enemey[j].pos,m_EnemSize };
				if (utils::IsOverlapping(otherEnemHitbox, enemHitbox))
				{
					const Vector2f pushDir{ m_enemey[j].pos - m_enemey[i].pos };

					m_enemey[i].vel = -pushDir * pushback * 0.5f * elepsedSec;
					m_enemey[j].vel = pushDir * pushback * 0.5f * elepsedSec;

					m_enemey[i].gotHit = true;
					m_enemey[j].gotHit = true;
				}
			}
		}
    }
}
