/// <summary>
/// デバッグ文字列画面表示クラス
/// 
/// 開発用の情報を画面に表示するためのクラス
/// ASCIIコードのみ対応
/// </summary>
#pragma once

#include <wrl/client.h>
#include <SpriteFont.h>
#include <SimpleMath.h>
#include <vector>

namespace MyLibrary
{
	class DebugText
	{
	public:
		// テキスト1個分のデータ
		struct Text
		{
			// 表示位置
			DirectX::XMFLOAT2	pos;
			// 表示文字列
			std::wstring	str;
		};

		// コンストラクタ
		DebugText(ID3D11Device*	d3dDevice, ID3D11DeviceContext* d3dContext);
		// デストラクタ
		virtual ~DebugText();
		// 描画
		// ※必ずSpriteBatchのBegin()とEnd()の間で呼び出すこと。
		void Draw();
		// テキスト追加（フォーマット指定付き）
		void AddText(const DirectX::XMFLOAT2& position, LPCWSTR format, ...);
	protected:
		// スプライトバッチ
		std::unique_ptr<DirectX::SpriteBatch>	spriteBatch;
		// スプライトフォント
		std::unique_ptr<DirectX::SpriteFont>	spriteFont;
		// 文字列の配列
		std::vector<DebugText::Text> textArray;
	};
}
