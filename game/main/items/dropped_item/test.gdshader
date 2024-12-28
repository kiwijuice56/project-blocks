shader_type spatial;
render_mode depth_prepass_alpha, depth_draw_always;

uniform sampler2D textures: filter_nearest, source_color;

void fragment() {
	// UV2.y carries the texture ID
	ALBEDO = texture(textures, UV).rgb;
	ALPHA = texture(textures, UV).a;
}
