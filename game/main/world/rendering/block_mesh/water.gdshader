shader_type spatial;
render_mode unshaded;

uniform vec4 albedo : source_color;
uniform float fade = 0.5;

uniform sampler2D noise;

void fragment() {
	ALBEDO = albedo.rgb;
	const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
	if (fade < fract(magic.z * fract(dot(FRAGCOORD.xy, magic.xy)))) {
		discard;
	}
}