#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define MAX_CHILDREN 4
#define MAX_FILE_INDEX 3

//sum numbers in text block
int Sum(int *numbers, int size){
    int sum = 0;
    for(int i = 0; i < size; i++) {
        sum += numbers[i];
    }
    return sum;
}

int main() {
    int numChildren;
    int fileIndex;
    

    // Input number of child processes
    do {
        printf("Enter the number of child processes (1, 2, 4): ");
        scanf("%d", &numChildren);
    } while (numChildren != 1 && numChildren != 2 && numChildren != 4);

    // Input index of the file to process
    do {
        printf("Enter the index of the file to process (1, 2, 3): ");
        scanf("%d", &fileIndex);
    } while (fileIndex < 1 || fileIndex > 3);

    //starts counting for total time
    clock_t begin = clock();

    // Create pipes between parent and children
    int pipes[MAX_CHILDREN][2]; 
    for (int i = 0; i < numChildren; i++) {
        pipe(pipes[i]);
    }

    // Fork child processes
    for (int i = 0; i < numChildren; i++) {
    
        if (fork() == 0) { 
            
            close(pipes[i][0]);

            // Open file corresponding to file_index
            char filename[20];
            sprintf(filename, "file%d.dat", fileIndex);
            FILE *file = fopen(filename, "r");
            if (file == NULL) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }

            // Seek to the appropriate position in the file
            fseek(file, i * (1000 / numChildren) * sizeof(int), SEEK_SET);

            // Read numbers from file
            int blockSize = (1000 / numChildren);
            int *numbers = (int *)malloc(blockSize * sizeof(int));
            for (int j = 0; j < blockSize; j++) {
                fscanf(file, "%d", &numbers[j]);
            }

            // Calculate sum of numbers in the block
            int sum = Sum(numbers, blockSize);

            
            write(pipes[i][1], &sum, sizeof(int));

    
            free(numbers);
            fclose(file);
            close(pipes[i][1]);
            exit(EXIT_SUCCESS);
        }
    }
    //ends time counter and prints time taken
    clock_t end = clock();
    double timeSpent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f \n", timeSpent);

    // Parent process
    printf("Each child process result:\n");
    int totalSum = 0;
    for (int i = 0; i < numChildren; i++) {
        
        close(pipes[i][1]);

        // Read sum from child
        int childSum;
        read(pipes[i][0], &childSum, sizeof(int));
        totalSum += childSum;
        //print child results
        printf("Child %d: %d\n", i+1, childSum);
        close(pipes[i][0]);
    }
    //print total result
    printf("Result Total: %d\n", totalSum);

    return 0;
}