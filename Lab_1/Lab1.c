/*
 *	Kendric Agdaca
 *	Kagdaca
 *	ECE 4310 Fall
 *	Due 9/6/22
 *	Lab1
 *
 *	Purpose: The purpose of this lab is to implement three versions of a 7x7 mean filter. The first uses 2D convolution; the second uses separable filters (1x7 and 7x1); and the third uses separable filters and a sliding window
 *
 *	Assumptions: Assumed that bridge.ppm is being used to test mean filter
 *
 *	Bugs:
 *
 *	Usage: ./Lab1 [image].ppm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main (int argc, char *argv[])
{
	FILE *fpt;
	unsigned char *image;
	unsigned char *smoothed;
	char header[320];
	int ROWS, COLS, BYTES;
	int r, c, r2, c2, sum;
	struct timespec tp1,tp2;


//Read image and check if valid. If valid, read magic numbers from image header and allocate memory for the image and the final altered image
	if ((fpt = fopen("bridge.ppm", "rb")) == NULL)
	{
		printf("Unable to open image for reading\n");
		exit(0);
	}

	fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);

	if (strcmp(header, "P5") != 0 || BYTES != 255)
	{
		printf("Not a greyscale 8-bit PPM image\n");
		exit(0);
	}

	image = (unsigned char *)calloc(ROWS * COLS, sizeof(unsigned char));
	header[0] = fgetc(fpt);
	fread(image, 1, COLS * ROWS, fpt);
	fclose(fpt);

	smoothed = (unsigned char *)calloc(ROWS * COLS, sizeof(unsigned char)); //holds new image


//start of 2D Convolution
	clock_gettime(CLOCK_REALTIME, &tp1);

	for (r = 3; r < ROWS - 3; r++)
	{
		for (c = 3; c < COLS - 3; c++)
		{
			sum = 0;

			for (r2 = -3; r2 <= 3; r2++)
			{
				for (c2 = -3; c2 <= 3; c2++)
				{
					sum += image[(r + r2) * COLS + (c + c2)];
				}
			}

			smoothed[r * COLS + c] = sum / 49;
		}
	}

	clock_gettime(CLOCK_REALTIME, &tp2);
	printf("Time to 2D Convolution smooth: %ld nanoseconds\n", tp2.tv_nsec - tp1.tv_nsec);

	fpt = fopen("2DConvolution.ppm", "w");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(smoothed, sizeof(char), COLS * ROWS, fpt);
	fclose(fpt);


//start of separable filter
	float *temp = (float *)calloc(ROWS * COLS, sizeof(float));//smoothed

	clock_gettime(CLOCK_REALTIME, &tp1);

	for (r = 0; r < ROWS; r++)
	{
		for (c = 3; c < COLS - 3; c++)
		{
			sum = 0;

			for (c2 = -3; c2 <= 3; c2++)
			{
				sum += image[r * COLS + (c + c2)];
			}

			temp[r * COLS + c] = sum;
		}
	}

	for (r = 3; r < ROWS - 3; r++)
	{
		for (c = 3; c < COLS; c++)
		{
			sum = 0;

			for (r2 = -3; r2 <= 3; r2++)
			{
				sum += temp[(r + r2) * COLS + c];
			}

			smoothed[r * COLS + c] = sum / 49;
		}
	}

	clock_gettime(CLOCK_REALTIME, &tp2);
	printf("Time to Separable filter smooth: %ld nanoseconds\n", tp2.tv_nsec - tp1.tv_nsec);

	fpt = fopen("Separable.ppm", "w");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(smoothed, COLS * ROWS, 1, fpt);
	fclose(fpt);


//start of sliding window
	clock_gettime(CLOCK_REALTIME, &tp1);

	for (r = 0; r < ROWS; r++)
	{
		for (c = 3; c < COLS - 3; c++)
		{
			if (c == 3)
			{
				sum = 0;

				for (c2 = -3; c2 <= 3; c2++)
				{
					sum += image[r * COLS + (c + c2)];
				}
			}

			else
			{
				sum -= image[r * COLS + (c - 4)];
				sum += image[r * COLS + (c + 3)];
			}

			temp[r * COLS + c] = sum;
		}
	}

	for (c = 3; c < COLS - 3; c++)
	{
		for (r = 3; r < ROWS - 3; r++)
		{
			if (r == 3)
			{
				sum = 0;

				for (r2 =-3; r2 <= 3; r2++)
				{
					sum += temp[(r + r2) * COLS + c];
				}
			}

			else
			{
				sum -= temp[(r - 4) * COLS + c];
				sum += temp[(r + 3) * COLS + c];
			}

			smoothed[r * COLS + c] = sum / 49;
		}
	}

	clock_gettime(CLOCK_REALTIME, &tp2);
	printf("Time to Sliding window  smooth: %ld nanoseconds\n", tp2.tv_nsec - tp1.tv_nsec);
	fpt = fopen("sliding.ppm", "w");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(smoothed, COLS * ROWS, 1, fpt);
	fclose(fpt);



//cleanup
	free(image);
	free(smoothed);
	free(temp);
}

