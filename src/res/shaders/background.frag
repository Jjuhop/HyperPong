#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Position;

uniform float u_Time;
uniform vec2 u_HighlightPos;
uniform vec2 u_WindDim;

float prf(uvec2 seeds) {
	uint seed = (seeds.x * 6967u) ^ (seeds.y * 7919u);
  uint state = seed * 747796405u + 2891336453u;
  uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return float((word >> 22u) ^ word) / float(4294967295u);
}

vec2 prGrad(uvec2 seeds) {
	float angle = prf(seeds) * 2 * 3.14159265359;
	return vec2(cos(angle), sin(angle));
}

float dottedWithGrad(vec2 icoord, vec2 coord) {
	vec2 grad = prGrad(floatBitsToUint(icoord));
	return dot(grad, coord - icoord);
}

float perlin(vec2 at, float scale) {
	vec2 coord = at / scale;
	vec2 icoord0 = floor(coord);
	vec2 icoord1 = icoord0 + 1.0;

	float n0 = dottedWithGrad(icoord0, coord);
	float n1 = dottedWithGrad(vec2(icoord1.x, icoord0.y), coord);
	
	float n2 = dottedWithGrad(vec2(icoord0.x, icoord1.y), coord);
	float n3 = dottedWithGrad(icoord1, coord);

	vec2 left = vec2(n0, n2);
	vec2 right = vec2(n1, n3);

	vec2 interp_w = smoothstep(icoord0, icoord1, coord);

	vec2 interp_x = (right - left) * interp_w.x + left;

	float interp_y = (interp_x.y - interp_x.x) * interp_w.y + interp_x.x;
	
	return (interp_y + 1.0) * 0.5;
}

const float coef = 1.0 / sqrt(2 * 3.14159265359);

void main() {
  float change = u_Time * 50;
  vec2 pix = (v_Position.xy + 1.0) * u_WindDim * 0.5;
	vec3 c = vec3(perlin(pix + vec2(change, 0.0), 200.0), perlin(pix + vec2(0.0, change), 150), perlin(pix + change - 0.7, 250));
	//float d = max(1.0, length(u_HighlightPos - pix) * 0.01);
	//float d = length(u_HighlightPos - pix) * 0.01;
  //float dimming = coef * exp(-d*d * 0.5);
	float d = length(u_HighlightPos - pix);
  float dimming = 1.0 - smoothstep(0, 250, d) * 0.9;
	
	color = vec4(c * max(dimming, 0.0), 1.0);
	//color = vec4(max(dimming, 0.02), 0.0, 0.0, 1.0);
}