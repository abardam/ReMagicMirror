#include "stdafx.h"
#include "OpenGLControl.h"
#include "rgbquad_util.h"
#include "antonio_skeleton_util.h"
#include "shader.hpp"
#include "tenbo_load.h"
#include "objloader.hpp"
#include <time.h>
#include <SOIL.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

#include <vector>
#include <string>
#include <tuple>
#include <fstream>

#define XROT (-PI/16.)

float GLOBAL_AMBIENT[] = { 0.08,0.08,0.08,1.0 };
float MATERIAL_AMBIENT[] = { 0.8,0.8,0.8,1.0 };
float MATERIAL_DIFFUSE[] = { .3, .3, .3, 1.0 };
float MATERIAL_SPECULAR[] = { .3, .3, .3, 1.0 };
float MATERIAL_SHININESS[] = { .5, };

RGBQUAD swaprb[1024 * 1024];

float LIGHTS[] = { 
	.05, .05, .05, 1.0, 
	.3, .3, .3, 1.0,
	0.3, 0.3, 0.3, 1.0, 
	0.0, 2.0,-2.5, 0.0,
	.05, .05, .05, 1.0,
	.3, .3, .3, 1.0,
	0.0, 1.0, 0.0, 1.0,
	-4.0, 2.0, 10.0, 0.0, 
	.05, .05, .05, 1.0, 
	.3, .3, .3, 1.0, 
	0.0, 0.0, 1.0, 1.0,
	-4.0, 2.0, -10.0, 0.0
};


COpenGLControl::COpenGLControl():
	m_fPosX(0.f), 
	m_fPosY(0.f), 
	m_fZoom(-10.f), 
	m_fRotX(0.f), 
	m_fRotY(0.f), 
	m_fLastX(0.f), 
	m_fLastY(0.f),
	m_kinectManager(0), 
	m_textureInit(false),
	m_useKinectRotation(false),
	m_animPause(false),
	m_recordJoints(false),
	m_renderMirror(true),
	m_fontState(0),
	m_countdownTimer(0),
	m_animRenderMesh(true),
	m_animApplyTexture(true),
	m_drawKinectSkeleton(true),
	m_recordVideo(false),
	m_sequenceName("")
{

	time_t t = time(0);
	struct tm * now = localtime(&t);
	char buf[13];
	sprintf_s(buf, 13, "%04d%02d%02d%02d%02d", now->tm_year + 1900, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min);
	m_recordVideoIndex = 0;
	m_recordVideoPath = m_settingVideoRecordPath + "/" + buf + "/";
	std::wstring s2;
	s2.assign(m_recordVideoPath.begin(), m_recordVideoPath.end());
	_wmkdir(s2.c_str());
}


COpenGLControl::~COpenGLControl()
{
}

void COpenGLControl::oglCreate(CRect rect, CWnd * parent)
{
	m_oldWindow = rect;
	m_originalRect = rect;
	hWnd = parent;

	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL);
	CreateEx(0, className, L"OpenGL", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rect, parent, 0);

}

void COpenGLControl::oglSetTexture(RGBQUAD * img, int width, int height)
{
	if (!m_textureInit) {
		m_textureBackground = oglInitTexture(width, height);
		m_textureInit = true;
	}
	
	oglUpdateTexture(m_textureBackground, width, height, img);

}

void COpenGLControl::oglSetKinect(Kinect2Manager * km)
{
	m_kinectManager = km;
	m_kinectManager->InitializeDefaultSensor();
}

void COpenGLControl::Reset()
{
	m_fPosX = 0.f;
	m_fPosY = 0.f;
	m_fZoom = -10.f;
	m_fRotX = 0.f;
	m_fRotY = 0.f;
	m_fLastX = 0.f;
	m_fLastY = 0.f;
	m_useKinectRotation = false;
	m_animPause = false;
	m_recordJoints = false;
	m_countdownTimer = 0;
	m_recordVideo = false;
	m_sequenceName = "";

	time_t t = time(0);
	struct tm * now = localtime(&t);
	char buf[13];
	sprintf_s(buf, 13, "%04d%02d%02d%02d%02d", now->tm_year + 1900, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min);
	m_recordVideoPath = m_settingVideoRecordPath + "/" + buf + "/";
	m_recordVideoIndex = 0;
	std::wstring s2;
	s2.assign(m_recordVideoPath.begin(), m_recordVideoPath.end());
	_wmkdir(s2.c_str());
}

void COpenGLControl::ToggleVP()
{
	m_useKinectRotation = !m_useKinectRotation;
}

void COpenGLControl::LoadSequence(std::string sequence_name)
{
	if (sequence_name == "Test 1") {
		oglLoadTenbo("D:/TenboViewFiles/enzo-b-lr6-result/", "D:/TenboViewFiles/v_mapping_from_m-lr.arr", "D:/TenboViewFiles/uv-lr.nda", "D:/TenboViewFiles/faces-lr.nda", "D:/TenboViewFiles/enzo-b-lr6-texture/");

		m_animSkeletons.clear();
		for (int i = 40; i < 544; ++i) {
			m_animSkeletons.push_back(load_joints("D:/DumpKinect/enzo-b-1/joints" + std::to_string(i) + ".yml"));
		}
	}
	else if (sequence_name == "Test 2") {
		oglLoadTenbo("D:/TenboViewFiles/enzo-b-lr6-result/", "D:/TenboViewFiles/v_mapping_from_m-lr.arr", "D:/TenboViewFiles/uv-lr.nda", "D:/TenboViewFiles/faces-lr.nda", "D:/TenboViewFiles/enzo-b-lr6-texture/", 0, 100);

		m_animSkeletons.clear();
		for (int i = 40; i < 140; ++i) {
			m_animSkeletons.push_back(load_joints("D:/DumpKinect/enzo-b-1/joints" + std::to_string(i) + ".yml"));
		}
	}
	else if (sequence_name == "Test 3") {
		oglLoadVideo("D:/DumpKinect/enzo-b-1/", 0, 100);
	}
	else if (sequence_name == "Mirror 1") {

		oglLoadTenbo("D:/TenboViewFiles/max-c-k2-result-lr13-1/", "D:/TenboViewFiles/v_mapping_from_m-lr.arr", "D:/TenboViewFiles/uv-lr.nda", "D:/TenboViewFiles/faces-lr.nda", "D:/TenboViewFiles/max-c-k2-texture-2sides-strict-ave/", 0, 260);

		m_animSkeletons.clear();
		for (int i = 40; i < 300; ++i) {
			m_animSkeletons.push_back(load_joints("D:/DumpKinect/max-c-k2-1/joints" + std::to_string(i) + ".yml"));
		}
	}
	else if (sequence_name == "Video 1") {
		oglLoadVideo("D:/DumpKinect/max-c-k2-small/", 40, 300);
	}
	else if (sequence_name == "Mirror 2") {

		oglLoadTenbo("D:/TenboViewFiles/max-c-k2-result-lr13-1/", "D:/TenboViewFiles/v_mapping_from_m-lr.arr", "D:/TenboViewFiles/uv-lr.nda", "D:/TenboViewFiles/faces-lr.nda", "D:/TenboViewFiles/max-c-k2-texture-2sides-strict-ave/", 316, 436);

		m_animSkeletons.clear();
		for (int i = 356; i < 476; ++i) {
			m_animSkeletons.push_back(load_joints("D:/DumpKinect/max-c-k2-1/joints" + std::to_string(i) + ".yml"));
		}
	}
	else if (sequence_name == "Video 2") {
		oglLoadVideo("D:/DumpKinect/max-c-k2-small/", 356, 476);
	}
	else if (sequence_name == "Mirror 3") {

		oglLoadTenbo("D:/TenboViewFiles/max-c-k2-result-lr13-1/", "D:/TenboViewFiles/v_mapping_from_m-lr.arr", "D:/TenboViewFiles/uv-lr.nda", "D:/TenboViewFiles/faces-lr.nda", "D:/TenboViewFiles/max-c-k2-texture-2sides-strict-ave/", 916, 1060);

		m_animSkeletons.clear();
		for (int i = 956; i < 1100; ++i) {
			m_animSkeletons.push_back(load_joints("D:/DumpKinect/max-c-k2-1/joints" + std::to_string(i) + ".yml"));
		}
	}
	else if (sequence_name == "Video 3") {
		oglLoadVideo("D:/DumpKinect/max-c-k2-small/", 956, 1100);
	}
	else if (sequence_name == "Mirror 4") {

		oglLoadTenbo("D:/TenboViewFiles/max-c-k2-result-lr13-1/", "D:/TenboViewFiles/v_mapping_from_m-lr.arr", "D:/TenboViewFiles/uv-lr.nda", "D:/TenboViewFiles/faces-lr.nda", "D:/TenboViewFiles/max-c-k2-texture-2sides-strict-ave/", 1124, 1216);

		m_animSkeletons.clear();
		for (int i = 1164; i < 1256; ++i) {
			m_animSkeletons.push_back(load_joints("D:/DumpKinect/max-c-k2-1/joints" + std::to_string(i) + ".yml"));
		}
	}
	else if (sequence_name == "Video 4") {
		oglLoadVideo("D:/DumpKinect/max-c-k2-small/", 1164, 1256);
	}

	m_sequenceName = sequence_name;
}

void COpenGLControl::Pause()
{
	m_animPause = !m_animPause;
}

void COpenGLControl::Record(std::string suffix, bool start_record)
{
	if (m_recordJoints) {
		// stop recording

		m_recordJoints = false;
		//m_recordVideo = false;
	}

	if (start_record) {
		//m_recordVideo = true;
		RestartSequence();
		m_recordIndex = 0;
		m_recordSequence.clear();
		m_recordJoints = true;

		// save path
		time_t t = time(0);
		struct tm * now = localtime(&t);
		char buf[13];
		sprintf_s(buf, 13, "%04d%02d%02d%02d%02d", now->tm_year + 1900, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min);
		m_recordPath = m_settingRecordPath + "/" + buf + suffix + m_sequenceName;
		std::wstring s2;
		s2.assign(m_recordPath.begin(), m_recordPath.end());
		_wmkdir(s2.c_str());
	}
}

void COpenGLControl::ToggleMirror()
{
	m_renderMirror = !m_renderMirror;
}

void COpenGLControl::ToggleMesh()
{
	m_animRenderMesh = !m_animRenderMesh;
}

void COpenGLControl::ToggleTexture()
{
	m_animApplyTexture = !m_animApplyTexture;
}

void COpenGLControl::ToggleSkeleton()
{
	m_drawKinectSkeleton = !m_drawKinectSkeleton;
}

void COpenGLControl::RestartSequence()
{
	m_currentFrame = 0;
	m_countdownTimer = 90;
	m_recordJoints = false;
	m_useKinectRotation = true;
}


BEGIN_MESSAGE_MAP(COpenGLControl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void COpenGLControl::oglInitialize()
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		16,
		0, 0, 0, 0, 0, 0, 0
	};

	hdc = GetDC()->m_hDC;

	m_nPixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, m_nPixelFormat, &pfd);

	hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClearDepth(1.f);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	OnDraw(NULL);

	if (glewInit() != GLEW_OK) {
		exit(-1);
	}

	m_programTenbo = LoadShaders("ShadowVertexShader.vertexshader", "ShadowFragmentShader.fragmentshader");
	m_glslMVPTenbo = glGetUniformLocation(m_programTenbo, "MVP");
	m_glslNormalMatrix = glGetUniformLocation(m_programTenbo, "normalMatrix");
	m_glslTextureTenbo = glGetUniformLocation(m_programTenbo, "myTextureSampler");
	m_glslGlobalAmbient = glGetUniformLocation(m_programTenbo, "Global_ambient");
	m_glslMaterialAmbient = glGetUniformLocation(m_programTenbo, "material.ambient");
	m_glslMaterialDiffuse = glGetUniformLocation(m_programTenbo, "material.diffuse");
	m_glslMaterialSpecular = glGetUniformLocation(m_programTenbo, "material.specular");
	m_glslMaterialShininess = glGetUniformLocation(m_programTenbo, "material.shininess");
	m_glslLights = glGetUniformLocation(m_programTenbo, "lights");
	m_glslTextureWeight = glGetUniformLocation(m_programTenbo, "tex_weight");

	glGenBuffers(1, &m_vertexBufferTenbo);
	glGenBuffers(1, &m_uvBufferTenbo);
	glGenBuffers(1, &m_normalBufferTenbo);
	glGenTextures(1, &m_textureTenbo);

	m_programBackground = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	m_glslMVPBackground = glGetUniformLocation(m_programBackground, "MVP");
	m_glslTextureBackground = glGetUniformLocation(m_programBackground, "myTextureSampler");

	m_programFont = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFontFragmentShader.fragmentshader");
	m_glslMVPFont = glGetUniformLocation(m_programFont, "MVP");
	m_glslTextureFont = glGetUniformLocation(m_programFont, "myTextureSampler");

	m_programLine = LoadShaders("SimpleColorVertexShader.vertexshader", "SimpleColorFragmentShader.fragmentshader");
	m_glslMVPLine = glGetUniformLocation(m_programLine, "MVP");

	m_programLine3D = LoadShaders("SimpleColorVertexShader.vertexshader", "SimpleColorFragmentShader.fragmentshader");
	m_glslMVPLine3D = glGetUniformLocation(m_programLine3D, "MVP");

	float background_vertices[] = { -1.0f, -1.0f, 0.f, 1.0f, -1.0f, 0.f, 1.0f, 1.0f, 0.f, -1.0f, 1.0f, 0.f};
	float background_uvs[] = { 0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f };

	glGenBuffers(1, &m_vertexBufferBackground);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferBackground);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), background_vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &m_uvBufferBackground);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferBackground);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), background_uvs, GL_STATIC_DRAW);

	float font_vertices[] = { -0.1f, -0.8f, 0.f, 0.1f, -0.8f, 0.f, 0.1f, -0.6f, 0.f, -0.1f, -0.6f, 0.f };
	float font_uvs[32];
	char to_draw[] = " 123";

	for (int i = 0; i < 4; ++i) {
		char fchar = to_draw[i];
		int index = fchar - 32;
		int row = index / 8;
		int col = index % 8;
		int top = row * 64;
		int bot = top + 64;
		int left = col * 64;
		int right = left + 64;
		font_uvs[i * 8 + 0] = left / 512.;
		font_uvs[i * 8 + 1] = top / 512.;
		font_uvs[i * 8 + 2] = right / 512.;
		font_uvs[i * 8 + 3] = top / 512.;
		font_uvs[i * 8 + 4] = right / 512.;
		font_uvs[i * 8 + 5] = bot / 512.;
		font_uvs[i * 8 + 6] = left / 512.;
		font_uvs[i * 8 + 7] = bot / 512.;
	}

	glGenBuffers(1, &m_vertexBufferFont);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferFont);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), font_vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &m_uvBufferFont);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferFont);
	glBufferData(GL_ARRAY_BUFFER, 32 * sizeof(float), font_uvs, GL_STATIC_DRAW);

	glGenBuffers(1, &m_vertexBufferLine);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferLine);
	glBufferData(GL_ARRAY_BUFFER, 2 * NUM_SEGMENTS * 2 * 3 * sizeof(float), NULL, GL_STATIC_DRAW);

	float line_colors[2 * NUM_SEGMENTS * 2 * 4 * sizeof(float)];
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < NUM_SEGMENTS; ++j) {
			for (int k = 0; k < 2; ++k) {
				line_colors[i*NUM_SEGMENTS * 2 * 4 + j * 2 * 4 + k * 4 + 0] = i * 255.;
				line_colors[i*NUM_SEGMENTS * 2 * 4 + j * 2 * 4 + k * 4 + 1] = (1 - i) * 255.;
				line_colors[i*NUM_SEGMENTS * 2 * 4 + j * 2 * 4 + k * 4 + 2] = 0.;
				line_colors[i*NUM_SEGMENTS * 2 * 4 + j * 2 * 4 + k * 4 + 3] = 255.;
			}
		}
	}

	glGenBuffers(1, &m_colorBufferLine);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferLine);
	glBufferData(GL_ARRAY_BUFFER, 2 * NUM_SEGMENTS * 2 * 4 * sizeof(float), line_colors, GL_STATIC_DRAW);


	glGenBuffers(1, &m_vertexBufferLine3D);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferLine3D);
	glBufferData(GL_ARRAY_BUFFER, 2 * NUM_SEGMENTS * 2 * 3 * sizeof(float), NULL, GL_STATIC_DRAW);

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < NUM_SEGMENTS; ++j) {
			for (int k = 0; k < 2; ++k) {
				line_colors[i*NUM_SEGMENTS * 2 * 4 + j * 2 * 4 + k * 4 + 0] = (1 - i) * 255.;
				line_colors[i*NUM_SEGMENTS * 2 * 4 + j * 2 * 4 + k * 4 + 1] = i * 255.;
				line_colors[i*NUM_SEGMENTS * 2 * 4 + j * 2 * 4 + k * 4 + 2] = 0.;
				line_colors[i*NUM_SEGMENTS * 2 * 4 + j * 2 * 4 + k * 4 + 3] = 255.;
			}
		}
	}

	glGenBuffers(1, &m_colorBufferLine3D);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferLine3D);
	glBufferData(GL_ARRAY_BUFFER, 2 * NUM_SEGMENTS * 2 * 4 * sizeof(float), line_colors, GL_STATIC_DRAW);


	int texture_width, texture_height;
	unsigned char* image = SOIL_load_image("D:/TenboViewFiles/ExportedFont.bmp", &texture_width, &texture_height, 0, SOIL_LOAD_RGBA);

	m_textureFont = oglInitTexture(texture_width, texture_height);
	oglUpdateTexture(m_textureFont, texture_width, texture_height, (RGBQUAD*)image);
	SOIL_free_image_data(image);

	float width = m_oldWindow.Width();
	float height = m_oldWindow.Height();

	m_projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.0001f, 1000.0f);
	m_modelview = glm::lookAt(glm::vec3(0, 0, -5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

}

void COpenGLControl::oglDrawScene()
{
	glm::mat4 ortho = glm::ortho(-1, 1, 1, -1);
	int err;
	// background

	glUseProgram(m_programBackground);

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureBackground);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	// glUniform1i(m_glslTextureBackground, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferBackground);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferBackground);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);


	glUniformMatrix4fv(m_glslMVPBackground, 1, GL_FALSE, &ortho[0][0]);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindTexture(GL_TEXTURE_2D, 0);

	// font

	glUseProgram(m_programFont);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureFont);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(m_glslTextureFont, 0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferFont);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferFont);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)(m_fontState * 8 * sizeof(GLfloat)));

	glUniformMatrix4fv(m_glslMVPFont, 1, GL_FALSE, &ortho[0][0]);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);

	if (m_renderMirror) {

		m_modelview = oglGetModelView();
		glm::mat4 mvp = m_projection * m_modelview;

		if (m_animRenderMesh) {
			glUseProgram(m_programTenbo);

			// Bind our texture in Texture Unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_textureTenbo);
			// Set our "myTextureSampler" sampler to user Texture Unit 0
			// glUniform1i(m_glslTextureTenbo, 0);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferTenbo);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferTenbo);
			glVertexAttribPointer(
				1,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferTenbo);
			glVertexAttribPointer(
				2,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0);


			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);


			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_modelview)));

			glUniformMatrix4fv(m_glslMVPTenbo, 1, GL_FALSE, &mvp[0][0]);
			glUniformMatrix3fv(m_glslNormalMatrix, 1, GL_FALSE, &normalMatrix[0][0]);
			glUniform4fv(m_glslLights, 12, LIGHTS);
			glUniform4fv(m_glslGlobalAmbient, 1, GLOBAL_AMBIENT);
			glUniform4fv(m_glslMaterialAmbient, 1, MATERIAL_AMBIENT);
			glUniform4fv(m_glslMaterialDiffuse, 1, MATERIAL_DIFFUSE);
			glUniform4fv(m_glslMaterialSpecular, 1, MATERIAL_SPECULAR);
			glUniform1fv(m_glslMaterialShininess, 1, MATERIAL_SHININESS);

			float texture_weight = m_animApplyTexture ? 1 : 0;
			glUniform1f(m_glslTextureWeight, texture_weight);


			// Draw the triangle !
			// glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

			glDrawElements(GL_TRIANGLES, m_faces.size(), GL_UNSIGNED_INT, m_faces.data());

			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else {

			glUseProgram(m_programLine3D);
			glUniformMatrix4fv(m_glslMVPLine3D, 1, GL_FALSE, &mvp[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferLine3D);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferLine3D);
			glVertexAttribPointer(
				1,
				4,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0);

			glLineWidth(20.f);

			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDrawArrays(GL_LINES, 0, NUM_SEGMENTS * 2);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);
		}

		if (m_drawKinectSkeleton) {
			glUseProgram(m_programLine);
			glUniformMatrix4fv(m_glslMVPLine, 1, GL_FALSE, &ortho[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferLine);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferLine);
			glVertexAttribPointer(
				1,
				4,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void*)0);

			glLineWidth(20.f);

			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, 0);
			glDrawArrays(GL_LINES, 0, NUM_SEGMENTS * 2);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);
		}

	}

}

void COpenGLControl::oglLoadTenbo(std::string result_path, std::string mapping_filename, std::string uv_filename, std::string faces_filename, std::string texture_filepath, int start_idx, int stop_idx)
{
	m_renderMirror = true;
	m_animVerticesSequence.clear();
	m_animTextures.clear();
	RestartSequence();
	std::vector<std::tuple<int, int, int>> faces;
	int f = start_idx;

	int texture_width, texture_height;
	while (true) {
		// Read our .obj file
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals; // Won't be used at the moment.
		bool res = loadOBJsimple((result_path + "/" + std::to_string(f) + "-vert.obj").c_str(), vertices, faces);

		if (res) {
			m_animVerticesSequence.push_back(vertices);
			unsigned char* image = SOIL_load_image((texture_filepath + "/texture" + std::to_string(f) + ".png").c_str(), &texture_width, &texture_height, 0, SOIL_LOAD_RGBA);
			std::vector<unsigned char> im_vec(texture_width * texture_height*4);
			memcpy(im_vec.data(), image, texture_height*texture_width*4);
			SOIL_free_image_data(image);
			m_animTextures.push_back(im_vec);

			++f;

			if (stop_idx > -1 && f >= stop_idx) {
				break;
			}
		}
		else {
			break;
		}
	}


	// read mapping file
	read_arr(mapping_filename.c_str(), m_animMapping);

	// read uv file
	std::vector<GLfloat> uvs_gl;
	read_nda(uv_filename.c_str(), uvs_gl, true);
	for (int i = 0; i < uvs_gl.size(); i += 2) {
		uvs_gl[i + 1] = 1 - uvs_gl[i + 1];
	}

	// read faces
	std::vector<int> faces_nda;
	read_nda(faces_filename.c_str(), faces_nda, true);
	faces.clear();
	for (int i = 0; i < faces_nda.size(); i += 3) {
		faces.push_back(std::make_tuple(faces_nda[i], faces_nda[i + 1], faces_nda[i + 2]));
	}

	std::vector<GLuint> faces_gl;
	for (std::tuple<int, int, int> f : faces) {
		int v0, v1, v2;
		std::tie(v0, v1, v2) = f;
		faces_gl.push_back(v0);
		faces_gl.push_back(v1);
		faces_gl.push_back(v2);
	}

	// Load it into a VBO

	std::vector<glm::vec3> vertices_0 = remap_vector(m_animVerticesSequence[0], m_animMapping);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferTenbo);
	glBufferData(GL_ARRAY_BUFFER, vertices_0.size() * sizeof(glm::vec3), vertices_0.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferTenbo);
	glBufferData(GL_ARRAY_BUFFER, uvs_gl.size() * sizeof(float), uvs_gl.data(), GL_STATIC_DRAW);

	std::vector<glm::vec3> normals_0 = find_normals((float*)vertices_0.data(), vertices_0.size(), (int*)faces_gl.data(), faces_gl.size()/3);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferTenbo);
	glBufferData(GL_ARRAY_BUFFER, normals_0.size() * 3 * sizeof(float), normals_0.data(), GL_STATIC_DRAW);

	m_faces = faces_gl;

	// find center

	glm::vec3 center(0, 0, 0);
	for (glm::vec3 pt : vertices_0) {
		center += pt;
	}
	center /= vertices_0.size();
	m_rotationPoint = center;

	// texture
	if (!m_animTextures.empty()) {
		glBindTexture(GL_TEXTURE_2D, m_textureTenbo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_animTextures[0].data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void COpenGLControl::oglLoadVideo(std::string video_path, int start_frame, int stop_frame)
{
	m_renderMirror = false;
	m_videoFilenames.clear();
	RestartSequence();
	for (int i = start_frame; i < stop_frame; ++i) {
		m_videoFilenames.push_back(video_path + "/" + "rgba" + std::to_string(i) + ".png");
	}
}

glm::mat4 COpenGLControl::oglGetModelView()
{

	glm::mat4 rotation;
	float x, y, z;
	if(m_useKinectRotation){
		m_fRotX = XROT;
		m_fRotY = m_kinectRotationAngle;
		m_fPosX = m_kinectPosition.x + m_settingReenactmentOffsetX;
		m_fPosY = m_kinectPosition.y + m_settingReenactmentOffsetY;
		m_fZoom = m_kinectPosition.z + m_settingReenactmentOffsetZ;
	}
	else {
	}

	rotation = get_rotation(m_fRotX, 0, 0) * get_rotation(0, m_fRotY, 0);
	x = m_fPosX;
	y = m_fPosY;
	z = m_fZoom;

	// zero it
	glm::mat4 rel_rotation_base = glm::mat4(1.0f);
	rel_rotation_base[3][0] = x;
	rel_rotation_base[3][1] = y;
	rel_rotation_base[3][2] = z;

	glm::mat4 rel_rotation_inv = glm::mat4(1.0f);
	rel_rotation_inv[3][0] = -m_rotationPoint.x;
	rel_rotation_inv[3][1] = -m_rotationPoint.y;
	rel_rotation_inv[3][2] = -m_rotationPoint.z;

	return rel_rotation_base * rotation * rel_rotation_inv;
}

void COpenGLControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CWnd::OnPaint() for painting messages
}


int COpenGLControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	oglInitialize();
	return 0;
}

void COpenGLControl::OnDraw(CDC * pDC)
{
	/*glLoadIdentity();
	glTranslatef(0.f, 0.f, -m_fZoom);
	glTranslatef(m_fPosX, m_fPosY, 0.f);
	glRotatef(m_fRotX, 1.f, 0.f, 0.f);
	glRotatef(m_fRotY, 0.f, 1.f, 0.f);*/
}


void COpenGLControl::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) {
	case 1:
	{

		if (m_kinectManager) {
			m_kinectManager->Update(Update::Color | Update::Depth | Update::Body | Update::BodyIndex);

			if (m_kinectManager->getSkeletonIsGood()) {
				// get angle of body
				// right shoulder -> left shoulder
				Joint * skeleton_joints = m_kinectManager->GetJoints();
				Joint rs = skeleton_joints[JointType_ShoulderRight];
				Joint ls = skeleton_joints[JointType_ShoulderLeft];
				float angle = atan2f(rs.Position.Z - ls.Position.Z, rs.Position.X - ls.Position.X);
				m_kinectRotationAngle = -angle;
				Joint sp = skeleton_joints[JointType_SpineMid];
				m_kinectPosition = glm::vec3(sp.Position.X, sp.Position.Y, -sp.Position.Z);


				ICoordinateMapper * cm = m_kinectManager->getCoordinateMapper();
				if (cm) {
					int * kinectJointMap = getKinectJointMap();
					int * kinectSegments = getKinectSegments();

					Joint * jptr = m_kinectManager->GetJoints();

					std::vector<CameraSpacePoint> jcam(NUM_JOINTS);
					for (int j = 0; j < NUM_JOINTS; ++j)
					{
						jcam[j] = jptr[kinectJointMap[j]].Position;
					}
					std::vector<ColorSpacePoint> jcol(NUM_JOINTS);

					cm->MapCameraPointsToColorSpace(NUM_JOINTS, jcam.data(), NUM_JOINTS, jcol.data());

					float x_ratio = (2.f) / m_kinectOrigWidth;
					float y_ratio = (2.f) / m_kinectOrigHeight;

					float x_offset = -1.f;
					float y_offset = -1.f;

					// glColor3f(1, 0, 0);
					// glPointSize(10.f);
					// glBegin(GL_POINTS);
					// 
					// for (int j = 0; j < NUM_JOINTS; ++j) {
					// 	float x = jcol[j].X * x_ratio + x_offset;
					// 	float y = jcol[j].Y * y_ratio + y_offset;
					// 
					// 	glVertex2f(x, y);
					// }
					// 
					// glEnd();
					// 
					// glBegin(GL_LINES);
					// 
					// for (int j = 0; j < NUM_SEGMENTS; ++j) {
					// 	int j1 = kinectSegments[2 * j + 0];
					// 	int j2 = kinectSegments[2 * j + 1];
					// 
					// 	float x1 = jcol[j1].X * x_ratio + x_offset;
					// 	float y1 = jcol[j1].Y * y_ratio + y_offset;
					// 	float x2 = jcol[j2].X * x_ratio + x_offset;
					// 	float y2 = jcol[j2].Y * y_ratio + y_offset;
					// 
					// 	glVertex2f(x1, y1);
					// 	glVertex2f(x2, y2);
					// }
					// 
					// glEnd();

					float skeleton_vertex_buffer[NUM_SEGMENTS * 2 * 3];
					for (int j = 0; j < NUM_SEGMENTS; ++j) {
						int j1 = kinectSegments[2 * j + 0];
						int j2 = kinectSegments[2 * j + 1];

						float x1 = jcol[j1].X * x_ratio + x_offset;
						float y1 = jcol[j1].Y * y_ratio + y_offset;
						float x2 = jcol[j2].X * x_ratio + x_offset;
						float y2 = jcol[j2].Y * y_ratio + y_offset;

						skeleton_vertex_buffer[j * 2 * 3 + 0] = x1;
						skeleton_vertex_buffer[j * 2 * 3 + 1] = y1;
						skeleton_vertex_buffer[j * 2 * 3 + 2] = 0.;
						skeleton_vertex_buffer[j * 2 * 3 + 3] = x2;
						skeleton_vertex_buffer[j * 2 * 3 + 4] = y2;
						skeleton_vertex_buffer[j * 2 * 3 + 5] = 0.;
					}

					oglUpdateBuffer(m_vertexBufferLine, skeleton_vertex_buffer, NUM_SEGMENTS * 2 * 3);
				}

				if (m_recordJoints) {
					JointFrame jf;
					jf.joints.resize(JointType_Count);
					jf.orientations.resize(JointType_Count);
					for (int j = 0; j < JointType_Count; ++j) {
						jf.joints[j] = skeleton_joints[j];
						jf.orientations[j] = m_kinectManager->GetJointOrientations()[j];
					}
					jf.color_time = m_kinectManager->GetColorTime();
					jf.depth_time = m_kinectManager->GetDepthTime();
					jf.lefthand = m_kinectManager->getHandLeftConfidence();
					jf.righthand = m_kinectManager->getHandRightConfidence();
					save_joints(m_recordPath + "/joints" + std::to_string(m_recordIndex) + ".yml", m_currentFrame, jf);
					++m_recordIndex;
				}
			}

			if (!m_animPause) {
				if (m_countdownTimer > 0) {
					m_countdownTimer--;
					m_fontState = m_countdownTimer / 30 + 1;
				}
				else {
					m_fontState = 0;
					++m_currentFrame;
					m_useKinectRotation = false;
				}
			}

			if (m_renderMirror) {
				if (!m_animVerticesSequence.empty()) {
					if (m_currentFrame >= m_animVerticesSequence.size()) {
						RestartSequence();
					}
					std::vector<glm::vec3> remapped = remap_vector(m_animVerticesSequence[m_currentFrame], m_animMapping);
					oglUpdateBuffer(m_vertexBufferTenbo, (float*)remapped.data(), remapped.size() * 3);

					std::vector<glm::vec3> normals_0 = find_normals((float*)remapped.data(), remapped.size(), (int*)m_faces.data(), m_faces.size() / 3);
					oglUpdateBuffer(m_normalBufferTenbo, (float*)normals_0.data(), normals_0.size() * 3);

					resize_rgbquad((RGBQUAD*)m_animTextures[m_currentFrame].data(), 1024, 1024, swaprb, 1024, 1024, true);
					oglUpdateTexture(m_textureTenbo, 1024, 1024, swaprb);
					

					int anim_skeleton_index = m_currentFrame < m_animSkeletons.size()-1 ? m_currentFrame : m_animSkeletons.size()-1;

					int * kinectJointMap = getKinectJointMap();
					int * kinectSegments = getKinectSegments();

					Joint * jptr = m_animSkeletons[anim_skeleton_index].data();
					std::vector<glm::vec3> jv3(NUM_JOINTS);
					for (int j = 0; j < NUM_JOINTS; ++j)
					{
						jv3[j].x = jptr[kinectJointMap[j]].Position.X;
						jv3[j].y = jptr[kinectJointMap[j]].Position.Y;
						jv3[j].z = -jptr[kinectJointMap[j]].Position.Z;

					}

					std::vector<glm::vec3> segv3(NUM_SEGMENTS * 2);
					for (int j = 0; j < NUM_SEGMENTS; ++j) {
						int j1 = kinectSegments[j * 2 + 0];
						int j2 = kinectSegments[j * 2 + 1];
						segv3[j * 2 + 0] = jv3[j1];
						segv3[j * 2 + 1] = jv3[j2];
					}

					oglUpdateBuffer(m_vertexBufferLine3D, (float*)segv3.data(), NUM_SEGMENTS * 2 * 3);
					
				}

				RGBQUAD * rgbx = m_kinectManager->GetColorRGBX();
				int width = m_kinectManager->getColorWidth();
				int height = m_kinectManager->getColorHeight();

				if (width > 0 && height > 0) {

					//resize texture (faster)
					int nWidth = 2048;
					int nHeight = 1024;

					std::vector<RGBQUAD> rgbx_res(nWidth * nHeight);

					resize_rgbquad(rgbx, width, height, rgbx_res.data(), nWidth, nHeight);

					oglSetTexture(rgbx_res.data(), nWidth, nHeight);

					m_kinectOrigWidth = width;
					m_kinectOrigHeight = height;
					m_kinectNewWidth = nWidth;
					m_kinectNewHeight = nHeight;
					
					
					//record

					if (m_recordVideo) {
						if (m_recordVideoIndex % 2 == 0) {

							int sWidth = 1980 / 4;
							int sHeight = 1080 / 4;

							std::vector<RGBQUAD> rgbx_sm(sWidth * sHeight);
							resize_rgbquad(rgbx, width, height, rgbx_sm.data(), sWidth, sHeight, true);

							SOIL_save_image((m_recordVideoPath + "/" + std::to_string(m_recordVideoIndex) + ".bmp").c_str(), SOIL_SAVE_TYPE_BMP, sWidth, sHeight, 4, (unsigned char*)rgbx_sm.data());
						}
						m_recordVideoIndex++;
					}

				}
			}
			else {
				if (!m_videoFilenames.empty()) {
					if (m_currentFrame >= m_videoFilenames.size()) {
						RestartSequence();
					}

					int texture_width, texture_height;
					unsigned char* image = SOIL_load_image(m_videoFilenames[m_currentFrame].c_str(), &texture_width, &texture_height, 0, SOIL_LOAD_RGBA);

					int nWidth = 2048;
					int nHeight = 1024;

					std::vector<RGBQUAD> rgbx_res(nWidth * nHeight);

					resize_rgbquad((RGBQUAD*)image, texture_width, texture_height, rgbx_res.data(), nWidth, nHeight, true);

					oglSetTexture(rgbx_res.data(), nWidth, nHeight);
					SOIL_free_image_data(image);
				}
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		oglDrawScene();

		SwapBuffers(hdc);

		break;
	}

	default:
		break;
	}

	CWnd::OnTimer(nIDEvent);
}


void COpenGLControl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED) return;

	glViewport(0, 0, cx, cy);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(35.0f, (float)cx / (float)cy, 0.01f, 2000.0f);

	glMatrixMode(GL_MODELVIEW);
}


void COpenGLControl::OnMouseMove(UINT nFlags, CPoint point)
{
	int diffX = (int)(point.x - m_fLastX);
	int diffY = (int)(point.y - m_fLastY);
	m_fLastX = (float)point.x;
	m_fLastY = (float)point.y;

	if (nFlags & MK_LBUTTON) {
		m_fRotX += (float)m_settingRadiansPerPixel * diffY;
		float rotXmod = m_fRotX / (2 * PI);
		float temp;
		float rotXmodFrac = modf(rotXmod, &temp);
		m_fRotX = rotXmodFrac * (2 * PI);

		m_fRotY += (float)m_settingRadiansPerPixel * diffX;
		float rotYmod = m_fRotY / (2 * PI);
		float rotYmodFrac = modf(rotYmod, &temp);
		m_fRotY = rotYmodFrac * (2 * PI);

	}
	else if (nFlags & MK_RBUTTON) {
		m_fZoom -= (float)0.1f * diffY;
	}
	else if (nFlags & MK_MBUTTON) {
		m_fPosX += (float)0.05f * diffX;
		m_fPosY -= (float)0.05f * diffY;
	}

	OnDraw(NULL);

	CWnd::OnMouseMove(nFlags, point);
}

GLuint oglInitTexture(int width, int height)
{
	GLuint texture;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// when texture area is small, bilinear filter the closest mipmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_NEAREST);
	// when texture area is large, bilinear filter the original
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// the texture wraps over at the edges (repeat)
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, img);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, 0);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

	return texture;
}

void oglUpdateTexture(GLuint texture, int width, int height, RGBQUAD* img) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*) img);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void oglUpdateBuffer(GLuint buffer, float * vertices, int num_vertices, GLintptr offset) {
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, offset, num_vertices * sizeof(float), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::mat4 get_rotation(float x, float y, float z) {
	return glm::mat4(cos(y)*cos(z), -cos(y)*sin(z), sin(y), 0, cos(x)*sin(z) + sin(x)*sin(y)*cos(z), cos(x)*cos(z) - sin(x)*sin(y)*sin(z), -sin(x)*cos(y), 0, sin(x)*sin(z) - cos(x)*sin(y)*cos(z), sin(x)*cos(z) + cos(x)*sin(y)*sin(z), cos(x)*cos(y), 0, 0, 0, 0, 1);
}

void save_joints(std::string filename, int frame, const JointFrame& jf)
{
	std::ofstream fout(filename);
	fout << "%YAML:1.0\n";
	fout << "frame: " << frame << std::endl;
	fout << "lefthand: " << jf.lefthand << std::endl;
	fout << "righthand: " << jf.righthand << std::endl;
	fout << "depth_time: " << std::to_string(jf.depth_time) << std::endl;
	fout << "color_time: " << std::to_string(jf.color_time) << std::endl;
	fout << "joints:" << std::endl;
	for (int i = 0; i < jf.joints.size(); ++i) {
		fout << "   - \n";
		fout << "      joint_type: " << jf.joints[i].JointType << std::endl;
		fout << "      position: " << "[ " << jf.joints[i].Position.X << ", " << jf.joints[i].Position.Y << ", " << jf.joints[i].Position.Z << ", " << "1. ]" << std::endl;
		fout << "      tracking_state: " << jf.joints[i].TrackingState << std::endl;
		fout << "      orientation: " << "[ " << jf.orientations[i].Orientation.x << ", " << jf.orientations[i].Orientation.y << ", " << jf.orientations[i].Orientation.z << ", " << jf.orientations[i].Orientation.w << " ]" << std::endl;
	}
}

std::vector<Joint> load_joints(std::string filename) {
	std::vector<Joint> joints;
	std::ifstream fin(filename);
	std::string temp;
	fin >> temp; // yaml
	fin >> temp >> temp; // frame: x
	fin >> temp >> temp; // lefthand: x
	fin >> temp >> temp; // righthand: x
	fin >> temp >> temp; // depth_time: x
	fin >> temp >> temp; // color_time: x
	fin >> temp; // joints
	JointOrientation orientation;
	while (true) {
		Joint joint;
		fin >> temp; // -
		if (temp.find('-') == -1) {
			break;
		}
		fin >> temp >> temp; // joint_type: x
		fin >> temp >> temp; // position: [
		fin >> joint.Position.X >> temp >> joint.Position.Y >> temp >> joint.Position.Z >> temp;
		fin >> temp >> temp; // 1. ]
		fin >> temp >> temp; // tracking_state: x
		fin >> temp >> temp; // orientation: [
		fin >> orientation.Orientation.x >> temp >> orientation.Orientation.y >> temp >> orientation.Orientation.z >> temp >> orientation.Orientation.w; 
		fin >> temp; // ]
		joints.push_back(joint);
	}

	return joints;
}

std::vector<glm::vec3> find_normals(float * vertices, int num_vertices, int * faces, int num_faces) {
	std::vector<glm::vec3> normals(num_vertices, glm::vec3(0, 0, 0));
	for (int f = 0; f < num_faces; ++f) {
		int vf0 = faces[f * 3 + 0];
		int vf1 = faces[f * 3 + 1];
		int vf2 = faces[f * 3 + 2];
		glm::vec3 v0(vertices[vf0 * 3 + 0], vertices[vf0 * 3 + 1], vertices[vf0 * 3 + 2]);
		glm::vec3 v1(vertices[vf1 * 3 + 0], vertices[vf1 * 3 + 1], vertices[vf1 * 3 + 2]);
		glm::vec3 v2(vertices[vf2 * 3 + 0], vertices[vf2 * 3 + 1], vertices[vf2 * 3 + 2]);
		glm::vec3 v01 = v1 - v0;
		glm::vec3 v02 = v2 - v0;
		glm::vec3 c = glm::normalize(glm::cross(v01, v02));
		
		normals[vf0] += c;
		normals[vf1] += c;
		normals[vf2] += c;
	}
	for (int i = 0; i < normals.size(); ++i) {
		normals[i] = glm::normalize(normals[i]);
	}
	return normals;
}