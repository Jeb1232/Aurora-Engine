#pragma once
#include"../dllHeader.h"
#include"Mathf.h"

struct AURORAENGINE_API Vector3 {
	float x, y, z;

	Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

	static Vector3 zero() {
		Vector3 retVector = { 0.0f,0.0f,0.0f };
		return retVector;
	}

	static Vector3 one() {
		Vector3 retVector = { 1.0f,1.0f,1.0f };
		return retVector;
	}

	static Vector3 forward() {
		Vector3 retVector = { 0.0f,0.0f,1.0f };
		return retVector;
	}

	static Vector3 back() {
		Vector3 retVector = { 0.0f,0.0f,-1.0f };
		return retVector;
	}

	static Vector3 left() {
		Vector3 retVector = { -1.0f,0.0f,0.0f };
		return retVector;
	}

	static Vector3 right() {
		Vector3 retVector = { 1.0f,0.0f,0.0f };
		return retVector;
	}

	static Vector3 up() {
		Vector3 retVector = { 0.0f,1.0f,0.0f };
		return retVector;
	}

	static Vector3 down() {
		Vector3 retVector = { 0.0f,-1.0f,0.0f };
		return retVector;
	}


	float magnitude() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	Vector3 normalized() const {
		double mag = magnitude();
		if (mag == 0) {
			return Vector3::zero();
		}
		return Vector3(x / mag, y / mag, z / mag);
	}

	static float distance(const Vector3& v1, const Vector3& v2) {
		Vector3 diff(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);

		return diff.magnitude();
	}

	static float dot(const Vector3& v1, const Vector3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}


	static float angle(const Vector3& v1, const Vector3& v2) {
		float dotProduct = Vector3::dot(v1, v2);
		float magnitudes = v1.magnitude() * v2.magnitude();


		if (magnitudes == 0) {
			return 0.0; 
		}

		double angleInRadians = std::acos(dotProduct / magnitudes);

		return angleInRadians * (360 / (PI * 2));
	}
};
