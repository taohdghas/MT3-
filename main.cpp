#include <Novice.h>
#include <algorithm>
#include "Mymath.h"
#include "imgui.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
const char kWindowTitle[] = "LE2C_13_サノ_ハヤテ_タイトル";

struct Sphere {
	Vector3 center;
	float radius;
};

struct Pendulum {
	Vector3 anchor;
	float length;//紐の長さ
	float angle;
	float angularVelocity;//角速度
	float angularAcceleration;//角加速度
};

//線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	return v1 * (1 - t) + v2 * t;
}
Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t) {
	//制御点p0,p1を線形補間
	Vector3 p0p1 = Lerp(p0, p1, t);
	//制御点p1,p2を線形補間
	Vector3 p1p2 = Lerp(p1, p2, t);
	//制御点p0,p1,p1p2をさらに線形補間
	Vector3 p = Lerp(p0p1, p1p2, t);
	return p;
}
void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	for (int index = 0; index < 32; index++) {
		float t0 = index / 32.0f;
		float t1 = (index + 1) / 32.0f;

		Vector3 bezier0 = Bezier(controlPoint0, controlPoint1, controlPoint2, t0);
		Vector3 bezier1 = Bezier(controlPoint0, controlPoint1, controlPoint2, t1);

		Vector3 Screenbezier0 = Transform(Transform(bezier0, viewProjectionMatrix), viewportMatrix);
		Vector3 Screenbezier1 = Transform(Transform(bezier1, viewProjectionMatrix), viewportMatrix);
		Vector3 ScreenPoint0 = Transform(Transform(controlPoint0, viewProjectionMatrix), viewportMatrix);
		Vector3 ScreenPoint1 = Transform(Transform(controlPoint1, viewProjectionMatrix), viewportMatrix);
		Vector3 ScreenPoint2 = Transform(Transform(controlPoint2, viewProjectionMatrix), viewportMatrix);

		Novice::DrawLine(int(Screenbezier0.x), int(Screenbezier0.y), int(Screenbezier1.x), int(Screenbezier1.y), color);
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& ViewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const float pi = 3.14159f;
	const uint32_t kSubdivision = 20;//分割数
	const float kLonEvery = 2.0f * pi / kSubdivision;//経度分割1つ分の角度
	const float kLatEvery = pi / kSubdivision;//緯度分割1つ分の角度
	//緯度の方向に分割-π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;//現在の緯度
		//経度の方向に分割 θ ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;//現在の経度
			//world座標系でのa,b,cを求める
			Vector3 a = {
			sphere.center.x + sphere.radius * cosf(lat) * cosf(lon),
			sphere.center.y + sphere.radius * sin(lat),
			sphere.center.z + sphere.radius * cos(lat) * sin(lon)
			};
			Vector3 b = {
				sphere.center.x + sphere.radius * cos(lat) * cos(lon + kLonEvery),
				sphere.center.y + sphere.radius * sin(lat),
				sphere.center.z + sphere.radius * cos(lat) * sin(lon + kLonEvery)
			};
			Vector3 c = {
				sphere.center.x + sphere.radius * cos(lat + kLatEvery) * cos(lon),
				sphere.center.y + sphere.radius * sin(lat + kLatEvery),
				sphere.center.z + sphere.radius * cos(lat + kLatEvery) * sin(lon)
			};
			//a,b,cをScreen座標系まで変換
			Vector3 a_transformed = Transform(a, ViewProjectionMatrix);
			Vector3 b_transformed = Transform(b, ViewProjectionMatrix);
			Vector3 c_transformed = Transform(c, ViewProjectionMatrix);

			Vector3 a_screen = Transform(a_transformed, viewportMatrix);
			Vector3 b_screen = Transform(b_transformed, viewportMatrix);
			Vector3 c_screen = Transform(c_transformed, viewportMatrix);
			//ab,bcで線を引く
			Novice::DrawLine(int(a_screen.x), int(a_screen.y), int(b_screen.x), int(b_screen.y), color);
			Novice::DrawLine(int(a_screen.x), int(a_screen.y), int(c_screen.x), int(c_screen.y), color);
		}
	}
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

		Vector3 start{ kGridHalfWidth, 0.0f, z };
		Vector3 end{ -kGridHalfWidth, 0.0f, z };


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

	float deltaTime = 1.0f / 60.0f;

	bool IsStart = false;

	Pendulum pendulum;
	pendulum.anchor = { 0.0f,1.0f,0.0f };
	pendulum.length = 0.8f;
	pendulum.angle = 0.7f;
	pendulum.angularVelocity = 0.0f;
	pendulum.angularAcceleration = 0.0f;

	Vector3 cameraTranslate(0.0f, 1.9f, -6.49f);
	Vector3 cameraRotate(0.26f, 0.0f, 0.0f);
	Vector3 cameraScale(1.0f, 1.0f, 1.0f);
	Vector3 rotate{ 0.0f,0.0f,0.0f };
	Vector3 translate{ 0.0f,0.0f,0.0f };

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };
	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);
		/// ↓更新処理ここから
		ImGui::Begin("window");
		if (ImGui::Button("START")) {
			IsStart = true;
		}
		ImGui::SliderFloat3("CameraTranslate", &cameraTranslate.x, -7.0f, 7.0f);
		ImGui::SliderFloat3("CameraRotate", &cameraRotate.x, -1.0f, 1.0f);
		ImGui::End();

		pendulum.angularAcceleration =
			-(9.8f / pendulum.length) * std::sin(pendulum.angle);
		pendulum.angularVelocity += pendulum.angularAcceleration * deltaTime;
		pendulum.angle += pendulum.angularVelocity * deltaTime;

		

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(windowWidth) / float(windowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(windowWidth), float(windowHeight), 0.0f, 1.0f);

		/// ↑更新処理ここまで
		/// ↓描画処理ここから
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere, worldViewProjectionMatrix, viewportMatrix, WHITE);
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
