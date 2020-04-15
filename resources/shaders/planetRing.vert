#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; 
uniform MaterialInfo material1; 
uniform int numberOfAsteroids;
uniform float time;
// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader -- using Gouraud (interpolated) shading
out vec3 vColour;	// Colour computed using reflectance model
out vec2 vTexCoord;	// Texture coordinate


// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}
///https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float pseudoRandom(vec2 vector){
    return fract(sin(dot(vector.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
//Determines the position of the asteroid in the ring depending on the gl_instanceID.
//Converts polar coordinates to cartesian coordinates.
vec3 positionOfAsteroid(int i){
    float ringRadius = 750;
    float angleIncrement = 360.0f / numberOfAsteroids;
    int numberPerLayer = (numberOfAsteroids / 30);
    int currentLayer = (i / numberPerLayer);
    float currentRadius = ringRadius - (currentLayer * 2.0f);
    float x = currentRadius * cos((i*angleIncrement - time/30000) + (currentLayer * 3));
    float z = currentRadius * sin((i*angleIncrement - time/30000) + (currentLayer * 3));
    return vec3(x, pseudoRandom(vec2(i,0))*5.0f, z);
}

// This is the entry point into the vertex shader
void main()
{	
	// Transform the vertex spatial position using 
	vec3 asteroidPosition = (inPosition*pseudoRandom(vec2(gl_InstanceID,0))*5.0f) + positionOfAsteroid(gl_InstanceID);
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(asteroidPosition, 1.0f);
	
	// Get the vertex normal and vertex position in eye coordinates
	vec3 vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vec4 vEyePosition = matrices.modelViewMatrix * vec4(asteroidPosition, 1.0f);
		
	// Apply the Phong model to compute the vertex colour
	vColour = PhongModel(vEyePosition, vEyeNorm);
	
	// Pass through the texture coordinate
	vTexCoord = inCoord;
} 
	