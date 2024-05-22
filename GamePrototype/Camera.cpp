#include "pch.h"
#include "Camera.h"

Camera::Camera(float screenWidth, float screenHeight)
	: m_ScreenHeight{screenHeight}
	, m_ScreenWidth{screenWidth}
{
}

void Camera::Aim(float levelW, float levelH, const Point2f& trackCenter)
{
	Point2f center{ trackCenter.x - m_ScreenWidth * 0.5f,trackCenter.y - m_ScreenHeight * 0.5f };

	glPushMatrix();
	/*if (trackCenter.x + m_ScreenWidth * 0.5f > levelW)
	{
		center.x = levelW - m_ScreenWidth;
	}

	if (trackCenter.y + m_ScreenHeight * 0.5f > levelH)
	{
		center.y = levelH - m_ScreenHeight;
	}

	if (trackCenter.x - m_ScreenWidth * 0.5f < 0)
	{
		center.x = 0;
	}

	if (trackCenter.y - m_ScreenHeight * 0.5f < 0)
	{
		center.y = 0;
	}*/

	glTranslatef(-center.x, -center.y, 0);
}

Point2f Camera::Reset()
{
	glPopMatrix();
	return Point2f(0,0);
}
