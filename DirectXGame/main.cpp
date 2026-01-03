#include "GameScene.h"
#include "TitleScene.h"
#include <KamataEngine.h>
#include <Windows.h>

using namespace KamataEngine;

// Sceneはここに
TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

// シーン
enum class Scene {
	kUnKnown = 0,

	kTitle,
	kGame,
};

// 現在シーン
Scene scene = Scene::kUnKnown;

// シーン切り替え関数
void ChangeScene() {
	switch (scene) {

		// タイトル
	case Scene::kTitle:
		if (titleScene->IsFinished()) { // 次のシーンへ進むとき
			scene = Scene::kGame;       // シーン変更
			delete titleScene;          // 旧シーンの解放
			titleScene = nullptr;
			// 次のシーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;

		// ゲームシーン
	case Scene::kGame:
		if (gameScene->IsFinished()) { // 次のシーンへ進むとき
			switch (gameScene->GetExitRequest()) {
				//リトライするとき
			case GameScene::ExitRequest::Retry:
				delete gameScene;
				gameScene = new GameScene();
				gameScene->Initialize();
				scene = Scene::kGame;//再スタート
				break;
				//タイトルに戻るとき
			case GameScene::ExitRequest::Title:
				delete gameScene;
				gameScene = nullptr;
				titleScene = new TitleScene();
				titleScene->Initialize();
				scene = Scene::kTitle;//タイトルに戻る
				break;
				//死んだとき
				case GameScene::ExitRequest::Death:
				delete gameScene;
				gameScene = new GameScene();
				gameScene->Initialize();
				scene = Scene::kGame; //再スタート
				break;
			}
		}
		break;
	}
}

// 更新関数
void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

// 描画関数
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2C_10_オノ_ゲンキ_AL4");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// 最初のシーン
	scene = Scene::kTitle;

	// シーン初期化

	titleScene = new TitleScene();
	titleScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();

		// シーン切り替え
		ChangeScene();
		// 更新シーン
		UpdateScene();

		// imGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// 描画シーン
		DrawScene();

		// 軸表示の描画
		AxisIndicator::GetInstance()->Draw();

		// プリミティブ描画のリセット
		PrimitiveDrawer::GetInstance()->Reset();

		// imGui描画
		imguiManager->Draw();

		// 描画終了
		dxCommon->PostDraw();
	}

	// シーンの解放
	delete titleScene;
	delete gameScene;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}