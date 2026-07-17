#pragma once

/// <summary>
/// シーン遷移類 > 基底クラス
/// 
/// シーン遷移の演出用基底クラス
/// </summary>
class ChangeScene {

protected:

	/// <summary>
	/// 処理終了フラグ
	/// </summary>
	bool isProcess_;

	/// <summary>
	/// シーン遷移フラグ
	/// </summary>
	bool isSceneChange_;

public:

	/// <summary>
	///
	/// </summary>
	/// <returns> 処理中フラグ </returns>
	bool GetIsProcess() const { return isProcess_; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns> シーン遷移フラグ </returns>
	bool GetIsSceneChange() const { return isSceneChange_; }

	/// <summary>
	/// 更新関数 純粋仮想
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画関数 純粋仮想
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ChangeScene();

	/// <summary>
	/// 仮想デストラクタ 空
	/// </summary>
	virtual ~ChangeScene() {};

};