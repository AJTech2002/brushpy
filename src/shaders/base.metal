#include <metal_stdlib>
using namespace metal;

struct VertexOut {
  float4 position [[position]];
  float2 uv;
};

vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
                              constant simd::float3 *vertexPositions) {
  float3 pos = vertexPositions[vertexID];

  VertexOut out;
  out.position = float4(pos, 1.0f);

  // Remap x,y from [-1, 1] clip space to [0, 1] UV space
  out.uv = pos.xy * 0.5f + 0.5f;

  return out;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]],
                               texture2d<float> tex [[texture(0)]]) {

  constexpr sampler s(filter::linear, address::clamp_to_edge);
  return tex.sample(s, in.uv);
  //   return float4(in.uv, 0.0, 1.0);
}

kernel void compositeCompute(texture2d<float, access::write> outTexture
                             [[texture(0)]],
                             texture2d<float, access::read> inTexture
                             [[texture(1)]],
                             constant int2 &start [[buffer(0)]],
                             constant int2 &end [[buffer(1)]],
                             uint2 gid [[thread_position_in_grid]]) {

  uint2 absolute_gid = gid + uint2(start);
  if (absolute_gid.x >= outTexture.get_width() ||
      absolute_gid.y >= outTexture.get_height()) {
    return;
  }

  float2 uv = float2(absolute_gid) /
              float2(outTexture.get_width(), outTexture.get_height());

  float4 color = float4(uv.x, uv.y, 0.0, 1.0);
  outTexture.write(color, gid);
}