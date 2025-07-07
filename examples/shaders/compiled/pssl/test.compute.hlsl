struct SpriteComputeData
{
    float3 Position;
    float Rotation;
    float2 Scale;
    float2 Padding;
    float TexU;
    float TexV;
    float TexW;
    float TexH;
    float4 Color;
};

struct SpriteVertex
{
    float4 Position;
    float2 Texcoord;
    float4 Color;
};

ByteAddressBuffer ComputeBuffer : register(t0);
RWByteAddressBuffer VertexBuffer : register(u0);

static uint3 gl_GlobalInvocationID;
struct SPIRV_Cross_Input
{
    uint3 gl_GlobalInvocationID : SV_DispatchThreadID;
};

void comp_main()
{
    float3 _55 = asfloat(ComputeBuffer.Load3(gl_GlobalInvocationID.x * 64 + 0));
    float _57 = asfloat(ComputeBuffer.Load(gl_GlobalInvocationID.x * 64 + 12));
    float2 _59 = asfloat(ComputeBuffer.Load2(gl_GlobalInvocationID.x * 64 + 16));
    float _61 = asfloat(ComputeBuffer.Load(gl_GlobalInvocationID.x * 64 + 32));
    float _63 = asfloat(ComputeBuffer.Load(gl_GlobalInvocationID.x * 64 + 36));
    float _65 = asfloat(ComputeBuffer.Load(gl_GlobalInvocationID.x * 64 + 40));
    float _67 = asfloat(ComputeBuffer.Load(gl_GlobalInvocationID.x * 64 + 44));
    float4 _69 = asfloat(ComputeBuffer.Load4(gl_GlobalInvocationID.x * 64 + 48));
    float _75 = cos(_57);
    float _76 = sin(_57);
    float4x4 _87 = mul(float4x4(float4(_59.x, 0.0f, 0.0f, 0.0f), float4(0.0f, _59.y, 0.0f, 0.0f), float4(0.0f, 0.0f, 1.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f)), mul(float4x4(float4(_75, _76, 0.0f, 0.0f), float4(-_76, _75, 0.0f, 0.0f), float4(0.0f, 0.0f, 1.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f)), float4x4(float4(1.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 1.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 1.0f, 0.0f), float4(_55, 1.0f))));
    uint _89 = gl_GlobalInvocationID.x * 4u;
    VertexBuffer.Store4(_89 * 48 + 0, asuint(mul(float4(0.0f, 0.0f, 0.0f, 1.0f), _87)));
    uint _92 = _89 + 1u;
    VertexBuffer.Store4(_92 * 48 + 0, asuint(mul(float4(1.0f, 0.0f, 0.0f, 1.0f), _87)));
    uint _95 = _89 + 2u;
    VertexBuffer.Store4(_95 * 48 + 0, asuint(mul(float4(0.0f, 1.0f, 0.0f, 1.0f), _87)));
    uint _98 = _89 + 3u;
    VertexBuffer.Store4(_98 * 48 + 0, asuint(mul(float4(1.0f, 1.0f, 0.0f, 1.0f), _87)));
    VertexBuffer.Store2(_89 * 48 + 16, asuint(float2(_61, _63)));
    float _102 = _61 + _65;
    VertexBuffer.Store2(_92 * 48 + 16, asuint(float2(_102, _63)));
    float _105 = _63 + _67;
    VertexBuffer.Store2(_95 * 48 + 16, asuint(float2(_61, _105)));
    VertexBuffer.Store2(_98 * 48 + 16, asuint(float2(_102, _105)));
    VertexBuffer.Store4(_89 * 48 + 32, asuint(_69));
    VertexBuffer.Store4(_92 * 48 + 32, asuint(_69));
    VertexBuffer.Store4(_95 * 48 + 32, asuint(_69));
    VertexBuffer.Store4(_98 * 48 + 32, asuint(_69));
}

[numthreads(64, 1, 1)]
void main(SPIRV_Cross_Input stage_input)
{
    gl_GlobalInvocationID = stage_input.gl_GlobalInvocationID;
    comp_main();
}
