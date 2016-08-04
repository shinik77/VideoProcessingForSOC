#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>
#include "graphic_api.h"

#define PRINTLINE	printf("%s , %d \r\n",__FILE__,__LINE__)
#define PRINTVAR(A)	do{int ch;printf("%s=0x%x(%d)\r\n",#A,A,A);ch = getchar();} while (0);

#define AMAZON2_GRAPHIC_NAME  "/dev/amazon2_graphic"

#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		480

static int graphic_handle = -1;

static float __sintab[91] =
{
	0.0000000000000000f,	0.0174524064372835f,	0.0348994967025010f,	0.0523359562429438f,
	0.0697564737441253f,	0.0871557427476582f,	0.1045284632676535f,	0.1218693434051475f,
	0.1391731009600654f,	0.1564344650402309f,	0.1736481776669303f,	0.1908089953765448f,
	0.2079116908177593f,	0.2249510543438650f,	0.2419218955996677f,	0.2588190451025208f,
	0.2756373558169992f,	0.2923717047227367f,	0.3090169943749474f,	0.3255681544571567f,
	0.3420201433256687f,	0.3583679495453003f,	0.3746065934159120f,	0.3907311284892738f,
	0.4067366430758002f,	0.4226182617406994f,	0.4383711467890774f,	0.4539904997395468f,
	0.4694715627858908f,	0.4848096202463370f,	0.5000000000000000f,	0.5150380749100542f,
	0.5299192642332050f,	0.5446390350150271f,	0.5591929034707468f,	0.5735764363510461f,
	0.5877852522924731f,	0.6018150231520483f,	0.6156614753256583f,	0.6293203910498375f,
	0.6427876096865393f,	0.6560590289905073f,	0.6691306063588582f,	0.6819983600624985f,
	0.6946583704589973f,	0.7071067811865475f,	0.7193398003386511f,	0.7313537016191705f,
	0.7431448254773942f,	0.7547095802227720f,	0.7660444431189780f,	0.7771459614569709f,
	0.7880107536067220f,	0.7986355100472928f,	0.8090169943749474f,	0.8191520442889918f,
	0.8290375725550417f,	0.8386705679454240f,	0.8480480961564260f,	0.8571673007021123f,
	0.8660254037844386f,	0.8746197071393958f,	0.8829475928589269f,	0.8910065241883679f,
	0.8987940462991670f,	0.9063077870366500f,	0.9135454576426009f,	0.9205048534524403f,
	0.9271838545667874f,	0.9335804264972017f,	0.9396926207859084f,	0.9455185755993168f,
	0.9510565162951536f,	0.9563047559630355f,	0.9612616959383189f,	0.9659258262890683f,
	0.9702957262759965f,	0.9743700647852352f,	0.9781476007338056f,	0.9816271834476640f,
	0.9848077530122081f,	0.9876883405951377f,	0.9902680687415703f,	0.9925461516413220f,
	0.9945218953682733f,	0.9961946980917455f,	0.9975640502598242f,	0.9986295347545739f,
	0.9993908270190957f,	0.9998476951563912f,	1.0000000000000000f,
};

void draw_rectfill(int x, int y, int w, int h, EGL_COLOR clr)
{
	if (graphic_handle < 0)
		return;
	DrawRectFillArg arg;
	arg.rect.x = x;
	arg.rect.y = y;
	arg.rect.w = w;
	arg.rect.h = h;
	arg.clr = clr;
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_RECT_FILL, &arg);
}

void clear_screen(void)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_CLEAR_SCREEN, 0);
}

void flip(void)
{
	if (graphic_handle < 0)
		return;
	ioctl(graphic_handle, AMAZON2_IOCTL_FLIP, 0);
}
void flipwait(void)
{
	if (graphic_handle < 0)
		return;
	ioctl(graphic_handle, AMAZON2_IOCTL_FLIPWAIT, 0);
}

SURFACE* create_surface(int w, int h, int bpp)
{
	SURFACE* surf;
	CreateSurfaceArg arg;
	arg.w = w;
	arg.h = h;
	arg.bpp = bpp;
	surf = (SURFACE*)ioctl(graphic_handle, AMAZON2_IOCTL_CREATE_SURFACE, &arg);
	return surf;
}

void release_surface(SURFACE* surf)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_RELEASE_SURFACE, surf);
}
void draw_surface(SURFACE* src_surf, int dx, int dy)
{
	DrawSurfaceArg ar;
	ar.surf = src_surf;
	ar.dx = dx;
	ar.dy = dy;
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_SURFACE, &ar);
}
void draw_surfacerect(SURFACE* src_surf, int dx, int dy, EGL_RECT* pRect)
{
	DrawSurfaceRectArg ar;
	ar.surf = src_surf;
	ar.dx = dx;
	ar.dy = dy;
	memcpy(&ar.rect, pRect, sizeof(EGL_RECT));
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_SURFACE_RECT, &ar);
}
void draw_surface_scalerectrect(SURFACE* src_surf, EGL_RECT* pDestRect, EGL_RECT* pSrcRect)
{
	DrawSurfaceScaleRectArg ar;
	ar.surf = src_surf;
	memcpy(&ar.dest_rect, pDestRect, sizeof(EGL_RECT));
	memcpy(&ar.surf_rect, pSrcRect, sizeof(EGL_RECT));
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_SURFACE_RECT, &ar);
}

static float mysin(U32 angle)
{
	angle %= 360;

	if(angle <= 90)
		return __sintab[angle];
	else if(angle <= 180)
		return __sintab[180 - angle];
	else if(angle <= 270)
		return -__sintab[angle - 180];
	else
		return -__sintab[360 - angle];
}

static float mycos(U32 angle)
{
	return mysin(angle + 90);
}

int draw_rotate_value(int cdx, int cdy, int ctx, int cty, float zoom, unsigned int angle, DrawRaw_value* draw_value)
{
	float sinval, cosval;

	int InitDX, InitDY;
	int EndX, EndY;
	int InitSX, InitSY;
	int dxSx, dxSy;
	int dySx, dySy;
	int dx,dy;
	int x, y;
	int cosa;
	int sina;
	int rhzoom;
	long tx,ty;

	x = y = 0;
	dx = SCREEN_WIDTH;	// screen width (320)
	dy = SCREEN_HEIGHT;	// screen height (480)

	sinval = mysin(angle);
	cosval = mycos(angle);

	tx = (-cdx/zoom) * cosval + (-cdy/zoom) * sinval;
	ty = (cdx/zoom) * sinval +  (-cdy/zoom) * cosval;

	if( zoom<=0 )   rhzoom = 0;
	else            rhzoom = (int)((float)(1<<9)/zoom);
	cosa = (S32)(rhzoom * cosval);
	sina = (S32)(rhzoom * sinval);

	if( dx <= 0 || dy <= 0 )
		return -1;

	InitDX 	= x;
	InitDY 	= y;
	EndX	= x+dx-1;
	EndY	= y+dy-1;

	InitSX  = (x+tx+ctx)*512;
	dxSx    = cosa;
	dxSy    = -sina;

	InitSY  = (y+ty+cty)*512;
	dySx    = sina;
	dySy    = cosa;

	draw_value->InitDX=InitDX;
	draw_value->InitDY=InitDY;
	draw_value->EndX=EndX;
	draw_value->EndY=EndY;
	draw_value->InitSX=InitSX;
	draw_value->InitSY=InitSY;
	draw_value->dxSx=dxSx;
	draw_value->dxSy=dxSy;
	draw_value->dySx=dySx;
	draw_value->dySy=dySy;

	return 0;
}

void read_fpga_video_data(U16* buf)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_READ_FPGA_VIDEO_DATA, buf);
}

void draw_fpga_video_data(U16* buf, int dx, int dy)
{
	DrawFPGADataArg ar;
	ar.buf = buf;
	ar.dx = dx;
	ar.dy = dy;
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_FPGA_VIDEO_DATA, &ar);
}

void draw_fpga_video_data_full(U16* buf)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_FPGA_VIDEO_DATA_FULL, buf);
}

void draw_img_from_buffer(U16* buf, int cdx, int cdy, int ctx, int cty, float zoom, int angle)
{
	DrawRaw_value draw_value;
	draw_value.imgbuf = buf;
	draw_rotate_value(cdx, cdy, ctx, cty, zoom, angle, &draw_value);

	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_IMG_FROM_BUFFER, &draw_value);
}

void direct_camera_display_on(void)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_CAM_DISP_ON, 0);
}

void direct_camera_display_off(void)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_CAM_DISP_OFF, 0);
	clear_screen();
}

int direct_camera_display_stat(void)
{
	return ioctl(graphic_handle, AMAZON2_IOCTL_CAM_DISP_STAT, 0);
}



/******************************************************************
		BMP load
******************************************************************/

#define BI_RGB        0L
typedef struct  {
	// Bitmap file header, 14 bytes (2 bytes ommited)
	U32   bfSize;
	U32    bfReserved;
	U32   bfOffBits;

	// Bitmap info header, 20 bytes
	U32  biSize;
	S32   biWidth;
	S32   biHeight;
	U16   biPlanes;
	U16   biBitCount;
	U32  biCompression;
	U32  biSizeImage;
	S32   biXPelsPerMeter;
	S32   biYPelsPerMeter;
	U32  biClrUsed;
	U32  biClrImportant;
} BITMAPFILEHEADER;

typedef struct  {
	U8    rgbBlue;
	U8    rgbGreen;
	U8    rgbRed;
	U8    rgbReserved;
} RGBQUAD;

#define EXTRACT_READ32(startaddr,offset) (U32)(startaddr[offset] + (U32)(startaddr[offset+1]<<8) + (U32)(startaddr[offset+2]<<16) + (U32)(startaddr[offset+3]<<24))

static BITMAPFILEHEADER bmpfh;


SURFACE* LoadSurfaceInfoFromRGB(U8* bmpdata, U8 bpp, U32 w, U32 h, U32 bmpdatasize, U8* pal)
{
	SURFACE *surface = NULL;
	long i;
	U32  j;
	if (!((bpp == 24) || (bpp == 8) || (bpp == 4))) {
		return 0;
	}
	if ((bpp == 8) || (bpp == 4))
	{
		if (!(surface = create_surface(w, h, bpp)))
		{
			return 0;
		}
		U32 ibpl = (bmpfh.bfSize - bmpfh.bfOffBits) / h;
		surface->pitch = ibpl;
		if (bpp == 8)
		{
			for (i = 0; i < h; i++)
			{
				memcpy(surface->pixels + i*ibpl, bmpdata + (h - 1 - i)*ibpl, surface->w);
			}
			surface->pal->nColors = 256;
			memcpy(surface->pal->colors, pal, 256 * 4);
		}
		else
		{
			for (i = 0; i < h; i++)
			{
				memcpy(surface->pixels + i*ibpl, bmpdata + (h - 1 - i)*ibpl, surface->w / 2);
			}
			surface->pal->nColors = 16;
			memcpy(surface->pal->colors, pal, 16 * 4);
		}

		return surface;
	}
	// 180 x 120 x 3 (bpp = 24)///////
	else if (bpp == 24)
	{
		U32 screenbpp = 16;
		if (!(surface = create_surface(w, h, screenbpp)))
		{
			return 0;
		}

		if (screenbpp == 32)//screen: 4byte(1byte reserved) per pixel
		{
			U32 ibpl;
			U32* rgb32;
			ibpl = ((w * 3) + 3) & 0xfffffffc;
			rgb32 = (U32*)surface->pixels;
			bmpdata += ((h - 1)*ibpl);
			for (i = 0; i < h; i++)
			{
				for (j = 0; j < w; j++)
				{
					rgb32[j] = MAKE_RGB888(bmpdata[j * 3 + 2], bmpdata[j * 3 + 1], bmpdata[j * 3 + 0]);
				}
				bmpdata -= ibpl; //4byte align
				rgb32 += (surface->pitch / 4);
			}
			surface->pixtype = PIX_FMT_RGB888;
		}
		else // convert to rgb888 to rgb565
		{
			U32 ibpl;
			U16* rgb565data;
			ibpl = ((w * 3) + 3) & 0xfffffffc;
			rgb565data = (U16*)surface->pixels;

			bmpdata += ((h - 1)*ibpl);
			for (i = 0; i < h; i++)
			{
				for (j = 0; j < w; j++)
				{
					rgb565data[j] = MAKE_RGB565(bmpdata[j * 3 + 2], bmpdata[j * 3 + 1], bmpdata[j * 3 + 0]);
				}
				bmpdata -= ibpl; //4byte align
				rgb565data += (surface->pitch / 2);
			}
			surface->pixtype = PIX_FMT_RGB565;
		}

	}
	return surface;
}

SURFACE* loadbmp(char* fname)
{
	FILE* fp;
	SURFACE* surface;
	void* bmpdata;
	U8* pal = 0;
	U32 datasize;
	U16 ID;

	/* load windows BMP file */
	fp = fopen(fname, "r");
	if (fp== NULL)
	{
		printf("Cannot open : %s\r\n", fname);
		return 0;
	}
	fread(&ID, 1, 2, fp);
	fread((void *)(&bmpfh), 1, sizeof(BITMAPFILEHEADER), fp);
	if (ID != 0x4d42) // "BM"
	{
		printf("Invalid BMP-ID\r\n");
		fclose(fp);
		return 0;
	}

	if (bmpfh.biBitCount == 8 || bmpfh.biBitCount == 4)
	{
		U32 coltableoffset = 14 + bmpfh.biSize;
		fseek(fp, coltableoffset,SEEK_SET);
		if (bmpfh.biBitCount == 4)
		{
			pal = (U8*)malloc(64);
			fread(pal,1, 64, fp);
		}
		else
		{
			U32 coltablesize = bmpfh.bfOffBits - coltableoffset;
			pal = (U8*)malloc(coltablesize);
			fread(pal, 1, coltablesize, fp);
		}
	}
	fseek(fp, bmpfh.bfOffBits, SEEK_SET);
	datasize = bmpfh.bfSize - bmpfh.bfOffBits;
	bmpdata = malloc(datasize);
	if (bmpdata == NULL)
	{
		printf("failed to malloc\r\n");
		fclose(fp);
		return 0;
	}
	fread(bmpdata,1, datasize, fp);
	surface = LoadSurfaceInfoFromRGB((U8*)bmpdata, bmpfh.biBitCount, bmpfh.biWidth, bmpfh.biHeight, datasize, pal);
	if (pal)
		free(pal);
	free(bmpdata);
	return surface;
}

SURFACE* loadbmpp(U8* startaddr)
{
	U32 datasize;
	U16 bpp;
	U32 w, h;
	U8* bmpdata;
	U32 filesize;
	U32 offset;
	U32 bisize;

	if ((startaddr[0] != 0x42) || (startaddr[1] != 0x4d))// "BM"
	{
		printf("Invalid BMP-ID\r\n");
		return 0;
	}
	filesize = EXTRACT_READ32(startaddr, 2);
	offset = EXTRACT_READ32(startaddr, 10);
	bisize = EXTRACT_READ32(startaddr, 14);
	datasize = filesize - offset;
	bmpdata = (U8*)(offset + (U32)(startaddr));
	bpp = (U32)startaddr[0x1c] + (U32)(startaddr[0x1d] << 8);
	w = EXTRACT_READ32(startaddr, 0x12);
	h = EXTRACT_READ32(startaddr, 0x16);
	return LoadSurfaceInfoFromRGB(bmpdata, bpp, w, h, datasize, startaddr + 14 + bisize);
}

int open_graphic(void)
{
	if ((graphic_handle = open(AMAZON2_GRAPHIC_NAME, O_RDWR)) < 0) {
		printf("Open Error %s\n", AMAZON2_GRAPHIC_NAME);
		return -1;
	}
	return 0;
}

void close_graphic(void)
{
	if (graphic_handle != -1)
		close(graphic_handle);
}

////////////////////////////////////////////////////////////////////////

// Assume 180x120x16bit pixel
void color_ref(U16* buf, RGB565* pixel, int x, int y){
	// Clip
	if(x < 0 || x >= 180){
		printf("pixel index out of range - X:%d\n", x);
		return;
	}else if (y < 0 || y >= 120){
		printf("pixel index out of range - Y:%d\n", y);
		return;
	}else{
        printf("pixel value : 0x%X\n", buf[(119 - y)*180 + x]);
		pixel->b = BLUE_VALUE_IN565(buf[(119 - y)*180 + x]);
        pixel->g = GREEN_VALUE_IN565(buf[(119 - y)*180 + x]);
		pixel->r = RED_VALUE_IN565(buf[(119 - y)*180 + x]);
		return;
	}
}

// Assume 180x120x16bit pixel
void avr_rbg(U16* buf, RGB565* pixel){
	int x, y;
	U32 rsum = 0, bsum = 0, gsum = 0;
	for(y = 120-1; y >= 0; y--){
		for(x = 0; x < 180; x++){
			bsum += BLUE_VALUE_IN565(buf[y*180 + x]);
			gsum += GREEN_VALUE_IN565(buf[y*180 + x]);
			rsum += RED_VALUE_IN565(buf[y*180 + x]);
		}
	}
	pixel->b = bsum/(180*120);
	pixel->g = gsum/(180*120*2);
	pixel->r = rsum/(180*120);
	return;
}
void rgb2yuv(U16* buf, YUV* yuv_pixel){
    int width = 180;
    int height = 120;
    int r,c;
    float tmp,Y,U,V;
    U16 red = 0, green = 0, blue = 0;
    float Ysum = 0, Usum = 0, Vsum = 0;
    float midYsum = 0, midUsum = 0, midVsum = 0,
   		LeftYsum = 0, LeftUsum = 0, LeftVsum = 0,
		RightYsum = 0, RightUsum = 0, RightVsum = 0,
		MYsum = 0, MUsum = 0, MVsum = 0;

    for(r = 0; r < height; r++){
	for(c = 0; c < width; c++){
	 	red = RED_VALUE_IN565(buf[180*r + c]);
	 	blue = BLUE_VALUE_IN565(buf[180*r + c]);
	 	green = GREEN_VALUE_IN565(buf[180*r + c]);

	     Y = 0.299*red + 0.293*green + 0.114*blue;
	     U = 0.492*(0.886*blue - 0.299*red - 0.293*green);
	     V = 0.877*(0.701*red - 0.293*green - 0.114*blue);
	     /*if (r == 90 && c == 60) {
	        printf("<<In (90, 60) : >>\nrgb : %d, %d, %d / U : %0.3f, V : %0.3f\n", red, blue, green, U, V);
		}*/
	     Ysum += Y;
	     Usum += U;
	     Vsum += V;
		 /*if(r>=(height/3)&&r<=(2*height/3))
	     {
	        midYsum +=Y;
	        midUsum +=U;
	        midVsum +=V;
		}*/
	    if(c >= 0 && c < (width/3))
	    {
	        /*LeftYsum +=Y;
	        LeftUsum +=U;
	        LeftVsum +=V;*/
		}
		// (30<=r<60) && (45<=c<135) : total 270 pixels
		else if( (c >= (width/4)) && (c < (3*width/4)) && (r >= height/4) && (r < height/2))
		{
			MYsum +=Y;
			MUsum +=U;
			MVsum +=V;
		}else
		{
			/*RightYsum +=Y;
			RightUsum +=U;
			RightVsum +=V;*/
		}
	}
	}
	tmp = MYsum / (180 * 120);
	yuv_pixel->Y = tmp;
	tmp = MUsum / (180 * 120);
	yuv_pixel->U = tmp;
	tmp = MVsum / (180 * 120);
	yuv_pixel->V = tmp;



	//   printf("midYsum : %f, midUsum : %f, midVsum : %f\n",(float)midYsum/(40*180), (float)midUsum/(40*180), (float)midVsum/(40*180));
	/*printf("LY : %0.3f  / LU : %0.3f  / LV : %0.3f\n", LeftYsum/(120*60), LeftUsum/(120*60), LeftVsum/(120*60));
	printf("LMY : %0.3f / LMU : %0.3f / LMV : %0.3f\n", LMYsum/(120*60), LMUsum/(120*60), LMVsum/(120*60));
	printf("RY : %0.3f  / RU : %0.3f  / RV : %0.3f\n",(float)RightYsum/(120*60), (float)RightUsum/(120*60), (float)RightVsum/(120*60));
	*/return;
}
char checkColorFromYUV(uvset* spec) {
	printf("/ U -> %0.3f / V -> %0.3f\n",spec -> u,spec -> v);
	float U = spec -> u, V = spec -> v;
	if (U < 0)
	{
		if (V > 0) {
			if (V > 0.2)
				return 'r';
			else if (V > 0.1 && V <= 0.2)
				return 'o';
			else
				return 'y';
		}
		else
			return 'g';
	}
	else {
		return 'b';
	}

}
void checkArea(U16* buf) {
	U16 blue = 0x1f;
	int c, r;
	// (30<=r<60) && (45<=c<135) : total 270 pixels

	for (r = 29; r < 61; r++) {
		buf[180*r + 44] = blue;
		buf[180*r + 137] = blue;
	}
	for (c = 44; c < 137; c++) {
		buf[180*29 + c] = blue;
		buf[180*61 + c] = blue;
	}
}
void decision_queue_push(uvset* decision_queue, int size, float u, float v){
	int i, j;
	// if queue not full
	if(decision_queue[size-1].u == 0){
		for(i = 0; i < size; i++){
			if(decision_queue[i].u == 0){
				decision_queue[i].u = u;
				decision_queue[i].v = v;
				break;
			}
		}
	}
	else{
		for(i = 0; i < size-1; i++){
			decision_queue[i] = decision_queue[i+1];
		}
		decision_queue[size-1].u = u;
		decision_queue[size-1].v = v;
	}
}

void decision_queue_avg(uvset* decision_queue, int size, uvset* ret){
	float usum = 0, vsum = 0;
	int i, item_count = 0;
	for(i = 0; i < size; i++){
		if(decision_queue[i].u != 0){
			usum += decision_queue[i].u;
			vsum += decision_queue[i].v;
			item_count++;
		}
	}
	if(item_count == 0) return;

	ret->u = usum/item_count;
	ret->v = vsum/item_count;
}

// return allocated new image
U16* gray_scale(U16* buf){
	U16* grayed_image = (U16*)malloc(sizeof(U16)*180*120);
	int width = 180;
	int height = 120;
	double y_scale = 0; // Use 5-bit gray scale
	int r, c;
	for(r = 0; r < height; r++){
	for(c = 0; c < width; c++){
		y_scale =
		 (0.299*RED_VALUE_IN565(buf[180*r + c])
		+ 0.2935*GREEN_VALUE_IN565(buf[180*r + c])
		+ 0.114*BLUE_VALUE_IN565(buf[180*r + c]));
		grayed_image[180*r + c] = (U16)(((U16)y_scale<<11) | ((U16)y_scale<<6) | ((U16)y_scale));
	}
	}
	return grayed_image;
}

// Usage : mask_filtering(buf, mask_type);
// Assume gray-scaled image, 3x3 mask as input
// Dividing by summation of all array elements is required.
// Return filter-applied image pointer
void mask_filtering(U16* buf, S32* mask){
	int width = 180;
	int height = 120;
	U16 new_image[180*120];
	int half_masksize = 3 / 2;

	int r, c;
	for(r = 0; r < height; r++){
	for(c = 0; c < width; c++){
		U16 sum_of_pixel_count = 0;
		S32 sum_of_pixel_value = 0;

		int x, y;
		for(y = -half_masksize; y <= half_masksize; y++){
		for(x = -half_masksize; x <= half_masksize; x++){

			int px = c + x;
			int py = r + y;
			// If (r+dx, c+dy) pixel is rocated in valid range
			if((px >= 0) && (px < width) && (py >= 0) && (py < height)){
				sum_of_pixel_value += (BLUE_VALUE_IN565(buf[py*180 + px])*mask[3*(y+1) + (x+1)]);
				sum_of_pixel_count++;
				//if(r == 60 && c == 100) printf("px : %d, py : %d, pixelvalue : %d\n", px, py, sum_of_pixel_value);
			}
		}
		}
		sum_of_pixel_value = (U16)(sum_of_pixel_value / (10000.0));
		new_image[r*180 + c] = ((sum_of_pixel_value) | (sum_of_pixel_value<<6) | (sum_of_pixel_value<<11));
		/*if(r == 60 && c == 100) {
			printf("pcnt : %d\n", sum_of_pixel_count);
			printf("sum_of_pixel_value:%d\n", BLUE_VALUE_IN565(new_image[r*180 + c]));
		}*/
	}
	}
	memcpy(buf, new_image, sizeof(U16)*180*120);
}

// Usage : sobel_mask_filtering(buf, maskX, maskY, masksize, divisor);
// Assume gaussian_masked from gray-scaled image, two 3x3 mask as input
// Return filter-applied image pointer
void sobel_mask_filtering(U16* buf, S16* maskX, S16* maskY, int masksize){
	int width = 180;
	int height = 120;
	int half_masksize = masksize / 2;
	U16 new_image[180*120];
	int r, c;

	// Sobel mask : except edges of video
	for(r = 1; r < height - 1; r++){
	for(c = 1; c < width - 1; c++){
		S16 sum_of_pixel_valueX = 0;
		S16 sum_of_pixel_valueY = 0;
		U16 absXY = 0;

		int x, y;
		for(y = -half_masksize; y <= half_masksize; y++){
		for(x = -half_masksize; x <= half_masksize; x++){
			int px = c + x;
			int py = r + y;

			// If (r+dx, c+dy) pixel is rocated in valid range
			if((px >= 0) && (px < width) && (py >= 0) && (py < height)){
				sum_of_pixel_valueX += BLUE_VALUE_IN565(buf[py*180 + px])*maskX[(y + 1)*3 + (x + 1)];
				sum_of_pixel_valueY += BLUE_VALUE_IN565(buf[py*180 + px])*maskY[(y + 1)*3 + (x + 1)];
				//if (r == 100 && c == 100)printf("now masking. xvalue : %d, yvalue : %d\n", sum_of_pixel_valueX, sum_of_pixel_valueY);
			}
		}
		}
		// Clip this space.
		absXY = (U16)sqrt(((double)sum_of_pixel_valueX*sum_of_pixel_valueX + sum_of_pixel_valueY*sum_of_pixel_valueY)/15);
		//sum_of_pixel_valueX = (U16)CLIP5BIT((U16)sum_of_pixel_valueX);
		//if (r == 100 && c == 100)printf("after absXY clib. 5bit in absXY : %X\n", BLUE_VALUE_IN565(absXY));
		new_image[r*180 + c] = ((absXY) | (absXY<<6) | (absXY<<11));

	}// for 'c'
	}// for 'r'
	memcpy(buf, new_image, sizeof(U16)*180*120);
}


/*Hough space

180x120 x-y space to -->

angle
180
| OOOOOOOOOOOOOOOOOOOOOO
| OOOOOOOOOOOOOOOOOOOOOO
| OOOOOOOOOOOOOOOOOOOOOO
| OOOOOOOOOOOOOOOOOOOOOO
--------(diag)--------(diag*2)----->

*/
void hough_lines(U16* buf, U16 threshold_value,
                double resolution, U16 num_line, S16* p_radius, U16* p_theta,d_YUV* d_pixel_value){
	int width = 180, height = 120, r, c, i, j, k;
	U16 diagH = (U16)(sqrt((double)(180*180 + 120*120)));
	U16 diag = diagH*2;
	U16 res_step = (U16)(180/resolution); // In resolution 1, each step has 1 degree.
	U16 num_trans = diag*res_step;
	U16 hough_space[num_trans];
	U16 theta;
	memset(hough_space, 0, num_trans*sizeof(U16));

	for(r = 5; r < height - 5; r++){
	for(c = 5; c < width - 5; c++){

		int distance;
		// At each edge pixels
		if(BLUE_VALUE_IN565(buf[180*r + c]) > threshold_value){
			//printf("selected pixel : y=%d, x=%d\n", r, c);
			for(theta = 0; theta < 180; theta += (U16)resolution){
				distance = (int)(c*mysin(theta) + r*mycos(theta) + diagH + 0.5);
				hough_space[distance*res_step + (U16)(theta/resolution)]++;
			}
		}

	}
	}

	int highest_voted_pixel[6] = {0, 0, 0, 0, 0, 0};
	int highest_voted_pixel_index[6] = {0, 0, 0, 0, 0, 0};

	for(i = 0; i < num_trans; i++){
		if(hough_space[i] >= highest_voted_pixel[0]){
			// push highest_voted_pixel, index array
			for(j = 5; (j >= 0) && (hough_space[i] >= highest_voted_pixel[j]); j--){
				for(k = 1; k < j + 1; k++){
					highest_voted_pixel[k-1] = highest_voted_pixel[k];
					highest_voted_pixel_index[k-1] = highest_voted_pixel_index[k];
				}
				highest_voted_pixel[j] = hough_space[i];
				highest_voted_pixel_index[j] = i;
				break;
			}
		}
	}

	for(i = 0; i < 6; i++){
		p_radius[i] = (S16)(highest_voted_pixel_index[i]/res_step);
		p_theta[i] = (highest_voted_pixel_index[i] - p_radius[i]*res_step)*resolution;
		p_radius[i] -= diagH;
		//printf("line detected. voted : %d, index : %d,  r : %d / theta : %d\n", highest_voted_pixel[i], highest_voted_pixel_index[i], p_radius[i], p_theta[i]);
		//printf("\n");
	}
	float d_Usum=0,d_Vsum=0;
	for(i = 0; i < 6; i++){
		draw_line(buf, p_radius[i], p_theta[i],d_pixel_value);
		printf("each direct%d U_value : %0.3f, V_value : %0.3f\n",i+1,d_pixel_value->u_value,d_pixel_value->v_value);
	}

	/*for(line_count = 0; line_count < num_line; line_count++){
		printf("line no. %d  p_r : %d / p_t : %d\n", line_count, p_radius[line_count], p_theta[line_count]);
	}*/



}
// a = sin(theta)/cos(theta)
void draw_line(U16* buf, S16 r, U16 theta,d_YUV* d_pixel_value){
	S16 x, y;
	U16 blue = 0x1f;
	float Usum=0,Vsum=0;
	if(theta == 90){
		return;
	}
	for(x = 0; x < 180; x++){
		y = (S16)(-(mysin(theta)/mycos(theta))*x + r/mycos(theta));
		if (x%20 ==0) {
			//printf("y : %d\n", y);
		}
		if(y >= 0 && y < 120) {
			red = RED_VALUE_IN565(buf[180*y + x]);
			blue = BLUE_VALUE_IN565(buf[180*y + x]);
			green = GREEN_VALUE_IN565(buf[180*y + x]);
			Y = 0.299*red + 0.293*green + 0.114*blue;
			U = 0.492*(0.886*blue - 0.299*red - 0.293*green);
			V = 0.877*(0.701*red - 0.293*green - 0.114*blue);
			buf[180*y + x] = blue;
		}
		Usum += U;
		Vsum += V;
	}
	Usum = Usum/180;
	Vsum = Vsum/180;
	d_pixel_value->u_value = Usum;
	d_pixel_value->v_value = Vsum;

	//printf("----- r : %d, theta : %d\n", r, theta);
}




/*void buf_to_binaryfile(U16 *buf)
{
	// file write using U16* buf pointer.
	// save and open binaries. need compare with bmp pixeel values
	FILE *fp;
	if ((fp = fopen("samplebin", "wt")) == NULL)
	{
		printf("file open failed");
	}
	else
	{
		fwrite((void*)buf, 1, 120*180*2, fp);
		//printf("sizeof(buf) = %d", sizeof(buf));
		fclose(fp);
		printf("file is written. file size : %d\n", 180*120*2);
	}
}
char* fpgabuf_to_bmpfile(U16 *buf){
	FILE *fp;
	char fname[10] = "sample.bmp";
	BITMAPFILEHEADER header;
	int i;

	fp = fopen(fname, "wb");
	U16 biType[1] = {0x4d42};	// "BM"

	header.bfSize = sizeof(BITMAPFILEHEADER)+2 + 120*180*2;
	header.bfReserved = 0;
	header.bfOffBits = sizeof(BITMAPFILEHEADER)+2;

	header.biSize = 40;
	header.biWidth = 180;
	header.biHeight = 120;
	header.biPlanes = 1;
	header.biBitCount = 16;
	header.biCompression = 0;
	header.biSizeImage = 180*120*2;
	header.biXPelsPerMeter = 2834;
	header.biYPelsPerMeter = 2834;
	header.biClrUsed = 0;
	header.biClrImportant = 0;

	fwrite((void*)biType, 1, sizeof(U16), fp);
	fwrite(&header, 1, sizeof(BITMAPFILEHEADER), fp);
	fwrite((void*)buf, 1, 120*180*2, fp);
	fclose(fp);
	return fname;
}




SURFACE* fpgabuf_to_surface(U16 *buf){
	return NULL;
}*/
