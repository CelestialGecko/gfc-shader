#include "stdafx.h"
#include "CShader.h"

CShader::CShader(float c, float r, float s, bool cE, bool bO, bool rO) :
	col(c / 1000), row(r / 1000), scale(s <= 1000 ? s : 1000 ), blendingOn(bO), rainbowOn(rO), colourEffectsOn(cE) {
	DimScale();
}

void CShader::DimScale() {
	colS = static_cast<int>(col * scale);
	rowS = static_cast<int>(row * scale);
}

// the main shader system now slightly cleaner and doesnt have the
// stuff that was unessisary from before
void CShader::OverlayShader(CGraphics* g)
{
	// colours
	Uint8 r = 0, gr = 0, b = 0, a = 0;

	// getting the surface and the pixels from the games graphics
	// variables used a lot
	float screnWidth = g->GetWidth();
	float screnHeight = g->GetHeight();
	float pixWidth = screnWidth / colS;
	float pixHeight = screnHeight / rowS;
	SDL_Surface* surf = g->GetSurface();
	Uint32* pix = static_cast<Uint32*>(surf->pixels);
	int pitch = surf->pitch / sizeof(Uint32);

	g->Lock();
	for (int ro = 0; ro < rowS; ++ro) {
		for (int co = 0; co < colS; ++co) {
			// gets the start and center of the fat pixel
			pixStart.m_x = co * pixWidth;
			pixStart.m_y = ro * pixHeight;
			pixCenter.m_x = pixStart.m_x + pixWidth / 2;
			pixCenter.m_y = pixStart.m_y + pixHeight / 2;

			// gets the mean colour for this fat pixel - if the scale is max then it doesnt do this mumbo jumbo
			if (scale == 1000) {
				SDL_GetRGBA(pix[static_cast<int>(pixStart.m_y * pitch + pixStart.m_x)], surf->format, &r, &gr, &b, &a);
			}
			else {
				// this will get the average colour for a pixel
				GetBlockAverageColour(pixStart, pixWidth, pixHeight, surf->format, pix, pitch, screnWidth, screnHeight, r, gr, b);
			}

			// applies cool shader effects - wont need to run if there is nothing to apply
			// I was going to use this but even after all of the optimisation it just lags a lot
			// I wish I didnt waste my time doing this
			if (colourEffectsOn && (saturationFadeLevel > 0 || rainbowOn || brightnessChange > 0)) {
				ApplyColourEffects(r, gr, b);
			}
			// applys the screen blending - used for the thunder
			if (blendingOn) {
				r = r + (blendingColour.R() * (255 - r) / 255);
				gr = gr + (blendingColour.G() * (255 - gr) / 255);
				b = b + (blendingColour.B() * (255 - b) / 255);
			}

			//draws the fat pixel
			if (scale == 1000) {
				pix[static_cast<int>(pixStart.m_y * pitch + pixStart.m_x)] = SDL_MapRGBA(surf->format, r, gr, b, 255);
			}
			else{
				DrawPixelBlock(pixStart, pixWidth, pixHeight, surf->format, pix, pitch, screnWidth, screnHeight, r, gr, b);
			}
		}
	}
	// shifts the hue outside the loop
	if(rainbowOn)hueChange = fmod(hueChange += 1, 360.0f);
	g->Unlock();
}

// draws the fat pixel
void CShader::DrawPixelBlock(CVector& sta, float w, float h, SDL_PixelFormat* f, Uint32* p, int pit, int sW, int sH, Uint8& r, Uint8& g, Uint8& b) {
	// converts the colours back to the format used for drawing pixels
	Uint32 mapColour = SDL_MapRGBA(f, r, g, b, 255);
	// start and end of placing it
	int startX = static_cast<int>(sta.GetX());
	int startY = static_cast<int>(sta.GetY());
	int endX = static_cast<int>(sta.GetX() + w);
	int endY = static_cast<int>(sta.GetY() + h);

	// loops through rows
	for (int y = startY; y < endY; ++y) {
		// the offset will be the same for each row
		// might as well not do this a bajillion times
		int rowOffset = y * pit;
		for (int x = startX; x < endX; ++x) {
			// sets the location to the new colour
			p[rowOffset + x] = mapColour;
		}
	}
}

// calculates the average color for a fat pixel
void CShader::GetBlockAverageColour(CVector& sta, float w, float h, SDL_PixelFormat* f, Uint32* p, int pit, int sW, int sH, Uint8& r, Uint8& g, Uint8& b) {
	// variables needed for this
	int sumR = 0, sumG = 0, sumB = 0, totalPix = 0;
	// pre compute for because less lag
	int startX = static_cast<int>(sta.GetX());
	int startY = static_cast<int>(sta.GetY());
	int endX = static_cast<int>(sta.GetX() + w);
	int endY = static_cast<int>(sta.GetY() + h);

	// loop through the pixels to get the mean
	for (int y = startY; y < endY; ++y) {
		// the offset will be the same for each row
		// might as well not do this a bajillion times
		int rowOffset = y * pit;
		for (int x = startX; x < endX; ++x) {
			// gets the colour currently at this location
			Uint32 pixel = p[rowOffset + x];
			// gets the rgb values for it
			Uint8 a;
			SDL_GetRGBA(pixel, f, &r, &g, &b, &a);
			// does the mean
			sumR += r;
			sumG += g;
			sumB += b;
			totalPix++;
		}
	}
	r = sumR / totalPix, g = sumG / totalPix, b = sumB / totalPix;
}

// enables blending and sets the colour
void CShader::StartBlend(CColor& c) {
	blendingColour = c;
	blendingOn = true;
}


// NONE OF THIS WAS EVER USED IN THE FINAL GAME!

// faster version that doesnt use a tuple other then that its exactly the same
void CShader::fastRGBtoHSV(float &r, float &g, float &b) {
	// need them to be between 1 and 0
	r *= nom;
	g *= nom;
	b *= nom;
	// gets the largest, smallest and the difference
	float max = (std::max)(r, (std::max)(g, b));
	float min = (std::min)(r, (std::min)(g, b));
	float del = max - min;
	float hue = 0.0f, sat = 0.0f, vibe = max;
	// calculates the rest of the values (sat and hue)
	// when delta is 0 the the brightness is 0 so black
	if (del > 0.0f) {
		sat = del / max;
		// checks which colour is the highest
		// between -60 and 60 
		if (max == r) hue = 60.0f * ((g - b) / del);
		// between 60 and 180 
		else if (max == g) hue = 60.0f * (((b - r) / del) + 2.0f);
		// 180 to 300
		else hue = 60.0f * (((r - g) / del) + 4.0f);
	}
	// this is for the red so that the negative value converts to being between 300 and 360
	if (hue < 0.0f) hue += 360.0f;
	r = hue;
	g = sat;
	b = vibe;
}

// needed a super optimised version of this, based on the version from CColor
// its onyl really optiimised due to it not returning a object
void CShader::fastHSVtoRGB(float& h, float& s, float& v) {
	// stuff thats used more then once
	float temp1 = h / 60.0f;
	int temp2 = static_cast<int>(h / 60.0f);
	float temp3 = 255 * v;
	int Hu = temp2 % 6;
	float f = temp1 - temp2;
	int p = static_cast<int>(temp3 * (1 - s));
	int q = static_cast<int>(temp3 * (1 - f * s));
	int t = static_cast<int>(temp3 * (1 - (1 - f) * s));
	int b = static_cast<int>(temp3);
	switch (Hu)
	{
		case 0: h = b, s = t, v = p; break;
		case 1: h = q, s = b, v = p; break;
		case 2: h = p, s = b, v = t; break;
		case 3: h = p, s = q, v = b; break;
		case 4: h = t, s = p, v = b; break;
		case 5: h = b, s = p, v = q; break;
		default: h = 0, s = 0, v = 0;
	}
}

// new effects applier - never really used so there are no comments
void CShader::ApplyColourEffects(Uint8& r, Uint8& g, Uint8& b) {
	float h = static_cast<float>(r), s= static_cast<float>(g), v = static_cast<float>(b);
	fastRGBtoHSV(h, s, v);
	if (rainbowOn) {
		h = fmod(h + hueChange, 360.0f);
	}
	else hueChange = 0;
	if (saturationFadeLevel > 0)s = (std::min)(s + saturationFadeLevel, 1.0f);
	if (brightnessChange > 0)v = (std::min)(v + brightnessChange, 1.0f);
	fastHSVtoRGB(h, s, v);
	r = static_cast<Uint8>(h), g = static_cast<Uint8>(s), b = static_cast<Uint8>(v);
}

CShader CShader::operator=(const CVector& v) {
	col = v.GetX() / 1000;
	row = v.GetY() / 1000;
	DimScale();
	return *this;
}

CShader CShader::operator+=(const CVector& v) {
	col += v.GetX() / 1000;
	row += v.GetY() / 1000;
	DimScale();
	return *this;
}

CShader CShader::operator-=(const CVector& v) {
	col -= v.GetX() / 1000;
	row -= v.GetY() / 1000;
	DimScale();
	return *this;
}

CShader CShader::operator=(const float s) {
	if (AllowedScale(s))scale = s;
	DimScale();
	return *this;
}

CShader CShader::operator+=(const float s) {
	if (AllowedScale(scale, s))scale += s;
	DimScale();
	return *this;
}

CShader CShader::operator-=(const float s) {
	if (AllowedScale(scale, -s))scale -= s;
	DimScale();
	return *this;
}


