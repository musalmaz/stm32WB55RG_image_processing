/*
 * image_processing.h
 *
 *  Created on: May 30, 2024
 *      Author: musa
 */

#ifndef INC_DRIVERS_IMAGE_PROCESSING_H_
#define INC_DRIVERS_IMAGE_PROCESSING_H_

#include "stdint.h"
#include "math.h"


#define TILE_SIZE 8  // Example tile size, adjust as necessary
#define HIST_SIZE 256

#define KERNEL_SIZE 3
extern int kernel[KERNEL_SIZE][KERNEL_SIZE];

//void applyAverageFilter1D(uint8_t *gray_buffer, uint8_t *output_buffer);

void adjustContrastBrightness(uint8_t *gray_buffer, uint8_t *output_buffer, int BUFFER_SIZE,float alpha, float beta);

void equalizeTile(uint8_t *gray_buffer, int srcWidth, int srcHeight, int tx, int ty, int clip_limit, uint8_t *output_buffer);

void applyCLAHE(uint8_t *gray_buffer, uint8_t *output_buffer, int srcWidth, int srcHeight, int clip_limit);

void resizeBilinear(uint8_t *input, uint8_t *output, int srcWidth, int srcHeight, int dstWidth, int dstHeight);

void deblurImage(uint8_t *gray_buffer, uint8_t *output_buffer, int img_width, int img_height, int kernel_size, float kernel[kernel_size][kernel_size]);

void gamma_correction(uint8_t *input, uint8_t *output, int srcWidth, int srcHeight, float gamma);

void sharpen_image(uint8_t *gray_buffer, uint8_t *output_buffer, int img_width, int img_height);

#endif /* INC_DRIVERS_IMAGE_PROCESSING_H_ */
