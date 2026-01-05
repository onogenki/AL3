#define NOMINMAX
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

	const Vector3& targetVelocity = target_->GetVelocity();

	//追従対象とオフセットと " 追従対象の速度 " からカメラの " 目標座標 " を計算
	destination_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

	//座標空間によりゆったり追従(数学関数)
	camera_->translation_ = Lerp(camera_->translation_, destination_, kInterpolationRate);

	//追従対象が画面外に出ないように補正
	camera_->translation_.x = std::max(camera_->translation_.x, destination_.x + targetMargin.left);
	camera_->translation_.x = std::min(camera_->translation_.x, destination_.x + targetMargin.right);
	camera_->translation_.y = std::max(camera_->translation_.y, destination_.y + targetMargin.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, destination_.y + targetMargin.top);

	//移動範囲制限
	camera_->translation_.x = std::max(camera_->translation_.x, movableArea_.left);
	camera_->translation_.x = std::min(camera_->translation_.x, movableArea_.right);
	camera_->translation_.y = std::max(camera_->translation_.y, movableArea_.bottom);
	camera_->translation_.y = std::min(camera_->translation_.y, movableArea_.top);

	//行列を更新する
	camera_->UpdateMatrix();
}

void CameraController::Reset() 
{
	//追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	//追従対象とオフセットからカメラの目標座標を計算
	camera_->translation_ = targetWorldTransform.translation_ + targetOffset_;
}