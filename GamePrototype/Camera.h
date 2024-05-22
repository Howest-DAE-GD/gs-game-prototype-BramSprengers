#pragma once
class Camera
{
public:
	Camera(float screenWidth, float screenHeight);
	void Aim(float levelW, float levelH, const Point2f& trackCenter);
	Point2f Reset();

private:
	float m_ScreenWidth;
	float m_ScreenHeight;
};

