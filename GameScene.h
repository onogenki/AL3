#pragma once
#include <vector>
#include "KamataEngine.h"
#include "Player.h"
#include "skydome.h"
#include "MapChipField.h"
#include "CameraController.h"
#include "Enemy.h"
#include "DeathParticles.h"
#include"Fade.h"
#include "HitEffect.h"

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

	void GenerateBlocks();

	// 0210衝突判定と応答
	void CheckAllCollisions();

	//デスフラグのgetter
	bool isFinished() const { return finished_; }

	// エフェクトを生成  発生座標
	void CreateEffect(const Vector3& position);

private:

	//0212 ゲームのフェーズ(型)
	enum class Phase {
		kFadeIn,//フェードイン
		kPlay,//ゲームプレイ
		kDeath,//デス演出
		kFadeOut,//フェードアウト
	};

	//ゲームの現在フェーズ(変数)
	Phase phase_;

	void ChangePhase();
	 
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// スプライト
	Sprite* sprite_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// カメラ
	Camera camera_;

	// プレイヤー
	Player* player_ = nullptr;
	Model* player_model_ = nullptr;

	// ブロックモデル
	Model* block_model_ = nullptr;
	// もう一度std::vectorを重ねる
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	// デバックカメラ有効
	bool isDebugCameraActive_ = false;

	DebugCamera* debugCamera_ = nullptr;

	// 0203天球
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// カメラ移動
	CameraController* CController_ = nullptr;

	// enemyモデル
	Model* enemy_model_ = nullptr;

	// 0210敵の複数化
	std::list<Enemy*> enemies_;

	// 0211
	DeathParticles* deathParticles_ = nullptr;

	Model* deathParticle_model_ = nullptr;
	//0212 終了フラグ
	bool finished_ = false;

	//0213
	Fade* fade_ = nullptr;
	//死んだ後のフェードタイム
	int finishedTimer;
	Model* modelAttack_ = nullptr;

	// 0216
	//ヒットエフェクトのリスト
	std::list<HitEffect*> hitEffects_;

	Model* particle_model_ = nullptr;
};