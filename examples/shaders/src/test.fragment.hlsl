cbuffer UniformBlock : register(b0, space3)
{
    float2 resolution;
    float time;
}

float3 palette(float t)
{
    float3 a = float3(0.5, 0.5, 0.5);
    float3 b = float3(0.5, 0.5, 0.5);
    float3 c = float3(1.0, 1.0, 1.0);
    float3 d = float3(0.263, 0.416, 0.557);

    return a + b * cos(6.28318 * (c * t + d));
}

float4 main(float4 fragCoord : SV_Position) : SV_Target0
{
    float2 uv = (fragCoord.xy * 2.0 - resolution.xy) / resolution.y;
    float2 uv0 = uv;
    float3 finalColor = float3(0.0, 0.0, 0.0);

    for (float i = 0.0; i < 4.0; i += 1.0)
    {
        uv = frac(uv * 1.5) - 0.5;

        float d = length(uv) * exp(-length(uv0));
        float3 col = palette(length(uv0) + i * 0.4 + time * 0.4);

        d = sin(d * 8.0 + time) / 8.0;
        d = abs(d);
        d = pow(0.01 / d, 1.2);

        finalColor += col * d;
    }

    return float4(finalColor, 1.0);
}
