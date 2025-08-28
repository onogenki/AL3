#pragma once
#include "Fade.h"
#include "skydome.h"
#include "KamataEngine.h"
using namespace KamataEngine;

// リザルトシーン
class ResultScene {
public:
	enum class Phase {
		kFadeIn,  // フェードイン
		kMain,    // メイン部
		kFadeOut, // フェードアウト
	};

	~ResultScene();

	void Initialize();

	void Update();

	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

	// プレイヤーが勝ったかどうかの結果を受け取る
	void SetPlayerWinner(bool isPlayerWinner);

private:
	static inline const float kTimeResultMove = 2.0f;

	// ビュープロジェクション
	Camera camera_;
	WorldTransform worldTransformResult_;

	WorldTransform worldTransformWin_;
	WorldTransform worldTransformLose_;

	WorldTransform worldTransformSpace_;

	// 0203天球
	Skydome* skydome_ = nullptr;
	Model* modelSkydome_ = nullptr;

	//ResultFont
	Model* modelResult_ = nullptr;
	
	//陣取り結果
	Model* modelWin_ = nullptr;
	Model* modelLose_ = nullptr;

	//spaceFont
	Model* modelSpace_ = nullptr;

	float counter_ = 0.0f;
	// 0212 終了フラグ
	bool finished_ = false;
	// 0213
	Fade* fade_ = nullptr;
	// 現在のフェーズ
	Phase phase_ = Phase::kFadeIn;

	bool isPlayerWinner_; // 結果を保存するメンバ変数
};