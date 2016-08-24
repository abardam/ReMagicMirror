#pragma once
#include "stdafx.h"

HBITMAP rgbquad_to_hbitmap(RGBQUAD * rgbquad, int width, int height);
void resize_rgbquad(RGBQUAD * in, int in_w, int in_h, RGBQUAD * out, int out_w, int out_h, bool swap_rb=false);
void draw_pixel(RGBQUAD * in, int in_w, int in_h, int pt_x, int pt_y, RGBQUAD color);
void draw_circle(RGBQUAD * in, int in_w, int in_h, int c_x, int c_y, int c_r, RGBQUAD color);
