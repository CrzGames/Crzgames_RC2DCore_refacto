float4 main(uint vertexID : SV_VertexID) : SV_Position
{
    float2 positions[4] = {
        float2(-1.0,  1.0), // Haut-gauche
        float2( 1.0,  1.0), // Haut-droit
        float2(-1.0, -1.0), // Bas-gauche
        float2( 1.0, -1.0)  // Bas-droit
    };
    return float4(positions[vertexID], 0.0, 1.0);
}