#include <algorithm>
#include "Fade.h"

void Fade::Initialize() {
	//0213 スプライト生成
	sprite_ = Sprite::Create(0, Vector2{});

	sprite_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));
	sprite_->SetColor(Vector4(0, 0, 0, 1.f));
}

void Fade::Update() {

	//0213 フェード状態による分岐
	switch (status_) {
	case Status::None:
		//何もしない
		break;
	case Status::FadeIn:
		//フェードイン中の更新処理
		counter_ += 1.0f / 60.0f;// 1フレーム分の秒数をカウントアップ
		if (counter_ >= duration_) {// フェード継続時間に達したら打ち止め
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f)));

		break;
	case Status::FadeOut:
		//フェードアウト中の更新処理
		counter_ += 1.0f / 60.0f;// 1フレーム分の秒数をカウントアップ
		if (counter_ >= duration_) {// フェード継続時間に達したら打ち止め
			counter_ = duration_;
		}
		// 0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	}
}

void Fade::Draw() {

	//0213
	if (status_ == Status::None) {
		return;
	}

	//0213
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	sprite_->Draw();
	Sprite::PostDraw();
}

//0213 フェード開始
void Fade::Start(Status status, float duration) {

	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

//0213 フェード停止
void Fade::Stop() { status_ = Status::None; }

//0213 フェード終了判定
bool Fade::IsFinished() const {

	// フェード状態による分岐
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		// 1行バージョン 3項演算子   (return true; else return false;)
		return (counter_ >= duration_) ? true : false;
	}

	return true;
}
