#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <tgmath.h>

using namespace std;

//Constants
const int VM_WID = 1000, VM_HIG = 800;
const int VM_FR = 60;
const double PI = 3.14159265358979323846;
bool earthPlaced;

//Convert 0, 0 to middle of screen
sf::Vector2f localToGlobal(const sf::Vector2f& cart) {
	return { VM_WID / 2 + cart.x, VM_HIG / 2 + cart.y };
}

//Custom easy random integer
int randInt(int min, int max) {
	return rand() % (max - min) + min;
}

void rotatePointsClockwise(vector<pair<sf::Vector2f, float>>& points, float theta) {
	for (pair<sf::Vector2f, float>& pair : points) {
		float nX = pair.first.x * cosf(theta) - pair.first.y * sinf(theta);
		float nY = pair.first.x * sinf(theta) + pair.first.y * cosf(theta);
		pair.first.x = nX;
		pair.first.y = nY;
	}
}

void GenerateArm(vector<pair<sf::Vector2f, float>>& posRads, float rot_fac, int armDensity, int fuzz, float r, float b,
				float fuz_fac, int minRad, int maxRad) {
	//Point Generation
	sf::Vector2f tmp;
	for (double theta = -2 * PI; theta < PI / 1.5; theta += PI / armDensity) {
		int fuzzX = randInt(-fuzz, fuzz);
		int fuzzY = randInt(-fuzz, fuzz);
		tmp.x = r * powf(2.78, b * (theta)) * cosf(theta + PI * rot_fac) + fuzzX * fuz_fac;
		tmp.y = r * powf(2.78, b * (theta)) * sinf(theta + PI * rot_fac) + fuzzY * fuz_fac;
		posRads.push_back(make_pair(tmp, (float)randInt(minRad, maxRad) / (fuzzY + fuzzX) + 1));
	}
}

//Main program
int main() {

	//Setup
	srand(time(NULL));
	sf::RenderWindow window(sf::VideoMode(VM_WID, VM_HIG), "Galaxy Sim");
	window.setFramerateLimit(VM_FR);
	sf::CircleShape cir;
	cir.setFillColor(sf::Color::White);
	sf::Clock clock;
	float lastTime = 0;

	//Settings
	int numCivs = 15600000;
	int scale = 300;
	int disc_radius = 50000;
	int disc_height = 1000;
	double disc_vol = PI * disc_radius * disc_radius * disc_height;
	
	//Galaxy modeling
	int disc_radius_scaled = (int)(disc_radius / scale);
	double disc_vol_scaled = disc_vol / (4.0 / 3.0 * PI * powf((scale / 2), 3));
	int r = disc_radius_scaled;
	int mainArmDensity = 156;
	float b = 0.3;
	float rot_fac;
	float fuz_fac = 2.5f;
	int fuzz = (int)(0.03 * r);
	int minRad = 0.5, maxRad = 3;
	float totalRotDiff = PI / 1560.0;
	float totalRot = 0;

	//Civilization modeling
	double civRatio = numCivs / disc_radius_scaled;
	double detectionProb;
	if (civRatio < 0.002) {
		detectionProb = 0;
	}
	else if (civRatio >= 5) {
		detectionProb = 1;
	}
	else {
		detectionProb = -0.004757 * powf(civRatio, 4) + 0.06681 * powf(civRatio, 3) - 0.3605 * powf(civRatio, 2) +
			0.9215 * civRatio + 0.00826;
	}

	//Text
	sf::Font font;
	sf::Text legend;

	if (!font.loadFromFile("Minecraft.ttf")) {
		cout << "Error loading font!" << endl;
	}

	legend.setFont(font);
	legend.setFillColor(sf::Color::White);
	legend.setCharacterSize(25);
	legend.setPosition(sf::Vector2f(30, 30));

	//Point storage
	sf::Vector2f tmp;
	vector<pair<sf::Vector2f, float>> posRads;

	GenerateArm(posRads, 0, 100, 3, r, b, 2.3, 2, 4);
	GenerateArm(posRads, 0.09, 85, 5, r, b, 5.5, 1, 2);

	GenerateArm(posRads, 0.5, 100, 3.2, r, b, 2.5, 2, 4);
	GenerateArm(posRads, 0.58, 85, 5, r, b, 5.5, 1, 2);

	GenerateArm(posRads, 1, 100, 3.2, r, b, 2.5, 2, 4);
	GenerateArm(posRads, 1.08, 85, 5, r, b, 5.5, 1, 2);

	GenerateArm(posRads, 1.5, 100, 3.2, r, b, 2.5, 2, 4);
	GenerateArm(posRads, 1.58, 85, 5, r, b, 5.5, 1, 2);


	//Main loop
	while (window.isOpen()) {
		sf::Event ev;
		while (window.pollEvent(ev)) {
			if (ev.type == sf::Event::Closed) {
				window.close();
			}
		}

		//Rotate all points
		rotatePointsClockwise(posRads, -totalRotDiff);
		totalRot += totalRotDiff;
		float currentTime = clock.getElapsedTime().asSeconds();
		float fps = 1.0f / (currentTime - lastTime);
		lastTime = currentTime;
		string legendStr = "Pixel Size: " + to_string(scale) + " LY\nFPS: " + to_string((int)fps);
		legend.setString(sf::String(legendStr));

		//Rendering
		window.clear();

		for (pair<sf::Vector2f, float>& p : posRads) {
			cir.setPosition(localToGlobal(p.first));
			cir.setRadius(p.second);
			window.draw(cir);
		}

		window.draw(legend);
		window.display();
	}

	//End of Program
	return 0;
}