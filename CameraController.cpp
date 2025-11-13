#include <algorithm>
#include "CameraController.h"
#include "math.h"
#include "Player.h"

void CameraController::Initialize(Camera *camera)
{ 
	camera_ = camera;
}

void CameraController::Update()
{
	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
	//行列を更新する
	camera_.UpdateMatrix();

}

void CameraController::Reset() 
{
	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	//追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ + targetOffset_;
}