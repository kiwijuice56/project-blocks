shader_type spatial;
render_mode unshaded, fog_disabled;
uniform sampler2D texture_albedo : hint_screen_texture;
uniform float alpha : hint_range(0.0, 1.0) = 0.5;

void fragment() {
	vec4 albedo_tex = texture(texture_albedo, SCREEN_UV);
	ALBEDO = vec3(0.5) + albedo_tex.rgb;
	ALPHA = albedo_tex.a * alpha;
}