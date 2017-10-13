
#include "particle.hlsli"

// 頂点シェーダ
PSInput main( float4 pos : POSITION ,float4 color : COLOR)
{
	PSInput output;

    output.pos = pos;
    /* 左右反転
    output.pos.x *= -1.0f;
    output.pos.y *= -1.0f;
    */

    /* スケーリング
    output.pos.x *= 1.5f;
    output.pos.y *= 1.5f;
    */
    
    output.color = color;

	return output;
}