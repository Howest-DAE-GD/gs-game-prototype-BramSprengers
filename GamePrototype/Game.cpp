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
    
	int enemySize{ 3 };

	for (int i = 0; i < enemySize; i++)
	{
		BusSawEnem test{ Point2f(float(rand() % int(GetViewPort().width)), float(rand() % int(GetViewPort().height))) };
		m_BusSaw.push_back(test);
	}

	enemySize = 1;

	for (int i = 0; i < enemySize; i++)
	{
		SawEnem test{ Point2f(float(rand() % int(GetViewPort().width)), float(rand() % int(GetViewPort().height))),Vector2f(0,0),Vector2f(0,0), bool(rand() % 2) };
		m_SawEnem.push_back(test);
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
	for (int i{}; i < m_BusSaw.size(); ++i) utils::DrawLine(m_BusSaw[i].pos, m_BusSaw[i].pos + m_BusSaw[i].vel * 25);

	utils::SetColor(Color4f(1, 0, 0, 1));
	utils::DrawLine(m_Pos, m_Pos + m_Dir.Normalized() * 25,2);

    utils::SetColor(Color4f(1, 1, 1, 1));

	for (int i = 0; i < ((m_Slash.size()) / 2); i++)
	{
		std::vector<Point2f> box{m_Slash[i],m_Slash[i + 1], m_Slash[m_Slash.size() - i - 2],m_Slash[m_Slash.size() - i - 1] };
		utils::FillPolygon(box);
	}

    //utils::DrawPolygon(m_Slash,false);
	Color4f gray{ 0.25f,0.25f,0.25f,1 };
	Color4f lightGray{ 0.75f,0.75f,0.75f,1 };

	const float boxSize{ 50 };
	int tieth{ 3 };

	for(int i{}; i < m_BusSaw.size(); ++i)
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

			Point2f lp1{ m_SawEnem[i].box[0] + Vector2f(m_SawEnem[i].box[3] - m_SawEnem[i].box[0]) / tieth * j };
			Point2f lp2{ m_SawEnem[i].box[0] + Vector2f(m_SawEnem[i].box[3] - m_SawEnem[i].box[0]) / tieth * (j + 1)};
			Point2f lp3{ m_SawEnem[i].box[0] + Vector2f(m_SawEnem[i].box[3] - m_SawEnem[i].box[0]) / tieth * (j + 0.25f) - Vector2f(m_SawEnem[i].box[1] - m_SawEnem[i].box[0]).Normalized() * 15 };

			Point2f rp1{ m_SawEnem[i].box[1] + Vector2f(m_SawEnem[i].box[2] - m_SawEnem[i].box[1]) / tieth * j };
			Point2f rp2{ m_SawEnem[i].box[1] + Vector2f(m_SawEnem[i].box[2] - m_SawEnem[i].box[1]) / tieth * (j + 1) };
			Point2f rp3{ m_SawEnem[i].box[1] + Vector2f(m_SawEnem[i].box[2] - m_SawEnem[i].box[1]) / tieth * (j + 0.25f) - Vector2f(m_SawEnem[i].box[0] - m_SawEnem[i].box[1]).Normalized() * 15 };


			utils::FillTriangle(lp1, lp2, lp3);
			utils::FillTriangle(rp1, rp2, rp3);
		}

		utils::SetColor(lightGray);
		utils::FillPolygon(m_SawEnem[i].box);
		//utils::DrawEllipse(m_SawEnem[i].pos, m_SawEnem[i].size, m_SawEnem[i].size);
	}
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	if (e.keysym.sym == SDLK_KP_PLUS)
	{
		BusSawEnem test{ Point2f(float(rand() % int(GetViewPort().width)), float(rand() % int(GetViewPort().height))) };
		m_BusSaw.push_back(test);
	}

	if (e.keysym.sym == SDLK_KP_MINUS)
	{
		if(m_BusSaw.size() > 0) m_BusSaw.pop_back();
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

void Game::ClearBackground( ) const
{
	glClearColor( 0.15f, 0.15f, 0.15f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::UpdateAngel(float elepsedSec)
{
	m_MouseV = Vector2f(m_MouseP - m_Pos).Normalized();

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
			if (m_BusSaw[i].vel.Length() <= 1)
			{
				m_BusSaw[i].gotHit = false;
			}
		}

		m_BusSaw[i].pos += m_BusSaw[i].vel;
		//std::cout << multyplier << std::endl;

		Circlef enemHitbox{ m_BusSaw[i].pos,m_BusSaw[i].size};
		Circlef playerHitbox{ m_Pos,m_Size };

		if (utils::IsOverlapping(enemHitbox, playerHitbox) && m_Boost == 0)
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
	const float pushback{ 20.f };
	const float maxPushSpeed{ 20.f };

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

		if (utils::IsOverlapping(m_SawEnem[i].box, playerHitbox) && m_Boost == 0)
		{
			m_Vel = dirToPlayer * pushback * m_SawEnem[i].vel.Length() * 10 * elepsedSec;

			if (m_Vel.Length() >= maxPushSpeed) m_Vel = m_SawEnem[i].vel.Normalized() * (maxPushSpeed);

			m_stunTimer = 0.5f;
		}

		for (int j = 0; j < m_Top.size(); j++)
		{
			utils::HitInfo info{};

			std::vector<Point2f> line1{ m_SawEnem[i].box[0],m_SawEnem[i].box[1] };
			std::vector<Point2f> line2{ m_SawEnem[i].box[2],m_SawEnem[i].box[3] };
			std::vector<Point2f> line3{ m_SawEnem[i].box[0],m_SawEnem[i].box[3] };
			std::vector<Point2f> line4{ m_SawEnem[i].box[1],m_SawEnem[i].box[2] };

			if (utils::Raycast(line1, m_Bottom[j], m_Top[j], info) || utils::Raycast(line2, m_Bottom[j], m_Top[j], info))
			{
				//m_Vel = dirToPlayer * maxPushSpeed * elepsedSec;
				m_SawEnem[i].gotHit = true;
				m_SawEnem[i].vel = -dirToPlayer.Normalized() * pushback * (m_TopDis + m_BottomDis) * 0.5f * elepsedSec;
				if (m_SawEnem[i].vel.Length() >= maxPushSpeed)
				{
					m_SawEnem[i].vel = m_SawEnem[i].vel.Normalized() * maxPushSpeed;
				}
			}

			if (utils::Raycast(line3, m_Bottom[j], m_Top[j], info) || utils::Raycast(line4, m_Bottom[j], m_Top[j], info))
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
	}
}
