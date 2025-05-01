cbuffer UniformBlock : register(b0, space0)
{
    float2 resolution;
    float time;
}

float3 palette(float t)
{
    float3 a = float3(0.5, 0.5, 0.5);
    float3 b = float3(0.5, 0.5, 0.5);
    float3 c = float3(1.0, 1.0, 1.0);
    float3 d = float3(0.00, 0.33, 0.67); // <<< Modifie cette ligne pour tester !

    return a + b * cos(6.28318 * (c * t + d));
}

float op(float2 p)
{
    float s = sin(time * 0.5);
    float c = cos(time * 0.5);
    float2x2 rot = float2x2(c, -s, s, c);
    p = mul(rot, p);

    float d = length(p);
    for (int i = 0; i < 4; ++i)
    {
        p = abs(p) / dot(p, p) - 0.8;
        d = min(d, length(p));
    }
    return d;
}

float4 main(float4 fragCoord : SV_Position) : SV_Target0
{
    float2 uv = (fragCoord.xy * 2.0 - resolution.xy) / resolution.y;

    float d = op(uv);
    float colorFactor = exp(-5.0 * d);

    float3 color = palette(d + time * 0.1) * colorFactor;

    return float4(color, 1.0);
}
