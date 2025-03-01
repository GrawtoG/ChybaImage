#include <SFML/Graphics.hpp>
#include<iostream>

#include<filesystem>
#include<ctime>
#include<iomanip>
#include <vector>
#include <fstream>

#include "ditheringFun.h"

using namespace std;
using namespace sf;





int randomInt(int min, int max) //range : [min, max]
{
	static bool first = true;
	if (first)
	{
		srand(time(NULL)); //seeding for the first time only!
		first = false;
	}
	return min + rand() % ((max + 1) - min);
}
template <typename T>
void coutPosition(T shape) {

	cout << "x: " << shape.getPosition().x << " y: " << shape.getPosition().y << endl;
}

void ChangeResolution(int newPixelSize, Image& image) {


	Image elo;
	elo.create((image.getSize().x / newPixelSize), (image.getSize().y / newPixelSize));
	int eloY = 0;
	for (int y = 0; y < (image.getSize().y); y += newPixelSize) {
		int eloX = 0;
		for (int x = 0; x < (image.getSize().x); x += newPixelSize) {
			//elo.setPixel(eloX, eloY, getAverageColor(x, y, image, newPixelSize));
			eloX++;
		}
		eloY++;
	}
	image = elo;
}
void addPixelsToImage(int x, int y, Image& image) {
	Image elo;
	// BYNIU NIE DZIALA DOKOLORYWANIE TYCH DODANYCH PIKSELI W SAMYM ROGU NAPRAW TO LAMUSIE
	elo.create(image.getSize().x + x, image.getSize().y + y);
	for (int i = 0; i < image.getSize().x; i++) {
		for (int l = 0; l < image.getSize().y; l++) {
			elo.setPixel(i, l, image.getPixel(i, l));
		}
	}
	for (int i = image.getSize().x; i < elo.getSize().x; i++) {
		for (int l = 0; l < elo.getSize().y; l++) {
			elo.setPixel(i, l, image.getPixel(image.getSize().x - 1, l));
		}
	}
	for (int i = 0; i < elo.getSize().x; i++) {
		for (int l = image.getSize().y; l < elo.getSize().y; l++) {
			elo.setPixel(i, l, image.getPixel(i, image.getSize().y - 1));
		}
	}
	image = elo;
}
void imageToGrayscale(Image& image, unsigned int brightnessEquation=0) {
	float gray;
	for (int y = 0; y < image.getSize().y; y++) {
		for (int x = 0; x < image.getSize().x; x++) {
			Color pixelColor = image.getPixel(x, y);
			float r = (int)pixelColor.r;	
			float g = (int)pixelColor.g;
			float b = (int)pixelColor.b;
			switch (brightnessEquation) {
			case 0:
				gray = sqrt(0.299 * (r * r) + 0.587 * (g * g) + 0.114 * (b * b));
				break;
			case 1:
				gray = (0.2126 * r + 0.7152 * g + 0.0722 * b);
				break;
			case 2:
				gray = (0.299 * r + 0.587 * g + 0.114 * b);
				break;
			case 3:
				gray = sqrtf((r * r + g * g + b * b) / 3);
				break;
			default:
				cerr << "imageToGrayscale wrong brightness method!!!";
				return;
				break;
			}
		
			image.setPixel(x, y, Color(gray));
		}
	}
}

float sqrDistanceVec2(Vector2f A, Vector2f B) {
	Vector2f AB = Vector2f(B.x - A.x, B.y - A.y);
	return (AB.x * AB.x + AB.y * AB.y);
}
struct point {
	Vector2f pos;
	float distance = 0;
};

float mapFloat(float fval, float val_in_min, float val_in_max, float val_out_min, float val_out_max)
{
	return (fval - val_in_min) * (val_out_max - val_out_min) / (val_in_max - val_in_min) + val_out_min;
}

vector<Vector2f> regionToLines(Image image, int xZero, int yZero, int regionSize, float scalingFactor, float ilePikseliProcent, bool odLewej) {
	vector<Vector2f> points;
	int regionSizeX = regionSize;
	int regionSizeY = regionSize;
	vector<point> blackPixelsAll;
	vector<point> blackPixels;
	int coktorypixel;


	if (xZero + regionSize > image.getSize().x) {
		regionSizeX = image.getSize().x - xZero;

	}
	if (yZero + regionSize > image.getSize().y) {

		regionSizeY = image.getSize().y - yZero;
	}

	if (odLewej) {

		point puknt;
		puknt.pos = Vector2f(scalingFactor * xZero, scalingFactor * (yZero + regionSizeY));
		//blackPixelsAll.push_back(puknt);
		int endX = xZero + regionSizeX;
		int endY = yZero + regionSizeY;
		for (int x = xZero; x < endX; x++) {
			for (int y = yZero; y < endY; y++) {

				if (image.getPixel(x, y) != Color::White) {
					point punkcik;
					punkcik.pos = Vector2f(scalingFactor * x, scalingFactor * y);
					blackPixelsAll.push_back(punkcik);


				}
			}

		}
		if (ilePikseliProcent == 0) {

			point puknt;
			puknt.pos = Vector2f(scalingFactor * xZero, scalingFactor * (yZero + regionSizeY));
			points.push_back(Vector2f(scalingFactor * (xZero + regionSizeX), scalingFactor * (yZero + regionSizeY)));
			return points;
		}
		else {
			coktorypixel = floor(blackPixelsAll.size() / (blackPixelsAll.size() * ilePikseliProcent));

		}

		for (int i = 0; i < blackPixelsAll.size(); i += coktorypixel) {
			blackPixels.push_back(blackPixelsAll[i]);

		}
		float minDis = 1000000000000000;
		int minDisIndex = 0;
		for (int l = 0; l < blackPixels.size(); l++) {
			minDis = 10000000000000000000;
			for (int i = l; i < blackPixels.size(); i++) {
				float dis = sqrDistanceVec2(blackPixels.at(l).pos, blackPixels.at(i).pos);
				if (dis > 0 && dis < minDis) {
					minDis = dis;
					minDisIndex = i;
				}
			}
			points.push_back(blackPixels[minDisIndex].pos);
		}
		points.push_back(Vector2f(scalingFactor * (xZero + regionSizeX), scalingFactor * (yZero + regionSizeY)));
	}
	else {
		point puknt;
		puknt.pos = Vector2f(scalingFactor * xZero + regionSizeX, scalingFactor * (yZero + regionSizeY));
		//blackPixelsAll.push_back(puknt);
		int endX = xZero;
		int endY = yZero;
		for (int x = xZero + regionSizeX - 1; x > endX; x--) {
			for (int y = yZero + regionSizeY - 1; y > endY; y--) {
				if (image.getPixel(x, y) != Color(255, 255, 255)) {
					point punkcik;
					punkcik.pos = Vector2f(scalingFactor * x, scalingFactor * y);
					blackPixelsAll.push_back(punkcik);
				}
			}
		}
		if (ilePikseliProcent == 0) {

			point puknt;
			puknt.pos = Vector2f(scalingFactor * xZero, scalingFactor * (yZero + regionSizeY));
			points.push_back(Vector2f(scalingFactor * (xZero + regionSizeX), scalingFactor * (yZero + regionSizeY)));
			return points;
		}
		else {
			coktorypixel = floor(blackPixelsAll.size() / (blackPixelsAll.size() * ilePikseliProcent));

		}

		for (int i = 0; i < blackPixelsAll.size(); i += coktorypixel) {
			blackPixels.push_back(blackPixelsAll[i]);

		}

		float minDis = FLT_MAX;
		int minDisIndex = 0;
		for (int l = 0; l < blackPixels.size(); l++) {
			minDis = FLT_MAX;
			for (int i = l; i < blackPixels.size(); i++) {
				float dis = sqrDistanceVec2(blackPixels.at(l).pos, blackPixels.at(i).pos);
				if (dis > 0 && dis < minDis) {
					minDis = dis;
					minDisIndex = i;
				}
			}
			points.push_back(blackPixels[minDisIndex].pos);
			//cout << minDisIndex << endl;
			//blackPixels.erase(blackPixels.begin(), blackPixels.begin()+blackPixels.size());
		}


		points.push_back(Vector2f(scalingFactor * (xZero), scalingFactor * (yZero + regionSizeY)));
	}

	return points;
}

void DrawLine(Vector2f start, Vector2f end, Image& image) {
	float x{ end.x - start.x };
	float y{ end.y - start.y };
	const float max{ std::max(std::fabs(x), std::fabs(y)) };
	x /= max;
	y /= max;

	for (float n = 0; n < max; ++n)
	{
		// draw pixel at ( x0, y0 )
		image.setPixel(start.x, start.y, Color::Black);
		start.x += x; start.y += y;
	}
}



int main()
{
#pragma region window
	ContextSettings settings;
	settings.antialiasingLevel = 8;
	int windowWidht = 600;
	int windowHeigth = 600;
	RenderWindow window(VideoMode(windowWidht, windowHeigth, 32), "Title", Style::Default, settings);
	window.setFramerateLimit(60);
	Event event;
#pragma endregion

						 
	Image image;

	string filename = "girl.png";
	 
	if (!image.loadFromFile(filename)) {
		std::cout << "NIE MA PLIKU O TAKIEJ NAZWIE \"" + filename + "\"";
		return -1;
	}
	
	vector<sf::Color> kolorki;
	int da = 32;
	for (int i = 0; i < 256; i+=da) {
		for (int j = 0; j < 256; j+=da) {
			for (int k = 0; k < 256; k+=da) {
				kolorki.push_back(Color(i, j, k));
			}
		}
	}
	
	dithering(image, kolorki, { 500 }, 1, DitherignMehod::BURKES, Color::White);
	
	image.saveToFile("smoothing.png");
	cerr << "Plik zapisany!";
	

#pragma region obrazkiKolory
		
		/*
		for (int i = 0; i < kolorImages.size(); i++) {

			if (puste[i]) {
				//kolorImages[i].saveToFile("0PUSTE"+ to_string(i)+ filename);
			}
			else {
				//kolorImages[i].saveToFile(to_string(i) + filename);
			}

		}
		image.saveToFile(prefixyMetod[method] + "allColors.png");
		cout << "\nZapisane";
	*/
#pragma endregion

	while (window.isOpen()) {
		while (window.pollEvent(event)) {

			if (event.type == Event::Closed) {


				window.close();
			}
		}

		window.clear(Color::White);
		//here drawing (window.draw();)
		
		window.display();

	}

	return 0;



}









