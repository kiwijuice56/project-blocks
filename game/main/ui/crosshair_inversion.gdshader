shader_type canvas_item;

uniform sampler2D screen_texture: hint_screen_texture;

void fragment() {
	//COLOR.rgb = vec3(1.0) - texture(screen_texture, SCREEN_UV).rgb;
}

