#include <metal_stdlib>
#include <metal_types.h>
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

kernel void clearCompute(texture2d<float, access::write> target [[texture(0)]],
                         uint2 gid [[thread_position_in_grid]]) {
  if (gid.x >= target.get_width() || gid.y >= target.get_height()) {
    return;
  }
  target.write(float4(0.0, 0.0, 0.0, 0.0), gid);
}

kernel void compositeCompute(texture2d<float, access::read_write> active
                             [[texture(0)]],
                             texture2d<float, access::read> layer
                             [[texture(1)]],
                             constant int2 &start [[buffer(0)]],
                             constant int2 &end [[buffer(1)]],
                             uint2 gid [[thread_position_in_grid]]) {

  uint2 absolute_gid = gid + uint2(start);
  if (absolute_gid.x >= active.get_width() ||
      absolute_gid.y >= active.get_height() || absolute_gid.x < start.x ||
      absolute_gid.y < start.y || absolute_gid.x > end.x ||
      absolute_gid.y > end.y) {
    return;
  }

  float2 uv =
      float2(absolute_gid) / float2(active.get_width(), active.get_height());

  float4 color =
      layer.read(uint2(uv * float2(layer.get_width(), layer.get_height())));
  float4 currentColor = active.read(absolute_gid);
  float4 blendedColor = mix(currentColor, color, color.a);
  active.write(blendedColor, absolute_gid);
}

kernel void squareCompute(texture2d<float, access::read_write> active
                          [[texture(0)]],
                          texture2d<float, access::read> layer [[texture(1)]],
                          constant int2 &start [[buffer(0)]],
                          constant int2 &end [[buffer(1)]],
                          constant float4x4 &transform [[buffer(2)]],
                          constant float2 &size [[buffer(3)]],
                          constant float4 &color [[buffer(4)]],
                          uint2 gid [[thread_position_in_grid]]) {
  uint2 absolute_gid = gid + uint2(start);
  if (absolute_gid.x >= active.get_width() ||
      absolute_gid.y >= active.get_height() || absolute_gid.x < start.x ||
      absolute_gid.y < start.y || absolute_gid.x > end.x ||
      absolute_gid.y > end.y) {
    return;
  }

  float2 pos = float2(absolute_gid);

  // Apply the transform to the position
  float4 transformedPos = transform * float4(pos, 0.0, 1.0);
  float2 halfSize = size * 0.5;

  if (transformedPos.x >= -halfSize.x && transformedPos.x <= halfSize.x &&
      transformedPos.y >= -halfSize.y && transformedPos.y <= halfSize.y) {

    float4 currentColor = active.read(absolute_gid);
    float4 blendedColor = mix(currentColor, color, color.a);

    active.write(blendedColor, absolute_gid);
  }
}

kernel void circleCompute(texture2d<float, access::read_write> active
                          [[texture(0)]],
                          texture2d<float, access::read> layer [[texture(1)]],
                          constant int2 &start [[buffer(0)]],
                          constant int2 &end [[buffer(1)]],
                          constant float4x4 &transform [[buffer(2)]],
                          constant float2 &size [[buffer(3)]],
                          constant float4 &color [[buffer(4)]],
                          uint2 gid [[thread_position_in_grid]]) {
  uint2 absolute_gid = gid + uint2(start);
  if (absolute_gid.x >= active.get_width() ||
      absolute_gid.y >= active.get_height() || absolute_gid.x < start.x ||
      absolute_gid.y < start.y || absolute_gid.x > end.x ||
      absolute_gid.y > end.y) {
    return;
  }

  float2 pos = float2(absolute_gid);
  float4 transformedPos = transform * float4(pos, 0.0, 1.0);

  float distance = length(transformedPos.xy);

  if (distance <= size.x * 0.5) {
    float4 currentColor = active.read(absolute_gid);
    float4 blendedColor = mix(currentColor, color, color.a);

    active.write(blendedColor, absolute_gid);
  }
}

kernel void imageCompute(texture2d<float, access::read_write> active
                         [[texture(0)]],
                         texture2d<float, access::read> layer [[texture(1)]],
                         constant int2 &start [[buffer(0)]],
                         constant int2 &end [[buffer(1)]],
                         constant float4x4 &transform [[buffer(2)]],
                         constant float2 &size [[buffer(3)]],

                         texture2d<float> imageTexture [[texture(2)]],

                         uint2 gid [[thread_position_in_grid]]) {
  uint2 absolute_gid = gid + uint2(start);
  if (absolute_gid.x >= active.get_width() ||
      absolute_gid.y >= active.get_height() || absolute_gid.x < start.x ||
      absolute_gid.y < start.y || absolute_gid.x > end.x ||
      absolute_gid.y > end.y) {
    return;
  }

  float2 pos = float2(absolute_gid);

  // Apply the transform to the position
  float4 transformedPos = transform * float4(pos, 0.0, 1.0);
  float2 halfSize = size * 0.5;

  if (transformedPos.x >= -halfSize.x && transformedPos.x <= halfSize.x &&
      transformedPos.y >= -halfSize.y && transformedPos.y <= halfSize.y) {

    float4 currentColor = active.read(absolute_gid);
    float4 color =
        float4(1.0, 1.0, 1.0, 1.0); // Default color if texture sampling fails
    float2 imageUV = (transformedPos.xy + halfSize.xy) / size;
    color = imageTexture.sample(sampler(address::clamp_to_edge), imageUV);
    float4 blendedColor = mix(currentColor, color, color.a);

    active.write(blendedColor, absolute_gid);
  }
}

kernel void tintCompute(texture2d<float, access::read_write> active
                        [[texture(0)]],
                        texture2d<float, access::read> layer [[texture(1)]],
                        constant int2 &start [[buffer(0)]],
                        constant int2 &end [[buffer(1)]],
                        constant float4x4 &transform [[buffer(2)]],
                        constant float2 &size [[buffer(3)]],
                        constant float4 &tint [[buffer(4)]],
                        uint2 gid [[thread_position_in_grid]]) {
  uint2 absolute_gid = gid + uint2(start);
  if (absolute_gid.x >= active.get_width() ||
      absolute_gid.y >= active.get_height() || absolute_gid.x < start.x ||
      absolute_gid.y < start.y || absolute_gid.x > end.x ||
      absolute_gid.y > end.y) {
    return;
  }

  float2 pos = float2(absolute_gid);

  // Apply the transform to the position
  float4 transformedPos = transform * float4(pos, 0.0, 1.0);
  float2 halfSize = size * 0.5;

  if (transformedPos.x >= -halfSize.x && transformedPos.x <= halfSize.x &&
      transformedPos.y >= -halfSize.y && transformedPos.y <= halfSize.y) {

    float4 currentColor = active.read(absolute_gid);

    // tint only in non-transparent areas of currentColor
    float4 blendedColor = mix(currentColor, tint, tint.a * currentColor.a);

    active.write(blendedColor, absolute_gid);
  }
}