#version 400 core

in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied


void main()
{
	// Get the texel colour from the texture sampler
	vec4 vTexColour = texture(sampler0, vTexCoord);	

	if (bUseTexture){
		//vOutputColour = vTexColour;//*vec4(vColour, 1.0f);	// Combine object colour and texture 
		vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
	}
	else{
		//vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
		vOutputColour = vec4(1.0f, 0.0f, 0.0f, 1.0f);	// Just use the colour instead
	}
	
	
	
}
