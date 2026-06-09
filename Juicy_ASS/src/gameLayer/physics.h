#pragma once
#include <raylib.h>
#include <gameMap.h>
#include <raymath.h>

typedef Vector2 vec;

inline vec addVectors(vec a, vec b) {
	return { a.x + b.x, a.y + b.y };
}

inline vec subtractVectors(vec a, vec b) {
	return { a.x - b.x, a.y - b.y };
}

inline float dotProduct(vec a, vec b) {
	return  ( a.x * b.x + a.y * b.y );
}

inline vec multiplyVectorWithScalar(vec a, float s) {
	return { a.x * s, a.y * s };
}





struct Transform2D {

	Vector2 pos = {};
	float w = 0;
	float h = 0;

	Vector2 getCenter() const { return { pos.x, pos.y }; };
	Vector2 getTop() const { return { pos.x , pos.y - h * 0.5f }; }
	Vector2 getBottom() const { return { pos.x, pos.y + h * 0.5f }; }
	Vector2 getLeft() const { return { pos.x - w * 0.5f, pos.y }; }
	Vector2 getRight() const { return { pos.x + w * 0.5f, pos.y }; }
	Vector2 getTopLeft() const { return { pos.x - w * 0.5f, pos.y - h * 0.5f }; }
	Vector2 getTopRight() const { return { pos.x + w * 0.5f, pos.y - h * 0.5f }; }
	Vector2 getBottomLeft() const { return { pos.x - w * 0.5f, pos.y + h * 0.5f }; }
	Vector2 getBottomRight() const { return { pos.x + w * 0.5f, pos.y + h * 0.5f }; }


	Rectangle getAABB() {    //Axis Align Bounding Box
		return Rectangle{ pos.x - w * 0.5f, pos.y - h * 0.5f, w, h };
	}

	bool intersectPoint(Vector2 point, float delta = 0) {
		Rectangle aabb = getAABB();
		aabb.x -= delta;
		aabb.y -= delta;
		aabb.width += 2 * delta;
		aabb.height += 2 * delta;

		return CheckCollisionPointRec(point, aabb);
	}

	bool intersectTransform(Transform2D test, float delta = 0) {
		Rectangle a = getAABB();
		Rectangle b = test.getAABB();

		a.x -= delta;
		a.y -= delta;
		a.width += 2 * delta;
		a.height += 2 * delta;

		b.x -= delta;
		b.y -= delta;
		b.width += 2 * delta;
		b.height += 2 * delta;

		return CheckCollisionRecs(a, b);

	}

};


struct PhysicalEntity {

	Transform2D transform;
	vec lastPosition;

	vec velocity;
	vec accelaration = {0, 30};

	bool leftTouch = 0;
	bool rightTouch = 0;
	bool upTouch = 0;
	bool downTouch = 0;


	void teleport(vec pos) {
		transform.pos = pos;
		lastPosition = pos;
	}

	void updateFinal() {
		lastPosition = transform.pos;
	}


	void updateForces(float deltaTime) {

		//updating velocity 
		velocity = addVectors(velocity, multiplyVectorWithScalar(accelaration, deltaTime)); // v = v + at
		//updating position
		transform.pos = addVectors(transform.pos, multiplyVectorWithScalar(velocity, deltaTime)); // s = s + vt

		vec dragAcc = multiplyVectorWithScalar(velocity, Vector2Length(velocity));
		const float dragConstant = 0.1;

		if (Vector2Length(dragAcc) * dragConstant * deltaTime >= Vector2Length(velocity)) {
			velocity = {};
		}
		else {
			velocity = subtractVectors(velocity, multiplyVectorWithScalar(dragAcc, dragConstant * deltaTime));
		}

		if (Vector2Length(velocity) < 0.1) velocity = {};

	}


	void resolveConstrains();
	void checkCollisionOnce(GameMap& gameMap, vec &pos);
	vec performCollisonOnOneAxis(GameMap& gameMap, vec pos, vec delta);

};


