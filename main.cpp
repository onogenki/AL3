#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>
using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2C_06_オノ_ゲンキ_AL3");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// ゲームシーンのインスタンス生成
	GameScene* gameScene = new GameScene();
	// ゲームシーンの初期化
	gameScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}
		//imGui受付開始
		imguiManager->Begin();

		// ゲームシーンの更新
		gameScene->Update();

		//imGui受付終了
		imguiManager->End();

		// 描画処理
		dxCommon->PreDraw();

		// ゲームシーンの描画
		gameScene->Draw();

		//軸表示の描画
		AxisIndicator::GetInstance()->Draw();

		//プリミティブ描画のリセット
		PrimitiveDrawer::GetInstance()->Reset();

		//ImGui描画
		imguiManager->Draw();
		imguiManager->Draw();

		//描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete gameScene;
	// nullptrの代入
	//gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}