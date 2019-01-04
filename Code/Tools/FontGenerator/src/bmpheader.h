/* BMP header file structs, same struct and field name as M$ has them
 * info from http://www.digicamsoft.com/bmp/bmp.html
 */

#include <stdint.h>

typedef struct {
  uint16_t	bfType;
  uint32_t	bfSize;
  uint16_t	bfReserved1;
  uint16_t	bfReserved2;
  uint32_t	bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
  uint32_t	biSize;
  int32_t	biWidth;
  int32_t	biHeight;
  uint16_t	biPlanes;
  uint16_t	biBitCount;
  uint32_t	biCompression;
  uint32_t	biSizeImage;
  int32_t	biXPelsPerMeter;
  int32_t	biYPelsPerMeter;
  uint32_t	biClrUsed;
  uint32_t	biClrImportant;
} BITMAPINFOHEADER;
