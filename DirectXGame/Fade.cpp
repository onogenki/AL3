#include "Fade.h"
#include <algorithm>

void Fade::Initialize() {
	// 暗転フェード
	TextureHandle_ = TextureManager::Load("white1x1.png");
	Sprite_ = Sprite::Create(TextureHandle_, {0.0f, 0.0f});
	Sprite_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));
	//Sprite_->SetColor(Vector4(0, 0, 0, 1));

	//暗転時の色
	switch (colorType_) {
	case FadeType::Black:
		fadeColor_ = {0, 0, 0, 1};
		break;
	case FadeType::White:
		fadeColor_ = {1, 1, 1, 1};
		break;
	}
	Sprite_->SetColor(fadeColor_);
}

void Fade::Update() {

	// フェード状態
	//  持続時間はフェードクラスで値決める
	switch (status_) {

	// 何もしない
	case Status::None:

		break;

	// フェードイン
	case Status::FadeIn:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード持続時間に近づくほどアルファ値を大きくする
		alphaIn_ = std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f);
		// 最後の値でフェードをするため
		Sprite_->SetColor(Vector4(fadeColor_.x, fadeColor_.y, fadeColor_.z, alphaIn_));

		break;


	// フェードアウト
	case Status::FadeOut:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード持続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		// 0.0fから1.0fの間で、経過時間がフェード持続時間に近づくほどアルファ値を大きくする
		alphaOut_ = std::clamp(counter_ / duration_, 0.0f, 1.0f);

		Sprite_->SetColor(Vector4(fadeColor_.x, fadeColor_.y, fadeColor_.z, alphaOut_));

		break;
	}
}

void Fade::Draw() {
	// 先頭に置く
	if (status_ == Status::None) {
		return;
	}

	//ポーズ画面のときにこれ書くとエラーが出る
	//Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	//Sprite::PostDraw();


	Sprite_->Draw();

}

// フェード開始関数
void Fade::Start(Status status, float duration,FadeType type) {
	status_ = status;
	duration_ = duration; // 値はフェードするクラスで決める
	counter_ = 0.0f;

	switch (type) {
	case FadeType::Black:
		fadeColor_ = {0, 0, 0, 1};
		break;
	case FadeType::White:
		fadeColor_ = {1, 1, 1, 1};
		break;
	}

}
// フェード終了関数
void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const { 
	return counter_ >= duration_; 
}