#include "TitleScene.h"
#include "GameScene.h"
#include "Math.h"

void TitleScene::Initialize() {

	// カメラ初期化
	camera_.Initialize();
	// スプライト画像読み込み
	textureHandleTitle_ = TextureManager::Load("2DTitle3D.png");
	textureHandleSpace_ = TextureManager::Load("space.png");

	// スプライト利用
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {100.0f, 50.0f});
	spriteSpace_ = Sprite::Create(textureHandleSpace_, {100.0f, 350.0f});

	// 天球
	skydome_ = new Skydome();
	// trueにすると反転描画になり、内側から見れる(天球用)
	skyDomeModel_ = Model::CreateFromOBJ("sky_under", true);
	skydome_->Initialize(skyDomeModel_, &camera_);
	// フェードを持ってくる
	fade_ = new Fade();
	fade_->Initialize();
	// 開幕時フェード時間はここで決める
	fade_->Start(Fade::Status::FadeIn, 1.0f,Fade::FadeType::White);
}

void TitleScene::Update() {

		fade_->Update();

	switch (phase_) {

		// フェードイン
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;

		// メイン
	case Phase::kMain:
		// タイトルの終了条件
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f,Fade::FadeType::Black);
			phase_ = Phase::kFadeOut;
		}
		break;

		// フェードアウト
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			isFinished_ = true;
		}
		break;
	}
}

void TitleScene::Draw() {

	// 3Dモデル

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	// 描画処理前処理
	Model::PreDraw(commandList);

	skydome_->Draw();

	// 描画処理後処理
	Model::PostDraw();

	// スプライト描画

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// スプライト描画処理前処理
	Sprite::PreDraw(dxCommon->GetCommandList());
	// ここにスプライトインスタンスの(2Dキャラ)描画処理を記述する

	spriteTitle_->Draw();
	spriteSpace_->Draw();

	fade_->Draw(); // 最前面に表示なので末尾
	// スプライト描画後処理
	Sprite::PostDraw();
}

TitleScene::~TitleScene() { 
	delete fade_;
	delete skyDomeModel_;
}