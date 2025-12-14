#include "TitleScene.h"
#include "Math.h"

void TitleScene::Initialize() 
{

}

void TitleScene::Update() { 
	
	if (Input::GetInstance()->PushKey(DIK_SPACE))
	{
		finished_ = true;
	}

}

void TitleScene::Draw()
{

	DirectXCommon* dxCommon_ = DirectXCommon::GetInstance();
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	Model::PostDraw();

}

