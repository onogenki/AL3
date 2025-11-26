#pragma once
#include "KamataEngine.h"
#include <vector>
#include"Player.h"
#include"Skydome.h"
#include"MapChipField.h"
#include"Math.h"
#include "CameraController.h"
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

	// ブロック生成
	void GeneratedBlocks();

private:
	// テクスチャハンドル(リソース読み込むために必要なもの)
	uint32_t textureHandle_ = 0;
	// スプライト(2D表示物)
	// Sprite* sprite_ = nullptr;
	
	//  3Dモデル
	Player* player_ = nullptr;
	Model* playerModel_ = nullptr;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// カメラ
	Camera camera_;

	//ブロック
	Model* blockModel_ = nullptr;
	//std::Vectorで可変個配列 それを2個使うことで2次元配列に
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	//天球
	Skydome* skydome_ = nullptr;
	Model* skyDomeModel_ = nullptr;
	
	//マップチップフィールド
	MapChipField* mapChipField_;

	//デバックカメラ有効
	bool isDebugCameraActive_ = false;
	// デバックカメラの生成
	DebugCamera* debugCamera_ = nullptr;

	// サウンドデータハンドル(音データそのもの)
	uint32_t soundDataHandle_ = 0;
	// 音声再生ハンドル(実際に再生している音)
	uint32_t voiceHandle_ = 0;

	// ImGuiで値を入力する変数
	float inputFloat3[3] = {0, 0, 0};

	//カメラ移動
	CameraController* CController_ = nullptr;
};