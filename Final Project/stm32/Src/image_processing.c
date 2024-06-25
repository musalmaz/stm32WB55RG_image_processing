/*
 * image_processing.c
 *
 *  Created on: May 30, 2024
 *      Author: musa
 */

#include "Drivers/image_processing.h"

//int kernel_size = 3;
int kernel_size = KERNEL_SIZE;  // Less common approach (initialization in header)
int kernel[KERNEL_SIZE][KERNEL_SIZE] = {
    {0, -1, 0},
    {-1, 5, -1},
    {0, -1, 0}
};

#define CLAMP(value, min, max) (((value) < (min)) ? (min) : (((value) > (max)) ? (max) : (value)))


void adjustContrastBrightness(uint8_t *gray_buffer, uint8_t *output_buffer, int BUFFER_SIZE, float alpha, float beta) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        uint8_t temp = (uint8_t)(((float)gray_buffer[i] * alpha) + beta);

//        // Clipping the values to ensure they remain between 0 and 255
//        if (temp > 255.0f) temp = 255.0f;
//        else if (temp < 0.0f) temp = 0.0f;

        output_buffer[i] = (uint8_t)temp;
    }
}

void equalizeTile(uint8_t *gray_buffer, int srcWidth, int srcHeight, int tx, int ty, int clip_limit, uint8_t *output_buffer) {
    int hist[HIST_SIZE] = {0};
    int clipped = 0;
    float scale;
    int excess;

    // Calculate histogram for the tile
    for (int y = 0; y < TILE_SIZE; y++) {
        for (int x = 0; x < TILE_SIZE; x++) {
            int by = ty * TILE_SIZE + y;
            int bx = tx * TILE_SIZE + x;
            if (by < srcHeight && bx < srcWidth) {
                hist[gray_buffer[by * srcWidth + bx]]++;
            }
        }
    }

    // Clip histogram and count excess
    for (int i = 0; i < HIST_SIZE; i++) {
        if (hist[i] > clip_limit) {
            clipped += hist[i] - clip_limit;
            hist[i] = clip_limit;
        }
    }

    // Redistribute excess
    excess = clipped / HIST_SIZE;
    int leftover = clipped % HIST_SIZE;
    for (int i = 0; i < HIST_SIZE; i++) {
        hist[i] += excess;
        if (leftover > 0) {
            hist[i]++;
            leftover--;
        }
    }

    // Calculate cumulative distribution function (CDF)
    int cdf_min = 0;
    int cdf = 0;
    for (int i = 0; i < HIST_SIZE; i++) {
        if (cdf_min == 0 && hist[i] != 0) cdf_min = hist[i];
        cdf += hist[i];
        hist[i] = (int)(((float)(cdf - cdf_min) / (TILE_SIZE * TILE_SIZE - cdf_min)) * 255);
    }

    // Equalize the tile and copy back to the output buffer
    for (int y = 0; y < TILE_SIZE; y++) {
        for (int x = 0; x < TILE_SIZE; x++) {
            int by = ty * TILE_SIZE + y;
            int bx = tx * TILE_SIZE + x;
            if (by < srcHeight && bx < srcWidth) {
                output_buffer[by * srcWidth + bx] = hist[gray_buffer[by * srcWidth + bx]];
            }
        }
    }
}

void applyCLAHE(uint8_t *gray_buffer, uint8_t *output_buffer, int srcWidth, int srcHeight, int clip_limit) {
    // Process each tile
	uint16_t NUM_TILES_X =  (srcWidth / TILE_SIZE);
    uint16_t  NUM_TILES_Y =  (srcHeight / TILE_SIZE);
    for (int ty = 0; ty < NUM_TILES_Y; ty++) {
        for (int tx = 0; tx < NUM_TILES_X; tx++) {
            equalizeTile(gray_buffer, srcWidth, srcHeight, tx, ty, clip_limit, output_buffer);
        }
    }
}


void resizeBilinear(uint8_t *input, uint8_t *output, int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
	float x_ratio = ((float)(srcWidth)) / dstWidth;
	float y_ratio = ((float)(srcHeight)) / dstHeight;
    float px, py;
    for (int i = 0; i < dstHeight; i++) {
        for (int j = 0; j < dstWidth; j++) {
            px = j * x_ratio;
            py = i * y_ratio;
            int x = (int)px;
            int y = (int)py;

            float x_diff = px - x;
            float y_diff = py - y;

            int index = y * srcWidth + x;

            // Handle boundary conditions
            uint8_t A = input[index];
            uint8_t B = (x + 1 < srcWidth) ? input[index + 1] : A;
            uint8_t C = (y + 1 < srcHeight) ? input[index + srcWidth] : A;
            uint8_t D = (x + 1 < srcWidth && y + 1 < srcHeight) ? input[index + srcWidth + 1] : A;

            // Bilinear interpolation formula
            uint8_t gray = (uint8_t)(
                A * (1 - x_diff) * (1 - y_diff) +
                B * x_diff * (1 - y_diff) +
                C * y_diff * (1 - x_diff) +
                D * x_diff * y_diff
            );

            output[i * dstWidth + j] = gray;
        }
    }
}

void deblurImage(uint8_t *gray_buffer, uint8_t *output_buffer, int img_width, int img_height, int kernel_size, float kernel[kernel_size][kernel_size]) {
    int offset = kernel_size / 2;

    for (int y = 0; y < img_height; y++) {
        for (int x = 0; x < img_width; x++) {
            float pixel_value = 0.0;

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    int readY = y + ky;
                    int readX = x + kx;

                    // Boundary conditions (clamp to edges)
                    if (readY < 0) readY = 0;
                    if (readX < 0) readX = 0;
                    if (readY >= img_height) readY = img_height - 1;
                    if (readX >= img_width) readX = img_width - 1;

                    int current_index = readY * img_width + readX;
                    pixel_value += gray_buffer[current_index] * kernel[ky + offset][kx + offset];
                }
            }

            // Clamp the result to valid grayscale range
            if (pixel_value < 0) pixel_value = 0;
            if (pixel_value > 255) pixel_value = 255;

            int output_index = y * img_width + x;
            output_buffer[output_index] = (uint8_t)pixel_value;
        }
    }
}

void gamma_correction(uint8_t *input, uint8_t *output, int width, int height, float gamma) {
    float invGamma = 1.0f / gamma;


    // Apply gamma correction directly
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			uint8_t colour = input[y * width + x];
			float normalized = (float)colour / 255.0f;
			uint8_t corrected = (uint8_t)(255.0f * powf(normalized, invGamma) + 0.5f);
			output[y * width + x] = corrected;
		}
	}
}

void sharpen_image(uint8_t *gray_buffer, uint8_t *output_buffer, int img_width, int img_height) {
    int kernel_center = KERNEL_SIZE / 2;

    // Iterate through each pixel (excluding borders)
    for (int y = kernel_center; y < img_height - kernel_center; ++y) {
        for (int x = kernel_center; x < img_width - kernel_center; ++x) {
            int sum = 0;

            // Apply the sharpening kernel
            for (int ky = -kernel_center; ky <= kernel_center; ++ky) {
                for (int kx = -kernel_center; kx <= kernel_center; ++kx) {
                    int pixel_value = gray_buffer[(y + ky) * img_width + (x + kx)];
                    int weight = kernel[ky + kernel_center][kx + kernel_center];
                    sum += weight * pixel_value;
                }
            }

            // Clamp the result and store in the output buffer
            output_buffer[y * img_width + x] = (uint8_t)CLAMP(sum, 0, 255);
        }
    }

    // Handle borders by copying input (simpler approach for this example)
    for (int y = 0; y < img_height; ++y) {
        for (int x = 0; x < kernel_center; ++x) {
            output_buffer[y * img_width + x] = gray_buffer[y * img_width + x];
            output_buffer[y * img_width + (img_width - 1 - x)] = gray_buffer[y * img_width + (img_width - 1 - x)];
        }
    }
    for (int x = 0; x < img_width; ++x) {
        for (int y = 0; y < kernel_center; ++y) {
            output_buffer[y * img_width + x] = gray_buffer[y * img_width + x];
            output_buffer[(img_height - 1 - y) * img_width + x] = gray_buffer[(img_height - 1 - y) * img_width + x];
        }
    }
}
