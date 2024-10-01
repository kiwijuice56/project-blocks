shader_type spatial;

uniform sampler2DArray textures;



void fragment() {
	ALBEDO = texture(textures, vec3(UV.x, UV.y, 0)).rgb;
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
