#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugText.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	// プレイヤー更新
	void PlayerUpdate();

	// ビーム更新
	void BeamUpdate();
	void BeamMove();
	void BeamBorn();

	// エネミー更新
	void EnemyUpdate();
	void EnemyMove();
	void EnemyBorn();

	// 当たり判定
	void Collision();            // 衝突判定
	void CollisionPlayerEnemy(); // 衝突判定（プレイヤーと敵）
	void CollisionBeamEnemy();   // 衝突判定（ビームと敵）

	void GamePlayUpdate();//ゲームプレイ更新
	void GamePlayDraw3D();//ゲームプレイ3D描画
	void GamePlayDraw2DBack();//ゲームプレイ2D背景描画
	void GamePlayDraw2DNear();//ゲームプレイ2D近景描画

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	
	// 背景
	uint32_t textureHandeleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_;

	//プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	// ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_;
	bool isBeamFlag = false;

	// エネミー
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_;
	bool isEnemyFlag = false; // 0:たってない,1:たってる

	// ゲームスコア
	int gameScore_ = 0;
	// プレイヤーライフ
	int playerLife_ = 3;
	//シーンモード
	int sceneMode_ = 0;
};
