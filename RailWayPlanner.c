/**
 * @file RailWayPlanner.c
 * @author  Ayelet avraham username ayeletavr <ayelet.avraham@mail.huji.ac.il>
 * @date 10 May 2020
 *
 * @brief this program finds the minimal price for a requested railway.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

//------------Constants.-------------
const int DEFAULT_LENGTH = -1;
const int DEFAULT_K_LINKERS = -1;
char* DEFAULT_LINKER_SET = {"linker_flag"};
const int DEFAULT_VALID = 0;
const int DEFAULT_ARR_SIZE = 0;
#define NOT_EXIST -1;
#define EMPTY_FILE -1;
#define EOF_ERROR -2;

/**
 * @brief this struct represents a section in the railway
 */
typedef struct Section
{
    int price;
    int length;
    char start[2];
    char end[2];
    int startIndex;
    int endIndex;
} Section;

/**
 * @brief check if string ends with input suffix.
 * @param string
 * @return true if end with, else false.
 */
int endWithSuffix(char* string, char *input)
{
    if (strlen(string) >= strlen(input))
    {
        if (!strcmp(string + strlen(string) - strlen(input), (input)))
        {
            return true;
        }
    }
    return false;
}


/**
 * @brief gets a string (input line number 4 to N) and creates a section out of it.
 * @param inputLine line 4 to N
 * @param section section object.
 * @return true if succeeded, else false.
 */
int createSection(char inputLine[1024], Section *section)
{
    char inputCopy[1024];
    strcpy(inputCopy, inputLine);
    const char sep[2] = ",";
    char *sectionField;
    // to get the first field - represents start linker
    sectionField = strtok(inputCopy, sep);
    int counter = 1;
    while (sectionField != NULL)
    {
        // checking validity and assigning each element separately.
        if (strlen(sectionField) != 1 && strcmp(sectionField, "\"") != 0)
        {
            return false;
        }
        else
        {
            strcpy(section->start, sectionField);
            counter++;
            sectionField = strtok(NULL, sep);
        }
        if (counter == 2) //end linker
        {
            if (strlen(sectionField) != 1 && strcmp(sectionField, "\"") != 0)
            {
                return false;
            }
            else
            {
                strcpy(section->end, sectionField);
                counter++;
                sectionField = strtok(NULL, sep);
            }
        }
        if (counter == 3) // length of section
        {
            long sectionLength;
            char *lengthLineRemain;
            sectionLength = strtol(sectionField, &lengthLineRemain, 10);
            if((sectionLength <= 0) || (strlen(lengthLineRemain)) != 0)
            {
                return false;
            }
            else
            {
                section->length = (int) sectionLength;
                counter++;
                sectionField = strtok(NULL, sep);
            }
        }
        if (counter == 4 && endWithSuffix(sectionField, "\n")) // price of section
        {
            long sectionPrice;
            char *priceLineRemain;
            sectionPrice = strtol(sectionField, &priceLineRemain, 10);
            if ((sectionPrice <= 0) || (strcmp(priceLineRemain, "\n") != 0))
            {
                return false;
            }
            else
            {
                section->price = (int) sectionPrice;
                sectionField = strtok(NULL, sep);
            }
        }
    }
    return true;
}

/**
 * @brief checks if the given linker is in the given array.
 * @param char linker
 * @param char* linkers array.
 * @return its index if linker in set, else -1.
 */
int findlinkerInlinkersSet(char* linker, char* linkersSet, int size)
{
    for (int i = size -1 ; i >= 0; i--)
    {
        if (strstr(&linkersSet[i], linker) != NULL)
        {
            return i;
        }
    }
    return NOT_EXIST;

}

/**
 * @brief add a given section to a given section array.
 * @param inputLine 4th to nth line.
 * @param sectionArr
 * @param lastIndex - section array's last index.
 * @return true if succeeded, else false.
 */
int addSectionToSectionArray(char inputLine[1024], Section* sectionArr, int lastIndex)
{
    Section toAdd;
    //defaultSectionConstructor(&toAdd);
    int result = createSection(inputLine, &toAdd);
    if(result == 0)
    {
        free(sectionArr);
        return false;
    }
    else
    {
        sectionArr[lastIndex] = toAdd;
    }
    return true;
}

/**
 * @brief this struct contains the input that parsed from file.
 */
typedef struct Input
{
    int railwayLength; // L
    int numOfDifferentLinkers; // K
    char *linkersSet; //
    Section *sections;
    int isValid; // 0 if valid, -1 if file is empty, -2 if EOF, positive number else.
    int sectionArrSize;
} Input;

/**
 * @brief gets the third line as a string, and an array allocated,
 * and creates a set of linkers. if the input is inValid, it frees the allocated memory.
 * @param string third line input.
 * @param result the allocated memory (array).
 * @return true if succeeded, else false.
 */
int  createLinkersSet(char string[1024], char* result, Input* input)
{
    const char sep[2] = ",";
    char *linker;
    int resultCounter = 0;
    // to get the first linker.
    linker = strtok(string, sep);
    // get other linkers and add it to array (set).
    while (linker != NULL)
    {
        if (strlen(linker) != 1 && !endWithSuffix(linker, "\n")) // validity check
        {
            free(result); // or it has to be only in the main?
            return false;
        }
        else
        {
            result[resultCounter] = (char) linker[0];
            resultCounter++;
            linker = strtok(NULL, sep);
        }
    }
    input->linkersSet = result;
    return true;
}



// ----------------------Helpers to parser----------------
/**
 * @brief gets the first line input and sets it as input's length.
 * @param inputLine
 * @return 0 if succeeded, else 1.
 */
int setLength(const char* inputLine, Input* result)
{
    long length;
    char *lengthLineRemain;
    length = strtol(inputLine, &lengthLineRemain, 10);
    if ((length < 0) || (strcmp(lengthLineRemain, "\n") != 0))
    {
        return EXIT_FAILURE;
    }
    result->railwayLength = (int) length;
    return EXIT_SUCCESS;
}

/**
 * @brief gets the second line input and sets it as input's number of linkers.
 * @param inputLine
 * @param result
 * @return 0 if succeeded, else 1.
 */
int setNumOfLinkers(const char* inputLine, Input* result)
{
    long numOfLinkers;
    char *linkersNumRemain;
    numOfLinkers = strtol(inputLine, &linkersNumRemain, 10);
    if ((numOfLinkers <= 0) || (strcmp(linkersNumRemain, "\n") != 0))
    {
        return EXIT_FAILURE;
    }
    result->numOfDifferentLinkers = (int) numOfLinkers;
    return EXIT_SUCCESS;
}

// ----------------------

/**
 * @brief gets a file (name of file), reads and parses it all the way into Input object.
 * @param inputFileName
 * @return Input object.
 */
Input parse(const char* inputFileName)
{
    Input result;
    // initialize input with default values.
    result.railwayLength = DEFAULT_LENGTH;
    result.numOfDifferentLinkers = DEFAULT_K_LINKERS;
    result.linkersSet = DEFAULT_LINKER_SET;
    //result.sections = DEFAULT_SECTION_ARR;
    result.isValid = DEFAULT_VALID;
    result.sectionArrSize = DEFAULT_ARR_SIZE;
    FILE *inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL)
    {
        result.isValid = EOF_ERROR;
        return result;
    }
    char inputLine[1024];
    int lineCounter = 1;
    int arrLastIndex = 0;
    if (fgets(inputLine, sizeof(inputLine), inputFile) == NULL)
    {
        result.isValid = EMPTY_FILE;
        fclose(inputFile);
        return result;
    }
    else
    {
        // handling first line.
        if (setLength(inputLine, &result) == EXIT_FAILURE)
        {
            result.isValid = lineCounter;
            fclose(inputFile);
            return result;
        }
        lineCounter++;
        fgets(inputLine, sizeof(inputLine), inputFile);

        // handling second line.
        if (setNumOfLinkers(inputLine, &result) == EXIT_FAILURE)
        {
            result.isValid = lineCounter;
            fclose(inputFile);
            return result;
        }
        lineCounter++;
        fgets(inputLine, sizeof(inputLine), inputFile);

        // handling third line.
        result.linkersSet = (char *) calloc(result.numOfDifferentLinkers, sizeof(char) * (result.numOfDifferentLinkers));
        if (!createLinkersSet(inputLine, result.linkersSet, &result))
        {

            result.isValid = lineCounter;
            fclose(inputFile);
            return result;
        }
        lineCounter++;
        fgets(inputLine, sizeof(inputLine), inputFile);

        // handling fourth-nth line.
        result.sections = (Section *) calloc(1, sizeof(Section));
        int startIndex = -1;
        int endIndex = -1;
        if(!addSectionToSectionArray(inputLine, result.sections, arrLastIndex))
        {
            result.isValid = lineCounter;
            fclose(inputFile);
            free(result.linkersSet);
            result.linkersSet = NULL;
            return result;
        }
        startIndex = findlinkerInlinkersSet(result.sections[lineCounter - 4].start, result.linkersSet,
                                                result.numOfDifferentLinkers);
        endIndex = findlinkerInlinkersSet(result.sections[lineCounter - 4].end, result.linkersSet,
                                              result.numOfDifferentLinkers);
        if ((startIndex == -1) || (endIndex == -1))
        {
            result.isValid = lineCounter;
            fclose(inputFile);
            free(result.linkersSet);
            result.linkersSet = NULL;
            free(result.sections);
            result.sections = NULL;
            return result;
        }
        result.sections[lineCounter - 4].startIndex = startIndex;
        result.sections[lineCounter - 4].endIndex = endIndex;
        result.sections = result.sections;
        lineCounter++;

        while (lineCounter > 4 && fgets(inputLine, sizeof(inputLine), inputFile) != NULL)
        {
            //memory reallocation to the rest of lines.
            arrLastIndex++;
            result.sections = (Section *) realloc(result.sections, sizeof(Section) *
            (arrLastIndex + 1));

            if(!addSectionToSectionArray(inputLine, result.sections, arrLastIndex))
            {
                result.isValid = lineCounter;
                fclose(inputFile);
                free(result.linkersSet);
                result.linkersSet = NULL;
                return result;
            }

            startIndex = findlinkerInlinkersSet(result.sections[lineCounter - 4].start, result.linkersSet,
                                                    result.numOfDifferentLinkers);
            endIndex = findlinkerInlinkersSet(result.sections[lineCounter - 4].end, result.linkersSet,
                                                  result.numOfDifferentLinkers);
            if ((startIndex == -1) || (endIndex == -1))
            {
                result.isValid = lineCounter;
                fclose(inputFile);
                free(result.linkersSet);
                result.linkersSet = NULL;
                free(result.sections);
                result.sections = NULL;
                return result;
            }
            result.sections[lineCounter - 4].startIndex = startIndex;
            result.sections[lineCounter - 4].endIndex = endIndex;
            lineCounter++;
        }
    }
    free(result.linkersSet);
    result.linkersSet = NULL;
    result.sectionArrSize = lineCounter - 4;
    fclose(inputFile);
    return result;
}

//----------Rail way problem algorithm.-----------

/**
 * @brief finds the most lower price for a given length rail, and a given index of linker.
 * @param priceToAdd
 * @param input
 * @param table
 * @param index
 * @param length
 * @return lower price.
 */
int findLowerPrice(int priceToAdd, Input input, int** table, int index, int length)
{
    int lowVal;
    if (priceToAdd == INT_MAX)
    {
        lowVal = INT_MAX;
    }
    else
    {
        lowVal = input.sections[index].price + priceToAdd;
    }
    if (lowVal < table[length][input.sections[index].endIndex])
    {
        table[length][input.sections[index].endIndex] = lowVal;
    }
    return lowVal;
}

/**
 * @brief Finds the minimal price to requested railway.
 * @param input
 * @return minimal price.
 */
int findMinimalPrice(Input input)
{
    // initializing the table.
    int ** table;
    table = (int**) malloc((input.railwayLength + 1) * sizeof(int*));
    for (int i = 0; i < input.railwayLength + 1; i++)
    {
        table[i] = (int*) malloc(input.numOfDifferentLinkers * sizeof(int));
    }
    for (int i = 0; i < input.railwayLength + 1; i++)
    {
        for (int j = 0; j < input.numOfDifferentLinkers; j++)
        {
            table[i][j] = INT_MAX;
        }
    }
    // base case:
    for (int i = 0; i < input.numOfDifferentLinkers; i++)
    {
        table[0][i] = 0; // first line is always zero
    }
    int diff;
    for (int l = 1; l < input.railwayLength + 1; l++)
    {
        for (int i = 0; i < input.numOfDifferentLinkers; i++)
        {
            for (int j = 0; j < input.sectionArrSize; j++)
            {
                diff = l - input.sections[j].length;
                if (input.sections[j].endIndex == i && diff >= 0)
                {
                    int priceToAdd = table[diff][input.sections[j].startIndex];
                    findLowerPrice(priceToAdd, input, table, j, l);
                }
            }
        }
    }
    free(input.sections);
    input.sections = NULL;
    // taking the min val for the whole trail:
    int minPrice = INT_MAX;
    for (int i = 0; i < input.numOfDifferentLinkers; i++)
    {
        if (table[input.railwayLength][i] < minPrice)
        {
            minPrice = table[input.railwayLength][i];
        }
    }
    for (int i = 0; i < input.railwayLength + 1; i++)
    {
        free(table[i]);
        table[i] = NULL;
    }
    free(table);
    table = NULL;

    if (minPrice == INT_MAX || minPrice == 0)
    {
        minPrice = -1;
    }
    if (input.railwayLength == 0)
    {
        minPrice = 0;
    }
    return minPrice;
}

/**
 * @brief driver of the program.
 * @param argc
 * @param argv
 * @return 0 if succeeded, else 1.
 */
int main(int argc, char *argv[])
{
    FILE *output = fopen("railway_planner_output.txt", "w+");
    if (argc != 2)
    {
        fprintf(output, "Usage: RailwayPlanner <InputFile>");
        fclose(output);
        return EXIT_FAILURE;
    }
    Input result = parse(argv[1]);
    if (result.isValid == -2)
    {
        fprintf(output, "File doesn't exists.");
        fclose(output);
        return EXIT_FAILURE;
    }
    if (result.isValid == -1)
    {
        fprintf(output, "File is empty.");
        fclose(output);
        return EXIT_FAILURE;
    }
    if (result.isValid > 0)
    {
        fprintf(output, "Invalid input in line: %d.", parse(argv[1]).isValid);
        fclose(output);
        return EXIT_FAILURE;
    }

    int minimalPrice = findMinimalPrice(result);
    fprintf(output, "The minimal price is: %d", minimalPrice);
    fclose(output);
    return EXIT_SUCCESS;
}
