#include <Novice.h>
#include <algorithm>
#include "Mymath.h"
#include "imgui.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
const char kWindowTitle[] = "LE2C_13_サノ_ハヤテ_タイトル";

//線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {

}

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,
	const Matrix4x4&viewProjectionMatrix,const Matrix4x4&viewportMatrix,uint32_t color) {

}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f; //Gridの半分の幅
	const uint32_t kSubdivision = 10;  //分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); //1つ分の長さ
	//奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + (xIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ x, 0.0f, -kGridHalfWidth };
		Vector3 end{ x,0.0f,kGridHalfWidth };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (x == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), color);
	}
	//左から右も同じように順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + (zIndex * kGridEvery);
		float color = float(0xAAAAAAFF);

		Vector3 start{ z,0.0f,-kGridHalfWidth };
		Vector3 end{ z,0.0f,kGridHalfWidth };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (z == 0.0f) {
			color = BLACK;
		}
		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), unsigned int(color));
	}
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	float windowWidth = 1280.0f;
	float windowHeight = 720.0f;

	Vector3 controlPoint[3] = {
		{-0.8f,0.58f,1.0f},
		{1.76f,1.0f,-0.3f},
		{0.94f,-0.7f,2.3f},
	};

	Vector3 cameraTranslate(0.0f, 1.9f, -6.49f);
	Vector3 cameraRotate(0.26f, 0.0f, 0.0f);
	Vector3 cameraScale(1.0f, 1.0f, 1.0f);
	Vector3 rotate{ 0.0f,0.0f,0.0f };
	Vector3 translate{ 0.0f,0.0f,0.0f };

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};
	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);
		/// ↓更新処理ここから
		ImGui::Begin("window");
		ImGui::SliderFloat3("CameraTranslate", &cameraTranslate.x,-7.0f,7.0f);
		ImGui::SliderFloat3("CameraRotate", &cameraRotate.x,-1.0f,1.0f);
		ImGui::End();


		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(windowWidth) / float(windowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(windowWidth), float(windowHeight), 0.0f, 1.0f);
	
		///
		/// ↑更新処理ここまで
		/// ↓描画処理ここから
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		///
		/// ↑描画処理ここまで
		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
