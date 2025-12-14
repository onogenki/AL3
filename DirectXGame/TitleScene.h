#pragma once
class TitleScene
{
public:
	void Initialize();

	void Update();

	void Draw();


	// デスフラグのgetter
	bool IsFinished() const { return finished_; }

	private:

	//終了フラグ
	bool finished_ = false;

};
