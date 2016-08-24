#pragma once
#include "afxwin.h"
#include <GL\glew.h>
#include <gl\GL.h>
#include <glm\glm.hpp>
#include <vector>

#include "Kinect2Manager.h"

#define PI 3.14159265

struct JointFrame {
	std::vector<Joint> joints;
	std::vector<JointOrientation> orientations;
	float lefthand;
	float righthand;
	long long depth_time;
	long long color_time;

};

class COpenGLControl :
	public CWnd
{
public:
	COpenGLControl();
	virtual ~COpenGLControl();

	void oglCreate(CRect rect, CWnd *parent);
	void oglSetTexture(RGBQUAD * img, int width, int height);
	void oglSetKinect(Kinect2Manager * kinect_manager);

	void Reset();
	void ToggleVP();
	void LoadSequence(std::string sequence_name);
	void Pause();
	void Record(std::string suffix, bool start_record=true);
	void ToggleMirror();
	void ToggleMesh();
	void ToggleTexture();
	void ToggleSkeleton();
	void RestartSequence();

	UINT_PTR m_unpTimer;

private:
	float m_settingRadiansPerPixel = 0.01f;
	float m_settingSecondsPerFrame = 1.f/33;
	float m_settingReenactmentOffsetX = 1.f;
	float m_settingReenactmentOffsetY = -0.25f;
	float m_settingReenactmentOffsetZ = -0.5f;
	std::string m_settingRecordPath = "D:/jointrecord/";
	std::string m_settingVideoRecordPath = "D:/vidrecord/";

	CWnd *hWnd;
	HDC hdc;
	HGLRC hrc;
	int m_nPixelFormat;
	CRect m_rect;
	CRect m_oldWindow;
	CRect m_originalRect;

	// tenbo shader

	GLuint m_programTenbo;
	GLuint m_vertexBufferTenbo;
	GLuint m_uvBufferTenbo;
	GLuint m_normalBufferTenbo;

	GLuint m_glslMVPTenbo;
	GLuint m_glslNormalMatrix;
	GLuint m_glslGlobalAmbient;
	GLuint m_glslMaterialAmbient;
	GLuint m_glslMaterialDiffuse;
	GLuint m_glslMaterialSpecular;
	GLuint m_glslMaterialShininess;
	GLuint m_glslLights;
	GLuint m_glslTextureWeight;

	glm::mat4 m_projection;
	glm::mat4 m_modelview;
	glm::vec3 m_rotationPoint;

	GLuint m_textureTenbo;
	GLuint m_glslTextureTenbo;
	
	// background shader

	GLuint m_programBackground;
	GLuint m_textureBackground;
	GLuint m_glslTextureBackground;
	GLuint m_vertexBufferBackground;
	GLuint m_uvBufferBackground;
	GLuint m_glslMVPBackground;

	// font shader

	GLuint m_programFont;
	GLuint m_textureFont;
	GLuint m_glslTextureFont;
	GLuint m_vertexBufferFont;
	GLuint m_uvBufferFont;
	GLuint m_glslMVPFont;

	// line shader

	GLuint m_programLine;
	GLuint m_vertexBufferLine;
	GLuint m_colorBufferLine;
	GLuint m_glslMVPLine;

	// line shader 3D

	GLuint m_programLine3D;
	GLuint m_vertexBufferLine3D;
	GLuint m_colorBufferLine3D;
	GLuint m_glslMVPLine3D;
	

	std::vector<GLuint> m_faces;

	float m_fPosX;
	float m_fPosY;
	float m_fZoom;
	float m_fRotX;
	float m_fRotY;
	float m_fLastX;
	float m_fLastY;

	int m_kinectOrigWidth;
	int m_kinectOrigHeight;
	int m_kinectNewWidth;
	int m_kinectNewHeight;

	bool m_textureInit;
	Kinect2Manager * m_kinectManager;

	bool m_drawKinectSkeleton;
	glm::mat4 m_kinectRotation;
	float m_kinectRotationAngle;
	bool m_useKinectRotation;
	glm::vec3 m_kinectPosition;

	// animation
	int m_currentFrame;
	std::vector<std::vector<glm::vec3>> m_animVerticesSequence;
	std::vector<int> m_animMapping;
	bool m_animPause;
	std::vector<std::vector<Joint>> m_animSkeletons;
	bool m_animRenderMesh;
	bool m_animApplyTexture;
	std::vector<std::vector<unsigned char>> m_animTextures;

	// video
	bool m_renderMirror;
	std::vector<std::string> m_videoFilenames;

	// font
	int m_fontState;

	// recording

	bool m_recordJoints;
	bool m_recordVideo;
	int m_recordIndex;
	int m_recordVideoIndex;
	std::string m_recordPath;
	std::string m_recordVideoPath;
	std::vector<std::vector<Joint>> m_recordSequence;
	std::string m_sequenceName;

	// countdown
	int m_countdownTimer;

	void oglInitialize();
	void oglDrawScene();
	void oglLoadTenbo(std::string result_path, std::string mapping_filename, std::string uv_filename, std::string faces_filename, std::string texture_filepath="", int start_idx=0, int stop_idx=-1);
	void oglLoadVideo(std::string video_path, int start_frame, int stop_frame);
	glm::mat4 oglGetModelView();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDraw(CDC *pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

GLuint oglInitTexture(int width, int height);
void oglUpdateTexture(GLuint texture, int width, int height, RGBQUAD* img);
void oglUpdateBuffer(GLuint buffer, float * vertices, int num_vertices, GLintptr offset=0);
glm::mat4 get_rotation(float x, float y, float z);
void save_joints(std::string filename, int frame, const JointFrame& jf);
std::vector<Joint> load_joints(std::string filename);
std::vector<glm::vec3> find_normals(float * vertices, int num_vertices, int * faces, int num_faces);