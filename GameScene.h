#pragma once
#include "KamataEngine.h"

// ゲームシーン
class GameScene {
public:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	//サウンドデータの読み込み
	uint32_t soundDataHandle_ = 0;
	//音声再生ハンドル
	uint32_t voiceHandle_ = 0;
	//スプライト
	KamataEngine::Sprite* sprite_ = nullptr;
	// 3Dモデル
	KamataEngine::Model* model_ = nullptr;
	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;

	//デバックカメラの生成
	KamataEngine::DebugCamera* debugCamera_ = nullptr; 
	//ImGuiで値を入力する変数
	float inputFloat3[3] = {0, 0, 0};

	// デストラクタ
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();
};