#include "StageScene.h"
#include "Math.h"
#include <numbers>

void StageScene::Initialize()
{

	//ステージセレクト1
	stage1Model_ = Model::CreateFromOBJ("stageSelect1");
	worldTransformStage1_.Initialize();
	worldTransformStage1_.scale_ = {10.0f,10.0f,10.0f};

	//player
	playerModel_ = Model::CreateFromOBJ("needle_Body");
	worldTransformPlayer_.Initialize();
	worldTransformPlayer_.translation_.x = -50.0f;
	worldTransformPlayer_.translation_.z = 35.0f;
	worldTransformPlayer_.scale_ = {10.0f,10.0f,10.0f};
	worldTransformPlayer_.rotation_.y = 0.75f * std::numbers::pi_v<float>;

	//spaceフォント
	textureHandleSpace_ = TextureManager::Load("space.png");
	spriteSpace_ = Sprite::Create(textureHandleSpace_, {300.0f, 350.0f});
    // backSpaceフォント
	textureHandleBackSpace_ = TextureManager::Load("backSpace.png");
	spriteBackSpace_ = Sprite::Create(textureHandleBackSpace_, {880.0f, -120.0f});

	camera_.Initialize();

	//フェードを持ってくる
	fade_ = new Fade();
	fade_->Initialize();
	// 開幕時フェード時間はここで決める
	fade_->Start(Fade::Status::FadeIn, 1.0f, Fade::FadeType::Black);
	// 天球
	skydome_ = new Skydome();
	// trueにすると反転描画になり、内側から見れる(天球用)
	skyDomeModel_ = Model::CreateFromOBJ("sky_under", true);
	skydome_->Initialize(skyDomeModel_, &camera_);
	
	//リセット
	phase_ = Phase::kFadeIn;
	isFinished_ = false;

	//selectBGM
	selectBGM_ = Audio::GetInstance()->LoadWave("stageSelect.mp3");
	isSelectBGMPlaying_ = false;
}

void StageScene::Update()
{

	fade_->Update();
	switch (phase_)
	{//フェードイン 
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		if (!isSelectBGMPlaying_) {
			selectHandle_ = Audio::GetInstance()->PlayWave(selectBGM_, true);
			isSelectBGMPlaying_ = true;
		}
		break;

		// メイン
	case Phase::kMain:
		// タイトルの終了条件
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f, Fade::FadeType::Black);
			phase_ = Phase::kFadeOut;
			exitRequest_ = ExitRequest::Play;
		}
		// タイトルの終了条件
		if (Input::GetInstance()->PushKey(DIK_BACKSPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f, Fade::FadeType::White);
			phase_ = Phase::kFadeOut;
			exitRequest_ = ExitRequest::Title;
		}
		break;

		// フェードアウト
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			isFinished_ = true;
		}
		Audio::GetInstance()->StopWave(selectHandle_);
		isSelectBGMPlaying_ = false;
		break;
	}


	camera_.TransferMatrix();
	// アフィン変換～DirectXに転送（座標）
	WorldTransformUpdate(worldTransformStage1_);
	WorldTransformUpdate(worldTransformPlayer_);

}

void StageScene::Draw()
{ 
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	skydome_->Draw();
	stage1Model_->Draw(worldTransformStage1_, camera_);
	playerModel_->Draw(worldTransformPlayer_, camera_);

	// 3Dモデル描画後処理
	Model::PostDraw();

	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	spriteSpace_->Draw();
	spriteBackSpace_->Draw();
	fade_->Draw();
	Sprite::PostDraw();
}

StageScene::~StageScene() {
	delete playerModel_;
	delete stage1Model_;
	delete fade_;
	delete skyDomeModel_;
}
