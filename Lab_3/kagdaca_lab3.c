/*
 *	Kendric Agdaca
 *	Kagdaca
 *	ECE 4310 Fall
 *	Due 10/4/22
 *	Lab3
 *
 *	Purpose: The purpose of this lab was to design and implement thinning, branchpoint, and endpoint detection to recognize letters in an image of text. This program is an extention of the code used in Lab_2. The difference between them is that much of the code has been reorganized where multiple parts of this code have been split and assigned its own function
 *
 *	Assumptions: Assumes that the files used for testing are parenthood.ppm, parenthood_e_template.ppm, and parenthood_gt.txt AND that they are already in the same directory folder when tested
 *
 *	Bugs: ROC doesn't print the right numbers to the csv file anymore. Happened when I went back thru to reformat and comment out my code, guess I ended up changing something without noticing and I cant for the life of my figure it out. Managed to recover an old test run csv so I based my curve off it
 *
 *	Usage: ./Lab3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/*
 *	Purpose: Auxiliary function to function "Thin". Determines if a pixel should be erased based on its edge properties
 *	Input:
 *	Output:
 */
void ThinAux(unsigned char *thinned, int InCols, int InRows, int Cols, int Rows, int *flag, int *EndPoint, int *BranchPoint)
{
	int A, B, C, D, i, j, k, NonEdge, Neighbor, current, next;
	j = k = NonEdge = Neighbor = current = next = A = B = C = D = 0;

//Go around center pixel in CW and check surrounding pixel properties
	for (i = (Cols - 1); i <= Cols; i++)
    	{
        	j = ((Rows - 1) * InCols) + i;
        	current = thinned[j];
        	k = ((Rows - 1) * InCols) + (i + 1);
        	next = thinned[k];


        	if ((current == 255) && (next == 0))
        	{
            		NonEdge++;
        	}
        	
		if (current == 255)
        	{
            		Neighbor++;
        	}
    	}

   
    	for (i = (Rows - 1); i <= Rows; i++)
    	{
        	j = (i * InCols) + (Cols + 1);
        	current = thinned[j];
        	k = ((i + 1) * InCols) + (Cols + 1);
        	next = thinned[k];


		if ((current == 255) && (next == 0))
        	{
            		NonEdge++;
        	}

		if (current == 255)
        	{
            		Neighbor++;
        	}
    	}


    
    	for (i = (Cols + 1); i > (Cols - 1); i--)
    	{
        	j = ((Rows + 1) * InCols) + i;
	        current = thinned[j];
	        k = ((Rows + 1) * InCols) + (i - 1);
	        next = thinned[k];


	        if ((current == 255) && (next == 0))
	        {
	        	NonEdge++;
	        }
        
		if (current == 255)
        	{
            		Neighbor++;
        	}
    	}



    	for (i = (Rows + 1); i > (Rows - 1); i--)
    	{
        	j = (i * InCols) + (Cols - 1);
        	current = thinned[j];
        	k = ((i - 1) * InCols) + (Cols - 1);
        	next = thinned[k];


        	if ((current == 255) && (next == 0))
        	{
            		NonEdge++;
        	}
        
		if (current == 255)
        	{
            		Neighbor++;
        	}
    	}





    A = thinned[((Rows - 1) * InCols) + Cols];
    B = thinned[(Rows * InCols) + (Cols + 1)];
    C = thinned[(Rows * InCols) + (Cols - 1)];
    D = thinned[((Rows + 1) * InCols) + Cols];





    	if (NonEdge == 1)
    	{
        	if ((Neighbor >= 2) && (Neighbor <= 6))
        	{
            		if ((A == 0) || (B == 0) || ((C == 0) && (D == 0)))
            		{
                		*flag = 1;
            		}
            		
			else
            		{
                		*flag = 0;
            		}
        	}
        	
		else
        	{
            		*flag = 0;
        	}
    	}
    
	else
    	{
        	*flag = 0;
    	}

    	if (NonEdge == 1)
    	{
        	*EndPoint = 1;
    	}
    
	if (NonEdge > 2)
    	{
        	*BranchPoint = 2;
    	}
}





/* 
 *	Purpose:
 *	Input:
 *	Output:
 */
unsigned char *Thin(unsigned char *ImageThresh, int InCols, int InRows)
{
	int i, j, k, Cols, Rows, flag, EndPoint, BranchPoint, mark, count;
	j = k = mark = count = EndPoint = BranchPoint = flag = 0;
	

	unsigned char *ImageThinned = (unsigned char *)calloc(InRows * InCols, sizeof(unsigned char));
	unsigned char *temp = (unsigned char *)calloc(InRows * InCols, sizeof(unsigned char));


	for (i = 0; i < (InRows * InCols); i++)
	{
		ImageThinned[i] = ImageThresh[i];
		temp[i] = ImageThresh[i];
	}

//Start of thinning algo
	while(k == 0)
	{
        	k = 1;
        	flag = mark = 0;
        	count++;

		for (Rows = 1; Rows < (InRows - 1); Rows++)
		{
			for (Cols = 1; Cols < (InCols - 1); Cols++)
			{
                		j = (Rows * InCols) + Cols;
                
				if (ImageThinned[j] == 255)
                		{
                    			flag = 0;
                    			ThinAux(ImageThinned, InCols, InRows, Cols, Rows, &flag, &EndPoint, &BranchPoint);
                    
					
					if (flag == 1)
                    			{
                        			j = (Rows * InCols) + Cols;
                        			temp[j] = 0;
                        			k = 0;
                        			mark += 1;
                    			}
               			}	        
			}
		}


        	for (i = 0; i < (InRows * InCols); i++)
		{
			ImageThinned[i] = temp[i];
		}
	}
    
	return ImageThinned;
}








unsigned char *PointLocate(unsigned char *ImageThin, int InCols, int InRows)
{
	int Cols, Rows, i, j, flag, EndPoint, BranchPoint, EndCount, BranchCount;
	unsigned char *ImageEndBranch, *thinned;
	Cols = Rows = i = j = flag = EndPoint = BranchPoint = EndCount = BranchCount = 0;


	ImageEndBranch = (unsigned char *)calloc(InCols * InRows, sizeof(unsigned char));
	thinned = (unsigned char *)calloc(InCols * InRows, sizeof(unsigned char));


	for (i = 0; i < (InCols * InRows); i++)
	{
		thinned[i] = ImageThin[i];
	}


	for (Rows = 1; Rows < (InRows - 1); Rows++)
	{
		for (Cols = 1; Cols < (InCols - 1); Cols++)
		{
			j = (Rows * InCols) + Cols;
			EndPoint = 0;
			BranchPoint = 0;


			if (ImageThin[j] == 255)
			{
				ThinAux(ImageThin, InCols, InRows, Cols, Rows, &flag, &EndPoint, &BranchPoint);

				if (EndPoint == 1)
				{
					ImageEndBranch[j] = 50;
					thinned[j] = 100;
					EndCount += 1;
				}

				if (BranchPoint == 2)
				{
					ImageEndBranch[j] = 150;
					thinned[j] = 200;
					BranchCount += 1;
				}
			}
		}
	}

	return ImageEndBranch;

}



void ROCCurve(unsigned char *ImageMSF, unsigned char *ImageEndBranch, int MSFCols, int MSFRows)
{
	
	FILE *Groundfpt, *ROCfpt;
    	int i, j, Rows1, Cols1, Rows2, Cols2, TP, FP, FN, TN, thresh, k, found, EndPoint, BranchPoint, match;
    	char letter[2], find[2];
    	unsigned char *temp = (unsigned char *)calloc(MSFCols * MSFRows, sizeof(unsigned char));


    	Rows1 = Cols1 = TP = FP = FN = TN = thresh = k = found = EndPoint = BranchPoint = 0;
    	strcpy(find, "e");


    	Groundfpt = fopen("parenthood_gt.txt", "r");
    	if (Groundfpt == NULL)
    	{
        	printf("Error, could not read Ground Truth text file\n");
        	exit(0);
    	}


	ROCfpt = fopen("Truth Table.csv", "w");
    	fprintf(ROCfpt, "Threshold,TP,FP,FN,TN,TPR,FPR,PPV\n");


    	for (i = 0; i <= 255; i++)
    	{
        	thresh = i;

        	for (j = 0; j < (MSFRows * MSFCols); j++)
        	{
            	if (ImageMSF[j] >= thresh)
            	{
                	temp[j] = 255;
            	}
            
		else
            	{
                	temp[j] = 0;
            	}
        }
        
    
	while((fscanf(Groundfpt, "%s %d %d\n", letter, &Cols1, &Rows1)) != EOF)
	{
        	EndPoint = BranchPoint = found = TP = FP = FN = TN = 0;


		for (Rows2 = Rows1 - 7; Rows2 <= (Rows1 + 7); Rows2++)
		{
			for (Cols2 = Cols1 - 4; Cols2 <= (Cols1 + 4); Cols2++)
		        {
		        	if (temp[(Rows2 * MSFCols) + Cols2] == 255)
		            	{
		                	found = 1;
		            	}
                    	    
				if (ImageEndBranch[(Rows2 * MSFCols) + Cols2] == 50)
                    	    	{
                        		EndPoint += 1;
                    	    	}
                    	    
				if (ImageEndBranch[(Rows2 * MSFCols) + Cols2] == 150)
                    	    	{
                      			BranchPoint += 1;
                    	    	}
		        }
		    }

   
            	if ((found == 1) && (EndPoint == 1) && (BranchPoint == 1))
            	{
                	found = 1;
            	}
            
		else
            	{
                	found = 0;              
            	}

		match = strcmp(letter, find);
            
		if ((found == 1) && (match == 0))
            	{
                	TP++;
            	}
            
		if ((found == 1) && (match != 0))
            	{
                	FP++;
            	}
                
		if ((found == 0) && (match != 0))
		{
			TN++;
		}

		if ((found == 0) && (match == 0))
            	{
                	FN++;
            	}      
	}


     
		fprintf(ROCfpt, "%d,%d,%d,%d,%d,%.2f,%.2f,%.2f\n", thresh, TP, FP, FN, TN, TP/(double)(TP +FN ),FP/(double)(FP+TN), FP/(double)(TP+FP));

		rewind(Groundfpt);
	}


    	fclose(Groundfpt);
    	fclose(ROCfpt);
}




int main (int argc, char *argv[])
{
	FILE *Infpt, *MSFfpt, *Outfpt;
	char header[256];
	int InRows, InCols, InBytes, MSFRows, MSFCols, MSFBytes;
	unsigned char *ImageIn, *ImageMSF, *ImageThresh, *ImageThin, *ImageEndBranch;


//Handle opening, allocating mem, and parsing data from parenthood.ppm
	if ((Infpt = fopen("parenthood.ppm", "rb")) == NULL)
	{
		printf("Unable to open parenthood.ppm\n");
		exit(0);
	}

	fscanf(Infpt, "%s %d %d %d", header, &InCols, &InRows, &InBytes);

	if (strcmp(header, "P5") != 0 || InBytes != 255)
	{
		printf("parenthood.ppm is not a greyscale 8-bit PPM image\n");
		exit(0);
	}

	ImageIn = (unsigned char *)calloc(InRows * InCols, sizeof(unsigned char));

	if (ImageIn == NULL)
	{
		printf("Unable to allocate mem for parenthood.ppm\n");
		exit(0);
	}

	header[0] = fgetc(Infpt);
	fread(ImageIn, 1, InCols * InRows, Infpt);
	fclose(Infpt);








//Handle opening, allocating mem, and parsing data from parenthood_e_template.ppm
	if ((MSFfpt = fopen("parenthood_e_template.ppm", "rb")) == NULL)
	{
		printf("Unable to open parenthood_e_template.ppm\n");
		exit(0);
	}

	fscanf(MSFfpt, "%s %d %d %d", header, &MSFRows, &MSFCols, &MSFBytes);

	if (strcmp(header, "P5") != 0 || MSFBytes != 255)
	{
		printf("parenthood_e_template.ppm is not a greyscale 8-bit PPM image\n");
		exit(0);
	}

	ImageMSF = (unsigned char *)calloc(MSFRows * MSFCols, sizeof(unsigned char));

	if (ImageMSF == NULL)
	{
		printf("Unable to allocate mem for parenthood_e_template.ppm\n");
		exit(0);
	}

	header[0] = fgetc(MSFfpt);
	fread(ImageMSF, 1, MSFCols * MSFRows, MSFfpt);
	fclose(MSFfpt);










//Threshold image at 128 to create binary image
	int i;
	int thresh = 128;

	ImageThresh = (unsigned char *)calloc(InRows * InCols, sizeof(unsigned char));
	for (i = 0; i < (InRows * InCols); i++)
	{
		if (ImageIn[i] <= thresh)
		{
			ImageThresh[i] = 255;
		}

		else
		{
			ImageThresh[i] = 0;
		}
	}



	Outfpt = fopen("ImageThresh.ppm", "w");
	fprintf(Outfpt, "P5 %d %d 255\n", InCols, InRows);
	fwrite(ImageThresh, InRows * InCols, sizeof(unsigned char), Outfpt);
	fclose(Outfpt);








//Thin the threshold image
	ImageThin = Thin(ImageThresh, InCols, InRows);



	Outfpt = fopen("ImageThin.ppm", "w");
	fprintf(Outfpt, "P5 %d %d 255\n", InCols, InRows);
	fwrite(ImageThin, InRows * InCols, sizeof(unsigned char), Outfpt);
	fclose(Outfpt);


	



	ImageEndBranch = PointLocate(ImageThin, InCols, InRows);

	Outfpt = fopen("ImageEndBranch.ppm", "w");
	fprintf(Outfpt, "P5 %d %d 255\n", InCols, InRows);
	fwrite(ImageEndBranch, InRows * InCols, 1, Outfpt);
	fclose(Outfpt);




	ROCCurve(ImageMSF, ImageEndBranch, MSFCols, MSFRows);


	
}


