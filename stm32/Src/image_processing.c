/*
 * image_processing.c
 *
 *  Created on: Apr 19, 2024
 *      Author: musa
 */

#include "image_processing.h"

void convertToGrayscale(uint8_t *block_buffer, uint8_t *gray_buffer) {
    for (int i = 0; i < 320*240; i++) {
		uint16_t rgb565 = block_buffer[i];

//		// Extract RGB components from RGB565
//		uint8_t r = (rgb565 >> 11) & 0x1F;  // 5 bits
//		uint8_t g = (rgb565 >> 5) & 0x3F;   // 6 bits
//		uint8_t b = rgb565 & 0x1F;          // 5 bits
		uint8_t r = (block_buffer[2*i] & 0xF8) >> 3; // Red channel
		uint8_t g = ((block_buffer[2*i] & 0x07) << 3) | ((block_buffer[2*i+1] & 0xE0) >> 5); // Green channel
		uint8_t b = (block_buffer[2*i+1] & 0x1F); // Blue channel

		// Convert to 8-bit components (scale up)
		uint8_t r8 = (r * 255) / 31;
		uint8_t g8 = (g * 255) / 63;
		uint8_t b8 = (b * 255) / 31;

		// Convert to grayscale using the weighted sum
		uint8_t gray = (uint8_t)(0.299 * r8 + 0.587 * g8 + 0.114 * b8);

		// Store the grayscale value
		gray_buffer[i] = gray;

    }
}

void applyAverageFilter1D(uint8_t *gray_buffer, uint8_t *output_buffer) {
    int offset = KERNEL_SIZE / 2;
    float avg_kernel[KERNEL_SIZE][KERNEL_SIZE];

    // Initialize the averaging kernel
    for (int i = 0; i < KERNEL_SIZE; i++) {
        for (int j = 0; j < KERNEL_SIZE; j++) {
            avg_kernel[i][j] = 1.0 / (KERNEL_SIZE * KERNEL_SIZE);
        }
    }

    // Apply the kernel to each "pixel" in the 1D gray_buffer
    for (int y = 0; y < BLOCK_SIZE; y++) {
        for (int x = 0; x < BLOCK_SIZE; x++) {
            float sum = 0.0;
            int count = 0;

            // Sum up the neighborhood
            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    int ny = y + ky;
                    int nx = x + kx;

                    // Check boundaries
                    if (nx >= 0 && nx < BLOCK_SIZE && ny >= 0 && ny < BLOCK_SIZE) {
                        int index = ny * BLOCK_SIZE + nx;
                        sum += gray_buffer[index] * avg_kernel[ky + offset][kx + offset];
                        count++;
                    }
                }
            }

            // Normalize the sum to get the average value
            int output_index = y * BLOCK_SIZE + x;
            output_buffer[output_index] = (uint8_t)(sum / count);
        }
    }
}

void adjustContrastBrightness(uint8_t *gray_buffer, uint8_t *output_buffer, int BUFFER_SIZE) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        int temp = gray_buffer[i] * alpha1 + beta1;

        // Clipping the values to ensure they remain between 0 and 255
        if (temp > 255) temp = 255;
        else if (temp < 0) temp = 0;

        output_buffer[i] = (uint8_t)temp;
    }
}

void equalizeTile(uint8_t *tile, int tile_size, int clip_limit) {
    int hist[HIST_SIZE] = {0};
    int clipped = 0;
    float scale;
    int excess;

    // Calculate histogram
    for (int i = 0; i < tile_size; i++) {
        hist[tile[i]]++;
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
        hist[i] = (int)(((float)(cdf - cdf_min) / (tile_size - cdf_min)) * 255);
    }

    // Equalize the tile
    for (int i = 0; i < tile_size; i++) {
        tile[i] = hist[tile[i]];
    }
}

void applyCLAHE(uint8_t *gray_buffer, uint8_t *output_buffer, int clip_limit) {
    uint8_t tile[TILE_SIZE * TILE_SIZE];

    // Process each tile
    for (int ty = 0; ty < NUM_TILES; ty++) {
        for (int tx = 0; tx < NUM_TILES; tx++) {
            // Copy data to the tile
            for (int y = 0; y < TILE_SIZE; y++) {
                for (int x = 0; x < TILE_SIZE; x++) {
                    int by = ty * TILE_SIZE + y;
                    int bx = tx * TILE_SIZE + x;
                    tile[y * TILE_SIZE + x] = gray_buffer[by * BLOCK_SIZE + bx];
                }
            }

            // Equalize the tile
            equalizeTile(tile, TILE_SIZE * TILE_SIZE, clip_limit);

            // Copy data back from the tile
            for (int y = 0; y < TILE_SIZE; y++) {
                for (int x = 0; x < TILE_SIZE; x++) {
                    int by = ty * TILE_SIZE + y;
                    int bx = tx * TILE_SIZE + x;
                    output_buffer[by * BLOCK_SIZE + bx] = tile[y * TILE_SIZE + x];
                }
            }
        }
    }
}

void resizeBilinear(uint8_t *input, uint8_t *output, int srcWidth, int srcHeight, int dstWidth, int dstHeight) {
    float x_ratio = srcWidth / (float)dstWidth;
    float y_ratio = srcHeight / (float)dstHeight;
    float px, py;
    for (int i = 0; i < dstHeight; i++) {
        for (int j = 0; j < dstWidth; j++) {
            px = (float)j * x_ratio;
            py = (float)i * y_ratio;
            int x = (int)px;
            int y = (int)py;

            float x_diff = px - x;
            float y_diff = py - y;
            int index = y * srcWidth + x;

            // Range check for pixel indices
            if (index >= 0 && index + srcWidth + 1 < srcWidth * srcHeight) {
                uint8_t A = input[index];
                uint8_t B = input[index + 1];
                uint8_t C = input[index + srcWidth];
                uint8_t D = input[index + srcWidth + 1];

                // Bilinear interpolation formula
                uint8_t gray = (uint8_t)(
                    A * (1 - x_diff) * (1 - y_diff) +
                    B * (x_diff) * (1 - y_diff) +
                    C * (y_diff) * (1 - x_diff) +
                    D * (x_diff) * (y_diff)
                );

                output[i * dstWidth + j] = gray;
            }
        }
    }
}

void deblurImage(uint8_t gray_buffer[BLOCK_SIZE * BLOCK_SIZE], uint8_t output_buffer[BLOCK_SIZE * BLOCK_SIZE], int kernel_size, float kernel[kernel_size][kernel_size]) {
    int offset = kernel_size / 2;

    for (int y = 0; y < BLOCK_SIZE; y++) {
        for (int x = 0; x < BLOCK_SIZE; x++) {
            float pixel_value = 0.0;

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    int readY = y + ky;
                    int readX = x + kx;

                    // Boundary conditions (clamp to edges)
                    if (readY < 0) readY = 0;
                    if (readX < 0) readX = 0;
                    if (readY >= BLOCK_SIZE) readY = BLOCK_SIZE - 1;
                    if (readX >= BLOCK_SIZE) readX = BLOCK_SIZE - 1;

                    int current_index = readY * BLOCK_SIZE + readX;
                    pixel_value += gray_buffer[current_index] * kernel[ky + offset][kx + offset];
                }
            }

            // Clamp the result to valid grayscale range
            if (pixel_value < 0) pixel_value = 0;
            if (pixel_value > 255) pixel_value = 255;

            int output_index = y * BLOCK_SIZE + x;
            output_buffer[output_index] = (uint8_t)pixel_value;
        }
    }
}
