#pragma once
#include "maths_funcs.h"
#include <math.h>
#include <stdlib.h>

// Define a class for the Monkey heads
class Banana {
private:
	int index;
	vec3 position;
	double rotation;
	float movement_unit = 1.0;
	vec3 x_movement = vec3(movement_unit, 0.0, 0.0);
	vec3 z_movement = vec3(0.0, 0.0, movement_unit);
	float speed = 0.4f;
	double distance_travelled = 0.0;
	double max_distance = 40.0f;
	float scaleFactor = 0.75f;
	vec3 scaleVector = vec3(0.25, 0.25, 0.25);
	float health = 1.0;

	void generateIndex() {
		index = rand() % 4;
		//printf("INDEX %d\n", index);
	}

	void setStartPoint() {
		vec3 positions[4] = { vec3(0.0, 0.5, -20.0), vec3(-20.0, 0.5, 0.0), vec3(0.0, 0.5, 20.0), vec3(20.0, 0.5, 0.0) };
		if (index == 0 || index == 2) {
			float randOffset = (float)(rand() % 3000) / 1500.0;
			randOffset -= 1.0;
			position = positions[index] + vec3(randOffset, 0.0, 0.0);
		}
		else {
			float randOffset = (float)(rand() % 3000) / 1500.0;
			randOffset -= 1.0;
			position = positions[index] + vec3(0.0, 0.0, randOffset);
		}
	}

	void setRotation() {
		double rotations[4] = { 90.0, 180.0, 270.0, 0.0 };
		rotation = rotations[index];
	}

	void setSpeed() {
		int rando = rand() % 500 + 500;
		//printf("RANDO: %i\n", rando);
		speed = (float)rando / 10000.0;
		//printf("SPEED: %f\n", speed);
	}

public:
	void initBanana() {
		generateIndex();
		setStartPoint();
		setRotation();
		setSpeed();
		distance_travelled = 0.0f;
	}

	vec3 getPosition() {
		return position;
	}

	double getRotation() {
		return rotation;
	}

	void updatePosition() {
		switch (index) {
		case 0:
			position = position + (z_movement * speed);
			distance_travelled += (movement_unit * speed);
			break;
		case 1:
			position = position + (x_movement * speed);
			distance_travelled += (movement_unit * speed);
			break;
		case 2:
			position = position - (z_movement * speed);
			distance_travelled += (movement_unit * speed);
			break;
		case 3:
			position = position - (x_movement * speed);
			distance_travelled += (movement_unit * speed);
			break;
		}
	}

	bool inBounds() {
		return distance_travelled < max_distance;
	}

	float getScaleFactor() {
		return scaleFactor;
	}

	vec3 getScaleVector() {
		return scaleVector;
	}

	float getHealth() {
		return health;
	}

	void updateHealth(float damage) {
		health -= damage;
	}

	bool stillAlive() {
		return health > 0.0;
	}
};