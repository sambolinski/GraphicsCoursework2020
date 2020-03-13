#version 400
in vec2 vTexCoord;
out vec4 vOutputColour;
uniform sampler2D sampler0;
uniform vec4 vColour;
uniform int width, height;
void main()
{
	float dx = 1.0 / float(width);
	float dy = 1.0 / float(height);
	int halfSize = 3;
	vec4 avg = vec4(0);
	for (int y = -halfSize; y <= halfSize; y++){
		for(int x = -halfSize; x <= halfSize; x++){
			avg += texture(sampler0, vTexCoord+vec2(x*dx, y*dy));
		}
	}
	int N = (2*halfSize+1);
	avg /= N*N;
	vOutputColour = avg;
	//vOutputColour = vec4(1.0,0.0,0.0,1.0);
}