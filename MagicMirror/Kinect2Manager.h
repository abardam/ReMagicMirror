#pragma once

#include <Windows.h>
#include <Kinect.h>

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

#define CAPTURE_SIZE_X_COLOR 1920
#define CAPTURE_SIZE_Y_COLOR 1080
#define CAPTURE_SIZE_X_DEPTH 512
#define CAPTURE_SIZE_Y_DEPTH 424

enum Update {
	Color = 0x01,
	Depth = 0x02,
	Body = 0x04,
	BodyIndex = 0x08,
	DepthRGBX = 0x10,
	MapDepthToColor = 0x20,
	MapColorToDepth = 0x40,
};

int countUpdate(unsigned int options);

class Kinect2Manager {

public:
	Kinect2Manager();
	~Kinect2Manager();

	HRESULT InitializeDefaultSensor();
	HRESULT InitializeDefaultSensorSeparateReaders();


	void Update(unsigned int options);

	RGBQUAD * GetDepthRGBX();
	USHORT * GetDepth();
	RGBQUAD * GetColorRGBX();
	INT64 GetDepthTime();
	INT64 GetColorTime();
	USHORT * GetDepthMappedToColor();
	int * GetDepthXMappedToColor();
	int * GetDepthYMappedToColor();
	RGBQUAD * GetColorMappedToDepth();
	int * GetColorXMappedToDepth();
	int * GetColorYMappedToDepth();
	RGBQUAD * GetBodyColorRGBX();
	RGBQUAD * GetBodyDepthRGBX();
	Joint * GetJoints();
	JointOrientation * GetJointOrientations();

	unsigned int getDepthWidth();
	unsigned int getDepthHeight();
	unsigned int getColorWidth();
	unsigned int getColorHeight();
	bool getSkeletonIsGood();
	ICoordinateMapper * getCoordinateMapper();

	int getHandLeftConfidence();
	int getHandRightConfidence();

	//void mapDepthToSkeleton(float * nDepthX, float * nDepthY, long * nDepth,
	//	float * nCameraX, float * nCameraY, float * nCameraZ);

	unsigned char * m_pBodyIndex;

	void DumpBuffers();
	void LoadBuffers();

private:


	//call this, then get the depth frame with GetDepth or GetDepthRGBX
	void UpdateDepth(IDepthFrame *);
	//call this, then get the color frame with GetColorRGBX
	void UpdateColor(IColorFrame *);
	//call this, then get the body frame with GetJoints
	void UpdateBody(IBodyFrame *);
	//call this, then get the segmented body with GetBodyColorRGBX
	void UpdateBodyIndex(IBodyIndexFrame *);

	void ProcessDepth(INT64 nTime, const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth);

	void ProcessDepthNoRGBX(INT64 nTime, const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth);

	void ProcessDepthToColor(const USHORT * pDepthBuffer, int nDepthWidth, int nDepthHeight, const DepthSpacePoint * pColorDepthMap, int nColorWidth, int nColorHeight);
	void ProcessColorToDepth(const RGBQUAD * pColorBuffer, int nColorWidth, int nColorHeight, const ColorSpacePoint * pDepthColorMap, int nDepthWidth, int nDepthHeight);


	void ProcessColor(INT64 nTime, RGBQUAD* pBuffer, int nWidth, int nHeight);


	void ProcessBody(unsigned int nTime, unsigned int nBodyCount, IBody * ppBodies[6]);


	void ProcessBodyFrameIndexColor(unsigned char * pBodyIndexBuffer, unsigned int nDepthWidth, unsigned int nDepthHeight, const DepthSpacePoint * pColorDepthMap, int nColorWidth, int nColorHeight);
	void ProcessBodyFrameIndexDepth(unsigned char * pBodyIndexBuffer, unsigned int nDepthWidth, unsigned int nDepthHeight, const ColorSpacePoint * pDepthColorMap, int nColorWidth, int nColorHeight);

	void CalculateColorDepthMap();
	void CalculateDepthColorMap();



	IKinectSensor * m_pKinectSensor;
	ICoordinateMapper * m_pCoordinateMapper;

	IBodyFrameReader * m_pBodyFrameReader;
	IColorFrameReader * m_pColorFrameReader;
	IDepthFrameReader * m_pDepthFrameReader;
	IBodyIndexFrameReader * m_pBodyIndexFrameReader;

	IMultiSourceFrameReader * m_pMultiSourceFrameReader;

	INT64 m_nStartTime;
	INT64 m_nDepthTime;
	INT64 m_nColorTime;

	RGBQUAD * m_pDepthRGBX;
	USHORT * m_pDepth;
	RGBQUAD * m_pColorRGBX;
	DepthSpacePoint * m_pColorDepthMap;
	ColorSpacePoint * m_pDepthColorMap;
	USHORT * m_pDepthMappedToColor;
	int * m_pDepthXMappedToColor;
	int * m_pDepthYMappedToColor;
	RGBQUAD * m_pColorMappedToDepth;
	int * m_pColorXMappedToDepth;
	int * m_pColorYMappedToDepth;
	RGBQUAD * m_pBodyColorRGBX; //player RGB pixels, mapped to color image
	RGBQUAD * m_pBodyDepthRGBX; //player RGB pixels, mapped to depth image

	Joint * m_pJoints;
	JointOrientation * m_pJointOrientations;

	UCHAR m_nBodyIndex;

	unsigned int m_nDepthWidth;
	unsigned int m_nDepthHeight;
	unsigned int m_nColorWidth;
	unsigned int m_nColorHeight;


	bool m_bCalculateDepthRGBX;
	bool m_bMapDepthToColor;
	bool m_bMapColorToDepth;
	bool m_bSkeletonIsGood;
	bool m_bColorDepthMapCalculated;
	bool m_bDepthColorMapCalculated;

	int m_nHandLeftConfidence;
	int m_nHandRightConfidence;

	bool m_bBuffersLoaded;
};