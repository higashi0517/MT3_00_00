#include <Novice.h>
//#include <math.h>
#include<assert.h>
#include <cmath>

const char kWindowTitle[] = "LE2C_23_ヒガシ_サチエ_00_05";

static const int kColumnWidth = 60;
static const int kRowHeight = 20;
static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;

typedef struct Matrix4x4 {

	float m[4][4];

} Matrix4x4;

typedef struct Vector3 {
	float x;
	float y;
	float z;
}Vector3;

// 単位行列の作成
Matrix4x4 MakeIdentity4x4() {

	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {

		for (int j = 0; j < 4; j++) {

			if (i == j) {

				result.m[i][j] = 1.0f;
			} else {
				result.m[i][j] = 0.0f;
			}
		}
	}
	return result;
}

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {

	Matrix4x4 result = MakeIdentity4x4();
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale) {

	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	return result;
}

// X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {

	Matrix4x4 result = MakeIdentity4x4();
	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);
	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);
	return result;
}

// Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {

	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);
	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);
	return result;
}

// Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {

	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);
	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);
	return result;
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {

	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {

		for (int j = 0; j < 4; j++) {

			result.m[i][j] = 0.0f;

			for (int k = 0; k < 4; k++) {

				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

// 逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {

	Matrix4x4 result{}; // 最後に返す逆行列
	float det = 0.0f;   // 行列式

	// --- 行列式を計算 ---
	{
		float sign = 1.0f;
		for (int i = 0; i < 4; i++) {
			// 小行列を作る
			float subm[3][3];
			int subi = 0;
			for (int row = 1; row < 4; row++) { // 1行目以外
				int subj = 0;
				for (int col = 0; col < 4; col++) {
					if (col == i) continue;
					subm[subi][subj] = m.m[row][col];
					subj++;
				}
				subi++;
			}

			// 小行列の行列式を求める
			float subdet =
				subm[0][0] * (subm[1][1] * subm[2][2] - subm[1][2] * subm[2][1]) -
				subm[0][1] * (subm[1][0] * subm[2][2] - subm[1][2] * subm[2][0]) +
				subm[0][2] * (subm[1][0] * subm[2][1] - subm[1][1] * subm[2][0]);
			// 交互に符号をつけて合計
			det += sign * m.m[0][i] * subdet;
			sign = -sign;
		}
	}

	// --- 行列式が0なら逆行列は存在しない ---
	if (det == 0.0f) {
		return result; // 全部0の行列を返す
	}

	// --- 余因子行列を作って、転置して、行列式で割る ---
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {

			// 小行列を作る
			float subm[3][3];
			int subi = 0;
			for (int i = 0; i < 4; i++) {
				if (i == row) continue; // row行目はスキップ
				int subj = 0;
				for (int j = 0; j < 4; j++) {
					if (j == col) continue; // col列目はスキップ
					subm[subi][subj] = m.m[i][j];
					subj++;
				}subi++;
			}

			// 小行列の行列式を求める
			float subdet =
				subm[0][0] * (subm[1][1] * subm[2][2] - subm[1][2] * subm[2][1]) -
				subm[0][1] * (subm[1][0] * subm[2][2] - subm[1][2] * subm[2][0]) +
				subm[0][2] * (subm[1][0] * subm[2][1] - subm[1][1] * subm[2][0]);

			// 符号を決める（チェス盤パターン）
			float sign = ((row + col) % 2 == 0) ? 1.0f : -1.0f;

			// 転置して代入（colとrowを逆にする）
			result.m[col][row] = (sign * subdet) / det;
		}
	}

	return result;
}

// 同次座標
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {

	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {

	Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

	Matrix4x4 result = Multiply(scaleMatrix, rotateXYZMatrix);
	result = Multiply(result, translateMatrix);
	return result;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspecRatio, float nearClip, float farClip) {

	Matrix4x4 result = MakeIdentity4x4();

	float f = 1.0f / std::tan(fovY * 0.5f);

	result.m[0][0] = f / aspecRatio;
	result.m[1][1] = f;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	result.m[3][3] = 0.0f;

	return result;
}

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {

	Matrix4x4 result = MakeIdentity4x4();

	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);

	return result;
}

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {

	Matrix4x4 result = MakeIdentity4x4();

	result.m[0][0] = width * 0.5f;
	result.m[1][1] = -height * 0.5f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + width * 0.5f;
	result.m[3][1] = top + height * 0.5f;
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;

	return result;
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {

	Novice::ScreenPrintf(x, y - kRowHeight, "%s", label);

	for (int row = 0; row < 4; ++row) {

		for (int column = 0; column < 4; ++column) {

			Novice::ScreenPrintf(
				x + column * kColumnWidth, y + row * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

void Vector3ScreenPrintf(int x, int y, const Vector3& vector, const char* label) {

	Novice::ScreenPrintf(x + kColumnWidth * 4, y, "%s", label);
	Novice::ScreenPrintf(x, y, "%6.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%6.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%6.02f", vector.z);
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 v1{ 1.2f,-3.9f, 2.5f };
	Vector3 v2{ 2.8f, 0.4f,-1.3f };
	Vector3 cross = Cross(v1, v2);
	Vector3 rotate{};
	Vector3 translate{};
	Vector3 cameraPosition{ 0.0f, 0.0f, -10.0f };
	// ローカル座標の頂点
	Vector3 kLocalVerticas[3] = {
		{ -1.0f, -1.0f, 0.0f }, // 頂点1
		{  1.0f, -1.0f, 0.0f }, // 頂点2
		{  0.0f,  1.0f, 0.0f }  // 頂点3
	};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// キー入力の処理
		// wキーで前に
		if (keys[DIK_W]) {

			translate.z += 0.1f;
		}
		// sキーで後ろに
		if (keys[DIK_S]) {

			translate.z -= 0.1f;
		}
		// aキーで左に
		if (keys[DIK_A]) {

			translate.x -= 0.1f;
		}
		// dキーで右に
		if (keys[DIK_D]) {

			translate.x += 0.1f;
		}

		// y軸の回転
		rotate.y += 0.1f;

		// 行列の計算
		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 1.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		Vector3 screenVertices[3];
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndcVertex = Transform(kLocalVerticas[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		Novice::DrawTriangle(
			int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y),
			int(screenVertices[2].x), int(screenVertices[2].y), RED,kFillModeSolid);

		///
		/// ↓描画処理ここから
		///



		///
		/// ↑描画処理ここまで
		///

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
