#include "GameScene.h"
#include "KamataEngine.h"
#include"TitleScene.h"
#include"ResultScene.h"
#include"RuleScene.h"
#include <Windows.h>
using namespace KamataEngine;

//0212
TitleScene* titleScene = nullptr;
RuleScene* ruleScene = nullptr;
GameScene* gameScene = nullptr;
ResultScene* resultScene = nullptr;

//シーン(型)
enum class Scene {
	kUnknown = 0, kTitle,kRule, kGame,kResult
};

//現在シーン(型)
Scene scene = Scene::kUnknown;

//シーン切り替え処理
void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			//シーン変更
			scene = Scene::kRule;
			//旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			//新シーンの生成と初期化
			ruleScene = new RuleScene;
			ruleScene->Initialize();
		}
		break;
	case Scene::kRule:
		if (ruleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			delete ruleScene;
			ruleScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->isFinished()) {
			gameScene->CountItems();
			// ゲーム終了時に勝敗を判定
			bool isPlayerWinner = gameScene->IsPlayerWinner();
			delete gameScene;
			gameScene = nullptr;
			resultScene = new ResultScene;
			// ResultSceneに勝敗情報を渡す
			resultScene->SetPlayerWinner(isPlayerWinner);
			resultScene->Initialize();
			// シーン変更
			scene = Scene::kResult;
		}
		break;
	case Scene::kResult:
		if (resultScene->IsFinished())
		{
			scene = Scene::kTitle;
			delete resultScene;
			resultScene = nullptr;
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}
//毎フレームの現在シーンを更新
void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kRule:
		ruleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kResult:
		resultScene->Update();
		break;
	}
}
//毎フレームの現在シーンを描画
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kRule:
		ruleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kResult:
		resultScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"LE2C_06_オノ_ゲンキ_AL3");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// ゲームシーンのインスタンス生成
	//GameScene* gameScene = new GameScene();
	// ゲームシーンの初期化
	//gameScene->Initialize();

	//最初のシーンの初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();


	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}
		//imGui受付開始
		imguiManager->Begin();

		// ゲームシーンの更新
		//gameScene->Update();
		
		//シーン切り替え
		ChangeScene();
		//現在シーン更新
		UpdateScene();
		
		//imGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		//gameScene->Draw();
		
		//現在シーンの描画
		DrawScene();

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
	delete titleScene;
	delete ruleScene;
	delete gameScene;
	delete resultScene;
	// nullptrの代入
	//gameScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}