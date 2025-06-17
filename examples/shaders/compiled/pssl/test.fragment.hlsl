cbuffer type_UniformBlock : register(b0)
{
    float2 UniformBlock_resolution : packoffset(c0);
    float UniformBlock_time : packoffset(c0.z);
};


static float4 gl_FragCoord;
static float4 out_var_SV_Target0;

struct SPIRV_Cross_Input
{
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 out_var_SV_Target0 : SV_Target0;
};

void frag_main()
{
    float2 _52 = ((gl_FragCoord.xy * 2.0f) - UniformBlock_resolution) / UniformBlock_resolution.y.xx;
    float2 _54;
    float3 _57;
    _54 = _52;
    _57 = 0.0f.xxx;
    for (float _59 = 0.0f; _59 < 4.0f; )
    {
        float2 _63 = _54 * 1.5f;
        float2 _55 = frac(_63) - 0.5f.xx;
        float _66 = length(_52);
        _54 = _55;
        _57 += ((0.5f.xxx + (0.5f.xxx * cos(((1.0f.xxx * ((_66 + (_59 * 0.4000000059604644775390625f)) + (UniformBlock_time * 0.4000000059604644775390625f))) + float3(0.263000011444091796875f, 0.41600000858306884765625f, 0.556999981403350830078125f)) * 6.28318023681640625f))) * pow(0.00999999977648258209228515625f / abs(sin(((length(_55) * exp(-_66)) * 8.0f) + UniformBlock_time) * 0.125f), 1.2000000476837158203125f));
        _59 += 1.0f;
        continue;
    }
    out_var_SV_Target0 = float4(_57, 1.0f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    gl_FragCoord.w = 1.0 / gl_FragCoord.w;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.out_var_SV_Target0 = out_var_SV_Target0;
    return stage_output;
}
