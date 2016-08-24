#include "stdafx.h"
#include "Kinect2Manager.h"
#include <Kinect.h>
#include <iostream>

#define _USE_KINECT

void DumpHR(HRESULT hr)
{
	//
	//if (hr < 0)
	//	hr += 0x100000000;
	//if (hr & 0x80000000)
	//	std::cout << ("Error code") << std::endl;
	//else
	//	std::cout << ("Success code") << std::endl;
	//
	//auto facility = (hr & 0x7FFF0000) >> 16;
	//
	//std::cout << "Facility " << facility << std::endl;
	//
	//auto scode = hr & 0x0000FFFF;
	//
	//std::cout << "SCode " << scode << std::endl;

}

void dump_buffer(const char * filename, const void * buffer, size_t size, size_t count) {
	FILE * file;

	if (fopen_s(&file, filename, "wb") == 0) {
		fwrite(buffer, size, count, file);
	}
	else {
		std::cout << "Problem dumping buffer\n";
	}

	fclose(file);
}

void load_buffer(const char * filename, void * buffer, size_t size, size_t count) {
	FILE * file;

	if (fopen_s(&file, filename, "r") == 0) {
		fread_s(buffer, size * count, size, count, file);
	}
	else {
		std::cout << "Problem loading buffer\n";
	}

	fclose(file);
}

void Kinect2Manager::DumpBuffers() {
	CreateDirectoryA("dump/", NULL);

	dump_buffer("dump/depth_rgbx.bin", m_pDepthRGBX, sizeof(RGBQUAD), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	dump_buffer("dump/depth.bin", m_pDepth, sizeof(USHORT), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	dump_buffer("dump/color_rgbx.bin", m_pColorRGBX, sizeof(RGBQUAD), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	dump_buffer("dump/color_depth_map.bin", m_pColorDepthMap, sizeof(DepthSpacePoint), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	dump_buffer("dump/depth_color_map_rgbx.bin", m_pDepthColorMap, sizeof(ColorSpacePoint), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	dump_buffer("dump/depth_x_mapped_to_color.bin", m_pDepthXMappedToColor, sizeof(int), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	dump_buffer("dump/depth_y_mapped_to_color.bin", m_pDepthYMappedToColor, sizeof(int), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	dump_buffer("dump/depth_mapped_to_color.bin", m_pDepthMappedToColor, sizeof(USHORT), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	dump_buffer("dump/color_x_mapped_to_depth.bin", m_pColorXMappedToDepth, sizeof(int), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	dump_buffer("dump/color_y_mapped_to_depth.bin", m_pColorYMappedToDepth, sizeof(int), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	dump_buffer("dump/color_mapped_to_depth.bin", m_pColorMappedToDepth, sizeof(RGBQUAD), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	dump_buffer("dump/body_index.bin", m_pBodyIndex, sizeof(unsigned char), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	dump_buffer("dump/joints.bin", m_pJoints, sizeof(Joint), JointType_Count);
	dump_buffer("dump/joint_orientations.bin", m_pJointOrientations, sizeof(JointOrientation), JointType_Count);
	dump_buffer("dump/best_body_index.bin", &m_nBodyIndex, sizeof(UCHAR), 1);

}

void Kinect2Manager::LoadBuffers() {

	load_buffer("dump/depth_rgbx.bin", m_pDepthRGBX, sizeof(RGBQUAD), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	load_buffer("dump/depth.bin", m_pDepth, sizeof(USHORT), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	load_buffer("dump/color_rgbx.bin", m_pColorRGBX, sizeof(RGBQUAD), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	load_buffer("dump/color_depth_map.bin", m_pColorDepthMap, sizeof(DepthSpacePoint), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	load_buffer("dump/depth_color_map_rgbx.bin", m_pDepthColorMap, sizeof(ColorSpacePoint), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	load_buffer("dump/depth_x_mapped_to_color.bin", m_pDepthXMappedToColor, sizeof(int), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	load_buffer("dump/depth_y_mapped_to_color.bin", m_pDepthYMappedToColor, sizeof(int), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	load_buffer("dump/depth_mapped_to_color.bin", m_pDepthMappedToColor, sizeof(USHORT), CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR);
	load_buffer("dump/color_x_mapped_to_depth.bin", m_pColorXMappedToDepth, sizeof(int), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	load_buffer("dump/color_y_mapped_to_depth.bin", m_pColorYMappedToDepth, sizeof(int), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	load_buffer("dump/color_mapped_to_depth.bin", m_pColorMappedToDepth, sizeof(RGBQUAD), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	load_buffer("dump/body_index.bin", m_pBodyIndex, sizeof(unsigned char), CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH);
	load_buffer("dump/joints.bin", m_pJoints, sizeof(Joint), JointType_Count);
	load_buffer("dump/joint_orientations.bin", m_pJointOrientations, sizeof(JointOrientation), JointType_Count);
	load_buffer("dump/best_body_index.bin", &m_nBodyIndex, sizeof(UCHAR), 1);

}

Kinect2Manager::Kinect2Manager() {

	m_pDepthRGBX = new RGBQUAD[CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH];
	m_pDepth = new USHORT[CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH];
	m_pColorRGBX = new RGBQUAD[CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR];
	m_pColorDepthMap = new DepthSpacePoint[CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR];
	m_pDepthColorMap = new ColorSpacePoint[CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH];
	m_pDepthXMappedToColor = new int[CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR];
	m_pDepthYMappedToColor = new int[CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR];
	m_pDepthMappedToColor = new USHORT[CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR];
	m_pColorXMappedToDepth = new int[CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH];
	m_pColorYMappedToDepth = new int[CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH];
	m_pColorMappedToDepth = new RGBQUAD[CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH];
	m_pBodyIndex = new unsigned char[CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH];

	m_pBodyColorRGBX = new RGBQUAD[CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR];
	m_pBodyDepthRGBX = new RGBQUAD[CAPTURE_SIZE_X_DEPTH * CAPTURE_SIZE_Y_DEPTH];

	m_pJoints = new Joint[JointType_Count];
	m_pJointOrientations = new JointOrientation[JointType_Count];

	m_nStartTime = 0;
	m_bCalculateDepthRGBX = false;
	m_bMapDepthToColor = true;
	m_bSkeletonIsGood = false;
	m_nBodyIndex = 0xff;
	m_bColorDepthMapCalculated = false;
}



Kinect2Manager::~Kinect2Manager() {
	if (m_pDepthRGBX) {
		delete[] m_pDepthRGBX;
		m_pDepthRGBX = NULL;
	}
	if (m_pDepth) {
		delete[] m_pDepth;
		m_pDepth = NULL;
	}
	if (m_pColorRGBX) {
		delete[] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}
	if (m_pColorDepthMap) {
		delete[] m_pColorDepthMap;
	}
	if (m_pDepthColorMap) {
		delete[] m_pDepthColorMap;
	}
	if (m_pDepthMappedToColor) {
		delete[] m_pDepthMappedToColor;
	}
	if (m_pDepthXMappedToColor) {
		delete[] m_pDepthXMappedToColor;
	}
	if (m_pDepthYMappedToColor) {
		delete[] m_pDepthYMappedToColor;
	}

	if (m_pColorMappedToDepth) {
		delete[] m_pColorMappedToDepth;
	}
	if (m_pColorXMappedToDepth) {
		delete[] m_pColorXMappedToDepth;
	}
	if (m_pColorYMappedToDepth) {
		delete[] m_pColorYMappedToDepth;
	}

	if (m_pBodyIndex) {
		delete[] m_pBodyIndex;
	}

	if (m_pBodyColorRGBX) {
		delete[] m_pBodyColorRGBX;
	}

	if (m_pBodyDepthRGBX) {
		delete[] m_pBodyDepthRGBX;
	}

	if (m_pJoints) {
		delete[] m_pJoints;
	}
	if (m_pJointOrientations) {
		delete[] m_pJointOrientations;
	}
}

HRESULT Kinect2Manager::InitializeDefaultSensor()
{
#ifdef _USE_KINECT

	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		//IBodyFrameSource* pBodyFrameSource = NULL;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr)) {
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_BodyIndex | FrameSourceTypes::FrameSourceTypes_Body,
				&m_pMultiSourceFrameReader);
		}

		//if (SUCCEEDED(hr))
		//{
		//	hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		//}
		//
		//if (SUCCEEDED(hr))
		//{
		//	hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		//}
		//
		//SafeRelease(pBodyFrameSource);
		//
		//
		//// get the color reader
		//IColorFrameSource* pColorFrameSource = NULL;
		//
		//if (SUCCEEDED(hr))
		//{
		//
		//	hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		//}
		//
		//if (SUCCEEDED(hr))
		//{
		//	hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
		//}
		//
		//SafeRelease(pColorFrameSource);
		//
		//
		//// get the depth reader
		//
		//IDepthFrameSource* pDepthFrameSource = NULL;
		//
		//if (SUCCEEDED(hr))
		//{
		//	hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		//}
		//
		//if (SUCCEEDED(hr))
		//{
		//	hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		//}
		//
		//SafeRelease(pDepthFrameSource);
		//
		////get the body frame index reader
		//
		//IBodyIndexFrameSource * pBodyIndexFrameSource = NULL;
		//
		//if (SUCCEEDED(hr)){
		//	hr = m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		//}
		//
		//if (SUCCEEDED(hr)){
		//	hr = pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);
		//}
		//
		//SafeRelease(pBodyIndexFrameSource);

	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		std::cout << "no ready Kinect found!";
		return E_FAIL;
	}

	return hr;
#else
	LoadBuffers();

	return 0;
#endif
}


HRESULT Kinect2Manager::InitializeDefaultSensorSeparateReaders()
{
#ifdef _USE_KINECT
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		SafeRelease(pBodyFrameSource);


		// get the color reader
		IColorFrameSource* pColorFrameSource = NULL;

		if (SUCCEEDED(hr))
		{

			hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
		}

		SafeRelease(pColorFrameSource);


		// get the depth reader

		IDepthFrameSource* pDepthFrameSource = NULL;

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		}

		SafeRelease(pDepthFrameSource);

		//get the body frame index reader

		IBodyIndexFrameSource * pBodyIndexFrameSource = NULL;

		if (SUCCEEDED(hr)) {
			hr = m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		}

		if (SUCCEEDED(hr)) {
			hr = pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);
		}

		SafeRelease(pBodyIndexFrameSource);

	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		std::cout << "no ready Kinect found!";
		return E_FAIL;
	}

	return hr;
#else
	LoadBuffers();
	return 0;
#endif
}

void Kinect2Manager::Update(unsigned int options) {

#ifdef _USE_KINECT

	IColorFrame * pColorFrame = NULL;
	IDepthFrame * pDepthFrame = NULL;
	IBodyFrame * pBodyFrame = NULL;
	IBodyIndexFrame * pBodyIndexFrame = NULL;
	IMultiSourceFrame * pMultiSourceFrame = NULL;

	m_nColorWidth = 0;
	m_nColorHeight = 0;
	m_nDepthWidth = 0;
	m_nDepthHeight = 0;

	m_bCalculateDepthRGBX = options & Update::DepthRGBX;

	int numUpdate = countUpdate(options);

	HRESULT hr = 1;

	if (numUpdate > 1) {
		hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
	}

	if (SUCCEEDED(hr)) {

		if (options & Update::Color) {
			IColorFrameReference* pColorFrameReference = NULL;

			if (numUpdate > 1) {
				hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);

				if (SUCCEEDED(hr))
				{
					hr = pColorFrameReference->AcquireFrame(&pColorFrame);
				}
			}
			else {
				hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);
			}

			if (SUCCEEDED(hr)) {
				m_bMapColorToDepth = options & Update::MapColorToDepth;
			}
			else {
				options &= ~Update::Color;
			}

			SafeRelease(pColorFrameReference);
		}
		if (options & Update::Depth) {
			IDepthFrameReference* pDepthFrameReference = NULL;

			if (numUpdate > 1) {

				hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
				if (SUCCEEDED(hr))
				{
					hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
				}
			}
			else {
				hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);
			}

			if (SUCCEEDED(hr)) {

				m_bMapDepthToColor = options & Update::MapDepthToColor;
			}
			else {
				options &= ~Update::Depth;
			}

			SafeRelease(pDepthFrameReference);
		}
		if (options & Update::Body) {
			IBodyFrameReference* pBodyFrameReference = NULL;

			if (numUpdate > 1) {

				hr = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameReference);
				if (SUCCEEDED(hr))
				{
					hr = pBodyFrameReference->AcquireFrame(&pBodyFrame);
				}
			}
			else {
				hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
			}
			SafeRelease(pBodyFrameReference);
		}



		if (options & Update::Color && options & Update::Depth) {




			if (options & Update::BodyIndex && options & Update::MapColorToDepth) {
				if (m_bDepthColorMapCalculated) {
					IBodyIndexFrameReference* pBodyIndexFrameReference = NULL;

					hr = pMultiSourceFrame->get_BodyIndexFrameReference(&pBodyIndexFrameReference);
					if (SUCCEEDED(hr))
					{
						hr = pBodyIndexFrameReference->AcquireFrame(&pBodyIndexFrame);
					}

					SafeRelease(pBodyIndexFrameReference);
				}
			}
		}
	}

	if (pBodyFrame) {
		UpdateBody(pBodyFrame);

	}
	if (pColorFrame) {
		UpdateColor(pColorFrame);
	}

	if (pDepthFrame) {
		UpdateDepth(pDepthFrame);


		if (options & Update::MapDepthToColor) {
			CalculateColorDepthMap();
			if (m_bColorDepthMapCalculated)
				ProcessDepthToColor(m_pDepth, m_nDepthWidth, m_nDepthHeight, m_pColorDepthMap, m_nColorWidth, m_nColorHeight);
		}

		if (options & Update::MapColorToDepth) {
			CalculateDepthColorMap();
			if (m_bDepthColorMapCalculated) {
				ProcessColorToDepth(m_pColorRGBX, m_nColorWidth, m_nColorHeight, m_pDepthColorMap, m_nDepthWidth, m_nDepthHeight);

				if (pBodyIndexFrame) {
					UpdateBodyIndex(pBodyIndexFrame);
				}
			}

		}
	}


	SafeRelease(pColorFrame);
	SafeRelease(pDepthFrame);
	SafeRelease(pBodyFrame);
	SafeRelease(pBodyIndexFrame);
	SafeRelease(pMultiSourceFrame);
#else

	m_nDepthWidth = CAPTURE_SIZE_X_DEPTH;
	m_nDepthHeight = CAPTURE_SIZE_Y_DEPTH;

	m_nColorWidth = CAPTURE_SIZE_X_COLOR;
	m_nColorHeight = CAPTURE_SIZE_Y_COLOR;

	if (options & Update::MapDepthToColor) {
		CalculateColorDepthMap();
		if (m_bColorDepthMapCalculated)
			ProcessDepthToColor(m_pDepth, m_nDepthWidth, m_nDepthHeight, m_pColorDepthMap, m_nColorWidth, m_nColorHeight);
	}

	if (options & Update::MapColorToDepth) {
		CalculateDepthColorMap();
		if (m_bDepthColorMapCalculated) {
			ProcessColorToDepth(m_pColorRGBX, m_nColorWidth, m_nColorHeight, m_pDepthColorMap, m_nDepthWidth, m_nDepthHeight);

		}

	}

	UpdateBodyIndex(NULL);

#endif
}

void Kinect2Manager::CalculateColorDepthMap() {
#ifdef _USE_KINECT
	HRESULT hr = m_pCoordinateMapper->MapColorFrameToDepthSpace(m_nDepthWidth*m_nDepthHeight, m_pDepth, m_nColorWidth*m_nColorHeight, m_pColorDepthMap);

	m_bColorDepthMapCalculated = SUCCEEDED(hr);
#else
	m_bColorDepthMapCalculated = true;
#endif
}

void Kinect2Manager::CalculateDepthColorMap() {
#ifdef _USE_KINECT
	HRESULT hr = m_pCoordinateMapper->MapDepthFrameToColorSpace(m_nDepthWidth*m_nDepthHeight, m_pDepth, m_nDepthWidth*m_nDepthHeight, m_pDepthColorMap);

	m_bDepthColorMapCalculated = SUCCEEDED(hr);
#else
	m_bDepthColorMapCalculated = true;
#endif
}

//after calling this, get the depth fram with GetDepth or GetDepthRGBX
void Kinect2Manager::UpdateDepth(IDepthFrame* pDepthFrame) {
#ifdef _USE_KINECT
	INT64 nTime = 0;
	IFrameDescription* pFrameDescription = NULL;
	int nWidth = 0;
	int nHeight = 0;
	USHORT nDepthMinReliableDistance = 0;
	USHORT nDepthMaxReliableDistance = 0;
	UINT nBufferSize = 0;
	UINT16 *pBuffer = NULL;

	HRESULT hr = pDepthFrame->get_RelativeTime(&nTime);

	if (SUCCEEDED(hr))
	{

		m_nDepthTime = nTime;
		hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
	}

	if (SUCCEEDED(hr))
	{
		hr = pFrameDescription->get_Width(&nWidth);
	}

	if (SUCCEEDED(hr))
	{
		m_nDepthWidth = nWidth;
		hr = pFrameDescription->get_Height(&nHeight);
	}

	if (SUCCEEDED(hr))
	{
		m_nDepthHeight = nHeight;
		hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
	}

	if (SUCCEEDED(hr))
	{
		hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxReliableDistance);
	}

	if (SUCCEEDED(hr))
	{
		hr = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
	}

	if (SUCCEEDED(hr))
	{

		if (m_bCalculateDepthRGBX)
			ProcessDepth(nTime, pBuffer, nWidth, nHeight, nDepthMinReliableDistance, nDepthMaxReliableDistance);
		else
			ProcessDepthNoRGBX(nTime, pBuffer, nWidth, nHeight, nDepthMinReliableDistance, nDepthMaxReliableDistance);

	}

	SafeRelease(pFrameDescription);
#else
#endif
}

void Kinect2Manager::ProcessDepth(INT64 nTime, const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth)
{
	if (!m_nStartTime)
	{
		m_nStartTime = nTime;
	}

	// Make sure we've received valid data
	if (m_pDepthRGBX && m_pDepth && pBuffer)
	{
		RGBQUAD* pRGBX = m_pDepthRGBX;
		USHORT * pDepth = m_pDepth;

		// end pixel is start + width*height - 1
		const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

		while (pBuffer < pBufferEnd)
		{
			USHORT depth = *pBuffer;
			*pDepth = depth;

			// To convert to a byte, we're discarding the most-significant
			// rather than least-significant bits.
			// We're preserving detail, although the intensity will "wrap."
			// Values outside the reliable depth range are mapped to 0 (black).

			// Note: Using conditionals in this loop could degrade performance.
			// Consider using a lookup table instead when writing production code.
			BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);

			pRGBX->rgbRed = intensity;
			pRGBX->rgbGreen = intensity;
			pRGBX->rgbBlue = intensity;

			++pRGBX;
			++pDepth;
			++pBuffer;
		}

	}
}

void Kinect2Manager::ProcessDepthNoRGBX(INT64 nTime, const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth)
{
	if (!m_nStartTime)
	{
		m_nStartTime = nTime;
	}

	// Make sure we've received valid data
	if (m_pDepth && pBuffer)
	{
		USHORT * pDepth = m_pDepth;

		// end pixel is start + width*height - 1
		const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

		while (pBuffer < pBufferEnd)
		{
			USHORT depth = *pBuffer;
			*pDepth = depth;

			++pDepth;
			++pBuffer;
		}

	}
}

void Kinect2Manager::ProcessDepthToColor(const USHORT * pDepthBuffer, int nDepthWidth, int nDepthHeight, const DepthSpacePoint * pColorDepthMap, int nColorWidth, int nColorHeight) {
	int colorSize = nColorWidth * nColorHeight;

	if (pDepthBuffer && pColorDepthMap) {
		USHORT * pDepthMappedToColor = m_pDepthMappedToColor;
		int * pDepthXMappedToColor = m_pDepthXMappedToColor;
		int * pDepthYMappedToColor = m_pDepthYMappedToColor;

		const DepthSpacePoint * pBufferEnd = pColorDepthMap + colorSize;

		while (pColorDepthMap < pBufferEnd) {
			DepthSpacePoint depthSpacePoint = *pColorDepthMap;
			int pointerValue = (depthSpacePoint.X + 0.5) + (depthSpacePoint.Y + 0.5) * nDepthWidth;
			*pDepthMappedToColor = pDepthBuffer[pointerValue];
			*pDepthXMappedToColor = depthSpacePoint.X;
			*pDepthYMappedToColor = depthSpacePoint.Y;

			++pColorDepthMap;
			++pDepthMappedToColor;
			++pDepthXMappedToColor;
			++pDepthYMappedToColor;
		}
	}
}


void Kinect2Manager::ProcessColorToDepth(const RGBQUAD * pColorBuffer, int nColorWidth, int nColorHeight, const ColorSpacePoint * pDepthColorMap, int nDepthWidth, int nDepthHeight) {
	int depthSize = nDepthWidth * nDepthHeight;
	int colorSize = nColorWidth * nColorHeight;

	if (pColorBuffer && pDepthColorMap) {
		RGBQUAD * pColorMappedToDepth = m_pColorMappedToDepth;
		int * pColorXMappedToDepth = m_pColorXMappedToDepth;
		int * pColorYMappedToDepth = m_pColorYMappedToDepth;

		const ColorSpacePoint * pBufferEnd = pDepthColorMap + depthSize;

		while (pDepthColorMap < pBufferEnd) {
			ColorSpacePoint colorSpacePoint = *pDepthColorMap;
			int colorX = colorSpacePoint.X + 0.5;
			int colorY = colorSpacePoint.Y + 0.5;
			int pointerValue = colorX + colorY * nColorWidth;

			if (colorX >= 0 && colorX< nColorWidth &&
				colorY >= 0 && colorY < nColorHeight)
			{
				*pColorMappedToDepth = pColorBuffer[pointerValue];
				*pColorXMappedToDepth = colorSpacePoint.X;
				*pColorYMappedToDepth = colorSpacePoint.Y;
			}

			++pDepthColorMap;
			++pColorMappedToDepth;
			++pColorXMappedToDepth;
			++pColorYMappedToDepth;
		}
	}
}

void Kinect2Manager::UpdateColor(IColorFrame* pColorFrame)
{
#ifdef _USE_KINECT
	INT64 nTime = 0;
	IFrameDescription* pFrameDescription = NULL;
	int nWidth = 0;
	int nHeight = 0;
	ColorImageFormat imageFormat = ColorImageFormat_None;
	UINT nBufferSize = 0;
	RGBQUAD *pBuffer = NULL;

	HRESULT hr = pColorFrame->get_RelativeTime(&nTime);

	if (SUCCEEDED(hr))
	{

		m_nColorTime = nTime;
		hr = pColorFrame->get_FrameDescription(&pFrameDescription);
	}

	if (SUCCEEDED(hr))
	{
		hr = pFrameDescription->get_Width(&nWidth);
	}

	if (SUCCEEDED(hr))
	{
		m_nColorWidth = nWidth;
		hr = pFrameDescription->get_Height(&nHeight);
	}

	if (SUCCEEDED(hr))
	{
		m_nColorHeight = nHeight;
		hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
	}

	if (SUCCEEDED(hr))
	{
		if (imageFormat == ColorImageFormat_Bgra)
		{
			hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));
		}
		else if (m_pColorRGBX)
		{
			pBuffer = m_pColorRGBX;
			nBufferSize = nWidth * nHeight * sizeof(RGBQUAD);
			hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Bgra);
		}
		else
		{
			hr = E_FAIL;
		}
	}

	if (SUCCEEDED(hr))
	{
		{
			ProcessColor(nTime, pBuffer, nWidth, nHeight);

		}
	}

	SafeRelease(pFrameDescription);
#else
#endif
}

void Kinect2Manager::ProcessColor(INT64 nTime, RGBQUAD* pBuffer, int nWidth, int nHeight)
{
	if (!m_nStartTime)
	{
		m_nStartTime = nTime;
	}

	// Make sure we've received valid data
	if (pBuffer)
	{
		RGBQUAD * pColor = m_pColorRGBX;

		// end pixel is start + width*height - 1
		const RGBQUAD * pBufferEnd = pBuffer + (nWidth * nHeight);

		while (pBuffer < pBufferEnd)
		{
			RGBQUAD color = *pBuffer;
			*pColor = color;

			++pColor;
			++pBuffer;
		}
	}
}

void Kinect2Manager::UpdateBody(IBodyFrame* pBodyFrame)
{
#ifdef _USE_KINECT
	INT64 nTime = 0;

	HRESULT hr = pBodyFrame->get_RelativeTime(&nTime);

	IBody* ppBodies[BODY_COUNT] = { 0 };

	if (SUCCEEDED(hr))
	{
		hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
	}

	if (SUCCEEDED(hr))
	{
		ProcessBody(nTime, _countof(ppBodies), ppBodies);
	}

	for (int i = 0; i < _countof(ppBodies); ++i)
	{
		SafeRelease(ppBodies[i]);
	}
#else
#endif
}

void Kinect2Manager::ProcessBody(unsigned int nTime, unsigned int nBodyCount, IBody * ppBodies[6]) {
#ifdef _USE_KINECT
	UCHAR bestBody = 0xff;
	float bestScore = 0;

	float trackingStateTable[3];
	trackingStateTable[TrackingState_Inferred] = 0.5;
	trackingStateTable[TrackingState_NotTracked] = 0;
	trackingStateTable[TrackingState_Tracked] = 1;

	for (int i = 0; i<nBodyCount; ++i) {
		IBody * body = ppBodies[i];

		BOOLEAN bodyTracked;
		HRESULT hr = body->get_IsTracked(&bodyTracked);

		if (!SUCCEEDED(hr) || !bodyTracked) continue;

		Joint joints[JointType_Count];
		hr = body->GetJoints(JointType_Count, joints);

		if (!SUCCEEDED(hr)) continue;

		float score = 0;
		for (int j = 0; j<JointType_Count; ++j) {
			score += trackingStateTable[joints[j].TrackingState];
		}

		if (score > bestScore) {
			bestScore = score;
			bestBody = i;
		}
	}

	if (bestBody == 0xff) {
		m_bSkeletonIsGood = false;
		m_nBodyIndex = 0xff;
		return;
	}
	HRESULT hr = ppBodies[bestBody]->GetJoints(JointType_Count, m_pJoints);

	if (!SUCCEEDED(hr)) {
		std::cerr << "Error saving joints\n";
		m_bSkeletonIsGood = false;
		m_nBodyIndex = 0xff;
		return;
	}

	hr = ppBodies[bestBody]->GetJointOrientations(JointType_Count, m_pJointOrientations);

	if (!SUCCEEDED(hr)) {
		std::cerr << "Error saving joint orientations\n";
		m_bSkeletonIsGood = false;
		m_nBodyIndex = 0xff;
		return;
	}

	m_bSkeletonIsGood = true;
	m_nBodyIndex = bestBody;

	TrackingConfidence handLeftConfidence;
	hr = ppBodies[bestBody]->get_HandLeftConfidence(&handLeftConfidence);
	if (!SUCCEEDED(hr)) {
		handLeftConfidence = TrackingConfidence::TrackingConfidence_Low;
	}
	m_nHandLeftConfidence = handLeftConfidence;


	TrackingConfidence handRightConfidence;
	hr = ppBodies[bestBody]->get_HandRightConfidence(&handRightConfidence);
	if (!SUCCEEDED(hr)) {
		handRightConfidence = TrackingConfidence::TrackingConfidence_Low;
	}
	m_nHandRightConfidence = handRightConfidence;
#else
#endif
}

void Kinect2Manager::UpdateBodyIndex(IBodyIndexFrame *  pBodyIndexFrame) {
#ifdef _USE_KINECT
	if (m_nDepthWidth == 0 || m_nDepthHeight == 0 || m_nColorWidth == 0 || m_nColorHeight == 0) {

		return;
	}

	if (m_nBodyIndex == 0xff) {
		return;
	}

	unsigned int nBufferSize = 0;
	unsigned char * pBuffer = NULL;

	HRESULT hr, hr2;
	hr = pBodyIndexFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);

	//if (SUCCEEDED(hr)){
	//	hr2 = m_pCoordinateMapper->MapColorFrameToDepthSpace(m_nDepthWidth*m_nDepthHeight, m_pDepth, m_nColorWidth*m_nColorHeight, m_pColorDepthMap);
	//
	//}

	if (SUCCEEDED(hr)) {
		ProcessBodyFrameIndexColor(pBuffer, m_nDepthWidth, m_nDepthHeight, m_pColorDepthMap, m_nColorWidth, m_nColorHeight);
		ProcessBodyFrameIndexDepth(pBuffer, m_nDepthWidth, m_nDepthHeight, m_pDepthColorMap, m_nColorWidth, m_nColorHeight);
	}
#else
	ProcessBodyFrameIndexColor(m_pBodyIndex, m_nDepthWidth, m_nDepthHeight, m_pColorDepthMap, m_nColorWidth, m_nColorHeight);
	ProcessBodyFrameIndexDepth(m_pBodyIndex, m_nDepthWidth, m_nDepthHeight, m_pDepthColorMap, m_nColorWidth, m_nColorHeight);


#endif
}

void Kinect2Manager::ProcessBodyFrameIndexColor(unsigned char * pBodyIndexBuffer, unsigned int nDepthWidth, unsigned int nDepthHeight, const DepthSpacePoint * pColorDepthMap, int nColorWidth, int nColorHeight) {

	int colorSize = nColorWidth * nColorHeight;

	if (pBodyIndexBuffer && pColorDepthMap) {
		RGBQUAD * pBodyColorRGBX = m_pBodyColorRGBX;
		RGBQUAD * pColorRGBX = m_pColorRGBX;

		const DepthSpacePoint * pBufferEnd = pColorDepthMap + colorSize;

		while (pColorDepthMap < pBufferEnd) {
			DepthSpacePoint depthSpacePoint = *pColorDepthMap;
			int pointerValue = depthSpacePoint.X + depthSpacePoint.Y * nDepthWidth;
			if (pointerValue > 0) {
				unsigned char bodyIndex = pBodyIndexBuffer[pointerValue];

				if (bodyIndex == m_nBodyIndex)
					*pBodyColorRGBX = *pColorRGBX;
				else {
					pBodyColorRGBX->rgbBlue = 0xff;
					pBodyColorRGBX->rgbGreen = 0xff;
					pBodyColorRGBX->rgbRed = 0xff;
					pBodyColorRGBX->rgbReserved = 0;
				}
			}
			else {
				pBodyColorRGBX->rgbBlue = 0xff;
				pBodyColorRGBX->rgbGreen = 0xff;
				pBodyColorRGBX->rgbRed = 0xff;
				pBodyColorRGBX->rgbReserved = 0;

			}

			++pColorDepthMap;
			++pBodyColorRGBX;
			++pColorRGBX;
		}
	}
}

void Kinect2Manager::ProcessBodyFrameIndexDepth(unsigned char * pBodyIndexBuffer, unsigned int nDepthWidth, unsigned int nDepthHeight, const ColorSpacePoint * pDepthColorMap, int nColorWidth, int nColorHeight) {

	int depthSize = nDepthWidth * nDepthHeight;
	int colorSize = nColorWidth * nColorHeight;

	if (pBodyIndexBuffer && pDepthColorMap) {
		for (int i = 0; i < depthSize; ++i) {
			unsigned char bodyIndex = pBodyIndexBuffer[i];

			m_pBodyIndex[i] = bodyIndex;

			ColorSpacePoint depthColorPoint = pDepthColorMap[i];
			int colorX = depthColorPoint.X + 0.5, colorY = depthColorPoint.Y + 0.5;
			int colorPointer = colorX + colorY*nColorWidth;

			if (colorX >= 0 && colorX < nColorWidth
				&& colorY >= 0 && colorY < nColorHeight
				&& bodyIndex == m_nBodyIndex
				) {
				m_pBodyDepthRGBX[i] = m_pColorRGBX[colorPointer];
			}
			else {

				m_pBodyDepthRGBX[i].rgbBlue = 0xff;
				m_pBodyDepthRGBX[i].rgbGreen = 0xff;
				m_pBodyDepthRGBX[i].rgbRed = 0xff;
				m_pBodyDepthRGBX[i].rgbReserved = 0;
			}

		}
	}
}

//legacy
//void mapDepthToSkeleton(float * nDepthX, float * nDepthY, long * nDepth, float * nCameraX, float * nCameraY, float * nCameraZ){
//	if (!m_pCoordinateMapper){
//		return;
//	}
//
//	DepthSpacePoint depthSpacePoint;
//	depthSpacePoint.X = *nDepthX;
//	depthSpacePoint.Y = *nDepthY;
//
//	CameraSpacePoint cameraSpacePoint;
//	m_pCoordinateMapper->MapDepthPointToCameraSpace(depthSpacePoint, *nDepth, &cameraSpacePoint);
//
//	*nCameraX = cameraSpacePoint.X;
//	*nCameraY = cameraSpacePoint.Y;
//	*nCameraZ = cameraSpacePoint.Z;
//
//}

RGBQUAD * Kinect2Manager::GetDepthRGBX() {
	return m_pDepthRGBX;
}

USHORT * Kinect2Manager::GetDepth() {
	return m_pDepth;
}

RGBQUAD * Kinect2Manager::GetColorRGBX() {
	return m_pColorRGBX;
}

INT64 Kinect2Manager::GetDepthTime() {
	return m_nDepthTime;
}

INT64 Kinect2Manager::GetColorTime() {
	return m_nColorTime;
}

USHORT * Kinect2Manager::GetDepthMappedToColor() {
	return m_pDepthMappedToColor;
}

int * Kinect2Manager::GetDepthXMappedToColor() {
	return m_pDepthXMappedToColor;
}
int * Kinect2Manager::GetDepthYMappedToColor() {
	return m_pDepthYMappedToColor;
}


RGBQUAD * Kinect2Manager::GetColorMappedToDepth() {
	return m_pColorMappedToDepth;
}

int * Kinect2Manager::GetColorXMappedToDepth() {
	return m_pColorXMappedToDepth;
}
int * Kinect2Manager::GetColorYMappedToDepth() {
	return m_pColorYMappedToDepth;
}

RGBQUAD * Kinect2Manager::GetBodyColorRGBX() {
	return m_pBodyColorRGBX;
}
RGBQUAD * Kinect2Manager::GetBodyDepthRGBX() {
	return m_pBodyDepthRGBX;
}

Joint * Kinect2Manager::GetJoints() {
	return m_pJoints;
}

JointOrientation * Kinect2Manager::GetJointOrientations() {
	return m_pJointOrientations;
}


unsigned int Kinect2Manager::getDepthWidth() {
	return m_nDepthWidth;
}

unsigned int Kinect2Manager::getDepthHeight() {
	return m_nDepthHeight;
}

unsigned int Kinect2Manager::getColorWidth() {
	return m_nColorWidth;
}

unsigned int Kinect2Manager::getColorHeight() {
	return m_nColorHeight;
}

bool Kinect2Manager::getSkeletonIsGood() {
	return m_bSkeletonIsGood;
}

ICoordinateMapper * Kinect2Manager::getCoordinateMapper() {
	return m_pCoordinateMapper;
}

int Kinect2Manager::getHandLeftConfidence() {
	return m_nHandLeftConfidence;
}

int Kinect2Manager::getHandRightConfidence() {
	return m_nHandRightConfidence;
}

int countUpdate(unsigned int options) {
	int cnt = 0;
	if (options & Color) {
		++cnt;
	}
	if (options & (Depth | Body | BodyIndex | DepthRGBX)) {
		++cnt;
	}
	return cnt;
}