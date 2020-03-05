#version 400 core

out vec4 vOutputColour;		// The output colour

uniform float time;
uniform float guiWidth;
uniform float guiHeight;

//Boosting data
uniform float timeBoosting;
uniform float maxBoost;
uniform float numberBoosts;

bool isInBoostBox(float x, float y, float width, float height){
	if(gl_FragCoord.x > x && gl_FragCoord.x < x + width && gl_FragCoord.y > y && gl_FragCoord.y < y + height){
		return true;
	}
	return false;
}

void main(){
    //baseColour
    vec3 baseColour = vec3(0.988, 0.518, 0.012);
    float fluctuation = sin(time * 0.001f*2.0f)*0.15f;
    vec3 colour = baseColour + fluctuation;

	
    if(gl_FragCoord.x < 10.0 || gl_FragCoord.y < 10.0 || gl_FragCoord.x > (guiWidth * 0.5f)-10.0 || gl_FragCoord.y > (guiHeight * 0.5f)-10.0){
		colour = (baseColour * 0.8) + fluctuation;
    }
	//Boosting Box
	if(isInBoostBox(guiWidth * 0.125f, guiHeight * 0.31, guiWidth * 0.25, guiHeight * 0.08)){
		colour = vec3(0.25f, 0.25f, 0.25f) + fluctuation;
	}

	//Boosting Bar
	float barWidth = 0;
	if(numberBoosts > 0 || timeBoosting > 0.0f){
		barWidth = (guiWidth * 0.23f) * (maxBoost - timeBoosting)/maxBoost;
	}
		barWidth = (guiWidth * 0.23f) * (maxBoost - timeBoosting)/maxBoost;
	
	if(isInBoostBox(guiWidth * 0.134f, guiHeight * 0.32f, barWidth, guiHeight * 0.06f)){
		colour = vec3(0.28, 0.73f, 0.26f) + fluctuation;
	}
	
    vOutputColour = vec4(colour,0.3f);	
	
}
