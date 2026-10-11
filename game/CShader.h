#pragma once
#include "stdafx.h"
#include <tuple>

// this is a shader system I came up with in a previous GFC game
// I converted it into a class to make it easier to use
// decided not to use the dynamic brightness for this as I dont think its that good

class CShader
{
private:
	// colour shifts
	float brightnessChange = 0.0f;
	float saturationFadeLevel = 0.0f;
	float hueChange = 1.0f;
	float nom = 1.0f / 255.0f;

	// allows for a blending colour
	CColor blendingColour = CColor::White();

	// the starting and center for the fat pixels
	CVector pixStart;
	CVector pixCenter;
	CColor meanColour;

	// toggles on different effects
	bool colourEffectsOn;
	bool blendingOn;
	bool rainbowOn;

	// screen dimentions
	float col;
	float row;
	// scale
	float scale;

	// scaled values - a lot easier for me to work with when these are seperate
	int colS = 0;
	int rowS = 0;

	// private methods
	void DimScale();
public:

	// c - number of pixels on the x axis, r - number of pixels on the y axis
	// s - pixel scale goes from 0 - 1000, cE - colour effects, bO - blending on, rO - rainbow on
	CShader(float c = 800, float r = 600, float s = 1000, bool cE = false, bool bO = false, bool rO = false);

	// main shader logic
	void OverlayShader(CGraphics* g);
	//CColor GetBlockAverageColour(CVector& start, float width, float height, SDL_PixelFormat* form, Uint32* pixels, int pitch, int screenWidth, int screenHeight);
	//void DrawPixelBlock(CVector& start, float width, float height, CColor& colour, SDL_PixelFormat* format, Uint32* pixels, int pitch, int screenWidth, int screenHeight);
	// optimised version of average colour that doesnt create a CColor object
	void GetBlockAverageColour(CVector& start, float width, float height, SDL_PixelFormat* form, Uint32* pixels, int pitch, int screenWidth, int screenHeight, Uint8& r, Uint8& g, Uint8& b);
	void DrawPixelBlock(CVector& start, float width, float height, SDL_PixelFormat* format, Uint32* pixels, int pitch, int screenWidth, int screenHeight, Uint8& r, Uint8& g, Uint8& b);


	// useful functions
	// old one lags too much
	void fastRGBtoHSV(float &v1, float &v2, float &v3);
	void fastHSVtoRGB(float& v1, float& v2, float& v3);

	// effects logic
	//CColor ApplyColourEffects(CColor& c);
	void ApplyColourEffects(Uint8&r, Uint8&g, Uint8&b);

	// enable other effects
	void EnableEffects() { colourEffectsOn = true; }
	void DisableEffects() { colourEffectsOn = false; }

	// individual effects
	void ToggleRainbow() { rainbowOn = !rainbowOn; }
	void StopRainbow() { rainbowOn = false; }
	void StartRainbow() { rainbowOn = true; }

	void ShiftSaturation(float s) { saturationFadeLevel+=s; }
	float GetSaturation() { return saturationFadeLevel; }
	void ResetSaturation() { saturationFadeLevel = 0; }

	void ShiftBrightness(float b) { brightnessChange += b; }
	float GetBrightness() { return brightnessChange; }
	void ResetBrightness() { brightnessChange = 0; }

	void StartBlend(CColor& c);
	void StopBlend() { blendingOn = false; }
	bool IsBlending() { return blendingOn; }

	// change dimensions, allows for vector to be assigned
	CShader operator=(const CVector& v);
	CShader operator+=(const CVector& v);
	CShader operator-=(const CVector& v);
	// scale
	CShader operator=(const float s);
	CShader operator+=(const float s);
	CShader operator-=(const float s);

	float GetScale() { return scale; }
	CVector GetDimensions() { return CVector(col, row); }
	bool AllowedScale(float s, float addS = NULL) { return addS != NULL ? (s + addS <= 1000 && s + addS >= 1) : (s <= 1000 && s >= 1); }
};

