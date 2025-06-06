#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

// 円周率
const float PI = 3.141592654f;

//CameraControllerのUpdate/Reset関数で必要
const Vector3 operator+(const Vector3& lhv, const Vector3& rhv);

//02_06のLerp関数
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
//02_06
const Vector3 operator*(const Vector3& v1, const float f);

// 代入演算子オーバーロード
Vector3& operator+=(Vector3& lhs, const Vector3& rhv);
Vector3& operator-=(Vector3& lhs, const Vector3& rhv);
Vector3& operator*=(Vector3& v, float s);
Vector3& operator/=(Vector3& v, float s);

Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

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

void worldTransformUpdate(WorldTransform& worldTransform);

float Lerp(float x1, float x2, float t);

float EaseInOut(float x1, float x2, float t);