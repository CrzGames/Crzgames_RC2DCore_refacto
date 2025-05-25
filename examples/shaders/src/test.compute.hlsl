RWTexture2D<float4> outputTexture : register(u0, space1);

[numthreads(32, 32, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // Écrire une couleur simple dans la texture (par exemple, un dégradé)
    float2 uv = float2(dispatchThreadID.xy) / float2(1920.0, 1080.0);
    outputTexture[dispatchThreadID.xy] = float4(uv.x, uv.y, 0.5, 1.0);
}