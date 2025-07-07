float4 main(uint vertexID : SV_VertexID) : SV_Position
{
    float2 positions[3] = {
        float2(-1.0, -1.0), // bas gauche
        float2(3.0, -1.0), // trop à droite
        float2(-1.0,  3.0)  // trop haut
    };
    return float4(positions[vertexID], 0.0, 1.0);
}
