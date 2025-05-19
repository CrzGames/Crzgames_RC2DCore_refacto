cbuffer Uniforms : register(b0) {
    float4 color; // Couleur globale (R, G, B, A)
};

float4 main() : SV_TARGET {
    return color;
}