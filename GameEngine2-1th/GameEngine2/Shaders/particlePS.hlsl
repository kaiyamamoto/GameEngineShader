
#include "particle.hlsli"

// ピクセルシェーダ
float4 main(PSInput input) : SV_TARGET
{
    float4 color = input.color;

    /* 白黒反転
    color.x = 1.0f - color.x;
    color.y = 1.0f - color.y;
    color.z = 1.0f - color.z;
    */
    
    return color;
}