#include "SpriteRenderer.h"
#include "Sprite.h"

#include <assert.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace MyLibrary;

// 静的メンバ変数の実体
std::unique_ptr<SpriteRenderer> SpriteRenderer::m_Instance;

SpriteRenderer * MyLibrary::SpriteRenderer::GetInstance()
{
	if (!m_Instance)
	{
		m_Instance.reset(new SpriteRenderer);
	}

	return m_Instance.get();
}

void SpriteRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context)
{
	assert(m_SpriteBatch == nullptr);

	m_SpriteBatch = std::make_unique<SpriteBatch>(context);

	m_CommonStates = std::make_unique<CommonStates>(device);
}

void SpriteRenderer::Begin()
{
	assert(m_SpriteBatch);

	m_SpriteBatch->Begin(SpriteSortMode::SpriteSortMode_Deferred, m_CommonStates->NonPremultiplied() );
}

void SpriteRenderer::End()
{
	assert(m_SpriteBatch);

	m_SpriteBatch->End();
}

void SpriteRenderer::Draw(ID3D11ShaderResourceView* texture, XMFLOAT2 const& position, RECT const* sourceRectangle, FXMVECTOR color, float rotation, const XMFLOAT2& origin, const XMFLOAT2& scale, SpriteEffects effects, float layerDepth)
{
	m_SpriteBatch->Draw(texture, position, sourceRectangle, color, rotation, origin, scale, effects, layerDepth);
}
