#pragma once
#include "KamataEngine.h"
#include<vector>
#include "Skydome.h"
#include "Player.h"
#include"math.h"


// ゲームシーン
class GameScene {

private:
	//3Dモデル
	KamataEngine::Model* blockModel_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	KamataEngine::Model* model_ = nullptr;

	//ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	//カメラ
	KamataEngine::Camera camera_;

	//ブロックモデル
	KamataEngine::Model* block_model_ = nullptr;
	//もう一度std::vectorを重ねる
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	//デバックカメラ有効
	bool isDebugCameraActive_ = false;

	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Skydome* skydome_ = nullptr;
	//3Dモデル
	KamataEngine::Model* modelSkydome_ = nullptr;

	Player* player_ = nullptr;

	math* math_ = nullptr;

public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	~GameScene();
};