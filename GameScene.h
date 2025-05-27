#pragma once
#include "KamataEngine.h"
#include<vector>
#include "Skydome.h"
#include "Player.h"
#include"Math.h"

using namespace KamataEngine;
// ゲームシーン
class GameScene {

	public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	~GameScene();

private:
	//3Dモデル
	Model* blockModel_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	Sprite* sprite_ = nullptr;

	Model* model_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_;
	//カメラ
	Camera camera_;

	//ブロックモデル
	Model* block_model_ = nullptr;
	//もう一度std::vectorを重ねる
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	//デバックカメラ有効
	bool isDebugCameraActive_ = false;

	DebugCamera* debugCamera_ = nullptr;

	Skydome* skydome_ = nullptr;
	//3Dモデル
	Model* modelSkydome_ = nullptr;

	Player* player_ = nullptr;

	Model* player_model_ = nullptr;

};