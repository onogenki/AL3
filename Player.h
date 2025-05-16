#pragma once
#include "KamataEngine.h"
#include "GameScene.h"

using namespace KamataEngine;

class Player {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Update();

	/// <summary>
	/// 初期化
	/// </summary>
	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};
