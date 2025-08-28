#include "Math.h"
#include "ResultScene.h"
#include <numbers>

void ResultScene::SetPlayerWinner(bool isPlayerWinner) { isPlayerWinner_ = isPlayerWinner; }


void ResultScene::Initialize() {

	modelResult_ = Model::CreateFromOBJ("resultFont", true);
	modelSpace_ = Model::CreateFromOBJ("spaceFont", true);

	//勝利敗北テキスト
	modelWin_ = Model::CreateFromOBJ("GameClearFont", true);
	modelLose_ = Model::CreateFromOBJ("GameOverFont", true);

	// カメラ初期化
	camera_.Initialize();

	const float kPlayerTitle = 2.0f;

	//resultFont
	worldTransformResult_.Initialize();

	worldTransformResult_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};


	// resultWin
	worldTransformWin_.Initialize();

	worldTransformWin_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};

	// resultLose
	worldTransformLose_.Initialize();

	worldTransformLose_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};



	//spaceFont
	worldTransformSpace_.Initialize();

	worldTransformSpace_.translation_.x = -2.0f;

	worldTransformSpace_.translation_.y = -19.0f;

	worldTransformSpace_.scale_ = {kPlayerTitle, kPlayerTitle, kPlayerTitle};


	// 0203天球
	// skydome生成
	skydome_ = new Skydome();
	// 初期化
	modelSkydome_ = Model::CreateFromOBJ("sky_sphere", true);
	skydome_->Initialize(modelSkydome_, &camera_);

	// 0213
	fade_ = new Fade();
	fade_->Initialize();

	fade_->Start(Fade::Status::FadeIn, 2.0f);
}

void ResultScene::Update() {

	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();

		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		// タイトルシーンの終了条件
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	counter_ += 1.0f / 60.0f;
	counter_ = std::fmod(counter_, kTimeResultMove);

	float angle = counter_ / kTimeResultMove * 2.0f * std::numbers::pi_v<float>;

	worldTransformResult_.translation_.y = std::sin(angle) + 10.0f;

	camera_.TransferMatrix();

	// アフィン変換～DirectXに転送(タイトル座標)
	WorldTransformUpdate(worldTransformResult_);

	WorldTransformUpdate(worldTransformWin_);
	WorldTransformUpdate(worldTransformLose_);

	// アフィン変換～DirectXに転送(タイトル座標)
	WorldTransformUpdate(worldTransformSpace_);

}

void ResultScene::Draw() {

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	// 天球描画
	skydome_->Draw();

	modelResult_->Draw(worldTransformResult_, camera_);

	modelSpace_->Draw(worldTransformSpace_, camera_);

	if (isPlayerWinner_) {
		// プレイヤーが勝者なら
		modelWin_->Draw(worldTransformWin_, camera_);
	} else {
		// プレイヤーが敗者なら
		modelLose_->Draw(worldTransformLose_, camera_);
	}

	Model::PostDraw();

	// 0213
	fade_->Draw();
}

ResultScene::~ResultScene() {
	delete modelResult_;
	delete modelWin_;
	delete modelLose_;
	delete modelSpace_;
	delete modelSkydome_;
	// 0213
	delete fade_;
}