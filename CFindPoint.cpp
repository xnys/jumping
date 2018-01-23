#include "stdafx.h"
#include "CFindPoint.h"

#define DEFAULT_FIND_COUNT 7

CFindPoint::CFindPoint()
{
}


CFindPoint::~CFindPoint()
{
}

bool CFindPoint::setPath(const char* path)
{
	if (!m_img.IsNull()) m_img.Destroy();

	return (m_img.Load(path) == S_OK);
}

bool CFindPoint::calculaate()
{
	m_width = m_img.GetWidth();
	m_height = m_img.GetHeight();
	std::tie(m_startX, m_startY) = getSPoint(m_width, m_height);
	std::tie(m_destX, m_destY) = getDPoint(m_width, m_height, m_startX, m_startY);
	if (m_startY == 0 || m_destY == 0) return false;

	return true;
}

ATL::CImage& CFindPoint::getImg()
{
	return m_img;
}

int CFindPoint::getWidth()
{
	return m_width;
}

int CFindPoint::getHeight()
{
	return m_height;
}

int CFindPoint::getSX()
{
	return m_startX;
}

int CFindPoint::getSY()
{
	return m_startY;
}

int CFindPoint::getDX()
{
	return m_destX;
}

int CFindPoint::getDY()
{
	return m_destY;
}

std::tuple<int, int> CFindPoint::getDPoint(const int width, const int height, const int sx, const int sy)
{
	unsigned char br = 0, bg = 0, bb = 0;
	unsigned char *data = nullptr;
	unsigned char r = 0, g = 0, b = 0;
	unsigned char r2 = 0, g2 = 0, b2 = 0;

	const int halfW = width / 2;
	const int halfH = height / 2;

	const int top = halfH + halfH - sy - 150;
	const int bottom = sy;
	int tx = 0, ty = 0;
	int x1 = 0, y1 = 0;
	if (fabs(sx - halfW) < 100)
	{
		CRect rect{sx - 45, 0, sx + 45, height};
		for (int fy = top; fy < bottom; ++fy)
		{
			data = (BYTE *)m_img.GetPixelAddress(width - 1, fy);
			br = *(data + 2);
			bg = *(data + 1);
			bb = *data;
			for (int fx = 0; fx < width - 1; ++fx)
			{
				if (rect.PtInRect(CPoint(fx, fy))) continue;

				data = (BYTE *)m_img.GetPixelAddress(fx, fy);
				r = *(data + 2);
				g = *(data + 1);
				b = *data;
				if (x1 == 0)
				{ // 找上顶点
					if ((abs(r - br) > 20 || abs(g - bg) > 20 || abs(b - bb) > 20))
					{
						// 找到上顶点，记录x
						x1 = fx;
						y1 = fy;
						for (int nx = fx + 1; nx < width - 1; ++nx)
						{
							data = (BYTE *)m_img.GetPixelAddress(nx, fy);
							r2 = *(data + 2);
							g2 = *(data + 1);
							b2 = *data;
							if (abs(r2 - r) > 1 || abs(g2 - g) > 1 || abs(b2 - b) > 1)
							{
								x1 = (fx + nx) / 2;
								break;
							}
						}
					}
				}
			}
		}
		tx = x1;
		ty = (y1 + sy) / 2;
		return std::make_tuple(tx, ty);
	}

	const int left = (sx > halfW) ? 0 : halfW;
	const int right = (sx > halfW) ? halfW : width;

	int x2 = 0, y2 = 0;
	int x3 = 0, y3 = 0;
	int default1 = DEFAULT_FIND_COUNT, default2 = DEFAULT_FIND_COUNT;
	for (int fy = top; fy < bottom; ++fy)
	{
		data = (BYTE *)m_img.GetPixelAddress(width - 1, fy);
		br = *(data + 2);
		bg = *(data + 1);
		bb = *data;
		for (int fx = left + 1; fx < right; ++fx)
		{
			data = (BYTE *)m_img.GetPixelAddress(fx, fy);
			r = *(data + 2);
			g = *(data + 1);
			b = *data;
			if (x1 == 0)
			{ // 找上顶点
				if ((abs(r - br) > 20 || abs(g - bg) > 20 || abs(b - bb) > 20))
				{
					// 找到上顶点，记录x
					x1 = fx;
					y1 = fy;
					y2 = y1;
					y3 = y1;
					for (int nx = fx + 1; nx < right; ++nx)
					{
						data = (BYTE *)m_img.GetPixelAddress(nx, fy);
						r2 = *(data + 2);
						g2 = *(data + 1);
						b2 = *data;
						if (abs(r2 - r) > 1 || abs(g2 - g) > 1 || abs(b2 - b) > 1)
						{
							x1 = (fx + nx) / 2;
							x2 = x1;
							x3 = x1;

							bool f2 = false, f3 = false;
							for (int ny = fy + 1; ny < bottom; ++ny)
							{
								data = (BYTE *)m_img.GetPixelAddress(width - 1, ny);
								br = *(data + 2);
								bg = *(data + 1);
								bb = *data;
								if (!f2)
								{
									// 找左顶点
									int minx = min(left, sx);
									for (int lx = x1 - 1; lx >= minx; --lx)
									{
										data = (BYTE *)m_img.GetPixelAddress(lx, ny);
										r2 = *(data + 2);
										g2 = *(data + 1);
										b2 = *data;
										if ((abs(r2 - br) < 5 && abs(g2 - bg) < 5 && abs(b2 - bb) < 5)
											|| (abs(r2 - 178) < 5 && abs(g2 - 172) < 5 && abs(b2 - 112) < 5))
										{
											if (x2 > lx)
											{
												x2 = lx;
												y2 = ny;
												default1 = DEFAULT_FIND_COUNT;
												break;
											}
											else if (x2 == lx)
											{
												--default1;
												break;
											}
											else
											{
												default1 = 0;
												break;
											}
										}
									}

									if (default1 == 0)
									{
										++x2; ++x2;
										data = (BYTE *)m_img.GetPixelAddress(x2, y2);
										r = *(data + 2);
										g = *(data + 1);
										b = *data;
										for (int yy = y2 + 1; yy < bottom; ++yy)
										{
											data = (BYTE *)m_img.GetPixelAddress(x2, yy);
											r2 = *(data + 2);
											g2 = *(data + 1);
											b2 = *data;
											if (abs(r2 - r) > 1 || abs(g2 - g) > 1 || abs(b2 - b) > 1)
											{
												y2 = (yy + y2) / 2;
												break;
											}
										}
										f2 = true;
									}
								}
								if (!f3)
								{
									// 找右顶点
									int maxx = min(right, sx);
									for (int rx = x1 + 1; rx < maxx; ++rx)
									{
										data = (BYTE *)m_img.GetPixelAddress(rx, ny);
										r2 = *(data + 2);
										g2 = *(data + 1);
										b2 = *data;
										if ((abs(r2 - br) < 5 && abs(g2 - bg) < 5 && abs(b2 - bb) < 5)
											|| (abs(r2 - 178) < 5 && abs(g2 - 172) < 5 && abs(b2 - 112) < 5))
										{
											if (x3 < rx)
											{
												x3 = rx;
												y3 = ny;
												default2 = DEFAULT_FIND_COUNT;
												break;
											}
											else if (x3 == rx)
											{
												--default2;
												break;
											}
											else
											{
												default2 = 0;
												break;
											}
										}
									}

									if (default2 == 0)
									{
										--x3; --x3;
										data = (BYTE *)m_img.GetPixelAddress(x3, y3);
										r = *(data + 2);
										g = *(data + 1);
										b = *data;
										for (int yy = y3 + 1; yy < bottom; ++yy)
										{
											data = (BYTE *)m_img.GetPixelAddress(x3, yy);
											r2 = *(data + 2);
											g2 = *(data + 1);
											b2 = *data;
											if (abs(r2 - r) > 1 || abs(g2 - g) > 1 || abs(b2 - b) > 1)
											{
												y3 = (yy + y3) / 2;
												break;
											}
										}
										f3 = true;
										break;
									}
								}
								if (f2 && f3)
									break;
							}
							tx = (x2 + x3) / 2;
							ty = (y2 + y3) / 2 - 2;
							return std::make_tuple(tx, ty);
						}
					}
				}
			}
		}
	}

	return std::make_tuple(0, 0);
}

std::tuple<int, int> CFindPoint::getSPoint(const int width, const int height)
{
	unsigned char br = 0, bg = 0, bb = 0;
	unsigned char *data = nullptr;
	unsigned char r = 0, g = 0, b = 0;
	int lx = 0, rx = 0;
	int tx = 0, ty = 0;
	bool isTarget = false;
	const int bottom = height * 1.0 * 0.75;
	for (int y = height * 1.0 * 0.4; ; ++y)
	{
		if (y > bottom)
		{
			break;
		}

		data = (BYTE *)m_img.GetPixelAddress(width - 1, y);
		br = *(data + 2);
		bg = *(data + 1);
		bb = *data;
		for (int x = 1; x < width; ++x)
		{
			data = (BYTE *)m_img.GetPixelAddress(x, y);
			r = *(data + 2);
			g = *(data + 1);
			b = *data;
			if (!isTarget)
			{
				if (abs(r - 52) < 5 && abs(g - 53) < 5 && abs(b - 56) < 5)
				{
					isTarget = true;
				}
				continue;
			}
			if (ty == 0 && abs(r - 122) < 5 && abs(g - 111) < 5 && abs(b - 156) < 5)
			{
				tx = x;
				ty = y;

				for (int tlx = x - 1; tlx >= 0; --tlx)
				{
					data = (BYTE *)m_img.GetPixelAddress(tlx, y);
					r = *(data + 2);
					g = *(data + 1);
					b = *data;
					if (abs(r - br) < 1 && abs(g - bg) < 1 && abs(b - bb) < 1)
					{
						lx = tlx + 1;
						break;
					}
				}
				for (int trx = x + 1; trx < width; ++trx)
				{
					data = (BYTE *)m_img.GetPixelAddress(trx, y);
					r = *(data + 2);
					g = *(data + 1);
					b = *data;
					if (abs(r - br) < 1 && abs(g - bg) < 1 && abs(b - bb) < 1)
					{
						rx = trx - 1;
						break;
					}
				}
				tx = (lx + rx) / 2;
				for (int dy = y + 1; dy < bottom; ++dy)
				{
					data = (BYTE *)m_img.GetPixelAddress(tx, dy);
					r = *(data + 2);
					g = *(data + 1);
					b = *data;
					if (abs(r - 56) < 5 && abs(g - 56) < 5 && abs(b - 96) < 5)
					{
						ty = dy;
						break;
					}
				}
				return std::make_tuple(tx, ty);
			}

#if 0
				int topy = 0, downy = 0;
				for (int uy = y - 1; ; --uy)
				{
					data = (BYTE *)img.GetPixelAddress(0, uy);
					br = *(data + 2);
					bg = *(data + 1);
					bb = *data;

					data = (BYTE *)img.GetPixelAddress(tx, uy);
					r = *(data + 2);
					g = *(data + 1);
					b = *data;
					if (abs(r - br) < 1 && abs(g - bg) < 1 && abs(b - bb) < 1)
					{
						topy = uy;
						break;
					}
				}
				for (int dy = y + 1; ; ++dy)
				{
					data = (BYTE *)img.GetPixelAddress(0, dy);
					br = *(data + 2);
					bg = *(data + 1);
					bb = *data;

					data = (BYTE *)img.GetPixelAddress(tx, dy);
					r = *(data + 2);
					g = *(data + 1);
					b = *data;
					if (abs(r - br) < 1 && abs(g - bg) < 1 && abs(b - bb) < 1)
					{
						downy = dy;
						break;
					}
				}
				ty = (topy + downy) / 2;

				for (int dy = y + 1; dy < bottom; ++dy)
				{
					data = (BYTE *)m_img.GetPixelAddress(tx, dy);
					r = *(data + 2);
					g = *(data + 1);
					b = *data;
					if (abs(r - 56) < 5 && abs(g - 56) < 5 && abs(b - 96) < 5)
					{
						ty = dy;
						break;
					}
				}
#endif
		}
	}

	return std::make_tuple(0, 0);
}
