#pragma once

#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
enum class DitherignMehod {
	NONE = 0,
	FS = 1,//Floyd–Steinberg 
	SIERRA = 2,//Sierra
	TLSIERRA = 3,//Two-line Sierra/Two-row Sierra 
	JJN = 4,//Jarvis, Judice & Ninke
	STUCKI = 5,//Stucki
	SIERRALITE = 6,//Sierra lite
	ATKINSON = 7, //Atkinson
	BURKES = 8,
};

enum class DitheringDirection {
	TBLR = 0,    // Top to Bottom, Left to Right
	TBRL = 1,    // Top to Bottom, Right to Left
	BTLR = 2,    // Bottom to Top, Left to Right
	BTRL = 3,    // Bottom to Top, Right to Left
	TBZIGZAG = 4, // Top to Bottom Zigzag (alternating directions)
	BTZIGZAG = 5  // Bottom to Top Zigzag (alternating directions)
};
const std::string prefixy[] = { "FS","Sierra","TLSierra","JJN","Stucki","SierraLite","Atkinson","Burkes" };
const std::string methodNames[] = { "Floyd–Steinberg","Sierra","Two-line_Sierra","Jarvis_Judice_Ninke","Stucki","SierraLite","Atkinson","Burkes" };

const float FSValues[4] = { 7 / 16.0f, 3 / 16.0f,5 / 16.0f,1 / 16.0f };
const int FSPos[4][2] = { {1,0},{-1,1},{0,1},{1,1} };

const float SierraValues[10] = { 5 / 32.0f,3 / 32.0f,2 / 32.0f,4 / 32.0f,5 / 32.0f,4 / 32.0f,2 / 32.0f,2 / 32.0f,3 / 32.0f,2 / 32.0f };
const int SierraPos[10][2] = { {1,0},{2,0},{-2,1},{-1,1},{0,1} ,{1,1},{2,1},{-1,2},{0,2},{1,2} };

const float TLSierraValues[7] = { 4 / 16.0f,3 / 16.0f,1 / 16.0f,2 / 16.0f,3 / 16.0f,2 / 16.0f,1 / 16.0f };
const int TLSierraPos[7][2] = { {1,0},{2,0},{-2,1},{-1,1},{0,1} ,{1,1},{2,1} };

const float JJNValues[12] = { 7 / 48.0f,5 / 48.0f,3 / 48.0f,5 / 48.0f,7 / 48.0f,5 / 48.0f,3 / 48.0f,1 / 48.0f,3 / 48.0f,5 / 48.0f,3 / 48.0f,1 / 48.0f };
const int JJNPos[12][2] = { {1,0},{2,0},{-2,1},{-1,1},{0,1} ,{1,1},{2,1},{-2,2},{-1,2},{0,2},{1,2},{2,2} };

const float StuckiValues[12] = { 8 / 42.0f,4 / 42.0f,2 / 42.0f,4 / 42.0f,8 / 42.0f,4 / 42.0f,2 / 42.0f,1 / 42.0f,2 / 42.0f,4 / 42.0f,2 / 42.0f,1 / 42.0f };
const int StuckiPos[12][2] = { {1,0},{2,0},{-2,1},{-1,1},{0,1} ,{1,1},{2,1},{-2,2},{-1,2},{0,2},{1,2},{2,2} };

const float SierraLiteValues[3] = { 2 / 4.0f,1 / 4.0f,1 / 4.0f };
const int SierraLitePos[3][2] = { {1,0},{-1,1},{0,1} };

const float AtkinsonValues[6] = { 1 / 8.0f,1 / 8.0f,1 / 8.0f,1 / 8.0f,1 / 8.0f,1 / 8.0f };
const int AtkinsonPos[6][2] = { {1,0},{2,0},{-1,1},{0,1},{1,1},{1,2} };

const float BurkesValues[7] = { 8 / 32.0f,4 / 32.0f,2 / 32.0f,4 / 32.0f,8 / 32.0f,4 / 32.0f,2 / 32.0f };
const int BurkesPos[7][2] = { {1,0},{2,0},{-2,1},{-1,1},{0,1},{1,1},{2,1} };

float sqrDistanceVec3f(sf::Vector3f A, sf::Vector3f B);
inline sf::Color findClosestColor(std::vector<sf::Color> kolory, sf::Color pixelColor, std::vector<float> maxDistancesSquared, sf::Color defaultColor);
//void dithering(sf::Image &originalImage, const std::vector<sf::Color> colorPalette, const std::vector<float> maxDistancesSquared, const float ditheringStrenght = 1, ditherignMehod dithMethod = STUCKI, sf::Color defaultColor = sf::Color::White);
void dithering(sf::Image &originalImage, const std::vector<sf::Color> colorPalette, const std::vector<float> maxDistances = { 300 }, const float ditheringStrenght = 1.0f, DitherignMehod dithMethod = DitherignMehod::STUCKI,  sf::Color defaultColor = sf::Color::White);
