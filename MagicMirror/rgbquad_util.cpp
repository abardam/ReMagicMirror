#include "stdafx.h"

#include "rgbquad_util.h"
#include "Kinect2Manager.h"

// KINECT: function to return the Kinect input as HBITMAP.

unsigned char pixels[CAPTURE_SIZE_X_COLOR * CAPTURE_SIZE_Y_COLOR * 3];
HBITMAP rgbquad_to_hbitmap(RGBQUAD * rgbx, int width, int height) {
	if (width != 0 && height != 0) {

		for (int x = 0; x < width && x < CAPTURE_SIZE_X_COLOR; ++x) {
			for (int y = 0; y < height && y < CAPTURE_SIZE_Y_COLOR; ++y) {
				pixels[(y * CAPTURE_SIZE_X_COLOR + x) * 3 + 0] = (rgbx + (y * width + x))->rgbBlue;
				pixels[(y * CAPTURE_SIZE_X_COLOR + x) * 3 + 1] = (rgbx + (y * width + x))->rgbGreen;
				pixels[(y * CAPTURE_SIZE_X_COLOR + x) * 3 + 2] = (rgbx + (y * width + x))->rgbRed;
			}
		}

		BITMAPINFOHEADER bmih;
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = CAPTURE_SIZE_X_COLOR;
		bmih.biHeight = -CAPTURE_SIZE_Y_COLOR;
		bmih.biPlanes = 1;
		bmih.biBitCount = 24;
		bmih.biCompression = BI_RGB;
		bmih.biSizeImage = 0;
		bmih.biXPelsPerMeter = 10;
		bmih.biYPelsPerMeter = 10;
		bmih.biClrUsed = 0;
		bmih.biClrImportant = 0;

		BITMAPINFO dbmi;
		ZeroMemory(&dbmi, sizeof(dbmi));
		dbmi.bmiHeader = bmih;
		dbmi.bmiColors->rgbBlue = 0;
		dbmi.bmiColors->rgbGreen = 0;
		dbmi.bmiColors->rgbRed = 0;
		dbmi.bmiColors->rgbReserved = 0;
		void* bits = (void*)&(pixels[0]);

		// Create DIB
		HBITMAP hBmp = CreateDIBSection(0, &dbmi, DIB_RGB_COLORS, &bits, NULL, 0);
		if (hBmp == NULL) {
			::MessageBox(NULL, __T("Could not load the desired image image"), __T("Error"), MB_OK);
			return 0;
		}
		// copy pixels into DIB.
		memcpy(bits, pixels, sizeof(pixels));

		return hBmp;
	}

	return 0;
}

void resize_rgbquad(RGBQUAD * in, int in_w, int in_h, RGBQUAD * out, int out_w, int out_h, bool swap_rb)
{
	float w_ratio = (in_w + 0.0) / out_w;
	float h_ratio = (in_h + 0.0) / out_h;
	int in_x, in_y;

	for (int x = 0; x < out_w; ++x) {
		//int x2 = x + 1;
		for (int y = 0; y < out_h; ++y) {
			//int y2 = y + 1;

			//int in_x = x * w_ratio;
			//int in_x2 = x2 * w_ratio;
			//int in_y = y * h_ratio;
			//int in_y2 = y2 * h_ratio;
			in_x = x * w_ratio;
			in_y = y * h_ratio;
			// linear interpolation
			/*unsigned int r = 0, g = 0, b = 0;

			in_x2 = in_x2 <= in_x ? in_x + 1 : in_x2;
			in_y2 = in_y2 <= in_y ? in_y + 1 : in_y2;
			for (int i_x = in_x; i_x < in_x2; ++i_x) {
				for (int i_y = in_y; i_y < in_y2; ++i_y) {
					r += in[i_y * in_w + i_x].rgbRed;
					g += in[i_y * in_w + i_x].rgbGreen;
					b += in[i_y * in_w + i_x].rgbBlue;
				}
			}

			int div = (in_x2 - in_x) * (in_y2 - in_y);
			r /= div;
			g /= div;
			b /= div;
			
			out[y * out_w + x].rgbRed = r;
			out[y * out_w + x].rgbGreen = g;
			out[y * out_w + x].rgbBlue = b;
			*/

			//nearest neighbor (faster)
			out[y * out_w + x] = in[in_y * in_w + in_x];
			if (swap_rb) {
				BYTE blue = out[y * out_w + x].rgbBlue;
				BYTE red = out[y * out_w + x].rgbRed;
				out[y * out_w + x].rgbBlue = red;
				out[y * out_w + x].rgbRed = blue;
				
			}
		}
	}
}

void draw_pixel(RGBQUAD * in, int in_w, int in_h, int pt_x, int pt_y, RGBQUAD color) {
	if (pt_x >= 0 && pt_x < in_w && pt_y >= 0 && pt_y < in_h) {
		in[pt_y * in_w + pt_x] = color;
	}
}

void draw_circle(RGBQUAD * in, int in_w, int in_h, int c_x, int c_y, int c_r, RGBQUAD color) {
	int x = c_r;
	int y = 0;
	int dec_over_2 = 1 - x;

	while (y <= x) {
		draw_pixel(in, in_w, in_h,  x + c_x,  y + c_y, color);
		draw_pixel(in, in_w, in_h,  y + c_x,  x + c_y, color);
		draw_pixel(in, in_w, in_h, -x + c_x,  y + c_y, color);
		draw_pixel(in, in_w, in_h, -y + c_x,  x + c_y, color);
		draw_pixel(in, in_w, in_h, -x + c_x, -y + c_y, color);
		draw_pixel(in, in_w, in_h, -y + c_x, -x + c_y, color);
		draw_pixel(in, in_w, in_h,  x + c_x, -y + c_y, color);
		draw_pixel(in, in_w, in_h,  y + c_x, -x + c_y, color);
		++y;
		if (dec_over_2 <= 0) {
			dec_over_2 += 2 * y + 1;
		}
		else {
			--x;
			dec_over_2 += 2 * (y - x) + 1;
		}
	}
}