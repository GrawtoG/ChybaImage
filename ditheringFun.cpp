#include "ditheringFun.h"

template <typename T>
T myClamp(T value, T min, T max) {
	return std::max(min, std::min(value, max));
}

inline float sqrDistanceVec3f(sf::Vector3f A, sf::Vector3f B) {
	sf::Vector3f AB = sf::Vector3f(B.x - A.x, B.y - A.y, B.z - A.z);
	//return (0.299 * AB.x * AB.x + 0.587 * AB.y * AB.y + 0.114 * AB.z * AB.z);
	return (AB.x * AB.x + AB.y * AB.y + AB.z * AB.z);
}
inline sf::Color findClosestColor(std::vector<sf::Color> kolory, sf::Color pixelColor, std::vector<float> maxDistancesSquared, sf::Color defaultColor) {
	sf::Vector3f pixelPos;
	pixelPos = sf::Vector3f((float)pixelColor.r, (float)pixelColor.g, (float)pixelColor.b);
	sf::Vector3f kolorPos;


	float distance;
	int minDisIndex = -1;
	std::vector<float>distances(kolory.size());

	for (int i = 0; i < kolory.size(); i++) {
		kolorPos = sf::Vector3f((int)kolory[i].r, (int)kolory[i].g, (int)kolory[i].b);
		distance = sqrDistanceVec3f(pixelPos, kolorPos);
		distances.at(i) = distance;


	}

	float minDistance = distances[0];

	for (int i = 0; i < kolory.size(); i++) {
		if (distances[i] <= maxDistancesSquared[i]) {
			if (distances[i] <= minDistance) {
				minDistance = distances[i];
				minDisIndex = i;
			}
		}
	}


	/*
	for (int i = 0; i < kolory.size(); ++i) {
		if (distances[i] <= minDistance) {
			minDistance = distances[i];
			minDisIndex = i;
		}
	}

	*/

	if (minDisIndex == -1) {

		return defaultColor;
		//return Color(191, 64, 191);
	}
	else {
		return kolory[minDisIndex];
	}

}
void dithering(sf::Image &originalImage, std::vector<sf::Color> colorPalette, const std::vector<float> maxDistances, const float ditheringStrenght, DitherignMehod dithMethod, sf::Color defaultColor) {

	std::vector<float> maxDistancesSquared;
	std::transform(maxDistances.begin(), maxDistances.end(), std::back_inserter(maxDistancesSquared), [](float a) {return a * a; });
	if (colorPalette.size() > maxDistances.size()) {
		maxDistancesSquared.resize(colorPalette.size(), maxDistancesSquared[maxDistancesSquared.size() - 1]);
	}
	int liczbaKolorow = colorPalette.size();
	std::vector<sf::Vector3f> positions(liczbaKolorow);
	std::vector<bool> puste;
	for (int i = 0; i < liczbaKolorow; i++) {
		positions.at(i) = (sf::Vector3f((int)colorPalette.at(i).r, (int)colorPalette.at(i).g, (int)colorPalette.at(i).b));
		puste.push_back(true);

	}
	


	sf::Color oldPixel;
	sf::Color newPixel;
	sf::Vector3f error;
	sf::Vector3f errorTemp;
	sf::Color newColor;
	for (int y = 0; y < originalImage.getSize().y - 2; ++y) {
		for (int x = 0; x < originalImage.getSize().x - 2; ++x) {
			oldPixel = originalImage.getPixel(x, y);
			if (oldPixel.a == 0) continue;
			newPixel = findClosestColor(colorPalette, oldPixel, maxDistancesSquared, defaultColor);

			originalImage.setPixel(x, y, newPixel);
			error = sf::Vector3f(oldPixel.r - newPixel.r, oldPixel.g - newPixel.g, oldPixel.b - newPixel.b);


			switch (dithMethod)
			{
			case DitherignMehod::NONE:

				break;
			case DitherignMehod::FS:
				for (int o = 0; o < 4; ++o) {
					errorTemp = sf::Vector3f(error.x * ditheringStrenght * FSValues[o], error.y * ditheringStrenght * FSValues[o], error.z * ditheringStrenght * FSValues[o]);
					newColor = originalImage.getPixel(x + FSPos[o][0], y + FSPos[o][1]);
					newColor = sf::Color(myClamp((int)std::lround(newColor.r + errorTemp.x), 0, 255), myClamp((int)std::lround(newColor.g + errorTemp.y), 0, 255), myClamp((int)std::round(newColor.b + errorTemp.z), 0, 255),newColor.a);
					originalImage.setPixel(x + FSPos[o][0], y + FSPos[o][1], newColor);
					
				}
				break;
			case DitherignMehod::SIERRA:
				for (int o = 0; o < 10; o++) {
					errorTemp = sf::Vector3f(error.x * ditheringStrenght * SierraValues[o], error.y * ditheringStrenght * SierraValues[o], error.z * ditheringStrenght * SierraValues[o]);
					newColor = originalImage.getPixel(x + SierraPos[o][0], y + SierraPos[o][1]);
					newColor = sf::Color(myClamp((int)std::lround(newColor.r + errorTemp.x), 0, 255), myClamp((int)std::lround(newColor.g + errorTemp.y), 0, 255), myClamp((int)std::round(newColor.b + errorTemp.z), 0, 255), newColor.a);
					originalImage.setPixel(x + SierraPos[o][0], y + SierraPos[o][1], newColor);
				}
				break;
			case DitherignMehod::TLSIERRA:
				for (int o = 0; o < 7; o++) {
					errorTemp = sf::Vector3f(error.x * ditheringStrenght * TLSierraValues[o], error.y * ditheringStrenght * TLSierraValues[o], error.z * ditheringStrenght * TLSierraValues[o]);
					newColor = originalImage.getPixel(x + TLSierraPos[o][0], y + TLSierraPos[o][1]);
					newColor = sf::Color(myClamp((int)std::lround(newColor.r + errorTemp.x), 0, 255), myClamp((int)std::lround(newColor.g + errorTemp.y), 0, 255), myClamp((int)std::round(newColor.b + errorTemp.z), 0, 255), newColor.a);
					originalImage.setPixel(x + TLSierraPos[o][0], y + TLSierraPos[o][1], newColor);
				}
				break;
			case DitherignMehod::JJN:
				for (int o = 0; o < 12; o++) {
					errorTemp = sf::Vector3f(error.x * ditheringStrenght * JJNValues[o], error.y * ditheringStrenght * JJNValues[o], error.z * ditheringStrenght * JJNValues[o]);
					newColor = originalImage.getPixel(x + JJNPos[o][0], y + JJNPos[o][1]);
					newColor = sf::Color(myClamp((int)std::lround(newColor.r + errorTemp.x), 0, 255), myClamp((int)std::lround(newColor.g + errorTemp.y), 0, 255), myClamp((int)std::round(newColor.b + errorTemp.z), 0, 255), newColor.a);
					originalImage.setPixel(x + JJNPos[o][0], y + JJNPos[o][1], newColor);
				}
				break;
			case DitherignMehod::STUCKI:
				for (int o = 0; o < 12; o++) {
					errorTemp = sf::Vector3f(error.x * ditheringStrenght * StuckiValues[o], error.y * ditheringStrenght * StuckiValues[o], error.z * ditheringStrenght * StuckiValues[o]);
					newColor = originalImage.getPixel(x + StuckiPos[o][0], y + StuckiPos[o][1]);
					newColor = sf::Color(myClamp((int)std::lround(newColor.r + errorTemp.x), 0, 255), myClamp((int)std::lround(newColor.g + errorTemp.y), 0, 255), myClamp((int)std::round(newColor.b + errorTemp.z), 0, 255), newColor.a);
					originalImage.setPixel(x + StuckiPos[o][0], y + StuckiPos[o][1], newColor);
				}
				break;
			case DitherignMehod::SIERRALITE:
				for (int o = 0; o < 3; o++) {
					errorTemp = sf::Vector3f(error.x * ditheringStrenght * SierraLiteValues[o], error.y * ditheringStrenght * SierraLiteValues[o], error.z * ditheringStrenght * SierraLiteValues[o]);
					newColor = originalImage.getPixel(x + SierraLitePos[o][0], y + SierraLitePos[o][1]);
					newColor = sf::Color(myClamp((int)std::lround(newColor.r + errorTemp.x), 0, 255), myClamp((int)std::lround(newColor.g + errorTemp.y), 0, 255), myClamp((int)std::round(newColor.b + errorTemp.z), 0, 255), newColor.a);
					originalImage.setPixel(x + SierraLitePos[o][0], y + SierraLitePos[o][1], newColor);
				}
				break;
			case DitherignMehod::ATKINSON:
				for (int o = 0; o < 6; o++) {
					errorTemp = sf::Vector3f(error.x * ditheringStrenght * AtkinsonValues[o], error.y * ditheringStrenght * AtkinsonValues[o], error.z * ditheringStrenght * AtkinsonValues[o]);
					newColor = originalImage.getPixel(x + AtkinsonPos[o][0], y + AtkinsonPos[o][1]);
					newColor = sf::Color(myClamp((int)std::lround(newColor.r + errorTemp.x), 0, 255), myClamp((int)std::lround(newColor.g + errorTemp.y), 0, 255), myClamp((int)std::round(newColor.b + errorTemp.z), 0, 255), newColor.a);
					originalImage.setPixel(x + AtkinsonPos[o][0], y + AtkinsonPos[o][1], newColor);
				}
				break;
			case DitherignMehod::BURKES:
				for (int o = 0; o < 7; o++) {
					errorTemp = sf::Vector3f(error.x * ditheringStrenght * BurkesValues[o], error.y * ditheringStrenght * BurkesValues[o], error.z * ditheringStrenght * BurkesValues[o]);
					newColor = originalImage.getPixel(x + BurkesPos[o][0], y + BurkesPos[o][1]);
					newColor = sf::Color(myClamp((int)std::lround(newColor.r + errorTemp.x), 0, 255), myClamp((int)std::lround(newColor.g + errorTemp.y), 0, 255), myClamp((int)std::round(newColor.b + errorTemp.z), 0, 255), newColor.a);
					originalImage.setPixel(x + BurkesPos[o][0], y + BurkesPos[o][1], newColor);
				}
				break;

			default:
				std::cerr << "Nieprawidlowa MetodaDitheringu!";
				break;
			}
		}
	}
}