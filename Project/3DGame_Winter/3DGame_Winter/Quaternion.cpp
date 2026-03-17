#include "Quaternion.h"

MATRIX Quaternion::GetMatrix() const
{
	MATRIX m;
	
	// 行列の計算 (標準的なクォータニオン to 行列の公式)
	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;
	
	// 1行目
	m.m[0][0] = 1.0f - 2.0f * (y2 + z2);
	m.m[0][1] = 2.0f * (xy + wz);
	m.m[0][2] = 2.0f * (xz - wy);
	m.m[0][3] = 0.0f;
	
	// 2行目
	m.m[1][0] = 2.0f * (xy - wz);
	m.m[1][1] = 1.0f - 2.0f * (x2 + z2);
	m.m[1][2] = 2.0f * (yz + wx);
	m.m[1][3] = 0.0f;
	
	// 3行目
	m.m[2][0] = 2.0f * (xz + wy);
	m.m[2][1] = 2.0f * (yz - wx);
	m.m[2][2] = 1.0f - 2.0f * (x2 + y2);
	m.m[2][3] = 0.0f;
	
	// 4行目 (平行移動成分はなし)
	m.m[3][0] = 0.0f;
	m.m[3][1] = 0.0f;
	m.m[3][2] = 0.0f;
	m.m[3][3] = 1.0f;
	
	return m;
}

Quaternion operator*(const Quaternion& lQ, const Quaternion& rQ)
{
	Quaternion tempQ;
	
	/*クオータニオンの掛け算*/
	tempQ.w = lQ.w * rQ.w - lQ.x * rQ.x - lQ.y * rQ.y - lQ.z * rQ.z;//実部
	tempQ.x = lQ.w * rQ.x + lQ.x * rQ.w + lQ.y * rQ.z - lQ.z * rQ.y;//虚部x
	tempQ.y = lQ.w * rQ.y + lQ.y * rQ.w + lQ.z * rQ.x - lQ.x * rQ.z;//虚部y
	tempQ.z = lQ.w * rQ.z + lQ.z * rQ.w + lQ.x * rQ.y - lQ.y * rQ.x;//虚部z
	return tempQ;
}

VECTOR operator*(const Quaternion& qRot, const VECTOR& right)
{
	Quaternion qPos, qInv;
	VECTOR vPos;

	//3次元座標をクオータニオンに変換
	qPos.w = 1.0f;
	qPos.x = right.x;
	qPos.y = right.y;
	qPos.z = right.z;

	//回転クォータニオンのインバースの作成
	//逆クォータニオンを出すのは大変なので、
	//3次元だと同じ値になる共役クオータニオンで作成(虚部だけマイナス反転)
	qInv.w = qRot.w;
	qInv.x = -qRot.x;
	qInv.y = -qRot.y;
	qInv.z = -qRot.z;

	//回転後のクオータニオンの作成
	qPos = qRot * qPos * qInv;

	//３次元座標に戻す
	vPos.x = qPos.x;
	vPos.y = qPos.y;
	vPos.z = qPos.z;

	return vPos;
}
