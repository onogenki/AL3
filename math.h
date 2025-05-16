#pragma once
#include"KamataEngine.h"

using namespace KamataEngine;

class math {

	public:

	void worldTransformUpdate(WorldTransform& worldTransform);

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
};
