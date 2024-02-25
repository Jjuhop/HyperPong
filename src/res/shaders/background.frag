#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_Position;

uniform float u_Offset;
uniform vec2 u_HighlightPos;

float prf(uvec2 seeds) {
	uint seed = seeds.x ^ (seeds.y * 313u);
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
	vec2 coord = at * scale;
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

void main() {
	vec3 c = vec3(perlin(v_Position.xy + vec2(u_Offset, 0.0), 2.0), perlin(v_Position.xy + vec2(0.0, u_Offset), 1.5), perlin(v_Position.xy + u_Offset - 0.7, 2.5));
	float d = max(1.0, length(u_HighlightPos - v_Position.xy) * 8);
	
	color = vec4(c * max(0.8 / d, 0.1), 1.0);
}