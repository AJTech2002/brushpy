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

  float4 color = inTexture.read(
      uint2(uv * float2(inTexture.get_width(), inTexture.get_height())));
  outTexture.write(color, absolute_gid);
}

kernel void squareCompute(texture2d<float, access::read_write> outTexture
                          [[texture(0)]],
                          constant int2 &start [[buffer(0)]],
                          constant int2 &end [[buffer(1)]],
                          constant float4x4 &transform [[buffer(2)]],
                          constant float2 &size [[buffer(3)]],
                          constant float4 &color [[buffer(4)]],
                          uint2 gid [[thread_position_in_grid]]) {
  uint2 absolute_gid = gid + uint2(start);
  if (absolute_gid.x >= outTexture.get_width() ||
      absolute_gid.y >= outTexture.get_height()) {
    return;
  }

  float2 pos = float2(absolute_gid);

  // Apply the transform to the position
  float4 transformedPos = transform * float4(pos, 0.0, 1.0);

  if (transformedPos.x >= 0.0 && transformedPos.x <= size.x &&
      transformedPos.y >= 0.0 && transformedPos.y <= size.y) {

    float4 currentColor = outTexture.read(absolute_gid);
    float4 blendedColor = mix(currentColor, color, color.a);

    outTexture.write(blendedColor, absolute_gid);
  }
}

kernel void circleCompute(texture2d<float, access::read_write> outTexture
                          [[texture(0)]],
                          constant int2 &start [[buffer(0)]],
                          constant int2 &end [[buffer(1)]],
                          constant float4x4 &transform [[buffer(2)]],
                          constant float2 &size [[buffer(3)]],
                          constant float4 &color [[buffer(4)]],
                          uint2 gid [[thread_position_in_grid]]) {
  uint2 absolute_gid = gid + uint2(start);
  if (absolute_gid.x >= outTexture.get_width() ||
      absolute_gid.y >= outTexture.get_height()) {
    return;
  }

  float2 pos = float2(absolute_gid);
  float4 transformedPos = transform * float4(pos, 0.0, 1.0);
  float2 center = size * 0.5;

  float distance = length(transformedPos.xy - center);

  if (distance <= size.x * 0.5) {
    float4 currentColor = outTexture.read(absolute_gid);
    float4 blendedColor = mix(currentColor, color, color.a);

    outTexture.write(blendedColor, absolute_gid);
  }
}