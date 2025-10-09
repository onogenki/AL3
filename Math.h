#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

// 円周率
const float PI = 3.141592654f;

struct AABB {
	Vector3 min;
	Vector3 max;
};

// 02_14 29枚目 単項演算子オーバーロード
Vector3 operator+(const Vector3& v);
Vector3 operator-(const Vector3& v);

// CameraControllerのUpdate/Reset関数で必要
const Vector3 operator+(const Vector3& lhv, const Vector3& rhv);

// 0206のLerp関数
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
// 0206
const Vector3 operator*(const Vector3& v1, const float f);

// 代入演算子オーバーロード
Vector3& operator+=(Vector3& lhs, const Vector3& rhv);
Vector3& operator-=(Vector3& lhs, const Vector3& rhv);
Vector3& operator*=(Vector3& v, float s);
Vector3& operator/=(Vector3& v, float s);

// 単位行列の作成
Matrix4x4 MakeIdentityMatrix();

// 拡大縮小行列の作成
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

// X軸回転行列の作成
Matrix4x4 MakeRotateXMatrix(float theta);
// Y軸回転行列の作成
Matrix4x4 MakeRotateYMatrix(float theta);
// Z軸回転行列の作成
Matrix4x4 MakeRotateZMatrix(float theta);

// 平行移動行列の作成
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

// 拡大縮小・回転・平行移動行列を使ってアフィン変換行列を作る関数
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate);

// 代入演算子オーバーロード
Matrix4x4& operator*=(Matrix4x4& lhm, const Matrix4x4& rhm);

// 2項演算子オーバーロード
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);

// ワールドトランスフォーム更新
void WorldTransformUpdate(WorldTransform& worldTransform);

float Lerp(float x1, float x2, float t);

float EaseIn(float x1, float x2, float t);

float EaseOut(float x1, float x2, float t);

float EaseInOut(float x1, float x2, float t);

bool IsCollision(const AABB& aabb1, const AABB& aabb2);

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// 0215
inline float ToRadians(float degrees) { return degrees * (3.1415f / 180.0f); }
inline float ToDegrees(float radians) { return radians * (180.0f / 3.1415f); }

// ベクトルの長さを計算する関数
float Length(const Vector3& v);