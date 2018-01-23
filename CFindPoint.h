#pragma once

#include <string>
#include <tuple>
#include <atlimage.h>

class CFindPoint
{
public:
	CFindPoint();
	~CFindPoint();
public:
	bool setPath(const char* path);

	bool calculaate();

	CImage& getImg();
	int     getWidth();
	int     getHeight();
	int     getSX();
	int     getSY();
	int     getDX();
	int     getDY();

private:
	std::tuple<int, int>     getSPoint(const int width, const int height);
	std::tuple<int, int>     getDPoint(const int width, const int height, const int sx, const int sy);

	CImage         m_img;
	int            m_width;
	int            m_height;
	int            m_startX;
	int            m_startY;
	int            m_destX;
	int            m_destY;
};

