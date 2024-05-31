/*
Name: Basim Khokhar
Date: November 11th, 2023
Description: This .cpp file contains all necessary functions for a program that allows a
             user to create a colored landmass map. Through the use of user given data and
             the functions dirtBallDrop, findMax, normalizeMapData, randIndex, displayLand (for specifc
	         array type), createAsciiMap, and storeToFile (for specifc array type), the main method
	         will output the raw, normalized, and final landmass maps to the console and store
	         this output in three respective files (raw_landmass.txt, normalized_landmass.txt,
	         and final_landmass.txt).
*/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <cstring> // used for strcmp() function
#include <math.h> // used for the findMax() functions math operations
#include <time.h> // used for randomization features
#include <fstream> // for ofstream (output file wrting) usage
#include "termcolor.hpp" // for the added Color output
using namespace std;

// Forward declarations for all non-main functions utilized
void dirtBallDrop(int**, int, int, int, int, int, int);
int findMax(int**, int, int);
void normalizeMapData(int**, int, int, int);
double randIndex();
void displayLand(int**, int, int); // specific for the raw and normalzied int arrays
void displayLand(char**, int, int); // specific for the final char array
char** createAsciiMap(int**, int, int, int);
void storeToFile(int**, int, int, int); // specific for the raw and normalized int arrays
void storeToFile(char**, int, int); // specific for the final char array

// This main function is the "driver" of the entire program. This function sets up the random
// seed value, enables the termcolor output, recieves the user's inputted values, and checks if
// the user's values are valid. This function also calls the functions that creates a raw landmass,
// normalizes that data, finalizes the map, writes all three arrays to files, and stops and memory
// leaks by deleting the arrays. 
int main(int argc, char** argv)
{
  cout << termcolor::colorize; // allows for the char map coloring
  
  // checks if the aguments are properly given. If valid, the user's seed is applied
  // to srand. Otherwise, the computer is responsible for the (pseudo)random values 
  if ((argc == 3) && (strcmp(argv[1], "-s") == 0) && (argv[2] != NULL)) {
    srand(atoi(argv[2]));
  }
  else {
    srand(time(NULL)); // computer decides the randomness/seed through time()
  }

  int width, height, waterLine, radius, powerRate, numBalls; // user given information
  int **feild; // Will be used to hold, process, and write the Raw and Normalized maps.
  char **finalMap; // Will hold and write the values for the polished map of ASCII characters
  int i, j;
  int dx = -1; // dx and dy will be the randomly generated Center X/Y values
  int dy = -1;
  int maxFromFeild; //used to store the highest value form the Raw map.
  
  // Recieiving the inputs from the user
  cout << "Welcome to the Terraformer!\n";
  cout << "Enter grid width: ";
  cin >> width;
  cout << "Enter grid height: ";
  cin >> height;

  while (true) { // keeps asking for input until acceptable value is given
    cout << "Enter value for waterline (40-200): ";
    cin >> waterLine;
    if((waterLine > 200) || (waterLine < 40)) {  // ensures the waterLine is always between 40-200
      cout << "\nwaterline value is invalid.\n";
    } 
    else {
      break;
    }
  }

  while (true) { // keeps asking for input until acceptable value is given
    cout << "Enter dirtball radius (minimum 2): ";
    cin >> radius;
     if(radius < 2) {  // ensures the radius is always 2 or greater
       cout << "\nradius value is invalid.\n"; 
     } 
     else {
       break;
     }
  }
  
  while (true) { // keeps asking for input until acceptable value is given
    cout << "Enter dirtball power rating (minimum = radius): ";
    cin >> powerRate;
    if(powerRate < radius) { // ensures the powerRate is never less than radius
      cout << "\npowerRate value is invalid.\n";
    } 
    else {
      break;
    }   
  }
  
  cout << "Enter number of dirtballs to drop: ";
  cin >> numBalls;

  feild = new int*[height]; // initializing the array
  for (i = 0; i < height; i++) { // initializing the array of arrays by creating a new array of size width
    feild[i] = new int[width];
  }

  for (i = 0; i < height; i++) { // filling each index with 0
    for (j = 0; j < width; j++) {
      feild[i][j] = 0;
    }
  }

  // with each dirtball dropped, a X and Y index is "randomly" chosen by randIndex() and sent
  // to be used in the dirtBallDrop() function.
  for (i = 0; i < numBalls; i++) {
    dx = -1;
    dy = -1;
    while (!((0 <= dx) && (dx < width)) && !((0 <= dy) && (dy < height))) {
      dx = randIndex() * width; 
      dy = randIndex() * height; 
    }
    dirtBallDrop(feild, width, height, dx, dy, radius, powerRate); // drops one dirtball
  }
  
  displayLand(feild, width, height); // prints raw array to console
  storeToFile(feild, width, height, 0); // prints raw array to file
  
  maxFromFeild = findMax(feild, width, height); //calculates max
  normalizeMapData(feild, width, height, maxFromFeild); //normalizes the array
  displayLand(feild, width, height); // prints normalized array to console
  storeToFile(feild, width, height, 1); // prints normalized array to file
  
  finalMap = createAsciiMap(feild, width, height, waterLine); // creates and stores final ASCII  arrray
  displayLand(finalMap, width, height); // prints final colored char array to console
  storeToFile(finalMap, width, height); //prints char array to file

  // Starts the process of deleting the entire 2D arrays made for this program.
  // Starts with deleting the "inside" layer
  for (int k = 0; k < height; k++) {
    delete[] feild[k];
    delete[] finalMap[k];
  }
  delete[] feild;  // The "outside" layer of each created array is deleted - leading to no memory leaks
  delete[] finalMap;
 
  return 0;
}

// This dirtBallDrop function places a dirtball in a randomly generated centerX/Y integer. Using the
// power value, the distance from each index to the centerX/Y is calculated. Depending on the distance
// value, an impact value is calculated and added to the integer stored in the array's current index.
void dirtBallDrop(int** feild, int width, int height, int centerX, int centerY, int radius, int power) {
  int x, y;
  double distance; // stores the calculated distance
  int impactVal; // will be stored in the array
  double xCalc, yCalc, xPow, yPow; // used for distance calculation
  for (y = 0; y < height; y++) {  // distance is calculated and checked in every array index
    for (x = 0; x < width; x++) {
      xCalc = x - centerX;  // all these equations contribute to the distance formula
      yCalc = (y - centerY);
      xPow = (xCalc * xCalc);
      yPow = (yCalc * yCalc);
      distance = (sqrt(xPow + yPow)); // final step - result is the distance
      if (distance <= radius) {
	impactVal = power - (int)(floor(distance));
	// impact value is added to the value in the current index - existing values are not overwritten 
	feild[y][x] += impactVal; 
      }
    }
  }
}

// This overloaded displayLand function is primarily used for the raw and normalized arrays. The values
// in the int arrays are printed to the console with two newLines added for organizaiton.  
void displayLand(int** feild, int width, int height) {
  int i, j;

  for (i = 0; i < height; i++) { // printing each index to console
    for (j = 0; j < width; j++) {
      cout << setw(4) << feild[i][j]; // setw(4) sets width of output to 4
    }
    cout << endl;
  }
  cout << "\n" << endl;
}

// This overloaded displayLand function is primarily used for the final char array. The values
// in the char array are printed to the console with a special color for each type of symbol.  
void displayLand(char** feild, int width, int height) {
  int i, j;
  char currVal;

  for (i = 0; i < height; i++) { // printing each index to console
    for (j = 0; j < width; j++) {
      currVal = feild[i][j];
      // "termcolor:color<red, green, blue>" from the termcolor.hpp file allows for 
      // the outputted text to have color to it. The cases are determined by the current
      // index's symbol. 
      switch(currVal){
      case '#':
	cout << termcolor::color<0, 17, 255> << "#";  // all "#" will be blue (deep water)
	break;
	
      case '~':
	cout << termcolor::color<132, 240, 255> << "~"; // all "~" will be light blue (shallow water)
	break;
	
      case '.':
	cout << termcolor::color<255, 220, 104> << "."; // all "." will be light orange (sandy coasts)
	break;

      case '-':
	cout << termcolor::color<142, 255, 107> << "-"; // all "-" will be light green (grassy plains)
	break;
	
      case '*':
	cout << termcolor::color<47, 172, 9> << "*"; // all "*" will be dark Green (forests)
	break;

      case '^':
	cout << termcolor::color<224, 241, 242> << "^"; // all "^" will be a blueish white (mountains)
	break;
      }
    }
    cout << endl;
  }
}

// This randIndex function generates random numbers pirmarily for the dirtball dropping array indexes
double randIndex()
{
  return (double)rand() / ((double)RAND_MAX+1); 
}

// This findMax function finds the largest integer in the created raw array of values. The array
// is fully traversed, index by index. 
int findMax(int** feild, int width, int height) {
  int largest = -1;
  int compareNum = -1;

  //traverses through the entire array, and replaces a temp variable with the largest (when found)
  for (int i = 0; i < height; i++) {
    for (int k = 0; k < width; k++) {
      compareNum = feild[i][k];
      if (compareNum > largest) { // if new large is found, replace it in "largest"
	largest = compareNum;
      }
    }
  }
  return largest; // whole array has been iterated thorugh, return the largest value
}

// This normalizeMapData function traverses throguh an entire array and normalizes each index's
// value on a scale of 0 to 255. This scale allows a user's map to always make visual sense. 
// Afterweards, the array will be ready to be finalized into its ASCII representation. 
void normalizeMapData(int** feild, int width, int height, int maxVal) {
  float highest = (float) maxVal; // recieved from the findMax() function called in main()
  int i = 0;
  int j = 0;

  // Normalizing each index by casting each element into a float to be divided by the highest
  // value as a float, then multiplied by 255.0. Result is casted back into an integer and stored.
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      feild[i][j] = (int) ((((float) (feild[i][j])) / highest) * 255.0);
    }
  }
}

// This createAsciiMap function finalizes the map by taking the normalized array and filling a
// new char 2D array with cooresponding and meaningful symbols. The user's waterline plays a
// major role in determining the final array's outcome.
char** createAsciiMap(int** feild, int width, int height, int waterLine) {
  float floatWaterL = (float) waterLine;
  float landZone = 255.0 - floatWaterL;
  char** polishedFeild; // new array needed to store chars (not integers like the other two arrays)
  int i = 0;
  int j = 0;

  polishedFeild = new char*[height]; // initializing the array
  for (i = 0; i < height; i++) { // initializing the array of arrays
    polishedFeild[i] = new char[width];
  }
  
  for (i = 0; i < height; i++) { // filling each index with 0
    for (j = 0; j < width; j++) {
      polishedFeild[i][j] = 0;
    }
  }

  // Each value in the array will receive a special Symbol that cooresponds to the 
  // normalized value of the element. These symbols are largely determined by the user's
  // given waterline value. Waterline and Landzone calculations are floored due to being 
  // compared to the array's integer values.
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      if (feild[i][j] < floor((0.5 * floatWaterL))) { // deep water
	polishedFeild[i][j] = '#';
      }
      else if ((feild[i][j] >= floor((0.5 * floatWaterL)) && (feild[i][j] <= floor(floatWaterL)))) {
	polishedFeild[i][j] = '~'; // shallow water
      } 
      else if ((feild[i][j] > floor(floatWaterL)) && (feild[i][j] < floor((floatWaterL + (0.15 * landZone))))) { 
	polishedFeild[i][j] = '.'; // sand/coast
      }	
      else if ((feild[i][j] > floor(floatWaterL)) && (feild[i][j] >= floor((floatWaterL + (0.15 * landZone)))) && (feild[i][j] < floor((floatWaterL + (0.40 * landZone))))) {
	polishedFeild[i][j] = '-'; // plains and grassy areas
      }
      else if ((feild[i][j] > floor(floatWaterL)) && (feild[i][j] >= floor((floatWaterL + (0.40 * landZone)))) && (feild[i][j] < floor((floatWaterL + (0.80 * landZone))))) {
	polishedFeild[i][j] = '*'; // forests
      }
      else { 
	polishedFeild[i][j] = '^'; // mountains
      }
    }
  }
  return polishedFeild; // new array with all the symbols is returned to be printed 
}

// This storeToFile function handles the raw and normalized arrays and prints the contents
// to a file called either raw_landmass.txt or normalized_landmass.txt, respectfully. The
// file name is determined with the finalChoice variable (given in the function call).
void storeToFile(int** feild, int width, int height, int fileChoice) {
  int i, j;
  ofstream file;
  
  // 0 indicates that the raw Array will be printed in raw_landmass.txt
  // 1 indicates that the normalized Array will be printed in normalized_landmass.txt
  if (fileChoice == 0) {
    file.open("raw_landmass.txt");
  }
  else if (fileChoice == 1) {
    file.open("normalized_landmass.txt");
  }
  for (i = 0; i < height; i++) { // filling each index with the chosen array's value
    for (j = 0; j < width; j++) {
      file << setw(4) << feild[i][j]; // setw(4) sets width of output to 4
    }
    file << endl;
  }
  file.close(); // close the file to avoid memory leaks
}

// This storeToFile function handles the final char ASCII array and prints the contents
// to a file called final_landmass.txt by iterating through each index. 
void storeToFile(char** feild, int width, int height) {
  int i, j;
  ofstream file;
  
  // Final ASCII Array will be always be printed in final_landmass.txt
  file.open("final_landmass.txt");
  for (i = 0; i < height; i++) { // filling each index with the ASCII value stored
    for (j = 0; j < width; j++) {
      file << feild[i][j]; // printing the current index to the file
    }
    file << endl;
  }
  file.close(); //close the file to avoid memory leaks
}
