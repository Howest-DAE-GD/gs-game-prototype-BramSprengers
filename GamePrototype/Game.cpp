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
    m_Top[0] = Point2f(sin(m_Angle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_Angle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y);
    m_Bottom[0] = Point2f(sin(m_Angle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_Angle) * (m_Ofset + m_Size) + m_Pos.y);

    m_Top[1] = Point2f(sin(m_Angle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_Angle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y);
    m_Bottom[1] = Point2f(sin(m_Angle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_Angle) * (m_Ofset + m_Size) + m_Pos.y);
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

	if (keys[SDL_SCANCODE_SPACE]) m_Boost = m_Axel * m_Boosting;

	if (keys[SDL_SCANCODE_LSHIFT]) m_Sprint = m_Axel*2;

	UpdateSpeed(elapsedSec);
	Slash(elapsedSec);
	
}

void Game::Draw( ) const
{
	ClearBackground( );

	const float angOfset{ 210 };

	Point2f p1{ m_Pos.x + sinf(m_Angle) * m_Size,m_Pos.y + cosf(m_Angle) * m_Size };
	Point2f p2{ m_Pos.x + sinf(m_Angle + angOfset) * m_Size, m_Pos.y + cosf(m_Angle + angOfset) * m_Size };
	Point2f p3{ m_Pos.x + sinf(m_Angle - angOfset) * m_Size, m_Pos.y + cosf(m_Angle - angOfset) * m_Size };

	utils::SetColor(Color4f(1, 0, 1, 1));
	utils::FillTriangle(p1, p2, p3);

	utils::SetColor(Color4f(0, 1, 0, 1));
	utils::DrawLine(m_Pos, m_Pos + m_Vel * 25,2);
	utils::SetColor(Color4f(1, 0, 0, 1));
	utils::DrawLine(m_Pos, m_Pos + m_Dir.Normalized() * 25,2);

    utils::DrawPolygon(m_Slash);
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{

}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{

}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	Point2f mouseP{ float(e.x),float(e.y) };
	Vector2f mouseV{ mouseP - m_Pos };

	m_Angle = mouseV.AngleWith(Vector2f(0, 1));
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

void Game::UpdateSpeed(float elepsedSec)
{
	/*Vector2f nDir{};
	 if (m_Dir.x != 0 || m_Dir.y != 0) nDir = m_Dir.Normalized() ;

	if (m_Vel.Length() <= m_MaxSpeed) m_Vel += nDir * m_Axel * elepsedSec;

	if (m_Vel.Length() > m_MaxSpeed) m_Vel = m_Vel.Normalized() * m_MaxSpeed;

	if ((m_Dir.x == 0 && m_Dir.y == 0)) m_Vel -= m_Vel * m_DropOff * elepsedSec;

	std::cout << "x " << m_Dir.x << ", y " << m_Dir.y << std::endl;

	m_Pos += m_Vel;*/

	Vector2f nDir{};
	if (m_Dir.x != 0 || m_Dir.y != 0) nDir = m_Dir.Normalized();

	if (m_Boost > m_Axel) m_Boost -= m_Boost * m_BoostDropOff * elepsedSec;
	else m_Boost = 0;

	m_Vel += nDir * m_Axel * elepsedSec;
	if (m_Vel.Length() > m_MaxSpeed) m_Vel = m_Vel.Normalized() * m_MaxSpeed;
	m_Vel += nDir * (m_Sprint + m_Boost) * elepsedSec;
	if ((m_Dir.x == 0 && m_Dir.y == 0)) m_Vel -= m_Vel * m_DropOff * elepsedSec;

	m_Pos += m_Vel;
}

void Game::Slash(float elepsedSec)
{
    m_Slash.clear();

    float speed{ 150.f };
    float dis{ 10.f };

    float bottomDis{};
    float topDis{};
            //setup for defoult ponts

                //making the first part of the list follow the mous
    

                //adding and moving a new point to the list
                if (Vector2f(m_Top[m_Top.size() - 2] - m_Top[m_Top.size() - 1]).Length() >= dis && m_Top.size() < 100)
                {
                    std::cout << "adding\n";
                    m_Bottom.push_back(m_Bottom[m_Bottom.size() - 1]);
                    m_Top.push_back(m_Top[m_Top.size() - 1]);
                }

                    for (int i = 0; i < m_Bottom.size() - 1; i++) // calculation of the distance between all points at the bottom
                    {
                        bottomDis += Vector2f(m_Bottom[i + 1] - m_Bottom[i]).Length();
                    }

                    m_Bottom[0] += Vector2f(m_Bottom[1] - m_Bottom[0]).Normalized() * Vector2f(m_Bottom[1] - m_Bottom[0]).Length() * (speed + m_Bottom.size() ) * elepsedSec;

                    for (int i = 0; i < m_Top.size() - 1; i++) // calculation of the distance between all points at the top
                    {
                        topDis += Vector2f(m_Top[i + 1] - m_Top[i]).Length();
                    }

                    m_Top[0] += Vector2f(m_Top[1] - m_Top[0]).Normalized() * Vector2f(m_Top[1] - m_Top[0]).Length() * (speed + m_Top.size() ) * elepsedSec;

                //remoeving the last one if the list is bigger than 2
                if (m_Bottom.size() > 2)
                {
                    std::cout << "deleting\n";
                    if (Vector2f(m_Bottom[0] - m_Bottom[1]).Length() <= 0.01f || Vector2f(m_Bottom[0] - m_Bottom[1]).Length() >= 1000.f )
                    {
                        m_Bottom.erase(m_Bottom.begin());
                        m_Top.erase(m_Top.begin());
                    }
                }

                m_Top[m_Top.size() - 1] = Point2f(sin(m_Angle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.x, cos(m_Angle) * (m_BladeSize + m_Ofset + m_Size) + m_Pos.y);
                m_Bottom[m_Bottom.size() - 1] = Point2f(sin(m_Angle) * (m_Ofset + m_Size) + m_Pos.x, cos(m_Angle) * (m_Ofset + m_Size) + m_Pos.y);

                for (int i{}; i < m_Bottom.size(); i++)
                {
                    m_Slash.push_back(m_Bottom[i]);
                }
                for (int i{ int(m_Top.size() -1) }; i >= 0; i--)
                {
                    m_Slash.push_back(m_Top[i]);
                }

                std::cout << Vector2f(m_Bottom[m_Bottom.size() - 1] - m_Bottom[m_Bottom.size() - 2]) << std::endl;

                // prep for the vertacies
                //vertices = new Vector3[top.Count + bottom.Count];
                //vertices = setupVectors(vertices, top.ToArray(), bottom.ToArray());

                // prep for the triangles
                //triangels = new int[(vertices.Length - 2) * 3];
                //triangels = setupTriangles(triangels);

                // mesh updating
                //updadeMesh(vertices, triangels, uv);

            //else // if the player is not in attack mode
            //{

            //    for (int i = 2; i < bottom.Count; i++) // making the list 2 long
            //    {
            //        bottom.RemoveAt(i);
            //        top.RemoveAt(i);
            //    }

            //    //keaping the 2 following the mouse
            //    top[0] = new Vector3(-Mathf.Sin(ang) * up.y, Mathf.Cos(ang) * up.y) + player.transform.position;
            //    bottom[0] = new Vector3(-Mathf.Sin(ang) * down.y, Mathf.Cos(ang) * down.y) + player.transform.position;

            //    top[1] = new Vector3(-Mathf.Sin(ang) * up.y, Mathf.Cos(ang) * up.y) + player.transform.position;
            //    bottom[1] = new Vector3(-Mathf.Sin(ang) * down.y, Mathf.Cos(ang) * down.y) + player.transform.position;

            //    mesh.Clear();
            //}

        //private Vector3[] setupVectors(Vector3[] p, Vector3[] t, Vector3[] b)
        //{
        //    for (int i = 0; i < p.Length; i += 2)
        //    {
        //        if (i < t.Length + b.Length)
        //        {
        //            if (i == 0)
        //            {
        //                p[i] = t[i / 2];
        //                p[i + 1] = b[i / 2];
        //            }
        //            else
        //            {
        //                p[i] = b[i / 2];
        //                p[i + 1] = t[i / 2];
        //            }
        //        }
        //        else
        //        {
        //            p[i] = b[b.Length - 1];
        //            p[i] = t[t.Length - 1];
        //        }
        //    }

        //    return p;
        //}

        //private int[] setupTriangles(int[] t)
        //{
        //    for (int i = 0; i < t.Length; i += 3)
        //    {
        //        if (i == 0)
        //        {
        //            t[i] = 0;
        //            t[i + 1] = 1;
        //            t[i + 2] = 2;
        //        }

        //        else if (i == 3)
        //        {
        //            t[i] = 0;
        //            t[i + 1] = 2;
        //            t[i + 2] = 3;
        //        }

        //        else
        //        {
        //            if (i % 2 == 0)
        //            {
        //                t[i] = triangels[i - 1];
        //                t[i + 1] = triangels[i] - 1;
        //                t[i + 2] = triangels[i] + 1;

        //            }
        //            else
        //            {
        //                t[i] = triangels[i - 3];
        //                t[i + 1] = triangels[i] + 1;
        //                t[i + 2] = triangels[i] + 2;
        //            }
        //        }
        //    }

        //    return t;
        //}
}