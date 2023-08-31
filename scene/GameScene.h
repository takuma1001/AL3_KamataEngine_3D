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

	/// <summary>
	/// プレイヤー更新
	/// </summary>
	void PlayerUpdate();

	/// <summary>
	/// ビームの更新
	/// </summary>
	void BeamUpdate();

	/// <summary>
	/// ビームの移動
	/// </summary>
	void BeamMove();

	/// <summary>
	/// ビーム発生
	/// </summary>
	void BeamBorn();

	/// <summary>
	/// 敵の更新
	/// </summary>
	void EnemyUpdate();

	/// <summary>
	/// 敵の移動
	/// </summary>
	void EnemyMove();

	/// <summary>
	/// 敵の生成
	/// </summary>
	void EnemyBorn();

	/// <summary>
	/// 衝突判定
	/// </summary>
	void Collision();

	/// <summary>
	/// 衝突判定(プレイヤーと敵)
	/// </summary>
	void CollisionPlayerEnemy();

	/// <summary>
	/// 衝突判定(ビームと敵)
	/// </summary>
	void CollisionBeamEnemy();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>

	// BG(スプライト)
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;

	// ビュープロジェクション（共通）
	ViewProjection viewProjection_;

	// ステージ
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = 0;
	WorldTransform worldTransformStage_[20];

	// プレイヤー
	uint32_t textureHandlePlayer_ = 0;
	Model* modelPlayer_ = nullptr;
	WorldTransform worldTransformPlayer_;

	// ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = {};
	WorldTransform worldTransformBeam_[10];
	bool isBeamFlag_[10] = {};

	int beamTimer_ = 0;

	// 敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_[10];
	int isEnemyFlag_[10] = {};
	float enemySpeed_[10] = {};     // 敵の速度
	float enemyJumpSpeed_[10] = {}; // 敵ジャンプの移動速度

	// タイトル(スプライト)
	uint32_t textureHandleTitle_ = 0;
	Sprite* spriteTitle_ = nullptr;

	// タイトル(エンター)
	uint32_t textureHandleEnter_ = 0;
	Sprite* spriteEnter_ = nullptr;

	// ゲームオーバー
	uint32_t textureHandleGameOver_ = 0;
	Sprite* spriteGameOver_ = nullptr;

	//操作方法
	uint32_t textureHandleControll_ = 0;
	Sprite* spriteControll_ = nullptr;


	// スコア表示(スプライト)
	uint32_t textureHandleNumber_ = 0;
	Sprite* spriteNumber_[5] = {};

	// ライフ
	uint32_t textureHandleLife_ = 0;
	Sprite* spriteLife_[3] = {};

	// すこあ
	uint32_t textureHandleScore_ = 0;
	Sprite* spriteScore_ = nullptr;

	//制限時間
	uint32_t textureHandleTimer_ = 0;
	Sprite* spriteTimer_ = nullptr;

	// デバッグテキスト
	DebugText* debugText_ = nullptr;

	int gameScore_ = 0;   // ゲームスコア
	int playerLife_ = 3;  // プレイヤーライフ
	int gameTimer_ = 0;   // タイマー
	int playerTimer_ = 0; // プレイヤータイマー
	int gameplayTimer_ = 1500;//ゲーム制限時間
	int collisionflag_ = 1;
	int collisionCount_ = 1;//なくす回数
	int colisionTimer_ = 200;//当たり判定消す



	//	サウンド
	uint32_t soundDataHandleTitleBGM_ = 0;    // タイトルBGM
	uint32_t soundDataHandleGamePlayBGM_ = 0; // ゲームプレイBGM
	uint32_t soundDataHandleGameOverBGM_ = 0; // ゲームオーバーBGM
	uint32_t soundDataHandleEnemyHitSE_ = 0;  // 敵ヒットSE
	uint32_t soundDataHandlePlayerHitSE_ = 0; // プレイヤーヒットSE
	uint32_t voiceHandleBGM_ = 0;             // 音声再生ハンドル

private:
	// ゲームプレイ
	void GamePlayUpdate();     // ゲームプレイ更新
	void GamePlayDraw3D();     // ゲームプレイ3D表示
	void GamePlayDraw2DBack(); // ゲームプレイ背景2D表示
	void GamePlayDraw2DNear(); // ゲームプレイ近景2D表示
	void GamePlayStart();      // ゲーム開始時

	// タイトル
	void TitleUpdate();     // タイトル画面の更新
	void TitleDraw2DNear(); // タイトル画面の2D表示

	//操作方法画面
	void ControllUpdate();
	void ControllDraw2DNear();

	// ゲームオーバー
	void GameOverUpdate();     // ゲームオーバー画面の更新
	void GameOverDraw2DNear(); // ゲームオーバー近景2D表示

	// ステージ
	void StageUpdate(); // ステージの更新

	// シーン遷移用
	int sceneMode_ = 1; // シーンモード(0:ゲームプレイ　1:タイトル)

	// 敵
	void EnemyJump(); // 敵の消滅の演出

	// スコア数値
	void DrawScore();
 //   //制限時間数値
	//void DrawTimer();
};
