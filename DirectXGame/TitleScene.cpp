#include "TitleScene.h"
#include "GameScene.h"
#include "Math.h"

void TitleScene::Initialize() {
	// スプライト画像読み込み
	textureHandleTitle_ = TextureManager::Load("2DTitle3D.png");
	textureHandleSpace_ = TextureManager::Load("space.png");

	// スプライト利用
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {100.0f, 50.0f});
	spriteSpace_ = Sprite::Create(textureHandleSpace_, {100.0f, 350.0f});

	// フェードを持ってくる
	fade_ = new Fade();
	fade_->Initialize();
	// フェード時間はここで決める
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

void TitleScene::Update() {

	switch (phase_) {

		// フェードイン
	case Phase::kFadeIn:
		fade_->Update();

		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;

		// メイン
	case Phase::kMain:
		// タイトルの終了条件
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;

		// フェードアウト
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
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

TitleScene::~TitleScene() { delete fade_; }