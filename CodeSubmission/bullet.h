#pragma once
#include "maths_funcs.h"
class Bullet {
private:
	vec3 pos;
	vec3 dir;
	float movement_unit = 1.0;
	vec3 x_movement = vec3(movement_unit, 0.0, 0.0);
	vec3 z_movement = vec3(0.0, 0.0, movement_unit);
	float speed = 0.4f;
	float distance_travelled = 0.0f;
	float max_distance = 10.0f;
	float scaleFactor;
	float rotation = 0.0;
	vec3 scaleVector;
	float damage = 1.0;
	float damageScale;
	bool visible;
	bool bazooka;

	void setStartPoint(vec3 position, vec3 direction) {
		pos = position + direction;
	}

	void setDirection(vec3 direction) {
		dir = vec3(direction.v[0], direction.v[1], direction.v[2]);
	}

	void setRotation() {
		print(dir);
		float x = dir.v[0], z = dir.v[2];
		if ((x <= 0 && x >= -1) && (z <= 1 && z >= 0)) {
			rotation = (90.0 * abs(x)) * -1.0;
		}
		else if ((x >= -1 && x <= 0) && (z <= 0 && z >= -1)) {
			rotation = ((90.0 * abs(z)) + 90.0) * -1.0;
		}
		else if ((x <= 1 && x >= 0) && (z >= -1 && z <= 0)) {
			rotation = ((90.0 * abs(x)) + 180.0) * -1.0;
		}
		else if ((x <= 1 && x >= 0) && (z >= 0 && z <= 1)) {
			rotation = ((90.0 * abs(z)) + 270.0) * -1.0;
		}
		printf("Angle: %f\n", rotation);
	}

	void setVisible() {
		visible = true;
	}

	void setBazooka(bool val) {
		bazooka = val;
	}

	void setScale() {
		if (bazooka) {
			scaleFactor = 1.0f;
			damageScale = 10.0f;
		}
		else {
			scaleFactor = 0.1f;
			damageScale = 0.1f;
		}
		scaleVector = vec3(scaleFactor, scaleFactor, scaleFactor);
	}

public:
	void initBullet(bool val, vec3 position, vec3 direction) {
		setStartPoint(position, direction);
		setVisible();
		setDirection(direction);
		setRotation();
		setBazooka(val);
		setScale();
	}

	vec3 getPosition() {
		return pos;
	}

	float getScaleFactor() {
		return scaleFactor;
	}

	vec3 getScaleVector() {
		return scaleVector;
	}

	void updatePosition() {
		pos += dir * speed;
		distance_travelled += movement_unit * speed;
	}

	bool inBounds() {
		return distance_travelled < max_distance;
	}

	double getRotation() {
		return rotation;
	}

	float getDamage() {
		return damage;
	}

	void setVisibility(bool val) {
		visible = val;
	}

	bool getVisibility() {
		return visible;
	}

	bool isBazooka() {
		return bazooka;
	}

	float getDamageScale() {
		return damageScale;
	}

	void setScaleFactor(float scale) {
		scaleFactor = scale;
	}
};
