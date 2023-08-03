#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"MathUtilityForText.h"
#include <time.h>
//コンストラクタ
GameScene::GameScene() {}
//デストラクタ
GameScene::~GameScene() {
	delete spriteBG_;   // BG
	delete modelStage_; // ステージ
	delete modelPlayer_;//プレイヤー
	delete modelBeam_;//ビーム
	delete modelEnemy_;//敵
	delete spriteTitle_;//タイトル
	delete spriteEnter_;//エンター
	delete spriteGameOver_;//ゲームオーバー
}
//初期化
void GameScene::Initialize() {
	
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// BG背景
	textureHandeleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandeleBG_, {0, 0});

	// ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	// ステージ
	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();

	// ステージの位置を変更
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	// 変換行列を更新
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_, worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformStage_.TransferMatrix();

	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	// ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransformBeam_.Initialize();

	// エネミー
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformEnemy_.Initialize();

	//タイトル
	textureHandleTitle_ = TextureManager::Load("title.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});

	//エンター
	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 500});

	// ゲームオーバー
	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {0, 0});

	// 乱数調整
	srand((unsigned int)time(NULL));

	// デバッグテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();
}
//全体の更新
void GameScene::GamePlayUpdate() {
	PlayerUpdate(), EnemyUpdate(), BeamUpdate(), Collision(), TitleUpdate(), GameOverUpdate();}
//更新
void GameScene::Update() { 
	//各シーンの更新処理の呼び起こし
	switch (sceneMode_) {
	case 0:
		GamePlayUpdate(); //ゲームプレイ更新
		break;
	case 1:
		TitleUpdate();//タイトル更新
		break;
	case 2:
		GameOverUpdate();//ゲームオーバー更新
		break;
	}
}
//プレイヤーの更新
void GameScene::PlayerUpdate() {
	// 移動

	// 右
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}

	// 左
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}

	// 移動範囲
	if (worldTransformPlayer_.translation_.x > 4) {
		worldTransformPlayer_.translation_.x = 4;
	}
	if (worldTransformPlayer_.translation_.x < -4) {
		worldTransformPlayer_.translation_.x = -4;
	}

	// 変換行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();
}
//ビームの更新
void GameScene::BeamUpdate() {

	BeamMove();
	BeamBorn();
	// 変換行列を更新
	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
	    worldTransformBeam_.scale_, worldTransformBeam_.rotation_,
	    worldTransformBeam_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformBeam_.TransferMatrix();
}
// ビーム移動
void GameScene::BeamMove() {
	// 移動
	if (isBeamFlag == true) {
		worldTransformBeam_.rotation_.x += 0.1f;
		worldTransformBeam_.translation_.z += 1.0f;

		if (worldTransformBeam_.translation_.z > 40.f) {
			isBeamFlag = false;
		}
	}
}
// 敵
void GameScene::EnemyUpdate() {
	EnemyMove();
	EnemyBorn();
	// 変換行列を更新
	worldTransformEnemy_.matWorld_ = MakeAffineMatrix(
	    worldTransformEnemy_.scale_, worldTransformEnemy_.rotation_,
	    worldTransformEnemy_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformEnemy_.TransferMatrix();
}
//敵の移動
void GameScene::EnemyMove() {
	if (isEnemyFlag == true) {
		worldTransformEnemy_.rotation_.x -= -0.15f;
		worldTransformEnemy_.translation_.z -= 0.5f;

		// 画面橋処理
		if (worldTransformEnemy_.translation_.z < -5.f) {
			isEnemyFlag = false;
		}
	}
}
//敵の行動w
void GameScene::EnemyBorn() {
	if (isEnemyFlag == false) {
		isEnemyFlag = true;

		// 乱数処理
		int x = rand() % 80;
		float x2 = (float)x / 10 + -4;

		worldTransformEnemy_.translation_.x = x2;
		worldTransformEnemy_.translation_.z = 40.f;
	}
}
// ビーム発射
void GameScene::BeamBorn() {
	if (input_->PushKey(DIK_SPACE)) {
		isBeamFlag = true;
		worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		worldTransformBeam_.translation_.y = worldTransformPlayer_.translation_.y;
	}
}
//プレイヤーと敵の当たり判定
void GameScene::CollisionPlayerEnemy() {
	// 敵が存在すれば
	if (isEnemyFlag == 1) {
		// 差を求める
		float dx = abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_.translation_.x);
		float dz = abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_.translation_.z);

		// 衝突したら
		if (dx < 1 && dz < 1) {
			playerLife_ -= 1;
			// 存在しない
			isEnemyFlag = 0;
		}
		//ライフがゼロになった時
		if (playerLife_ == 0) {
			sceneMode_ = 2;
		}
	}
}
//ビームと敵の当たり判定
void GameScene::CollisionBeamEnemy() {
	if (isEnemyFlag == 1) {
		float ex = abs(worldTransformBeam_.translation_.x - worldTransformEnemy_.translation_.x);
		float ez = abs(worldTransformBeam_.translation_.z - worldTransformEnemy_.translation_.z);

		// 衝突したら
		if (ex < 1 && ez < 1) {
			gameScore_ += 1;
			isEnemyFlag = 0;
		}
	}
}
//当たり判定
void GameScene::Collision() {
	// 衝突判定（プレイヤーと敵）
	CollisionPlayerEnemy();
	//ビームと敵
	CollisionBeamEnemy();
}
//タイトル更新
void GameScene::TitleUpdate() {
	//エンターキー押したとき
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイに変更
		sceneMode_ = 0;
		GamePlayStart();
	}
}
//タイトル2D前景描画
void GameScene::TitleDraw2DNear() {
	//ゲームタイマーを足す
	gameTimer_++;
	// タイトル表示
	spriteTitle_->Draw();
	//エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}
//ゲームオーバー更新
void GameScene::GameOverUpdate() {
	// エンターキー押したとき
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイに変更
		sceneMode_ = 1;
	}
}
//ゲームオーバー2D前景描画
void GameScene::GameOverDraw2DNear() {
	// ゲームタイマーを足す
	gameTimer_++;
	//ゲームオーバー表示
	spriteGameOver_->Draw();
	//エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

//ゲームリスタート
void GameScene::GamePlayStart() { 
	//ライフ初期化
	playerLife_ = 3;
	//スコア初期化
	gameScore_ = 0;
	//プレイヤー更新
	PlayerUpdate();
}

//3D描画
void GameScene::GamePlayDraw3D() {
	// ステージ
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);
	// プレイヤー
	modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	// ビーム
	if (isBeamFlag == true) {
		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}
	// エネミー
	if (isEnemyFlag == true) {
		modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	}
}
//2D背景描画
void GameScene::GamePlayDraw2DBack() {
	// 背景
	spriteBG_->Draw();
}
//2D近景描画
void GameScene::GamePlayDraw2DNear() {
	/// ゲームスコア
	char str[100];
	sprintf_s(str, "SCORE:%d", gameScore_);
	debugText_->Print(str, 200, 10, 2);
	// プレイヤーライフ
	sprintf_s(str, "LIFE:%d", playerLife_);
	debugText_->Print(str, 900, 10, 2);
}
//描画
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	//各シーンの背景2D表示を呼び出す
	switch (sceneMode_) { 
	case 0:
		GamePlayDraw2DBack();//ゲームプレイ2D背景表示
		break;
	case 2:
		GamePlayDraw2DBack(); // ゲームプレイ2D背景表示
		break;
	}
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion
	
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	switch (sceneMode_) { 
	case 0:
		GamePlayDraw3D();//ゲームプレイ3D表示
		break;
	case 2:
		GamePlayDraw3D(); // ゲームプレイ3D表示
		break;
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	switch (sceneMode_) { 
    case 0:
		GamePlayDraw2DNear();//ゲームプレイ2D表示
		break;
	case 1:
		TitleDraw2DNear();//タイトル2D表示
		break;
	case 2:
		GameOverDraw2DNear();//ゲームオーバー2D表示
		GamePlayDraw2DNear(); // ゲームプレイ2D表示
		break;
	}
	debugText_->DrawAll();
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
