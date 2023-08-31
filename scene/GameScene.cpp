#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"MathUtilityForText.h"
#include <time.h>
// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete spriteBG_;// BG
	delete modelStage_;// ステージ
	delete modelPlayer_;// プレイヤー
	delete modelBeam_;// ビーム
	delete modelEnemy_;// 敵
	delete spriteTitle_;// タイトル
	delete spriteEnter_;// エンター
	delete spriteGameOver_;// ゲームオーバー
	delete spriteScore_;// スコア
	delete spriteTimer_;//制限時間
	delete spriteControll_;//操作方法

	for (int i = 0; i < 5; i++) {
		delete spriteNumber_[i]; // スコア数値
	}
	for (int i = 0; i < 3; i++) {
		delete spriteLife_[i]; // ライフ
	}
}

// 初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// BG(2Dスプライト)
	textureHandleBG_ = TextureManager::Load("bg.png");
	spriteBG_ = Sprite::Create(textureHandleBG_, {2,2});

	// ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	// ステージ
	textureHandleStage_ = TextureManager::Load("stage3.png");
	modelStage_ = Model::Create();
	// ステージの数ループする
	for (int i = 0; i < 20; i++) {
		worldTransformStage_[i].Initialize();
	}
	for (int i = 0; i < 20; i++) {
		// ステージの位置を変更
		worldTransformStage_[i].translation_ = {0, -1.5f, 2.0f * i - 5};
		worldTransformStage_[i].scale_ = {4.5f, 1, 1};

		// 変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}

	// プレイヤー
	textureHandlePlayer_ = TextureManager::Load("hayao.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f,0.5f};
	worldTransformPlayer_.Initialize();

	// ビーム
	textureHandleBeam_ = TextureManager::Load("ammo.png");
	modelBeam_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].scale_ = {0.3f, 0.3f, 0.3f};
		worldTransformBeam_[i].Initialize();
	}

	// 敵
	textureHandleEnemy_ = TextureManager::Load("Beetle.png");
	modelEnemy_ = Model::Create();
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformEnemy_[i].Initialize();
	}

	// タイトル(2Dスプライト)
	textureHandleTitle_ = TextureManager::Load("forest.png");
	spriteTitle_ = Sprite::Create(textureHandleTitle_, {0, 0});

	// タイトル(エンター)
	textureHandleEnter_ = TextureManager::Load("enter.png");
	spriteEnter_ = Sprite::Create(textureHandleEnter_, {400, 450});

	// ゲームオーバー
	textureHandleGameOver_ = TextureManager::Load("gameover.png");
	spriteGameOver_ = Sprite::Create(textureHandleGameOver_, {0, 100});

	//操作方法
	textureHandleControll_ = TextureManager::Load("controll.png");
	spriteControll_ = Sprite::Create(textureHandleControll_, {0, 0});

	// 乱数の初期化
	srand((unsigned int)time(NULL));

	// デバッグテキスト
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();

	// スコア数値(2Dスプライト)
	textureHandleNumber_ = TextureManager::Load("number.png");
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(textureHandleNumber_, {300.0f + i * 26, 0});
	}

	// ライフ(2Dスプライト)
	// textureHandleLife_=TextureManager::Load("player.png");
	for (int i = 0; i < playerLife_; i++) {
		spriteLife_[i] = Sprite::Create(textureHandlePlayer_, {800.0f + i * 60, 0});
		spriteLife_[i]->SetSize({40, 40});
	};

	// すこあ
	textureHandleScore_ = TextureManager::Load("score.png");
	spriteScore_ = Sprite::Create(textureHandleScore_, {160, 0});

	


	// サウンドデータの読み込み
	soundDataHandleTitleBGM_ = audio_->LoadWave("Audio/Ring05.wav");
	soundDataHandleGamePlayBGM_ = audio_->LoadWave("Audio/Ring08.wav");
	soundDataHandleGameOverBGM_ = audio_->LoadWave("Audio/Ring09.wav");
	soundDataHandleEnemyHitSE_ = audio_->LoadWave("Audio/chord.wav");
	soundDataHandlePlayerHitSE_ = audio_->LoadWave("Audio/tada.wav");

	// タイトルBGMを再生
	//voiceHandleBGM_ = audio_->PlayWave(soundDataHandleTitleBGM_, true);
}

// 更新
void GameScene::Update() {
	switch (sceneMode_) {
	case 0:
		GamePlayUpdate(); // ゲームプレイ更新
		break;

	case 1:
		TitleUpdate(); // タイトル更新
		gameTimer_ += 1;
		break;

	case 2:
		GameOverUpdate(); // ゲームオーバー画面更新
		gameTimer_ += 1;
		break;
	case 3:
		ControllUpdate();//操作方法
		break;
	}
}

void GameScene::PlayerUpdate() {
	// 変換行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();

	// 右へ移動
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	// 左へ移動
	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}

	if (collisionCount_ == 0&&colisionTimer_==0) {
		collisionflag_ = 1;
	}
	if (input_->PushKey(DIK_E)) {
		collisionCount_ = 0;
		collisionflag_ = 0;
	}
	


	worldTransformPlayer_.translation_.x = max(worldTransformPlayer_.translation_.x, -4);
	worldTransformPlayer_.translation_.x = min(worldTransformPlayer_.translation_.x, 4);

	if (playerTimer_ > 0) {
		playerTimer_ -= 1; // タイマーが0より大きければ1を引く
	}
}

// ビーム更新
void GameScene::BeamUpdate() {
	BeamMove();
	BeamBorn();

	// 変換行列を更新
	for (int i = 0; i < 10; i++) {
		worldTransformBeam_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformBeam_[i].scale_, worldTransformBeam_[i].rotation_,
		    worldTransformBeam_[i].translation_);

		// 変換行列を定数バッファに転送
		worldTransformBeam_[i].TransferMatrix();
	}
}

// ビーム移動
void GameScene::BeamMove() {
	// 奥へ移動,回転
	for (int i = 0; i < 10; i++) {
		if (isBeamFlag_[i] == false)
			continue;
		worldTransformBeam_[i].rotation_.x += 0.1f;
		worldTransformBeam_[i].translation_.z += 0.3f;

		// 画面外に行ったら
		if (worldTransformBeam_[i].translation_.z >= 40) {
			isBeamFlag_[i] = false;
			worldTransformBeam_[i].translation_.z = 0;
		}
	}
}

// ビーム発生(発射)
void GameScene::BeamBorn() {
	for (int i = 0; i < 10; i++) {
		if (isBeamFlag_[i] == true)
			continue;

		// 発射タイマーが0ならば
		if (beamTimer_ == 0) {

			if (input_->TriggerKey(DIK_SPACE)) {
				isBeamFlag_[i] = true;
				worldTransformBeam_[i].translation_.x = worldTransformPlayer_.translation_.x;
				worldTransformBeam_[i].translation_.z = worldTransformPlayer_.translation_.z;
				break;
			}
		} else {
			// 発射タイマーが1以上
			// 10を越えると再び発射可能
			beamTimer_++;
		}
		if (beamTimer_ > 10) {
			beamTimer_ = 0;
		}
	}
}

// 敵更新
void GameScene::EnemyUpdate() {
	EnemyMove();
	EnemyBorn();
	EnemyJump();

	// 変換行列を更新
	for (int i = 0; i < 10; i++) {
		worldTransformEnemy_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformEnemy_[i].scale_, worldTransformEnemy_[i].rotation_,
		    worldTransformEnemy_[i].translation_);

		// 変換行列を定数バッファに転送
		worldTransformEnemy_[i].TransferMatrix();
	}
}

// 敵移動
void GameScene::EnemyMove() {
	for (int i = 0; i < 10; i++) {
		// 敵の回転、斜め移動
		if (isEnemyFlag_[i] == 1) {
			worldTransformEnemy_[i].rotation_.x -= 0.15f;
			worldTransformEnemy_[i].translation_.z -= 0.5f;
			worldTransformEnemy_[i].translation_.x += enemySpeed_[i];

			if (worldTransformEnemy_[i].translation_.x > 4) {
				enemySpeed_[i] = -0.1f;
			} else if (worldTransformEnemy_[i].translation_.x < -4) {
				enemySpeed_[i] = 0.1f;
			}

			// タイマーにより速度を設定
			worldTransformEnemy_[i].translation_.z -= 0.1f;
			worldTransformEnemy_[i].translation_.z -= gameTimer_ / 1000.f;
		}

		// 画面端処理
		if (worldTransformEnemy_[i].translation_.z < -5)
			isEnemyFlag_[i] = 0;
	}
}

// 敵生成
void GameScene::EnemyBorn() {
	// 乱数で発生
	if (rand() % 10 == 0) {
		// 敵の数ループする
		for (int e = 0; e < 10; e++) {

			// 発生処理
			if (isEnemyFlag_[e] == 0) {
				isEnemyFlag_[e] = 1;

				// 乱数処理
				int x = rand() % 80;
				float x2 = (float)x / 10 - 4;

				worldTransformEnemy_[e].translation_.x = x2;
				worldTransformEnemy_[e].translation_.y = 0;
				worldTransformEnemy_[e].translation_.z = 40.f;

				// 敵のスピード
				if (rand() % 2 == 0) {
					enemySpeed_[e] = 0.1f;
				} else {
					enemySpeed_[e] = -0.1f;
				}

				// ループ終了
				break;
			}
		}
	}
}

// 衝突判定
void GameScene::Collision() {
	CollisionPlayerEnemy();
	CollisionBeamEnemy();
}


// 衝突判定(プレイヤーと敵)
void GameScene::CollisionPlayerEnemy() {
		// 敵が存在すれば
	if (collisionflag_ == 1) {

		for (int i = 0; i < 10; i++) {
			if (isEnemyFlag_[i] == 1) {
				// 差を求める
				float dx = abs(
				    worldTransformPlayer_.translation_.x - worldTransformEnemy_[i].translation_.x);
				float dz = abs(
				    worldTransformPlayer_.translation_.z - worldTransformEnemy_[i].translation_.z);

				// 衝突したら
				if (dx < 1 && dz < 1) {
					// 存在しない
					isEnemyFlag_[i] = 0;

					// ライフ減算
					playerLife_ -= 1;

					// プレイヤーヒットSE
					audio_->PlayWave(soundDataHandlePlayerHitSE_);

					// タイマーの値変動
					playerTimer_ = 60;
				}
			}
		}
	}
	if (collisionflag_ == 0) {
		colisionTimer_--;
	}
	if (colisionTimer_ == 0) {
		collisionflag_ = 1;
	}
}

void GameScene::CollisionBeamEnemy() {

	// 敵の数ループする
	for (int e = 0; e < 10; e++) {
		if (isEnemyFlag_[e] == 1) {
			// ビームの数ループする
			for (int b = 0; b < 10; b++) {
				// ビームが存在すれば
				if (isBeamFlag_[b] == true) {
					float dx =
					    abs(worldTransformBeam_[b].translation_.x -
					        worldTransformEnemy_[e].translation_.x);
					float dz =
					    abs(worldTransformBeam_[b].translation_.z -
					        worldTransformEnemy_[e].translation_.z);

					// 衝突したら
					if (dx < 1 && dz < 1) {
						// 存在しない
						isBeamFlag_[b] = false;
						isEnemyFlag_[e] = 2;

						// 敵のジャンプ
						enemyJumpSpeed_[e] = 1;

						gameScore_ += 100;

						// エネミーヒットSE
						audio_->PlayWave(soundDataHandleEnemyHitSE_);
					}
				}
			}
		}
	}
}

// 描画
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// 各シーンの背景2D表示を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DBack(); // ゲームプレイ2D表示
		break;
	case 2:
		GamePlayDraw2DBack(); // ゲームプレイ2D表示
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

	// 各シーンの背景3D表示を呼び出す
	switch (sceneMode_) {
	case 0:
		GamePlayDraw3D(); // ゲームプレイ3D表示
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
	switch (sceneMode_) {
	case 0:
		GamePlayDraw2DNear(); // ゲームプレイ2D近景表示
		break;
	case 1:
		TitleDraw2DNear(); // タイトル2D近景表示
		break;
	case 2:
		GameOverDraw2DNear(); // ゲームオーバー2D近景表示
		GamePlayDraw2DNear(); // ゲームプレイ2D表示
		break;
	case 3:
		ControllDraw2DNear();
	}
	debugText_->DrawAll();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

// ゲームプレイ更新
void GameScene::GamePlayUpdate() {
	gameplayTimer_--;
	
	if (playerLife_ <= 0||gameplayTimer_==0) {
		sceneMode_ = 2;
	}

	// 自機更新
	PlayerUpdate();

	// 弾更新
	BeamUpdate();

	// 敵更新
	EnemyUpdate();

	// 当たり判定
	Collision();

	// ステージ更新
	StageUpdate();
}

// ゲームプレイ表示3D
void GameScene::GamePlayDraw3D() {
	// ステージ
	for (int i = 0; i < 20; i++) {
		modelStage_->Draw(worldTransformStage_[i], viewProjection_, textureHandleStage_);
	}
	// プレイヤー
	if (playerTimer_ % 4 < 2) {
		modelPlayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);
	}

	// ビーム
	for (int i = 0; i < 10; i++) {
		if (isBeamFlag_[i] == true)
			modelBeam_->Draw(worldTransformBeam_[i], viewProjection_, textureHandleBeam_);
	}
	// 敵
	for (int i = 0; i < 10; i++) {
		if (isEnemyFlag_[i] != 0)
			modelEnemy_->Draw(worldTransformEnemy_[i], viewProjection_, textureHandleEnemy_);
	}
}

// ゲームプレイ表示2D背景
void GameScene::GamePlayDraw2DBack() {
	// 背景
	spriteBG_->Draw();
}

// ゲームプレイ近景2D近景
void GameScene::GamePlayDraw2DNear() {
	// score
	spriteScore_->Draw();
	GameScene::DrawScore();

	for (int i = 0; i < playerLife_; i++) {
		spriteLife_[i]->Draw();
	}
}

// タイトル更新
void GameScene::TitleUpdate() {
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイへ変更
		sceneMode_ = 3;

		// BGＭ切り替え
		audio_->StopWave(voiceHandleBGM_); // 現在のＢＧＭを停止
		voiceHandleBGM_ =
		    audio_->PlayWave(soundDataHandleGamePlayBGM_, true); // ゲームプレイBGMを再生
	}
}

// タイトル表示
void GameScene::TitleDraw2DNear() {
	spriteTitle_->Draw();

	// エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
	gameTimer_ += 1;
}

// 開始時
void GameScene::GamePlayStart() {
	playerLife_ = 3;
	sceneMode_ = 0;
	gameScore_ = 0;
	gameTimer_ = 0;
	playerTimer_ = 0;
	gameplayTimer_ = 1500;
	collisionflag_ = 1;
	collisionCount_ = 1;  // なくす回数
	colisionTimer_ = 200; // 当たり判定消す


	for (int i = 0; i < 10; i++) {
		isEnemyFlag_[i] = 0;
		worldTransformEnemy_[i].translation_.y = 0;
	}
}

// ゲームオーバー
void GameScene::GameOverUpdate() {
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイへ変更
		sceneMode_ = 1;

		// BGＭ切り替え
		audio_->StopWave(voiceHandleBGM_); // 現在のＢＧＭを停止
		voiceHandleBGM_ =
		    audio_->PlayWave(soundDataHandleTitleBGM_, true); // 遷移時にタイトルBGMを再生
	}
	gameTimer_ += 1;
}

//操作方法画面
void GameScene::ControllUpdate() {
	if (input_->TriggerKey(DIK_RETURN)) {
		// モードをゲームプレイへ変更
		GamePlayStart();

	for (int e = 0; e < 10; e++) {
			isEnemyFlag_[e] = 0;
		}
		for (int b = 0; b < 10; b++) {
			isBeamFlag_[b] = false;
		}

		worldTransformPlayer_.translation_.x = 0;

	}
}

void GameScene::ControllDraw2DNear() { 
	
	spriteControll_->Draw(); 
}

// ゲームオーバー近景
void GameScene::GameOverDraw2DNear() {
	spriteGameOver_->Draw();

	// エンター表示
	if (gameTimer_ % 40 >= 20) {
		spriteEnter_->Draw();
	}
}

// ステージ更新
void GameScene::StageUpdate() {
	// 各ステージでループ
	for (int i = 0; i < 20; i++) {
		// 手前に移動
		worldTransformStage_[i].translation_.z -= 0.1f;
		// 端まで来たら奥へ戻る
		if (worldTransformStage_[i].translation_.z < -5) {
			worldTransformStage_[i].translation_.z += 40;
		}
		// 変換行列を更新
		worldTransformStage_[i].matWorld_ = MakeAffineMatrix(
		    worldTransformStage_[i].scale_, worldTransformStage_[i].rotation_,
		    worldTransformStage_[i].translation_);
		// 変換行列を定数バッファに転送
		worldTransformStage_[i].TransferMatrix();
	}
}

// 敵ジャンプ
void GameScene::EnemyJump() {
	// 敵でループ
	for (int i = 0; i < 10; i++) {
		// 消滅演出ならば
		if (isEnemyFlag_[i] == 2) {
			// 移動(Y座標に速度を加える)
			worldTransformEnemy_[i].translation_.y += enemyJumpSpeed_[i];
			// 速度を減らす
			enemyJumpSpeed_[i] -= 0.1f;
			// 斜め移動
			worldTransformEnemy_[i].translation_.x += enemySpeed_[i] * 4;
			// 下へ落ちると消滅
			if (worldTransformEnemy_[i].translation_.y < -3) {
				isEnemyFlag_[i] = 0; // 存在しない
			}
		}
	}
}

// スコア数値の表示
void GameScene::DrawScore() {
	// 各桁の値を取り出す
	int eachNumber[5] = {};  // 各桁の値
	int number = gameScore_; // 表示する数字

	int keta = 10000; // 最初の桁
	for (int i = 0; i < 5; i++) {
		eachNumber[i] = number / keta; // 今の桁の値を求める
		number = number % keta;        // 次の桁以下の値を取り出す
		keta = keta / 10;              // 桁を進める
	}

	// 各桁の数値を描画
	for (int i = 0; i < 5; i++) {
		spriteNumber_[i]->SetSize({32, 64});
		spriteNumber_[i]->SetTextureRect({32.0f * eachNumber[i], 0}, {32, 64});
		spriteNumber_[i]->Draw();
	}
}
