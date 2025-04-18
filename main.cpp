#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "LE2C_23_ヒガシ_サチエ_00_02";

static const int kColumnWidth = 60;
static const int kRowHeight = 20;

typedef struct Matrix4x4 {

	float m[4][4];

} Matrix4x4;

// 加算
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {

	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {

		for (int j = 0; j < 4; j++) {

			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

// 減算
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {

	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {

		for (int j = 0; j < 4; j++) {

			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
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

// 逆行列を求める関数
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
				}
				subi++;
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

// 転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {

	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {

		for (int j = 0; j < 4; j++) {

			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
}

// 単位行列の作成
Matrix4x4 MakeIdentity4x4x() {

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

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix) {

	for (int row = 0; row < 4; ++row) {

		for (int column = 0; column < 4; ++column) {

			Novice::ScreenPrintf(
				x + column * kColumnWidth, y + row * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Matrix4x4 m1 = {

		3.2f, 0.7f, 9.6f, 4.4f,
		5.5f, 1.3f, 7.8f, 2.1f,
		6.9f, 8.0f, 2.6f, 1.0f,
		0.5f, 7.2f, 5.1f, 3.3f
	};

	Matrix4x4 m2 = {

		4.1f, 6.5f, 3.3f, 2.2f,
		8.8f, 0.6f, 9.9f, 7.7f,
		1.1f, 5.5f, 6.6f, 0.0f,
		3.3f, 9.9f, 8.8f, 2.2f
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

		Matrix4x4 resultAdd = Add(m1, m2);
		Matrix4x4 resultMultiply = Multiply(m1, m2);
		Matrix4x4 resultSubtract = Subtract(m1, m2);
		Matrix4x4 inverseM1 = Inverse(m1);
		Matrix4x4 inverseM2 = Inverse(m2);
		Matrix4x4 itransposeM1 = Transpose(m1);
		Matrix4x4 itransposeM2 = Transpose(m2);
		Matrix4x4 identity = MakeIdentity4x4x();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::ScreenPrintf(0, 0, "Add");
		MatrixScreenPrintf(0, 20, resultAdd);

		Novice::ScreenPrintf(0, kRowHeight * 5, "Subtract");
		MatrixScreenPrintf(0, kRowHeight * 5 + 20, resultSubtract);

		Novice::ScreenPrintf(0, kRowHeight * 5 * 2, "Multiply");
		MatrixScreenPrintf(0, kRowHeight * 5 * 2 + 20, resultMultiply);

		Novice::ScreenPrintf(0, kRowHeight * 5 * 3, "InverseM1");
		MatrixScreenPrintf(0, kRowHeight * 5 * 3 + 20, inverseM1);

		Novice::ScreenPrintf(0, kRowHeight * 5 * 4, "InverseM2");
		MatrixScreenPrintf(0, kRowHeight * 5 * 4 + 20, inverseM2);

		Novice::ScreenPrintf(kColumnWidth * 5, 0, "TransposeM1");
		MatrixScreenPrintf(kColumnWidth * 5, 20, itransposeM1);

		Novice::ScreenPrintf(kColumnWidth * 5, kRowHeight * 5, "TransposeM2");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5 + 20, itransposeM2);

		Novice::ScreenPrintf(kColumnWidth * 5, kRowHeight * 5 * 2, "Identity");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5 * 2 + 20, identity);

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