#include "Sprite.h"

using namespace DirectX;
using namespace MyLibrary;

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="manager">スプライトレンダラー</param>
Sprite::Sprite()
{
	m_Rotation = 0.0f;
	m_Scale = Vector2(1, 1);
	m_Color = Colors::White;
	m_Texture = nullptr;
	m_TextureRect = RECT{0,0,0,0};
	m_AnchorPoint = Vector2(0.5f, 0.5f);
	m_Parent = nullptr;
	m_SpriteManager = nullptr;
	m_IsDirty = true;
}

void Sprite::Update(bool recursive)
{
	// ダーティでなければ更新不要
	if (!CheckDirty())	return;

	Matrix scalem;
	Matrix rotm;
	Matrix transm;

	scalem = Matrix::CreateScale(Vector3(m_Scale.x, m_Scale.y, 1.0f));

	rotm = Matrix::CreateRotationZ(XMConvertToRadians(m_Rotation));

	transm = Matrix::CreateTranslation(Vector3(m_Position.x, m_Position.y, 0));

	// ワールド行列をSRTの順に合成
	m_LocalWorld = Matrix::Identity;
	m_LocalWorld *= scalem;
	m_LocalWorld *= rotm;
	m_LocalWorld *= transm;

	// 親行列があれば
	if (m_Parent)
	{
		// 親行列を掛ける
		m_LocalWorld = m_LocalWorld * m_Parent->GetLocalWorld();
	}

	// 更新したのでダーティフラグを下す
	m_IsDirty = false;

	if (recursive)
	{
		// 階層を辿って全ての子オブジェクトを更新
		if (m_Children.size() != 0)
		{
			for (Sprite*& child : m_Children)
			{
				child->Update();
			}
		}
	}
}

void Sprite::Draw(bool recursive)
{
	const float EPSILON = 1.0e-3f;

	Update(recursive);

	ID3D11ShaderResourceView*	shaderResourceView = nullptr;

	Vector2 origin(0, 0);

	if (m_Texture)
	{
		shaderResourceView = m_Texture->shaderResourceView.Get();

		// テクスチャサイズから回転中心を計算
		origin = m_AnchorPoint;
		origin.x *= (m_TextureRect.right - m_TextureRect.left);
		origin.y *= (m_TextureRect.bottom - m_TextureRect.top);
	}

	float rotation = 0.0f;

	// 座標を行列から抜き出す
	Vector2 position;
	position.x = m_LocalWorld._41;
	position.y = m_LocalWorld._42;

	// スケーリングを行列から計算
	Vector2 scale;
	scale.x = Vector2(m_LocalWorld._11, m_LocalWorld._12).Length();
	scale.y = Vector2(m_LocalWorld._21, m_LocalWorld._22).Length();

	if (fabsf(scale.x) > EPSILON && fabsf(scale.x) > EPSILON)
	{
		// 回転角を行列から計算
		rotation = atan2(m_LocalWorld._12, m_LocalWorld._11);
	}	

	m_SpriteManager->Draw(shaderResourceView, position, &m_TextureRect, m_Color, rotation, origin, scale);

	if (recursive)
	{
		// 階層を辿って全ての子オブジェクトを描画
		if (m_Children.size() != 0)
		{
			for (Sprite*& child : m_Children)
			{
				child->Draw();
			}
		}
	}
}

void Sprite::SetParent(Sprite* parent)
{
	// 親オブジェクト設定
	this->m_Parent = parent;
	// 子オブジェクトリストに追加
	parent->m_Children.push_back(this);
}

void Sprite::AddChild(Sprite* child)
{
	// 親オブジェクト設定
	child->m_Parent = this;
	// 子オブジェクトリストに追加
	this->m_Children.push_back(child);
}

void Sprite::SetDirty()
{
	m_IsDirty = true;

	// 階層を辿って全ての子オブジェクトをダーティにする
	if (m_Children.size() != 0)
	{
		for (Sprite*& child : m_Children)
		{
			child->SetDirty();
		}
	}
}

bool Sprite::CheckDirty()
{
	return m_IsDirty;
}

void Sprite::SetPosition(const Vector2& position)
{
	m_Position = position;
	SetDirty();
}

void Sprite::SetRotation(float rotation)
{
	m_Rotation = rotation;
	SetDirty();
}

void Sprite::SetScale(float scale)
{
	m_Scale = Vector2(scale, scale);
	SetDirty();
}

void Sprite::SetScale(const Vector2& scale)
{
	m_Scale = scale;
	SetDirty();
}

SpriteFactory::SpriteFactory(SpriteRenderer * spriteRenderer, TextureCache* textureCache)
{
	m_SpriteRenderer = spriteRenderer;	
	m_TextureCache = textureCache;
}

std::unique_ptr<Sprite> SpriteFactory::CreateFromFile(const wchar_t* filename)
{
	std::unique_ptr<Sprite> sprite = std::make_unique<Sprite>();

	sprite->SetSpriteManager(m_SpriteRenderer);
	
	// テクスチャロード
	Texture* texture = m_TextureCache->LoadTexture(filename);
	sprite->SetTexture(texture);

	// テクスチャ全体を貼り付ける設定
	RECT rect = RECT{0,0,0,0};
	rect.right = texture->desc.Width;
	rect.bottom = texture->desc.Height;
	sprite->SetTextureRect(rect);

	return std::move(sprite);
}