#include <metal_stdlib>
using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float2 uv;
};

vertex VertexOut
vertexShader(uint vertexID [[vertex_id]],
             constant simd::float3* vertexPositions)
{
    float3 pos = vertexPositions[vertexID];

    VertexOut out;
    out.position = float4(pos, 1.0f);

    // Remap x,y from [-1, 1] clip space to [0, 1] UV space
    out.uv = pos.xy * 0.5f + 0.5f;

    return out;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]]) {
    // in.uv.x and in.uv.y are now usable as UV coordinates
    return float4(in.uv.x, 228.0f/255.0f, in.uv.y, 1.0f);
}
