struct VertexInput {
    float2 position : POSITION;
};

struct VertexOutput {
    float4 position : SV_POSITION;
};

cbuffer Uniforms : register(b0) {
    float2 resolution; // Résolution logique (ex. : 1920x1080)
    float2 padding;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    // Convertir les coordonnées logiques (pixels) en NDC (-1 à 1)
    output.position = float4(
        (input.position.x / resolution.x) * 2.0 - 1.0,  // X : 0..resolution.x -> -1..1
        ((input.position.y / resolution.y) * 2.0 - 1.0), // Y : 0..resolution.y -> -1..1
        0.0,
        1.0
    );
    return output;
}