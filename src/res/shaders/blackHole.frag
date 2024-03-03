#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec2 v_TexCoord;

uniform float u_Time;

const float nrays = 11.0;
const float singul_rad = 0.3;
const float singul_boundary = 0.05;
const vec4 singul_col = vec4(0.0, 0.0, 0.0, 0.95);
const vec4 boundary_col = vec4(1.0, 1.0, 1.0, 1.0);


void main() {
  vec2 coord = v_TexCoord * 2.0 - 1.0;
  float d = length(coord);
  if (d < singul_rad) {
    color = singul_col;
  } else if (d <= singul_rad + singul_boundary * 0.5) {
    float coef = smoothstep(singul_rad, singul_rad + singul_boundary * 0.5, d);
    color = boundary_col * coef + singul_col * (1.0 - coef);
  } else if (d <= 1) {
    float angle = atan(coord.y, coord.x) + 1.0 / d + u_Time;
    float ticks = angle * nrays / (2 * 3.14159265359);
    float col_mag1 = pow(mod(ticks, 1.0), pow(d + 1, 3.0));
    float col_mag2 = pow(mod(-ticks, 1.0), pow(d + 1, 2.0));
    float col_mag = max(col_mag1, col_mag2);
    vec4 base_col = vec4(0.05, 1.0, 1.0, 1.0);

    // Still interpolate with boundary color
    float coef = smoothstep(singul_rad + singul_boundary * 0.5, singul_rad + singul_boundary, d);
    color = base_col * col_mag * coef + boundary_col * (1.0 - coef);
  } else {
    color = vec4(0.0, 0.0, 0.0, 0.0);
  }
}