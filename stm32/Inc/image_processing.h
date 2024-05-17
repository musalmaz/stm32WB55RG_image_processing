/*
 * image_processing.h
 *
 *  Created on: Apr 19, 2024
 *      Author: musa
 */

#ifndef INC_DRIVERS_IMAGE_PROCESSING_H_
#define INC_DRIVERS_IMAGE_PROCESSING_H_

#include "stdint.h"


#define BLOCK_SIZE 80
#define KERNEL_SIZE 5
#define IMG_SIZE (BLOCK_SIZE * BLOCK_SIZE)

#define alpha1  1.2;  // Contrast control
#define beta1  0;       // Brightness control

#define TILE_SIZE 8
#define NUM_TILES (BLOCK_SIZE / TILE_SIZE)
#define HIST_SIZE 256

void convertToGrayscale(uint8_t *block_buffer, uint8_t *gray_buffer);

void applyAverageFilter1D(uint8_t *gray_buffer, uint8_t *output_buffer);

void adjustContrastBrightness(uint8_t *gray_buffer, uint8_t *output_buffer, int BUFFER_SIZE);

void equalizeTile(uint8_t *tile, int tile_size, int clip_limit);

void applyCLAHE(uint8_t *gray_buffer, uint8_t *output_buffer, int clip_limit);

void resizeBilinear(uint8_t *input, uint8_t *output, int srcWidth, int srcHeight, int dstWidth, int dstHeight);

void deblurImage(uint8_t gray_buffer[BLOCK_SIZE * BLOCK_SIZE], uint8_t output_buffer[BLOCK_SIZE * BLOCK_SIZE], int kernel_size, float kernel[kernel_size][kernel_size]);

#endif /* INC_DRIVERS_IMAGE_PROCESSING_H_ */
