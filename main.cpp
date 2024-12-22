#include <SFML/Graphics.hpp>
#include<iostream>


#include<ctime>
#include<iomanip>
#include <vector>
#include <fstream>

using namespace std;
using namespace sf;


//dithering methods
#define FS 1			//Floyd–Steinberg 
#define Sierra 2		//Sierra 
#define TLSierra 3		//Two-line Sierra/Two-row Sierra 
#define JJN 4			//Jarvis, Judice & Ninke
#define Stucki 5		//Stucki
#define SierraLite 6	//Sierra lite

string methodsNames[6] = { "FS","Sierra","TLSierra","JJN","Stucki","SierraLite" };

int SierraValues[10] = { 5,3,2,4,5,4,2,2,3,2 };
int SierraPos[10][2] = { {1,0},{2,0},{-2,1},{-1,1},{0,1} ,{1,1},{2,1},{-1,2},{0,2},{1,2} };

int TLSierraValues[7] = { 4,3,1,2,3,2,1 };
int TLSierraPos[7][2] = { {1,0},{2,0},{-2,1},{-1,1},{0,1} ,{1,1},{2,1}};

int JJNValues[12] = {7,5,3,5,7,5,3,1,3,5,3,1 };
int JJNPos[12][2] = { {1,0},{2,0},{-2,1},{-1,1},{0,1} ,{1,1},{2,1},{-2,2},{-1,2},{0,2},{1,2},{2,2} };

int StuckiValues[12] = { 8,4,2,4,8,4,2,1,2,4,2,1 };
int StuckiPos[12][2] = { {1,0},{2,0},{-2,1},{-1,1},{0,1} ,{1,1},{2,1},{-2,2},{-1,2},{0,2},{1,2},{2,2} };

int SierraLiteValues[3] = {2,1,1};
int SierraLitePos[3][2] = { {1,0},{-1,1},{0,1} };

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
Color getAverageColor(int x, int y, Image image, int newPixelSize) {
	Image elo;
	int rSum = 0;
	int gSum = 0;
	int bSum = 0;
	for (int l = 0; l < newPixelSize; ++l) {
		for (int i = 0; i < newPixelSize; ++i) {
			rSum += (int)image.getPixel(x + i, y + l).r;
			gSum += (int)image.getPixel(x + i, y + l).g;
			bSum += (int)image.getPixel(x + i, y + l).b;
		}
	}
	rSum = rSum / (newPixelSize * newPixelSize);
	gSum = gSum / (newPixelSize * newPixelSize);
	bSum = bSum / (newPixelSize * newPixelSize);
	return Color(rSum, gSum, bSum);
}
void ChangeResolution(int newPixelSize, Image& image) {


	Image elo;
	elo.create((image.getSize().x / newPixelSize), (image.getSize().y / newPixelSize));
	int eloY = 0;
	for (int y = 0; y < (image.getSize().y); y += newPixelSize) {
		int eloX = 0;
		for (int x = 0; x < (image.getSize().x); x += newPixelSize) {
			elo.setPixel(eloX, eloY, getAverageColor(x, y, image, newPixelSize));
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
void imageToGrayscale(Image& image) {
	for (int y = 0; y < image.getSize().y; y++) {
		for (int x = 0; x < image.getSize().x; x++) {
			Color pixelColor = image.getPixel(x, y);
			float r = (int)pixelColor.r;
			float g = (int)pixelColor.g;
			float b = (int)pixelColor.b;
			float gray = sqrtf((r * r + g * g + b * b) / 3);
			image.setPixel(x, y, Color(gray, gray, gray));
		}
	}
}
float sqrDistance(Vector3f A, Vector3f B) {
	Vector3f AB = Vector3f(B.x - A.x, B.y - A.y, B.z - A.z);
	return (AB.x * AB.x + AB.y * AB.y + AB.z * AB.z);
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
Color findClosestColor(vector<Color> kolory, Color pixelColor, vector<float> maxDistances,Color defaultColor) {
	Vector3f pixelPos;
	pixelPos = Vector3f((float)pixelColor.r, (float)pixelColor.g, (float)pixelColor.b);
	Vector3f kolorPos;


	float distance;
	int minDisIndex = -1;
	vector<float>distances(kolory.size());

	for (int i = 0; i < kolory.size(); i++) {
		kolorPos = Vector3f((int)kolory[i].r, (int)kolory[i].g, (int)kolory[i].b);
		distance = sqrDistance(pixelPos, kolorPos);
		distances.at(i) = distance;

		//cout << "distance: " << distance << endl;
	}
	
	//cout << distances[0]<<endl;
	//cout << distances[2]<<endl;
	
	float minDistance = distances[0];
	for (int i = 0; i < kolory.size(); i++) {
		//cout << "Max Distances " << i << " : " << maxDistances[i] * maxDistances[i]<<endl;
		if (distances[i] <= maxDistances[i] * maxDistances[i]) {
			if (distances[i] <= minDistance) {
				minDistance = distances[i];
				minDisIndex = i;
			}
		}
	}
	
	//cout << "minDistance: " << minDistance<<endl;
	//cout << "------------------------------" << endl;
	//cout << minDisIndex << endl;
	
	if (minDisIndex == -1) {

		return defaultColor;
		//return Color(191, 64, 191);
	}
	else {
		//cout << minDisIndex << endl;
		return kolory[minDisIndex];
	}

}

#pragma region settings


//imageToGrayscale(image);
//vector<Color> kolory = { Color::Red,Color::Green, Color::Black, Color(50,196,232), Color(196,153,9), Color::Yellow };
//vector<Color> kolory = { Color::Black, Color::Cyan,Color::Magenta,Color::Yellow};
//vector<Color> kolory = { Color::Black,Color::Cyan,Color::Yellow,Color::Magenta,Color::White};
vector<Color> kolory = { Color::Black,Color::Red,Color::Blue,Color::Green};

//vector<Color> kolory = { Color(0,100,100), Color(255,255,0),Color(100,0,100),Color::Black };
//vector<Color> kolory = { Color::Red, Color::Green, Color::Blue,Color::Black };
Color DefaulColor = Color::White;//if none color is close enough this color is chosen
vector<float> maxDistances = { 300,300,300,300,100,300,300,200,200,200,200,200,200,200 };

const int regionSize = 10;
const float scalingFactor = 1;
const int ditheringStrenght = 1;
const float ilePikseliProcent = 0.5;// to tylko dzia³a na linie tu wartoœci u³amkowe np 0.53 nie 53

const int feedRate = 20000;
const string penUpCommand = "M3S50";
const string penDownCommand = "M5";
const float delay = 0.2;
const float pixelToMm = 0.5;


#define ditheringMethod FS; //FS , Sierra, TLSierra, JJN, Stucki, SierraLite; na samej gorze masz napisane jakie to s¹


#pragma endregion
int main()
{
#pragma region window
	ContextSettings settings;
	settings.antialiasingLevel = 8;
	int windowWidht = 1000;
	int windowHeigth = 600;
	RenderWindow window(VideoMode(windowWidht, windowHeigth, 32), "Title", Style::Default, settings);
	window.setFramerateLimit(60);
	Event event;
#pragma endregion

	
	Image image;
	string filename = "wall.png";
	if (!image.loadFromFile(filename)) {
		std::cout << "NIE MA PLIKU O TAKIEJ NAZWIE \"" + filename + "\"";
		return -1;
	}
	
	string methodNameAddOn="FS"; 
	int licznik = 0;

start:
	if (licznik >= 5) {
		return 0;

	}
	switch (licznik) {
	case 1:
		methodNameAddOn = "Sierra";
		break;
	case 2:
		methodNameAddOn = "TLSierra";
		break;
	case 3:
		methodNameAddOn = "JJN";
		break;
	case 4:
		methodNameAddOn = "Stucki";
		break;
	case 5:
		methodNameAddOn = "SierraLite";
		break;

	}

	
	vector<vector<Vector2f>> koloryPos;



	for (int i = 0; i < kolory.size(); i++) {
		vector<Vector2f> elo;
		koloryPos.push_back(elo);
	}
	vector<Image> kolorImages(kolory.size());
	for (int i = 0; i < kolory.size(); i++) {
		kolorImages[i].create(image.getSize().x, image.getSize().y, Color::White);
	}

#pragma region ToPNG
	int kropka = filename.find('.');
	string rozszerzenie = filename.substr(kropka + 1, filename.size() - kropka);
	if (rozszerzenie != "png") {
		Image elo;
		elo.loadFromFile(filename);
		filename = filename.substr(0, kropka) + ".png";
		elo.saveToFile(filename);
		image.loadFromFile(filename);

	}
	else {
		image.loadFromFile(filename);

	}
#pragma endregion

#pragma region obrazkiKolory
	int liczbaKolorow = kolory.size();
	vector<Vector3f> positions(liczbaKolorow);
	vector<bool> puste;
	for (int i = 0; i < liczbaKolorow; i++) {
		positions.at(i) = (Vector3f((int)kolory.at(i).r, (int)kolory.at(i).g, (int)kolory.at(i).b));
		puste.push_back(true);
	}
	
	
	

	Color oldPixel;
	Color newPixel;
	Color error;
	Color errorTemp;
	for (int y = 0; y < image.getSize().y - 2; y++) {
		for (int x = 0; x < image.getSize().x - 2; x++) {
			oldPixel = image.getPixel(x, y);
			newPixel = findClosestColor(kolory, oldPixel, maxDistances, DefaulColor);
			for (int i = 0; i < kolory.size(); i++) {
				if (newPixel == kolory[i]) {
					kolorImages[i].setPixel(x, y, newPixel);

					koloryPos.at(i).push_back(Vector2f(x, y));
					
					puste.at(i) = false;
					break;
				}
			}
			image.setPixel(x, y, newPixel);
			error = oldPixel - newPixel;


			
			switch (licznik)
			{
			case 0:
				errorTemp = Color((int)error.r * (ditheringStrenght*7 / (float)16), (int)error.g * (ditheringStrenght * 7 / (float)16), (int)error.b * (ditheringStrenght * 7 / (float)16));
				image.setPixel(x + 1, y, image.getPixel(x + 1, y) + errorTemp);
				errorTemp = Color((int)error.r * (ditheringStrenght * 3 / (float)16), (int)error.g * (ditheringStrenght * 3 / (float)16), (int)error.b * (ditheringStrenght * 3 / (float)16));
				image.setPixel(x - 1, y + 1, image.getPixel(x - 1, y + 1) + errorTemp);
				errorTemp = Color((int)error.r * (ditheringStrenght * 5 / (float)16), (int)error.g * (ditheringStrenght * 5 / (float)16), (int)error.b * (ditheringStrenght * 5 / (float)16));
				image.setPixel(x, y + 1, image.getPixel(x, y + 1) + errorTemp);
				errorTemp = Color((int)error.r * (ditheringStrenght * 1 / (float)16), (int)error.g * (ditheringStrenght * 1 / (float)16), (int)error.b * (ditheringStrenght * 1 / (float)16));
				image.setPixel(x + 1, y + 1, image.getPixel(x + 1, y + 1) + errorTemp);
				break;
			case 1:
				for (int o = 0; o < 10; o++) {
					errorTemp = Color((int)error.r * (ditheringStrenght * SierraValues[o] / (float)32), (int)error.g * (ditheringStrenght * SierraValues[o] / (float)32), (int)error.b * (ditheringStrenght * SierraValues[o] / (float)32));
					image.setPixel(x + SierraPos[o][0], y + SierraPos[o][1], image.getPixel(x + SierraPos[o][0], y + SierraPos[o][1]) + errorTemp);
				}
				break;
			case 2:
				for (int o = 0; o < 7; o++) {
					errorTemp = Color((int)error.r * (ditheringStrenght * TLSierraValues[o] / (float)16), (int)error.g * (ditheringStrenght * TLSierraValues[o] / (float)16), (int)error.b * (ditheringStrenght * TLSierraValues[o] / (float)16));
					image.setPixel(x + TLSierraPos[o][0], y + TLSierraPos[o][1], image.getPixel(x + TLSierraPos[o][0], y + TLSierraPos[o][1]) + errorTemp);
				}
				break;
			case 3:
				for (int o = 0; o < 12; o++) {
					errorTemp = Color((int)error.r * (ditheringStrenght * JJNValues[o] / (float)48), (int)error.g * (ditheringStrenght * JJNValues[o] / (float)48), (int)error.b * (ditheringStrenght * JJNValues[o] / (float)48));
					image.setPixel(x + JJNPos[o][0], y + JJNPos[o][1], image.getPixel(x + JJNPos[o][0], y + JJNPos[o][1]) + errorTemp);
				}
				break;
			case 4:
				for (int o = 0; o < 12; o++) {
					errorTemp = Color((int)error.r * (ditheringStrenght * StuckiValues[o] / (float)42), (int)error.g * (ditheringStrenght * StuckiValues[o] / (float)42), (int)error.b * (ditheringStrenght * StuckiValues[o] / (float)42));
					image.setPixel(x + StuckiPos[o][0], y + StuckiPos[o][1], image.getPixel(x + StuckiPos[o][0], y + StuckiPos[o][1]) + errorTemp);
				}
				break;
			case 5:
				for (int o = 0; o < 3; o++) {
					errorTemp = Color((int)error.r * (ditheringStrenght * SierraLiteValues[o] / (float)4), (int)error.g * (ditheringStrenght * SierraLiteValues[o] / (float)4), (int)error.b * (ditheringStrenght * SierraLiteValues[o] / (float)4));
					image.setPixel(x + SierraLitePos[o][0], y + SierraLitePos[o][1], image.getPixel(x + SierraLitePos[o][0], y + SierraLitePos[o][1]) + errorTemp);
				}
				break;
			default:
				break;
			}
/*

#if FS == ditheringMethod
			errorTemp = Color((int)error.r * (7 / (float)16), (int)error.g * (7 / (float)16), (int)error.b * (7 / (float)16));
			image.setPixel(x + 1, y, image.getPixel(x + 1, y) + errorTemp);
			errorTemp = Color((int)error.r * (3 / (float)16), (int)error.g * (3 / (float)16), (int)error.b * (3 / (float)16));
			image.setPixel(x - 1, y + 1, image.getPixel(x - 1, y + 1) + errorTemp);
			errorTemp = Color((int)error.r * (5 / (float)16), (int)error.g * (5 / (float)16), (int)error.b * (5 / (float)16));
			image.setPixel(x, y + 1, image.getPixel(x, y + 1) + errorTemp);
			errorTemp = Color((int)error.r * (1 / (float)16), (int)error.g * (1 / (float)16), (int)error.b * (1 / (float)16));
			image.setPixel(x + 1, y + 1, image.getPixel(x + 1, y + 1) + errorTemp);
				


			
#elif Sierra == ditheringMethod
			for (int o = 0; o < 10; o++) {
				errorTemp = Color((int)error.r * (SierraValues[o] / (float)32), (int)error.g * (SierraValues[o] / (float)32), (int)error.b * (SierraValues[o] / (float)32));
				image.setPixel(x + SierraPos[o][0], y + SierraPos[o][1], image.getPixel(x + SierraPos[o][0], y + SierraPos[o][1]) + errorTemp);
			}

#elif TLSierra == ditheringMethod
			cout << "TLSIERRA";
			for (int o = 0; o < 7; o++) {
				errorTemp = Color((int)error.r * (TLSierraValues[o] / (float)16), (int)error.g * (TLSierraValues[o] / (float)16), (int)error.b * (TLSierraValues[o] / (float)16));
				image.setPixel(x + TLSierraPos[o][0], y + TLSierraPos[o][1], image.getPixel(x + TLSierraPos[o][0], y + TLSierraPos[o][1]) + errorTemp);
		}

#elif JJN == ditheringMethod
			for (int o = 0; o < 12; o++) {
				errorTemp = Color((int)error.r * (JJNValues[o] / (float)48), (int)error.g * (JJNValues[o] / (float)48), (int)error.b * (JJNValues[o] / (float)48));
				image.setPixel(x + JJNPos[o][0], y + JJNPos[o][1], image.getPixel(x + JJNPos[o][0], y + JJNPos[o][1]) + errorTemp);
		}
#elif Stucki == ditheringMethod
			for (int o = 0; o < 12; o++) {
				errorTemp = Color((int)error.r * (StuckiValues[o] / (float)42), (int)error.g * (StuckiValues[o] / (float)42), (int)error.b * (StuckiValues[o] / (float)42));
				image.setPixel(x + StuckiPos[o][0], y + StuckiPos[o][1], image.getPixel(x + StuckiPos[o][0], y + StuckiPos[o][1]) + errorTemp);
		}
#elif SierraLite == ditheringMethod
			for (int o = 0; o < 3; o++) {
				errorTemp = Color((int)error.r * (SierraLiteValues[o] / (float)4), (int)error.g * (SierraLiteValues[o] / (float)4), (int)error.b * (SierraLiteValues[o] / (float)4));
				image.setPixel(x + SierraLitePos[o][0], y + SierraLitePos[o][1], image.getPixel(x + SierraLitePos[o][0], y + SierraLitePos[o][1]) + errorTemp);
		}


#endif
			

			
			*/


			

		}
	}

	for (int i = 0; i < kolorImages.size(); i++) {
		
		if (puste[i]) {
			//kolorImages[i].saveToFile("0PUSTE"+ to_string(i)+ filename);
		}
		else {
			//kolorImages[i].saveToFile(to_string(i) + filename);
		}
		
	}
	image.saveToFile(methodNameAddOn +"allColors.png");

	licznik++;
	goto start;
#pragma endregion
	std::cout << "juz" << endl;
/*
#pragma region naGcode
	for (int i = 0; i < liczbaKolorow; i++) {


		int dl = koloryPos[i].size();
		ofstream gcodeOut(to_string(i) +filename.substr(0,filename.length() - 4) + ".gcode");
		gcodeOut << penUpCommand+"\nG90\nG21\nG1 F"+to_string(feedRate)+"\n";
		gcodeOut << "G1 X0Y0" << endl;
		gcodeOut << "G1 X0"<< " Y"<<image.getSize().y*pixelToMm << endl;
		gcodeOut << "G1 X" << image.getSize().x * pixelToMm << " Y" << image.getSize().y * pixelToMm << endl;
		gcodeOut << "G1 X"<<image.getSize().x*pixelToMm<<"Y0"<< endl;
		gcodeOut << "G1 X0Y0" << endl;
		for (int d = 0; d < dl; d++) {
			//cout << "x: "<<koloryPos[i][d].x<<" y: "<< koloryPos[i][d].x << endl;
			gcodeOut << "G1 X" << koloryPos[i][d].x*pixelToMm << " Y" << (image.getSize().y-koloryPos[i][d].y) * pixelToMm << endl;
			gcodeOut << penDownCommand<<endl;
			gcodeOut << "G4 P" << to_string(delay) << endl;
			gcodeOut << penUpCommand<<endl;
			gcodeOut << "G4 P" << to_string(delay)<<endl;

		}
		gcodeOut << "G1 X0 Y0";
		gcodeOut.close();

	}
#pragma endregion
	*/
	/*
#pragma region points
	/*
	vector<Vector2f> points;
	vector<Vector2f> regionPoints;
	int kol = 0;
	//cout << kolorImages[0].getSize().y << endl;
	for (int y = 0; y < kolorImages[0].getSize().y; y += regionSize) {
		//cout << y << endl;
		
		if (kol % 2 == 0) {
			for (int x = 0; x < kolorImages[0].getSize().x; x += regionSize) {

				regionPoints = regionToLines(kolorImages[0], x, y, regionSize, scalingFactor, ilePikseliProcent, true);
				points.insert(points.end(), regionPoints.begin(), regionPoints.end());
			}
		}
		else {
			for (int x = kolorImages[0].getSize().x; x > 0; x -= regionSize) {

				
				
				regionPoints = regionToLines(kolorImages[0], x, y, regionSize, scalingFactor, ilePikseliProcent, false);
				//cout << "ads" << endl;
				points.insert(points.end(), regionPoints.begin(), regionPoints.end());
				//cout << "dgs" << endl;
				//cout << x << endl;
			}
			//cout << "afdfds" << endl;
		}
		//cout << "elo" << endl;
		kol++;
	}
	
#pragma endregion
	*/
	
	

#pragma region Linie
	
	/*for (int i = 0; i < koloryPos.at(0).size(); i++) {
		cout << i<<": x: " << koloryPos[0].at(i).x << " y: " << koloryPos[0].at(i).y << endl;
	}*/


	//cout << "KONIEasdadsC;" << endl;
	//VertexArray vertices;
	//vertices.setPrimitiveType(LineStrip);
	//for (int i = 0; i < points.size(); i++) {
	//	vertices.append(points[i]);
	//	vertices[i].color = Color::Black;
	//}
	std::cout << "KONIEC;" << endl;


	/*
	Image linie;
	linie.create(kolorImages[0].getSize().x, kolorImages[0].getSize().y, Color::White);
	//cout << vertices.getVertexCount();
	for (int i = 0; i < vertices.getVertexCount()-2 ; i++) {
		//linie.setPixel(vertices[i].position.x, vertices[i].position.y, Color::Black);
		DrawLine(vertices[i].position, vertices[i + 1].position, linie);
		//cout << i << " : x:" << vertices[i].position.x <<" y: " << vertices[i].position.y << endl;
	}

	linie.saveToFile("linigfde.png");
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
		//window.draw(vertices);
		
		window.display();

	}

	return 0;



}


