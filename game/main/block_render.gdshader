shader_type spatial;

uniform sampler2DArray textures: filter_nearest;

void fragment() {
	// UV2.y carries the texture ID
	ALBEDO = texture(textures, vec3(UV.x, UV.y, UV2.y)).rgb;
}
