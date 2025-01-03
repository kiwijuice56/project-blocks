shader_type spatial;
render_mode cull_disabled, fog_disabled, unshaded;

uniform vec4 albedo : source_color;
uniform vec4 albedo_far : source_color;
uniform float fade = 0.5;
uniform float max_dist = 4.0;

uniform sampler2D noise;

void fragment() {
	const vec3 magic = vec3(0.06711056, 0.00583715, 52.9829189);
	if (fade < fract(magic.z * fract(dot(FRAGCOORD.xy, magic.xy)))) {
		discard;
	}

	vec4 upos = INV_PROJECTION_MATRIX * vec4(SCREEN_UV * 2.0 - 1.0, FRAGCOORD.z, 1.0);
	vec3 pixel_position = upos.xyz / upos.w ;
	float dist = -pixel_position.z + 0.7 * texture(noise, SCREEN_UV + vec2(cos(TIME * 0.05), sin(TIME * 0.04))).r;
	float x = clamp(dist / max_dist, 0., 1.);
	ALBEDO = albedo_far.rgb * x + (1.0 - x) * albedo.rgb;
}

void light() {
	DIFFUSE_LIGHT = vec3(0.);
}