#version 400

in vec2 vTexCoord;
out vec4 vOutputColour;
uniform sampler2D sampler0;
uniform vec4 vColour;
uniform int width, height;
uniform bool isBoosting;

//Radial blur on render texture
vec4 radialBlur(){
	//Made using: https://stackoverflow.com/questions/4579020/how-do-i-use-a-glsl-shader-to-apply-a-radial-blur-to-an-entire-scene (adapted to Opengl 4)
    float samples[10];
    samples[0] = -0.08;
    samples[1] = -0.05;
    samples[2] = -0.03;
    samples[3] = -0.02;
    samples[4] = -0.01;
    samples[5] =  0.01;
    samples[6] =  0.02;
    samples[7] =  0.03;
    samples[8] =  0.05;
    samples[9] =  0.08;
	float sampleDistance = 1.0f;
	float sampleStrength = 2.0f;

	vec2 blurVector = vTexCoord - vec2(0.5f, 0.5f);
	float blurDistance = length(blurVector);
	vec2 blurVectorLength = normalize(blurVector);

	vec4 colour = texture(sampler0,vTexCoord); 
    vec4 sum = colour;
    for (int i = 0; i < 10; i++)
        sum += texture( sampler0, 0.5 + blurVectorLength * samples[i] * sampleDistance );

    sum *= 1.0/11.0;
    float t = blurDistance * sampleStrength;
    t = clamp(t, 0.0f, 1.0f);

    return mix( colour, sum, t );

}
void main()
{
	if(isBoosting){
		vOutputColour = radialBlur();
	}else{
		
	}
	//vOutputColour = vec4(1.0,0.0,0.0,1.0);
}