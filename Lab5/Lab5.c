/*
	Kendric Agdaca
	Kagdaca
	ECE 4310 Fall
	Lab 5
	11/3/22

	Purpose: The purpose of this program was to design and implement an active contouring algorithm and apply it on a picture of a hawk perched on a tree branch given a text file
		of initial contour points that roughly outline the hawk. Through the use of a sobel filter in the process, those given initial contour points move closer to the hawk
		until a near perfect outline of a hawk is produced

	Assumptions: Assumes that hawk.ppm and initialcontour.txt are in the same directory as sln and that initialcontour.txt will always have 42 contour points.

	Bugs:

	How to Use: just run program following as stated in Assumptions above.


*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define SQR(x) ((x)*(x))

const int XKern[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
const int YKern[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };


/*
	Purpose: Plots a small + symbol thats 2 pixels long from the center, centered on the contour points passed to the function
	Input:
		-OutImage: Hawk image to be plotted on
		-ICCol: Column values of where to plot + symbol over contour point
		-ICRow: Row values of where to plot + symbol over contour point
		-Cols: Number of Columns in hawk.ppm
		-Rows: Number of Rows in hawk.ppm
	Output: No return value, alters OutImage via pointers
*/
void PointPlot(unsigned char* OutImage, int* ICCol, int* ICRow, int COLS, int ROWS)
{
	int i, c, r;

//Iterate thru all the contour points. At each point, max the intensity 2 pixels in each direction to create a plus symbol
	for (i = 0; i < 42; i++)
	{
		for (c = -2; c <= 2; c++)
		{
			OutImage[ICRow[i] * COLS + (ICCol[i] + c)] = 255;
		}

		for (r = -2; r <= 2; r++)
		{
			OutImage[(ICRow[i] + r) * COLS + ICCol[i]] = 255;
		}
	}
	return;
}


/*
	Purpose: Normalize passed in image. Different from normal normalize function cause it's meant to normalize float images. Function was found online on how to normalize float images
	https://stats.stackexchange.com/questions/70801/how-to-normalize-data-to-0-1-range
	Input:
		-image: SobelImage (float image) to be normalized
		-COLS: Column coordinates of SobelImage
		-ROWS: Row coordinates of SobelImage
		-range: range of min and max coordinate values
	Output: no return value, alters image via pointers
*/
void Normalize(float* SobelImage, int COLS, int ROWS, int range)
{
	int i, max, min;
	max = 0;
	min = 1000;

	for (i = 0; i < ROWS * COLS; i++)
	{
		if (SobelImage[i] < min)
		{
			min = SobelImage[i];
		}
		if (SobelImage[i] > max)
		{
			max = SobelImage[i];
		}
	}

	for (i = 0; i < ROWS * COLS; i++)
	{
		SobelImage[i] = (SobelImage[i] - min) * range / (max - min);
	}

	return;
}


/*
	Purpose: Calculates the distance of the current contour point to the next contour point
	Input:
		-Energy: Window used to store distance between contour points
		-ICCol: Column coordinate of contour points
		-ICRow: Row coordinate of contour points
		-i: contour point number
	Output: No return value, alters Energy via pointers
*/
void CalcInternalEnergy1(float* Energy, int* ICCol, int* ICRow, int i)
{
	int c, r;

	for (r = -3; r <= 3; r++)
	{
		for (c = -3; c <= 3; c++)
		{
			if (i != 41)
			{
				Energy[(r + 3) * 7 + (c + 3)] = Dist(ICRow[i + 1], ICCol[i + 1], ICRow[i] + r, ICCol[i] + c);
			}
			
			else
			{
				Energy[(r + 3) * 7 + (c + 3)] = Dist(ICRow[0], ICCol[0], ICRow[i] + r, ICCol[i] + c);
			}
		}
	}
	return;
}


/*
	Purpose: Calculates the square of the distance of the current contour point to teh next contour point
	Input:
		-Energy: Window used to store the distance between contour points
		-ICCol: Column coordinate of contour points
		-ICRow: Row coordinate of contour points
		-avg: Average distance between all contour points
		-i: Contour point number
	Output: no return value, alters Energy via pointers
*/
void CalcInternalEnergy2(float* Energy, int* ICCol, int* ICRow, int avg, int i)
{
	int r, c;

	for (r = -3; r <= 3; r++)
	{
		for (c = -3; c <= 3; c++)
		{
			if (i != 41)
			{
				Energy[(r + 3) * 7 + (c + 3)] = SQR(avg - Dist(ICRow[i + 1], ICCol[i + 1], ICRow[i] + r, ICCol[i] + c));
			}
			else
			{
				Energy[(r + 3) * 7 + (c + 3)] = SQR(avg - Dist(ICRow[0], ICCol[0], ICRow[i] + r, ICCol[i] + c));
			}
		}
	}
	return;
}


/*
	Purpose: Calculates image gradient magnitude using convolution with a Sobel template
	Input:
		-Energy: Window used to store the distance between contour points
		-SobelImage: Inverted hawk image
		-ICCol: Column coordinate of contour points
		-ICRow: Row coordinate of contour points
		-COLS: Column coordinates of SobelImage
		-i: Contour point number
	Output: no return
*/
void CalcExternalEnergy(float* Energy, float* SobelImage, int* ICCol, int* ICRow, int COLS, int i)
{
	int r, c;

	for (r = -3; r <= 3; r++)
	{
		for (c = -3; c <= 3; c++)
		{
			Energy[(r + 3) * 7 + (c + 3)] = SQR(SobelImage[(ICRow[i] + r) * COLS + (ICCol[i] + c)]);
		}
	}
	return;
}



/*
	Purpose: Determines the coordinate of the smallest value within the 7x7 window then stores the coordinates of the location of smallest value
	Input:
		-MinCol: Column coordinate of smallest value to be returned via pointer
		-MinRow: Row coordinate of smallest value to be returned via pointer
		-TotalEnergy: Matrix of sum * 2 of both internal energies and external energy
	Output: no return value, alters MinCol and MinRow via pointers
*/
void Min(int* MinCol, int* MinRow, float* TotalEnergy)
{
	int i;
	float min = 10000;


	for (i = 0; i < 49; i++)
	{
		if (TotalEnergy[i] < min)
		{
			min = TotalEnergy[i];
			*MinCol = (i % 7) - 3;
			*MinRow = (i / 7) - 3;
		}
	}

}


/*
	Purpose: Simply calculates the average distance between two points, in this case the distance between two contour points
		https://byjus.com/maths/distance-between-two-points-formula/#:~:text=Distance%20between%20two%20points%20is,coordinate%20plane%20or%20x%2Dy%20plane.
	Input: The X and Y coordinates of both contour points
	Output: Average distance of the contour points passed into the function
*/
int Dist(int x1, int y1, int x2, int y2)
{
	int AvgDist;

	AvgDist = sqrt(SQR(x2 - x1) + SQR(y2 - y1));
	return(AvgDist);
}


/*
	Purpose: Active Contour Algorithm. First calculates the internal and external energies then using those, find the smallest value within the window. Then move contour points to those
		minimum value coordinates
		(used https://www.youtube.com/watch?v=wczD7yWVvcE&ab_channel=Kixcodes and lecture notes)
	Input:
		-SobelImage: image of the hawk with contour points labeled
		-ICCol: Column coordinates of contour points
		-ICRow: Row coordinates of contour points
		-COLS: Column coordinates of SobelImage
		-ROWS: Row coordinates of SobelImage
	Output: No return value, alters SobelImage via pointers
*/
void ActiveContour(float* SobelImage, int* ICCol, int* ICRow, int COLS, int ROWS)
{
	int i, j, k, MinCol, MinRow;
	float* InternalEnergy1 = (float*)calloc(49, sizeof(float));
	float* InternalEnergy2 = (float*)calloc(49, sizeof(float));
	float* ExternalEnergy = (float*)calloc(49, sizeof(float));
	float* TotalEnergy = (float*)calloc(49, sizeof(float));
	float AvgDist;


	for (i = 0; i < 31; i++)
	{
		//First calculate average dist between all contour points. Simply use a running sum distance formula between current point and next then divide by 42 to get the average
		AvgDist = 0;
		
		for (j = 0; j < 41; j++)
		{
			AvgDist += Dist(ICRow[j + 1], ICCol[j + 1], ICRow[j], ICCol[j]);
		}

		AvgDist += Dist(ICRow[0], ICCol[0], ICRow[j], ICCol[j]);
		AvgDist = AvgDist / 42;


		for (j = 0; j < 42; j++)
		{
			//Calculate first, second, and external energy. Normalize after each operation
			CalcInternalEnergy1(InternalEnergy1, ICCol, ICRow, j);
			Normalize(InternalEnergy1, 7, 7, 1);
			CalcInternalEnergy2(InternalEnergy2, ICCol, ICRow, AvgDist, j);
			Normalize(InternalEnergy2, 7, 7, 1);
			CalcExternalEnergy(ExternalEnergy, SobelImage, ICCol, ICRow, COLS, j);



			//Use the sum of all the energies * 2 and use it to find the minimum value of the window. After min calculation, use to move contour points
			for (k = 0; k < 49; k++)
			{
				TotalEnergy[k] = 2 * InternalEnergy1[k] + InternalEnergy2[k] + ExternalEnergy[k];
			}

			Min(&MinCol, &MinRow, TotalEnergy);
			ICRow[j] = ICRow[j] + MinRow;
			ICCol[j] = ICCol[j] + MinCol;
		}
	}

//Cleanup
	free(InternalEnergy1);
	free(InternalEnergy2);
	free(ExternalEnergy);
	free(TotalEnergy);

	return;
}


int main()
{
	FILE* fpt;
	unsigned char *InImage, *SobelOut, *OutImage;
	float *SobelImage, sum1, sum2;
	int *ICRow, *ICCol; //IC = initial contour
	char header[320];
	int ROWS, COLS, BYTES, i, r, c, r2, c2;
	i = 0;


	//Read in and store hawk.ppm
	if ((fpt = fopen("hawk.ppm", "rb")) == NULL)
	{
		printf("Unable to open hawk.ppm for reading\n");
		exit(0);
	}

	fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);

	if (strcmp(header, "P5") != 0 || BYTES != 255)
	{
		printf("Not a greyscale 8-bit PPM image\n");
		exit(0);
	}
	InImage = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
	header[0] = fgetc(fpt);
	fread(InImage, 1, COLS * ROWS, fpt);
	fclose(fpt);



	//Read in and store text file
	if ((fpt = fopen("initialcontour.txt", "rb")) == NULL)
	{
		printf("Unable to open intial contours file\n");
		exit(0);
	}

	ICCol = (int*)calloc(42, sizeof(int));
	ICRow = (int*)calloc(42, sizeof(int));

	while (!feof(fpt))
	{
		fscanf(fpt, "%d %d\n", &ICCol[i], &ICRow[i]);
		i++;
	}


	OutImage = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));
	SobelImage = (float*)calloc(ROWS * COLS, sizeof(float));
	SobelOut = (unsigned char*)calloc(ROWS * COLS, sizeof(unsigned char));


	//Plots + symbols over contour points onto hawk
	for (i = 0; i < ROWS * COLS; i++)//clone input image
	{
		OutImage[i] = InImage[i];
	}

	PointPlot(OutImage, ICCol, ICRow, COLS, ROWS);
	fpt = fopen("BaseImage.ppm", "w");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(OutImage, COLS * ROWS, 1, fpt);
	fclose(fpt);


	/*Sobel Filter    http://cuda-programming.blogspot.com/2013/01/sobel-filter-implementation-in-c.html         */
	for (r = 3; r < ROWS - 3; r++)
	{
		for (c = 3; c < COLS - 3; c++)
		{
			sum1 = 0;
			sum2 = 0;
			for (r2 = -1; r2 <= 1; r2++)
			{
				for (c2 = -1; c2 <= 1; c2++)
				{
					sum1 += InImage[(r + r2) * COLS + (c + c2)] * XKern[(r2 + 1) * 3 + (c2 + 1)];
					sum2 += InImage[(r + r2) * COLS + (c + c2)] * YKern[(r2 + 1) * 3 + (c2 + 1)];
				}
			}

			SobelImage[r * COLS + c] = sqrt(SQR(sum1) + SQR(sum2));
		}
	}

	Normalize(SobelImage, COLS, ROWS, 255);

	for (i = 0; i < ROWS * COLS; i++)//SobelImage is still in float cast type, convert to unsigned for outputting
	{
		SobelOut[i] = SobelImage[i];
	}

	fpt = fopen("Sobel.ppm", "w");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(SobelOut, COLS * ROWS, 1, fpt);
	fclose(fpt);




	//Normalize then invert sobel image to prep for active contouring
	Normalize(SobelImage, COLS, ROWS, 1);

	for (i = 0; i < ROWS * COLS; i++)
	{
		SobelImage[i] = 1 - SobelImage[i];
	}


	//Active Contouring
	ActiveContour(SobelImage, ICCol, ICRow, COLS, ROWS);

	for (i = 0; i < ROWS * COLS; i++)//Create another clone of hawk to get rid of initial contour points
	{
		OutImage[i] = InImage[i];
	}
	PointPlot(OutImage, ICCol, ICRow, COLS, ROWS);//Plot new contour points onto hawk

	fpt = fopen("FinalContour.ppm", "w");
	fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);
	fwrite(OutImage, COLS * ROWS, 1, fpt);
	fclose(fpt);


	FILE* Out;
	Out = fopen("NewContourPoint.csv", "w");
	for (i = 0; i < 42; i++)
	{
		fprintf(Out, "%d     %d\n", ICRow[i], ICCol[i]);
	}
	return(0);

//Cleanup
	free(InImage);
	free(SobelOut);
	free(OutImage);
	free(ICCol);
	free(ICRow);
}

