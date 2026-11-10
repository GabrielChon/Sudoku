#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
//#include <ncurses.h>

#define N 9



char **crmatrix();
int checkmatrix(char **, char *);
void prtmat(char **);

int checklines(char **, int);	//used for checking if there are dublicants in each row, column, or box
int checkcols(char **, int);
int checksquares(char **, int);
int checkall(char **, int);

int findempty(char **, int *, int *);	//functions used for solving the sudoku (explained later on)
int solvematrix(char **);
int safe(char **, int, int, int);

int main(int argc, char **argv){

	//Checking the arguments

	if(argc != 2){
		printf("Λάθος σύνταξη εντολών.\nΣωστή σύνταξη: %s -όνομα αρχείου-\n", argv[0]);
		exit(-2);
	}

	char **mat;
	int i, check;

	mat = crmatrix();
	check = checkmatrix(mat, argv[1]);
	if(check != 1){
		fprintf(stderr, "Λάθος στο κορμό του αρχείου.\nΤο αρχείο πρέπει να περιέχει 81 αριθμούς με διάταξη 9x9.\nΈξοδος από το πρόγραμμα...\n");
		for (i = 0; i < N; i++){
                	free(mat[i]);
        	}
        	free (mat);
		exit(-4);
	}

	check = checkall(mat, 1);

	if(!check){
		fprintf(stderr, "Ὲξοδος από το πρόγραμμα...\n");
		for (i = 0; i < N; i++){
                	free(mat[i]);
        	}
        	free (mat);
		exit(-4);
	}

	/*initscr();
	start_color();
	use_default_colors();

	init_pair(1, COLOR_RED, -1);
	init_pair(2, COLOR_BLUE, -1);

	prtmatnc(mat);

	getch();
	endwin();*/


	printf("Το αρχείο πληρεί τις προϋποθέσεις για ένα πάζλ Σουντόκου.\n\n");
	prtmat(mat);

	check = solvematrix(mat);
	if(check){
		fprintf(stdout, "Το παζλ λύθηκε!\n\n");
		prtmat(mat);
		printf("\n");
	}
	else{
		fprintf(stderr, "Δεν βρέθηκε λύση του πάζλ\n");
	}




	for (i = 0; i < N; i++){		//freeing the memory
		free(mat[i]);
	}
	free (mat);

	return 0;
}


char **crmatrix(){		//creates a two-dimensional matrix (no arguments used because the length (rows, columns) is fixed for a sudoku format (9x9))

	char **mat;
	int i, j;
	mat = (char **)malloc(N*sizeof(char *));
	if(!mat){
		fprintf(stderr, "Προβλημα με τη δέσμευση της μνήμης.\nΈξοδος από το πρόγραμμα...\n");
		exit(-1);
	}

	for(i = 0; i< N;  i++){
		mat[i] = (char *)malloc(N*sizeof(char));
		if(!mat[i]){
			fprintf(stderr, "Προβλημα με τη δέσμευση της μνήμης.\nΈξοδος από το πρόγραμμα...\n");
			exit(-1);
		}
	}

	return mat;
}


int checkmatrix(char **mat, char *v){				//checks if the file is 9x9 and 'inserts' the characters from the file to the matrix
	FILE *fp;

	int i, j, row = 0, col = 0;
	int ch;
	fp = fopen(v, "r");
	if(!fp){
		fprintf(stderr, "Πρόβλημα με το άνοιγμα του αρχείου.\nΈξοδος από το πρόγραμμα...\n");
		exit(-3);
	}
	while((ch=fgetc(fp)) != EOF){		//checks if there are 81 numbers (9 by 9) by counting the enters(\n)
		if(ch=='\n'){
			if(col != 9){
				fclose(fp);
				return 0;
			}
			row++;
			col = 0;

			if(row > 9){
				fclose(fp);
				return 0;
			}
		}
		else{
			if(ch < '0' || ch > '9' || col >= 9 || row >= 9){
				fclose(fp);
				return 0;
			}
			mat[row][col] = ch - '0';
			col++;
		}
	}



	if((row == 8 && col != 9) || (row == 9 && col != 0) || (row != 8 && row != 9)){
		fclose(fp);
		return 0;
	}


	fclose(fp);
	return 1;
}

void prtmat(char **mat){	//prints a 2d matrix
	int i, j;

	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			printf("%d", mat[i][j]);
			if(j % 3 == 2){			//(cool idea) gives a sudoku-style to the matrix (separates the numbers to 3x3 boxes)
				printf("   ");
			}
		}
		printf("\n");
		if(i % 3 == 2){
			printf("\n");
		}
	}
}


int checklines(char **mat, int verb){		//checks if there are repeating numbers in the sudoku's rows
	int *tmp;
	int i, j;

	tmp = (int*)malloc(10*sizeof(int));
	if(tmp == NULL){return 0;}

	for(i = 0; i< N ; i++){
		memset(tmp, 0, 10*sizeof(int));
		for(j = 0; j < N; j++){
			int value = mat[i][j];
			if(value==0)continue;

			if(tmp[value] == 1){
				if(verb!=0){printf("Σφάλμα.\nΣτη γραμμή %d, ο αριθμός %d επαναλαμβάνεται.\n", i + 1, value);}
				free(tmp);
				return 0;
			}

			tmp[value] = 1;
		}
	}

	free(tmp);
	return 1;
}




int checkcols(char **mat, int verb){			//checks if there are repeating numbers in the sudoku's columns
        int *tmp;
        int i, j;

        tmp = (int*)malloc(10*sizeof(int));
        if(tmp == NULL){return 0;}

        for(i = 0; i< N ; i++){

                memset(tmp, 0, 10*sizeof(int));
                for(j = 0; j < N; j++){
                        int value = mat[j][i];
                        if(value==0)continue;

                        if(tmp[value] == 1){
                                if(verb!=0){printf("Σφάλμα.\nΣτη στήλη %d, ο αριθμός %d επαναλαμβάνεται.\n", i + 1, value);}
                                free(tmp);
                                return 0;
                        }

                        tmp[value] = 1;
                }
        }

        free(tmp);
        return 1;

}

int checksquares(char **mat, int verb){			//checks if there are repeating numbers in the sudoku's squares
        int *tmp;
        int i, j;
	int sq;

        tmp = (int*)malloc(10*sizeof(int));
        if(tmp == NULL){return 0;}

        for(sq = 0; sq<N ; sq++){
                memset(tmp, 0, 10*sizeof(int));
		int stRow = (sq/3)*3;
		int stCol = (sq%3)*3;

		for(i = 0; i< 3; i++){
                	for(j = 0; j < 3; j++){
                        	int value = mat[stRow + i][stCol + j];
                        	if(value==0)continue;

                        	if(tmp[value] == 1){
                                	if(verb!=0){printf("Σφάλμα.\nΣτο τετράγωνο %d, ο αριθμός %d επαναλαμβάνεται.\n", sq + 1, value);}
                                	free(tmp);
                                	return 0;
                        	}

                        	tmp[value] = 1;
                	}
        	}
	}

        free(tmp);
        return 1;
}

int checkall(char **mat, int verb){		//inspects the whole grid. When verb is 1, if it finds a dublicant, it prints out error messages
	int lines, cols, squares;

	lines = checklines(mat, verb);
	cols = checkcols(mat, verb);
	squares = checksquares(mat, verb);

	return lines && cols && squares;

}


int findempty(char **mat, int *r, int *c){	//finds an empty spot on the grid and saves the coordinates to r, c. if there are no empty spots, it returns 0.

	int i, j;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			if(mat[i][j] == 0){
				*r = i;
				*c = j;
				return 1;
			}
		}
	}
	return 0;
}


int safe(char **mat, int row, int col, int num){	//checks if it is safe to insert a number on a spot.
							//checkall() scans the whole grid, but this function scans only the row, column and box the spot is assigned in.
							//it is more efficient
	int i, j, sq;


	for(i = 0; i < N; i++){			//checking row
		int value = mat[row][i];
		if(value == num){

			return 0;
		}
	}

	for(j = 0; j < N; j++){			//checking column
		int value = mat[j][col];
		if(value == num){
			return 0;
		}
	}

	int stRow = (row / 3)*3;		//checking box
	int stCol = (col / 3)*3;
	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			int value = mat[stRow + i][stCol + j];
			if(value == num){
				return 0;
			}
		}
	}

	return 1;
}


int solvematrix(char **mat){		//solves the sudoku puzzle using a recursive algorithm

	int i;
	int row;
	int col;
	int check = findempty(mat, &row, &col);

	if(!check){
		return 1;
	}

	for(i = 1; i<=9 ; i++){
		int s;
		s = safe(mat, row, col, i);
		if(s){
			mat[row][col] = i;
			int x = solvematrix(mat);
			if(x){
				return x;
			}
			mat[row][col] = 0;
		}
	}

	return 0;

}

