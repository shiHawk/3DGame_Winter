#pragma once
#include "DxLib.h"
#include <cmath>
class Quaternion
{
public:
	float w, x, y, z;

	Quaternion()
	{
		w = 0.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	};
	MATRIX GetMatrix() const;
private:

};
Quaternion operator *(const Quaternion& lQ, const Quaternion& rQ);
VECTOR operator *(const Quaternion& qRot, const VECTOR& right);
