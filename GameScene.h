#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

// ゲームシーン
class GameScene {
public:
	// テクスチャハンドル(リソース読み込むために必要なもの)
	uint32_t textureHandle_ = 0;
	//スプライト(2D表示物)
	Sprite* sprite_ = nullptr;
	// 3Dモデル
	Model* model_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// カメラ
	Camera camera_;

	//デバックカメラの生成
	DebugCamera* debugCamera_ = nullptr; 
	
	// サウンドデータの読み込み
	uint32_t soundDataHandle_ = 0;
	// 音声再生ハンドル
	uint32_t voiceHandle_ = 0;
	
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