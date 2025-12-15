#include "Fade.h"
#include <algorithm>

void Fade::Initialize()
{ 
	//暗転フェード
	BlackTextureHandle_ = TextureManager::Load("white1x1.png");
	BlackSprite_ = Sprite::Create(BlackTextureHandle_, {0.0f, 0.0f});
	BlackSprite_->SetSize(Vector2(WinApp::kWindowWidth, WinApp::kWindowHeight));
	BlackSprite_->SetColor(Vector4(0, 0, 0, 1));



}

void Fade::Update()
{

	//フェード状態
	// 持続時間はフェードクラスで値決める
	switch (status_) { 



	//何もしない
	case Status::None: 
		

		break;

		//フェードイン
		case Status::FadeIn:

			// 1フレーム分の秒数をカウントアップ
		    counter_ += 1.0f / 60.0f;
		    if (counter_ >= duration_) {
			    counter_ = duration_;
		    }
		    // 0.0fから1.0fの間で、経過時間がフェード持続時間に近づくほどアルファ値を大きくする
			BlackSprite_->SetColor(Vector4(0, 0, 0, std::clamp(1.0f - counter_ / duration_, 0.0f, 1.0f)));

			break;




		//フェードアウト
	    case Status::FadeOut:

			//1フレーム分の秒数をカウントアップ
		    counter_ += 1.0f / 60.0f;
			//フェード持続時間に達したら打ち止め
			if (counter_ >= duration_)
			{
			    counter_ = duration_;
			}
			//0.0fから1.0fの間で、経過時間がフェード持続時間に近づくほどアルファ値を大きくする
		    BlackSprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));

			break;
	}

}

void Fade::Draw()
{
	//先頭に置く
	if (status_ == Status::None) {
		return;
	}

	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());

	BlackSprite_->Draw();



	Sprite::PostDraw();
}

//フェード開始関数
void Fade::Start(Status status, float duration) 
{ 
	status_ = status; 
	duration_ = duration;//値はフェードするクラスで決める
	counter_ = 0.0f;
}
//フェード終了関数
void Fade::Stop()
{ 
	status_ = Status::None;
}

bool Fade::IsFinished()const {
	// フェード状態による分岐
	switch (status_) {

	// フェードイン
	case Status::FadeIn:

	// フェードアウト
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}
	return true;
}