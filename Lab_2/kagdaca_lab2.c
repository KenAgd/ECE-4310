/*
 *	Kendric Agdaca
 *	Kagdaca
 *	ECE 4310 Fall
 *	Due 9/20/22
 *	Lab2
 *
 *	Purpose: The purpose of this project was to design and implement a matched filter using normalized cross-correlation to recognize lettes in an image of text. This project specifically will search for the letter "e"
 *
 *	Assumptions: Assumes that the files to use for testing are parenthood.ppm, parenthood_e_template.ppm, and ground_truth.txt AND that they are already in the same directory folder when tested
 *
 *	Bugs:
 *
 *	Usage: ./Lab2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>





int main (int argc, char *argv[])
{
	FILE *Infpt, *Outfpt, *Tempfpt;
	char InHeader[80], TempHeader[80];
	int InRows, InCols, InMax, TempRows, TempCols, TempMax;
	unsigned char *ImageIn, *OutputImage, *TempImage;

//1) Open Input file and safe guard check of opened and meets 8-bit greyscale. If pass, parse data
	if ((Infpt = fopen("parenthood.ppm", "rb")) == NULL)
	{
		printf("Unable to open parenthood.ppm\n");
		exit(0);
	}

	fscanf(Infpt, "%s %d %d %d", InHeader, &InCols, &InRows, &InMax);

	if (strcmp(InHeader, "P5") != 0 || InMax != 255)
	{
		printf("Not a greyscale 8-bit PPM image\n");
		exit(0);
	}

	ImageIn = (unsigned char *)calloc(InRows * InCols, sizeof(unsigned char));

	if (ImageIn == NULL)
	{
		printf("Unable to allocate memory for ImageIn\n");
		exit(0);
	}

	InHeader[0] = fgetc(Infpt);
	fread(ImageIn, 1, InCols*InRows, Infpt);
	fclose(Infpt);





	if ((Tempfpt = fopen("parenthood_e_template.ppm", "rb")) == NULL)
	{
		printf("Unable to open parenthood_e_template.ppm\n");
		exit(0);
	}
	
	fscanf(Tempfpt, "%s %d %d %d", TempHeader, &TempCols, &TempRows, &TempMax);
	
	if (strcmp(TempHeader, "P5") != 0 || TempMax != 255)
	{
		printf("Not a greyscale 8-bit PPM image\n");
		exit(0);
	}
	
	TempImage = (unsigned char *)calloc(TempRows * TempCols, sizeof(unsigned char));

	if (TempImage == NULL)
	{
		printf("Unable to allocate memory for TempImage\n");
		exit(0);
	}

	TempHeader[0] = fgetc(Tempfpt);
	fread(TempImage, 1, TempCols*TempRows, Tempfpt);
	fclose(Tempfpt);




//Calculate zero mean filter of template
	int mean, i, sum;
	int *ZMTemp = (int *)calloc(TempCols * TempRows, sizeof(int));

	sum = 0;
	for (i = 0; i < (TempCols * TempRows); i++)
	{
		sum += TempImage[i];
	}

	mean = sum / (TempCols * TempRows);

	for (i = 0; i < (TempCols * TempRows); i++)
	{
		ZMTemp[i] = TempImage[i] - mean;
	}





//Calculate MSF
	int c, r, c2, r2;
	int *MSF = (int *)calloc(InCols * InRows, sizeof(int));

	for (r = 7; r < InRows - 7; r++)
	{
		for (c = 4; c < InCols - 4; c++)
		{
			sum = 0; 

			for (r2 = -7; r2 < TempRows - 7; r2++)
			{
				for (c2 = -4; c2 < TempCols - 4; c2++)
				{
					sum += ZMTemp[(TempCols * (r2 + 7)) + (c2 + 4)] * ImageIn[(InCols * (r + r2)) + (c + c2)];
				}
			}

			MSF[(InCols * r) + c] = sum;
		}
	}


//Normalize MSF
	int min = MSF[0];
	int max = MSF[0];
	OutputImage = (unsigned char *)calloc(InCols * InRows, sizeof(unsigned char));

	for (i = 1; i < InCols * InRows; i++)
	{
		if (MSF[i] < min)
		{
			min = MSF[i];
		}

		if (MSF[i] > max)
		{
			max = MSF[i];
		}
	}


	for (i = 0; i < InCols * InRows; i++)
	{
		OutputImage[i] = (MSF[i] - min) * (255)/ (max - min);
	}

	Outfpt = fopen("NormalMSF.ppm", "w");
	fprintf(Outfpt,  "P5 %d %d 255\n", InCols, InRows);
	fwrite(OutputImage, InCols * InRows, 1, Outfpt);
	fclose(Outfpt);




//ROC and Threshold
	FILE *Groundfpt, *ROCfpt;
	unsigned char *temp;
	int thresh, found, match;
	int TP, FP, TN, FN;
	int GroundCol, GroundRow;
	char letter[2], find[2];
	double TPR, FPR, PPV;


//Allocate memory for threshold and load in letter to find if able to successfully allocate memory. Then read in the ground truth file and prep a .csv file to store ROC data
	temp = (unsigned char *)calloc(InCols * InRows, sizeof(unsigned char));
	if (temp == NULL)
	{
		printf("Unable to allocate memory for Threshold\n");
		exit(0);
	}
	strcpy(find, "e");
	

	Groundfpt = fopen("ground_truth.txt", "r");
	if (Groundfpt == NULL)
	{
		printf("Unable to open Ground Truth file\n");
		exit(0);
	}
	ROCfpt = fopen("ROC.csv", "w");
	fprintf(ROCfpt, "Threshold,TP,FP,TN,FN,TPR,FPR,PPC\n");


//test all threshold values, from 0->255 one by one
	for (i = 0; i < 256; i++)
	{
		TP = FP = TN = FN = TPR = FPR = PPV = 0;
		thresh = i;


		for (c2 = 0; c2 < InCols * InRows; c2++)
		{
			if (OutputImage[c2] >= thresh)
			{
				temp[c2] = 255;
			}

			else
			{
				temp[c2] = 0;
			}
		}


	//Find letter
		while (!feof(Groundfpt))
		{
			fscanf(Groundfpt, "%s %d %d\n", letter, &GroundCol, &GroundRow);

			for (r = GroundRow - 7; r <= GroundRow + 7; r++)
			{
				for (c = GroundCol - 4; c <= GroundCol + 4; c++)
				{
					if (temp[(r * InCols) + c] == 255)
					{
						found = 1;
					}
				}
			}

			match = strcmp(letter, find);

			if (found == 1 && match == 0)
			{
				TP++;
			}

			if (found == 1 && match != 0)
			{
				FP++;
			}

			if (found == 0 && match != 0)
			{
				TN++;
			}

			if (found == 0 && match == 0)
			{
				FN++;
			}

			found = 0; //reset found before running next test interation
		}
		
		TPR = TP / (double)(TP + FN);
		FPR = FP / (double)(FP + TN);
		PPV = FP / (double)(TP + FP);

		fprintf(ROCfpt, "%d,%d,%d,%d,%d,%.2F,%.2F,%.2F\n", thresh, TP, FP, TN, FN, TPR, FPR, PPV);


		
		rewind(Groundfpt);//use rewind to reset file position pointer to the start, ready for next test iteration
	}

	fclose(Groundfpt);
	fclose(ROCfpt);

//This section was created after testing all threshold numbers, and it was found that 204 was the most optimal value. Reprints image using 204 threshold
	for (c2 = 0; c2 < InCols * InRows; c2++)
	{
		if (OutputImage[c2] < 204)
		{
			temp[c2] = 0;
		}
		else
		{
			temp[c2] = 255;
		}
	}
	Outfpt = fopen("ThreshOutput.ppm","w");
	fprintf(Outfpt, "P5 %d %d 255\n", InCols, InRows);
	fwrite(temp, InCols * InRows, 1, Outfpt);
	fclose(Outfpt);

//cleanup
	free(ImageIn);
	free(TempImage);
	free(ZMTemp);
	free(MSF);
	free(OutputImage);
	free(temp);
}
