#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
    m_trackWidth = 30;
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk
    /*
    m_controlPoints.push_back(glm::vec3(100, 5, 0));
    m_controlPoints.push_back(glm::vec3(71, 5, 71));
    m_controlPoints.push_back(glm::vec3(0, 10, 100));
    m_controlPoints.push_back(glm::vec3(-71, 20, 71));
    m_controlPoints.push_back(glm::vec3(-100, 30, 0));
    m_controlPoints.push_back(glm::vec3(-71, 10, -71));
    m_controlPoints.push_back(glm::vec3(0, 5, -100));
    m_controlPoints.push_back(glm::vec3(71, 5, -71));
	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)
    */
    m_controlPoints.push_back(glm::vec3( 291,  -100, -1452));
    m_controlPoints.push_back(glm::vec3( 291,  -100, -1162));
    m_controlPoints.push_back(glm::vec3( -16,  -100, -810));
    m_controlPoints.push_back(glm::vec3( -480, -100, -463));
    m_controlPoints.push_back(glm::vec3( -886,  584,  127));
    m_controlPoints.push_back(glm::vec3( -784,  584,  664));
    m_controlPoints.push_back(glm::vec3( -359,  584,  855));
    m_controlPoints.push_back(glm::vec3(   93,  584,  865));
    m_controlPoints.push_back(glm::vec3(  695,  584,  833));
    m_controlPoints.push_back(glm::vec3( 1125, -100,  510));
    m_controlPoints.push_back(glm::vec3( 1009, -100, -348));
    m_controlPoints.push_back(glm::vec3(  896, -100, -719));
    m_controlPoints.push_back(glm::vec3(  737, -100, -978));
    m_controlPoints.push_back(glm::vec3(  694, -100, -1308));
    m_controlPoints.push_back(glm::vec3(  674, -100, -1647));
    m_controlPoints.push_back(glm::vec3(  376, -100, -1703));
    for (unsigned int i = 0; i < m_controlPoints.size(); i++) {
        m_controlPoints[i] * 0.05f;
    }
    for (unsigned int i = 0; i < m_controlPoints.size(); i++) {
        m_controlPoints[i].y += -300.0f;
        m_controlPoints[i].x +=  100.0f;
        m_controlPoints[i].z += -300.0f;
    }
}


// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int) m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i-1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M-1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
// bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up) 
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 up)
{
	if (d < 0)
		return false;

	int M = (int) m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int) (d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;
	
	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j-1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}



void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
    SetControlPoints();
	// Call UniformlySampleControlPoints with the number of samples required
    UniformlySampleControlPoints(500);
	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
    glGenVertexArrays(1, &m_vaoCentreline);
    glBindVertexArray(m_vaoCentreline);

    CVertexBufferObject vbo;
    vbo.Create();
    vbo.Bind();
    glm::vec2 texCoord(0.0f, 0.0f);
    glm::vec3 normal(0.0f, 1.0f, 0.0f);

    for (unsigned int i = 0; i < m_centrelinePoints.size(); i++) {
        vbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3));
        vbo.AddData(&texCoord, sizeof(glm::vec2));
        vbo.AddData(&normal, sizeof(glm::vec3));
    }

    // Upload the VBO to the GPU
    vbo.UploadDataToGPU(GL_STATIC_DRAW);
    // Set the vertex attribute locations
    GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}


void CCatmullRom::CreateOffsetCurves(){

    for (unsigned int i = 0; i < m_centrelinePoints.size(); i++) {
        glm::vec3 T = m_centrelinePoints[(i + 1) % m_centrelinePoints.size()] - m_centrelinePoints[i];
        glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0, 1, 0))); //normalised so track width remains constant
        glm::vec3 l = m_centrelinePoints[i] - (N * m_trackWidth * 0.5f);
        glm::vec3 r = m_centrelinePoints[i] + (N * m_trackWidth * 0.5f);
        m_leftOffsetPoints.push_back(l);
        m_rightOffsetPoints.push_back(r);
    }
	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	// Note it is possible to only use one VAO / VBO with all the points instead.

    glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
    glBindVertexArray(m_vaoLeftOffsetCurve);

    CVertexBufferObject vboLeft;
    vboLeft.Create();
    vboLeft.Bind();
    glm::vec2 texCoord(0.0f, 0.0f);
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    for (unsigned int i = 0; i < m_leftOffsetPoints.size(); i++) {
        vboLeft.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
        vboLeft.AddData(&texCoord, sizeof(glm::vec2));
        vboLeft.AddData(&normal, sizeof(glm::vec3));
    }

    // Upload the VBO to the GPU
    vboLeft.UploadDataToGPU(GL_STATIC_DRAW);
    // Set the vertex attribute locations
    GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

    glGenVertexArrays(1, &m_vaoRightOffsetCurve);
    glBindVertexArray(m_vaoRightOffsetCurve);

    CVertexBufferObject vboRight;
    vboRight.Create();
    vboRight.Bind();
    for (unsigned int i = 0; i < m_rightOffsetPoints.size(); i++) {
        vboRight.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
        vboRight.AddData(&texCoord, sizeof(glm::vec2));
        vboRight.AddData(&normal, sizeof(glm::vec3));
    }

    // Upload the VBO to the GPU
    vboRight.UploadDataToGPU(GL_STATIC_DRAW);
    // Set the vertex attribute locations

    stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}


void CCatmullRom::CreateTrack(std::string directory, std::string filename) {

    m_texture.Load(directory + filename, true);

    // Set parameters for texturing using sampler object
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    //aniostropy form lecture slides
    GLfloat aniso;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    m_texture.SetSamplerObjectParameterf(GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
    // Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
    glGenVertexArrays(1, &m_vaoTrack);
    glBindVertexArray(m_vaoTrack);
    CVertexBufferObject vboTrack;
    vboTrack.Create();
    vboTrack.Bind();


    std::vector<glm::vec2> texCoords = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f)
    };
    glm::vec2 texCoord(0.0f, 0.0f);
    glm::vec3 normal(0.0f, 1.0f, 0.0f);

    std::vector<glm::vec3> rightOffsetNormals;
    std::vector<glm::vec3> leftOffsetNormals;

    
    for (int i = 0; i < m_rightOffsetPoints.size(); i++) {
        glm::vec3 T = glm::normalize(m_centrelinePoints[(i + 1) % m_centrelinePoints.size()] - m_centrelinePoints[i]);
        glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
        glm::vec3 B = glm::normalize(glm::cross(N, T));
        rightOffsetNormals.push_back(B);

        T = glm::normalize(m_centrelinePoints[(i + 1) % m_centrelinePoints.size()] - m_centrelinePoints[i]);
        N = glm::normalize(glm::cross(T, glm::vec3(0.0f, 1.0f, 0.0f)));
        B = glm::normalize(glm::cross(N, T));
        leftOffsetNormals.push_back(B);
    }
    for (unsigned int i = 0; i < m_rightOffsetPoints.size(); i++) {
        vboTrack.AddData(&m_leftOffsetPoints[i % m_leftOffsetPoints.size()], sizeof(glm::vec3)); 
        m_vertexCount++;
        vboTrack.AddData(&texCoords[0], sizeof(glm::vec2));
        vboTrack.AddData(&leftOffsetNormals[i], sizeof(glm::vec3));

        vboTrack.AddData(&m_rightOffsetPoints[i % m_rightOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoords[2], sizeof(glm::vec2));
        vboTrack.AddData(&rightOffsetNormals[i], sizeof(glm::vec3));

        vboTrack.AddData(&m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoords[1], sizeof(glm::vec2));
        vboTrack.AddData(&leftOffsetNormals[(i + 1) % leftOffsetNormals.size()], sizeof(glm::vec3));
        
        vboTrack.AddData(&m_rightOffsetPoints[i % m_rightOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoords[2], sizeof(glm::vec2));
        vboTrack.AddData(&rightOffsetNormals[i], sizeof(glm::vec3));

        vboTrack.AddData(&m_rightOffsetPoints[(i + 1) % m_rightOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoords[3], sizeof(glm::vec2));
        vboTrack.AddData(&rightOffsetNormals[(i + 1) % rightOffsetNormals.size()], sizeof(glm::vec3));

        vboTrack.AddData(&m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoords[1], sizeof(glm::vec2));
        vboTrack.AddData(&leftOffsetNormals[(i + 1) % leftOffsetNormals.size()], sizeof(glm::vec3));
    }
    
    /*
    for (unsigned int i = 0; i < m_rightOffsetPoints.size(); i++) {
        vboTrack.AddData(&m_leftOffsetPoints[i % m_leftOffsetPoints.size()], sizeof(glm::vec3)); 
        m_vertexCount++;
        vboTrack.AddData(&texCoord, sizeof(glm::vec2));
        vboTrack.AddData(&normal, sizeof(glm::vec3));

        vboTrack.AddData(&m_rightOffsetPoints[i % m_rightOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoord, sizeof(glm::vec2));
        vboTrack.AddData(&normal, sizeof(glm::vec3));

        vboTrack.AddData(&m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoord, sizeof(glm::vec2));
        vboTrack.AddData(&normal, sizeof(glm::vec3));
        
        vboTrack.AddData(&m_rightOffsetPoints[i % m_rightOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoord, sizeof(glm::vec2));
        vboTrack.AddData(&normal, sizeof(glm::vec3));

        vboTrack.AddData(&m_rightOffsetPoints[(i + 1) % m_rightOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoord, sizeof(glm::vec2));
        vboTrack.AddData(&normal, sizeof(glm::vec3));

        vboTrack.AddData(&m_leftOffsetPoints[(i + 1) % m_leftOffsetPoints.size()], sizeof(glm::vec3));
        m_vertexCount++;
        vboTrack.AddData(&texCoord, sizeof(glm::vec2));
        vboTrack.AddData(&normal, sizeof(glm::vec3));
    }
    */
    vboTrack.UploadDataToGPU(GL_STATIC_DRAW);
    GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    // Texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
    // Normal vectors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

}


void CCatmullRom::RenderCentreline()
{
	// Bind the VAO m_vaoCentreline and render it
    glBindVertexArray(m_vaoCentreline);
    glPointSize(3);
    glDrawArrays(GL_LINE_STRIP, 0, m_centrelinePoints.size());
    glDrawArrays(GL_POINTS, 0, m_centrelinePoints.size());
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
    glBindVertexArray(m_vaoLeftOffsetCurve);
    glPointSize(3);
    glDrawArrays(GL_LINE_STRIP, 0, m_leftOffsetPoints.size());
    glDrawArrays(GL_POINTS, 0, m_leftOffsetPoints.size());

	// Bind the VAO m_vaoRightOffsetCurve and render it
    glBindVertexArray(m_vaoRightOffsetCurve);
    glPointSize(3);
    glDrawArrays(GL_LINE_STRIP, 0, m_rightOffsetPoints.size());
    glDrawArrays(GL_POINTS, 0, m_rightOffsetPoints.size());
}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
    glDisable(GL_CULL_FACE);
    glBindVertexArray(m_vaoTrack);
    m_texture.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
    glEnable(GL_CULL_FACE);
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}