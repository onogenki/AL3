#pragma once
#include "CameraController.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Math.h"
#include "Player.h"
#include "Skydome.h"
#include"Enemy.h"
#include"DeathParticles.h"
#include <vector>
using namespace KamataEngine;

// ゲームシーン
class GameScene {
public:
	// シーンのフェーズ
	enum class Phase {
		kFadeIn,  // フェードイン
		kPlay,    // ゲームプレイ
		kDeath,   // デス演出
		kFadeOut, // フェードアウト
	};

	//ポーズ画面
	enum class PauseState
	{
		Resume, //タイトルに戻る
		Retry, //リトライ
		Title,//再開
	};

	//シーンが終わったら何をするか(リトライとタイトルに戻る分岐)
	enum class ExitRequest
	{
		None, //何もなし(基本これ)
		Retry,//リトライしたら
		Title,//タイトルに戻る押したら
		Death,//死んだら
	};

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	~GameScene();

	// ブロック生成
	void GeneratedBlocks();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

	//すべての当たり判定を行う
	void CheckAllCollision();

	//ポーズからのリトライとタイトルに戻る分岐Getter
	ExitRequest GetExitRequest() const { return exitRequest_; }

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

	//enemy
	Model* enemyModel_ = nullptr;
	std::list<Enemy*> enemies_;

	// ブロック
	Model* blockModel_ = nullptr;
	// std::Vectorで可変個配列 それを2個使うことで2次元配列に
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	//デバック縦横線
	float gridSize_;
	int numRows_;
	int numCols_;
	PrimitiveDrawer* drawer_;

	//当たり判定
	float playerFootY;//playerの足
	bool enemyHeadY;  //enemyの頭
	bool isFalling;//落ちてるとき

	ExitRequest exitRequest_ = ExitRequest::None;

	// 天球
	Skydome* skydome_ = nullptr;
	Model* skyDomeModel_ = nullptr;

	// ポーズ画面
	uint32_t PauseFont_ = 0;
	Sprite* SpritePauseFont_ = nullptr;

	//ポーズ画面タイトルへ
	uint32_t PauseTitle_ = 0;
	Sprite* SpritePauseTitle_ = nullptr;
	//ポーズ画面リザルトへ
	uint32_t PauseResum_ = 0;
	Sprite* SpritePauseResum_ = nullptr;
	//ポーズ画面リトライへ
	uint32_t PauseRetry_ = 0;
	Sprite* SpritePauseRetry_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// デバックカメラ有効
	bool isDebugCameraActive_ = false;
	// デバックカメラの生成
	DebugCamera* debugCamera_ = nullptr;

	// サウンドデータハンドル(音データそのもの)
	uint32_t soundDataHandle_ = 0;
	// 音声再生ハンドル(実際に再生している音)
	uint32_t voiceHandle_ = 0;

	// ImGuiで値を入力する変数
	float inputFloat3[3] = {0, 0, 0};

	//デスパーティクル
	std::list<DeathParticles*> deathParticlesList_;
	Model* deathParticlesModel_ = nullptr;

	// 現在のフェーズ
	Phase phase_ = Phase::kPlay;

	//ポーズかどうか
	bool isPause_ = false;

	//ポーズ画面か
	PauseState currentPauseState_ = PauseState::Resume;

	//リスタートフラグ
	bool Restart_ = false;

	// 暗転フェード
	Fade* fade_ = nullptr;

	// カメラ移動
	CameraController* CController_ = nullptr;

	bool finished_ = false;
};