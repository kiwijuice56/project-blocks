shader_type spatial;

uniform vec4 albedo : source_color;
uniform sampler2D noise_texture: source_color;
uniform float fade = 0.5;

void fragment() {
	ALBEDO = albedo.rgb;
	const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
	if (fade < 0.001 || fade < fract(magic.z * fract(dot(FRAGCOORD.xy, magic.xy)))) {
		discard;
	}
}