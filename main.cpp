#include <Novice.h>
#include <algorithm>
#include "Mymath.h"
#include "imgui.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
const char kWindowTitle[] = "LE2C_13_サノ_ハヤテ_タイトル";

struct AABB {
	Vector3 min;//最小点
	Vector3 max;//最大点
};
struct Sphere {
	Vector3 center;
	float radius;
};
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
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	const uint32_t kSubdivision = 10;
	const float kLonEvery = float(2.0f * M_PI) / kSubdivision;
	const float kLatEvery = float(M_PI) / kSubdivision;

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = float(-M_PI) / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			Vector3 a, b, c;

			a = { sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon), sphere.center.y + sphere.radius * std::sin(lat), sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon) };
			b = { sphere.center.x + sphere.radius * std::cos(lat + kLatEvery) * std::cos(lon), sphere.center.y + sphere.radius * std::sin(lat + kLatEvery), sphere.center.z + sphere.radius * std::cos(lat + kLatEvery) * std::sin(lon) };
			c = { sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon + kLonEvery), sphere.center.y + sphere.radius * std::sin(lat), sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon + kLonEvery) };

			a = Transform(a, viewProjectionMatrix);
			a = Transform(a, viewportMatrix);

			b = Transform(b, viewProjectionMatrix);
			b = Transform(b, viewportMatrix);

			c = Transform(c, viewProjectionMatrix);
			c = Transform(c, viewportMatrix);

			Novice::DrawLine(int(a.x), int(a.y), int(b.x), int(b.y), color);
			Novice::DrawLine(int(a.x), int(a.y), int(c.x), int(c.y), color);

		}
	}

}
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatirx, uint32_t color) {
	Vector3 vertices[8] = {
		{aabb.min.x, aabb.min.y, aabb.min.z},
		{aabb.max.x, aabb.min.y, aabb.min.z},
		{aabb.min.x, aabb.max.y, aabb.min.z},
		{aabb.max.x, aabb.max.y, aabb.min.z}, 
		{aabb.min.x, aabb.min.y, aabb.max.z},
		{aabb.max.x, aabb.min.y, aabb.max.z},
		{aabb.min.x, aabb.max.y, aabb.max.z},
		{aabb.max.x, aabb.max.y, aabb.max.z}
	};
	Vector3 Screen[8];
	for (int i = 0; i < 8; i++) {
		Screen[i] = Transform(Transform(vertices[i], viewProjectionMatrix), viewportMatirx);
	}


	Novice::DrawLine(int(Screen[0].x), int(Screen[0].y), int(Screen[1].x), int(Screen[1].y), color);
	Novice::DrawLine(int(Screen[0].x), int(Screen[0].y), int(Screen[2].x), int(Screen[2].y), color);
	Novice::DrawLine(int(Screen[0].x), int(Screen[0].y), int(Screen[4].x), int(Screen[4].y), color);
	Novice::DrawLine(int(Screen[1].x), int(Screen[1].y), int(Screen[3].x), int(Screen[3].y), color);
	Novice::DrawLine(int(Screen[1].x), int(Screen[1].y), int(Screen[5].x), int(Screen[5].y), color);
	Novice::DrawLine(int(Screen[2].x), int(Screen[2].y), int(Screen[3].x), int(Screen[3].y), color);
	Novice::DrawLine(int(Screen[2].x), int(Screen[2].y), int(Screen[6].x), int(Screen[6].y), color);
	Novice::DrawLine(int(Screen[3].x), int(Screen[3].y), int(Screen[7].x), int(Screen[7].y), color);
	Novice::DrawLine(int(Screen[4].x), int(Screen[4].y), int(Screen[5].x), int(Screen[5].y), color);
	Novice::DrawLine(int(Screen[4].x), int(Screen[4].y), int(Screen[6].x), int(Screen[6].y), color);
	Novice::DrawLine(int(Screen[5].x), int(Screen[5].y), int(Screen[7].x), int(Screen[7].y), color);
	Novice::DrawLine(int(Screen[6].x), int(Screen[6].y), int(Screen[7].x), int(Screen[7].y), color);
}
bool IsCollision(const AABB& aabb,const Sphere&sphere) {
	//最近接点を求める
	Vector3 closestPoint{ std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
    std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
	std::clamp(sphere.center.z,aabb.min.z,aabb.max.z) };
	//最近接点と弾の中心との距離を求める
	float distance = Length();
}
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);
	int color = WHITE;
	float windowWidth = 1280.0f;
	float windowHeight = 720.0f;

	AABB aabb1{
		.min{-0.5f,-0.5f,-0.5f},
		.max{0.0f,0.0f,0.0f},
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
		ImGui::SliderFloat3("box1 min", &aabb1.min.x, -3.0f,3.0f);
		ImGui::SliderFloat3("box1 max", &aabb1.max.x,-3.0f,3.0f);
		ImGui::End();

		aabb1.min.x = (std::min)(aabb1.min.x, aabb1.max.x);
		aabb1.max.x = (std::max)(aabb1.min.x, aabb1.max.x);
		aabb1.min.y = (std::min)(aabb1.min.y, aabb1.max.y);
		aabb1.max.y = (std::max)(aabb1.min.y, aabb1.max.y);
		aabb1.min.z = (std::min)(aabb1.min.z, aabb1.max.z);
		aabb1.max.z = (std::max)(aabb1.min.z, aabb1.max.z);


		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(windowWidth) / float(windowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(windowWidth), float(windowHeight), 0.0f, 1.0f);
	
		///
		/// ↑更新処理ここまで
		/// ↓描画処理ここから
		DrawAABB(aabb1, worldViewProjectionMatrix, viewportMatrix, color);
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
