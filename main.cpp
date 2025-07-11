#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include<assert.h>
#include <cmath>
#include <imgui.h>

const char kWindowTitle[] = "LE2C_23_ヒガシ_サチエ_02_03";

static const int kColumnWidth = 60;
static const int kRowHeight = 20;
static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;
const float pi = 3.1415926535f;

struct Matrix4x4 {

	float m[4][4];

};

struct Vector3 {

	float x;
	float y;
	float z;

};

struct Sphere {

	Vector3 center;
	float radius;

};

struct Segment {

	Vector3 origin;
	Vector3 diff;

};

struct Plane {

	Vector3 normal;
	float distance;
};

struct Triangle {
	Vector3 vertices[3];
};

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

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);
	const uint32_t kCenterIndex = kSubdivision / 2;

	// X方向の線（Z一定）
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		Vector3 start{ -kGridHalfWidth + kGridEvery * float(xIndex), 0.0f, -kGridHalfWidth };
		Vector3 end{ -kGridHalfWidth + kGridEvery * float(xIndex), 0.0f, kGridHalfWidth };

		uint32_t color = (xIndex == kCenterIndex) ? 0x000000FF : 0xAAAAAAFF;

		Vector3 screenStart = Transform(start, viewProjectionMatrix);
		Vector3 screenEnd = Transform(end, viewProjectionMatrix);
		screenStart = Transform(screenStart, viewportMatrix);
		screenEnd = Transform(screenEnd, viewportMatrix);

		Novice::DrawLine(
			int(screenStart.x), int(screenStart.y),
			int(screenEnd.x), int(screenEnd.y),
			color
		);
	}

	// Z方向の線（X一定）
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		Vector3 start{ -kGridHalfWidth, 0.0f, -kGridHalfWidth + kGridEvery * float(zIndex) };
		Vector3 end{ kGridHalfWidth, 0.0f, -kGridHalfWidth + kGridEvery * float(zIndex) };

		uint32_t color = (zIndex == kCenterIndex) ? 0x000000FF : 0xAAAAAAFF;

		Vector3 screenStart = Transform(start, viewProjectionMatrix);
		Vector3 screenEnd = Transform(end, viewProjectionMatrix);
		screenStart = Transform(screenStart, viewportMatrix);
		screenEnd = Transform(screenEnd, viewportMatrix);

		Novice::DrawLine(
			int(screenStart.x), int(screenStart.y),
			int(screenEnd.x), int(screenEnd.y),
			color
		);
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 20;
	const float kLonEvery = float(M_PI * 2.0f) / float(kSubdivision); // 経度の間隔
	const float kLatEvery = float(M_PI) / float(kSubdivision);       // 緯度の間隔

	// 緯度線（横方向）
	for (uint32_t latIndex = 1; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon1 = kLonEvery * lonIndex;
			float lon2 = kLonEvery * (lonIndex + 1);

			Vector3 p1{
				sphere.center.x + sphere.radius * cosf(lat) * cosf(lon1),
				sphere.center.y + sphere.radius * sinf(lat),
				sphere.center.z + sphere.radius * cosf(lat) * sinf(lon1)
			};
			Vector3 p2{
				sphere.center.x + sphere.radius * cosf(lat) * cosf(lon2),
				sphere.center.y + sphere.radius * sinf(lat),
				sphere.center.z + sphere.radius * cosf(lat) * sinf(lon2)
			};

			// スクリーン変換
			Vector3 sp1 = Transform(p1, viewProjectionMatrix);
			Vector3 sp2 = Transform(p2, viewProjectionMatrix);
			sp1 = Transform(sp1, viewportMatrix);
			sp2 = Transform(sp2, viewportMatrix);

			Novice::DrawLine((int)sp1.x, (int)sp1.y, (int)sp2.x, (int)sp2.y, color);
		}
	}

	// 経度線（縦方向）
	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
		float lon = kLonEvery * lonIndex;

		for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
			float lat1 = -float(M_PI) / 2.0f + kLatEvery * latIndex;
			float lat2 = lat1 + kLatEvery;

			Vector3 p1{
				sphere.center.x + sphere.radius * cosf(lat1) * cosf(lon),
				sphere.center.y + sphere.radius * sinf(lat1),
				sphere.center.z + sphere.radius * cosf(lat1) * sinf(lon)
			};
			Vector3 p2{
				sphere.center.x + sphere.radius * cosf(lat2) * cosf(lon),
				sphere.center.y + sphere.radius * sinf(lat2),
				sphere.center.z + sphere.radius * cosf(lat2) * sinf(lon)
			};

			// スクリーン変換
			Vector3 sp1 = Transform(p1, viewProjectionMatrix);
			Vector3 sp2 = Transform(p2, viewProjectionMatrix);
			sp1 = Transform(sp1, viewportMatrix);
			sp2 = Transform(sp2, viewportMatrix);

			Novice::DrawLine((int)sp1.x, (int)sp1.y, (int)sp2.x, (int)sp2.y, color);
		}
	}
}

// 正射影ベクトル
Vector3 Project(const Vector3& v1, const Vector3& v2) {

	float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; // 内積
	float lengthSquared = v2.x * v2.x + v2.y * v2.y + v2.z * v2.z;
	float scale = dot / lengthSquared;

	Vector3 result;
	result.x = v2.x * scale;
	result.y = v2.y * scale;
	result.z = v2.z * scale;
	return result;
}

Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {

	Vector3 segmentToPoint = { point.x - segment.origin.x, point.y - segment.origin.y, point.z - segment.origin.z };
	float t = (segmentToPoint.x * segment.diff.x + segmentToPoint.y * segment.diff.y + segmentToPoint.z * segment.diff.z) /
		(segment.diff.x * segment.diff.x + segment.diff.y * segment.diff.y + segment.diff.z * segment.diff.z);
	if (t < 0.0f) {
		return segment.origin;
	} else if (t > 1.0f) {
		return { segment.origin.x + segment.diff.x, segment.origin.y + segment.diff.y, segment.origin.z + segment.diff.z };
	} else {
		return { segment.origin.x + segment.diff.x * t, segment.origin.y + segment.diff.y * t, segment.origin.z + segment.diff.z * t };
	}
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return{ -vector.y, vector.x, 0.0f };
	}
	return{ 0.0f, -vector.z, vector.y };
}

Vector3 Multiply(float scalar, const Vector3& vector) {
	Vector3 result;
	result.x = scalar * vector.x;
	result.y = scalar * vector.y;
	result.z = scalar * vector.z;
	return result;
}

// Dot積を計算する関数
float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// スカラー掛け算演算子の定義
Vector3 operator*(const Vector3& v, float s) {
	return { v.x * s, v.y * s, v.z * s };
}
Vector3 operator*(float s, const Vector3& v) {
	return { v.x * s, v.y * s, v.z * s };
}
// 加算演算子
Vector3 operator+(const Vector3& a, const Vector3& b) {
	return { a.x + b.x, a.y + b.y, a.z + b.z };
}

// 正規化	
Vector3 Normalize(const Vector3& vector) {
	float length = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	if (length == 0.0f) {
		return { 0.0f, 0.0f, 0.0f };
	}
	return { vector.x / length, vector.y / length, vector.z / length };
}

// 平面を描画する関数
void DrawPlane(const Plane& plane, const Matrix4x4& vpm, const Matrix4x4& vm, uint32_t color) {

	Vector3 center = Multiply(plane.distance, plane.normal);
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y, -perpendiculars[0].z };
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z };

	Vector3 points[4];
	for (int32_t index = 0; index < 4; index++) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, vpm), vm);
	}

	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color);
}

bool IsCollision(const Triangle& triangle, const Segment& segment) {
	// 1. 三角形の頂点
	const Vector3& v0 = triangle.vertices[0];
	const Vector3& v1 = triangle.vertices[1];
	const Vector3& v2 = triangle.vertices[2];

	// 2. 三角形の法線
	Vector3 n = Cross(Subtract(v1, v0), Subtract(v2, v0));

	// 3. 線分のパラメータ表現
	Vector3 dir = Subtract(segment.diff, segment.origin); // 線分方向ベクトル
	float denom = Dot(n, dir);

	// 平行判定
	if (fabs(denom) < 1e-6f) return false;

	// 線分の始点から三角形の平面までの距離
	float t = Dot(n, Subtract(v0, segment.origin)) / denom;

	// tが0～1でなければ線分と交差しない
	if (t < 0.0f || t > 1.0f) return false;

	// 交点
	Vector3 p = segment.origin + dir * t;

	// ここで、あなたの点が三角形の中か判定ロジックを使う！
	// --- ここから下はあなたのロジックと同じでOK ---
	Vector3 v01 = Subtract(v1, v0);
	Vector3 v12 = Subtract(v2, v1);
	Vector3 v20 = Subtract(v0, v2);

	Vector3 v0p = Subtract(p, v0);
	Vector3 v1p = Subtract(p, v1);
	Vector3 v2p = Subtract(p, v2);

	Vector3 c0 = Cross(v01, v0p);
	Vector3 c1 = Cross(v12, v1p);
	Vector3 c2 = Cross(v20, v2p);

	if (Dot(c0, n) >= 0 && Dot(c1, n) >= 0 && Dot(c2, n) >= 0)
		return true;
	return false;
}

void DrawTriangle(const Triangle& triangle,const Matrix4x4& viewProjectionMatrix,const Matrix4x4& viewportMatrix, uint32_t color) {
	// 三角形の頂点をスクリーン座標に変換
	Vector3 screenVertices[3];
	for (int i = 0; i < 3; ++i) {
		screenVertices[i] = Transform(triangle.vertices[i], viewProjectionMatrix);
		screenVertices[i] = Transform(screenVertices[i], viewportMatrix);
	}
	// 三角形の辺を描画
	for (int i = 0; i < 3; ++i) {
		int nextIndex = (i + 1) % 3;
		Novice::DrawLine(
			int(screenVertices[i].x), int(screenVertices[i].y),
			int(screenVertices[nextIndex].x), int(screenVertices[nextIndex].y),
			color
		);
	}
}

Matrix4x4 MakeLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up) {
	Vector3 zaxis = Normalize(Subtract(target, eye)); // 視線ベクトル
	Vector3 xaxis = Normalize(Cross(up, zaxis));      // 右方向
	Vector3 yaxis = Cross(zaxis, xaxis);              // 上方向

	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = xaxis.x;
	result.m[1][0] = xaxis.y;
	result.m[2][0] = xaxis.z;
	result.m[0][1] = yaxis.x;
	result.m[1][1] = yaxis.y;
	result.m[2][1] = yaxis.z;
	result.m[0][2] = zaxis.x;
	result.m[1][2] = zaxis.y;
	result.m[2][2] = zaxis.z;
	result.m[3][0] = -Dot(xaxis, eye);
	result.m[3][1] = -Dot(yaxis, eye);
	result.m[3][2] = -Dot(zaxis, eye);
	return result;
}


// Windowsアプリでのエントリーイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 cameraTranslate{ 0.0f,3.0f,-10.0f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };
	Vector3 target{ 0.0f, 0.0f, 0.0f };
	Vector3 up{ 0.0f, 1.0f, 0.0f };
	Segment segment = { {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f} };
	Triangle triangle = { {{-1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}} };


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

		Matrix4x4 cameraMatrix = MakeAffineMatrix(
			{ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		//Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 viewMatrix = MakeLookAtMatrix(cameraTranslate, target, up);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(
			0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(
			0.0f, 0.0f, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		


		// 線分のスクリーン座標変換
		Vector3 start = Transform(segment.origin, viewProjectionMatrix);
		start = Transform(start, viewportMatrix);

		Vector3 end = Add(segment.origin, segment.diff);
		end = Transform(end, viewProjectionMatrix);
		end = Transform(end, viewportMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// ImGui
		ImGui::Begin("Window");
		ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("triangle.v0", &triangle.vertices[0].x, 0.01f);
		ImGui::DragFloat3("triangle.v1", &triangle.vertices[1].x, 0.01f);
		ImGui::DragFloat3("triangle.v2", &triangle.vertices[2].x, 0.01f);
		ImGui::DragFloat3("segment.origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("segment.diff", &segment.diff.x, 0.01f);
		ImGui::End();

		// 描画
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		if(IsCollision(triangle, segment)) {
			Novice::DrawLine(
				int(start.x), int(start.y),
				int(end.x), int(end.y),
				RED);
			DrawTriangle(triangle, viewProjectionMatrix, viewportMatrix, WHITE);
		} else {
			Novice::DrawLine(
				int(start.x), int(start.y),
				int(end.x), int(end.y),
				WHITE);
			DrawTriangle(triangle, viewProjectionMatrix, viewportMatrix, WHITE);
		}

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
