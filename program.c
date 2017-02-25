#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void readFileToCompress(char *);
void readFileToExpand(char *);
void compress(char **, int, int, int *, FILE *);
char * decToBin(long int);


int main( int argc, char *argv[] ) {
	//Check the arguements passed in
	if (argc != 2) {
		printf("Usage: %s is filename\n", argv[1]);
	} else {
		//If the arg passed in is a compressed mitch file
		if (strstr(argv[1], ".mitch") != NULL) {
		    readFileToExpand(argv[1]);
		} else {
			readFileToCompress(argv[1]);
		}
	}
	return EXIT_SUCCESS;
}

void readFileToCompress(char * fileName) {
	//Pointers to the open files
	FILE * filePtr;
	FILE * fileWritePtr;

	char oriFile[256];
	strcpy(oriFile, fileName);

	int currentChar;
	int pattern[3];
	char * information = NULL;
	int reading = 0;
	int currentAmountOfChars;
	int currentShift;
	int currentBitCount;
	int total = 0;l

	//Open the read and write files
	if ((filePtr = fopen(fileName, "r")) != NULL)
	{
		if ((fileWritePtr = fopen(strcat(fileName, ".mitch"), "wb")) == NULL)
		{
			printf("Error creating compressed file.");
		}

		int ch = getc(filePtr);
		int skip = 0;
		int spaceRead = 0;
	
		//Loop through till the end of the file
		while (ch != EOF) {
			//Get the current Byte
			char byte = tolower((char)ch);

			spaceRead = 0;
			if (reading == 0) {

				//Free the previous allocation
				if (information != NULL) {
					free(information);
				}
				//Create a new allocaiton
				information = (char *)malloc(256);
				//memset(information, 0, sizeof(char));
				for (int x = 0; x < 256; x++) {
					for (int i = 7; i >= 0; i--) {
						information[x] = 0 << i;
					}
				}

				currentAmountOfChars = 1;
				total++;
				currentShift = 7;
				currentBitCount = 0;
				skip = 0;
				//Create the pattern to look for
				int check;
				int current = 0;
				for (check = 7; 5 <= check; check--) {
					pattern[current] = (byte >> check) & 0x01;
					current++;
				}

				reading = 1;
			} else {
				//Check the start of the char to make sure it matches the current
				int check;
				int current = 0;
				for (check = 7; 5 <= check; check--) {
					if (pattern[current] != ((byte >> check) & 0x01)) {
						reading = 0;
					} 
					current++;
				}
				
				//Check for the space first 3 bit pattern
				if (((byte >> 7) & 0x01) == 0 && ((byte >> 6) & 0x01) == 0 && ((byte >> 5) & 0x01) == 1) {
					//The pattern mathes a space pattern
					if (((byte >> 4) & 0x01) == 0 &&
						((byte >> 3) & 0x01) == 0 &&
						((byte >> 2) & 0x01) == 0 &&
						((byte >> 1) & 0x01) == 0 &&
						((byte >> 0) & 0x01) == 0
					) {
							//It is a space so we will continue reading
							reading = 1;
					}
				}
				
				//If we hit the limit of what we can store, restart the compression
				if (currentAmountOfChars >= 254) {
					spaceRead = 0;
					reading = 0;			
				}			
			}
			
			//If we are still reading in new charcters, store them
			if (reading == 1) {
				int i;
				for (i = 4; 0 <= i; i--) {
					//If you are shifted all the way through the current byte
					//move to the next byte
					if (currentShift < 0) {
						currentShift = 7;
						currentAmountOfChars++;
						total++;
					}

					information[currentAmountOfChars - 1] |= ((byte >> i) & 0x01) << currentShift;
					
					currentShift--;
					currentBitCount++;
				}

				//If we reach the max amount we can store in our format, restart
				if (currentAmountOfChars >= 254) {
					reading = 0;			
				}
			}

			if (reading == 0) {
				//Skip getting a new character
				skip = 1;
				compress(&information, currentAmountOfChars, currentBitCount, pattern, fileWritePtr);
			}	

			if (skip == 0) {
				ch = getc(filePtr);
			}
		}
		
		//Compress the final string of information
		compress(&information, currentAmountOfChars, currentBitCount, pattern, fileWritePtr);

		fclose(filePtr);
		fclose(fileWritePtr);

		remove(oriFile);
	}
}

void compress(char ** pointerPassed, int amount, int totalBitCount, int * pattern, FILE * fileWritePtr) {

	char * string = *pointerPassed;

	//If the amount is greater than 0 we will compress
	if (amount > 1) {
		amount -= 1;

		//Check to see if we can save more information from the characters passed in
		int checking = 0;
		int matchPattern[5];
		int currentChecking = 0;
		int currentBitChecking = 2;
		int currentCharIndex = 0;
		int currentBitCount = 0;
		int p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;

		//Set the matchPattern to the first char
		matchPattern[0] = ((string[0] >> 7) & 0x01);
		matchPattern[1] = ((string[0] >> 6) & 0x01);
		matchPattern[2] = ((string[0] >> 5) & 0x01);
		matchPattern[3] = ((string[0] >> 4) & 0x01);
		matchPattern[4] = ((string[0] >> 3) & 0x01);

		//Checks through each character to find matching binary reading
		while (checking == 0) {
			if (currentBitCount + 5 >= totalBitCount) {
				checking = 1;
			} else {
				//If the currentbit checking is to low, start at next bit
				if (currentBitChecking == -1) {
					currentBitChecking = 7;
					currentChecking++;
				}

				//When we finished checking the current char
				if (currentCharIndex > 4) {
					currentCharIndex = 0;
				}

				//Checks to see if the pattern does not match
				if (((string[currentChecking] >> currentBitChecking) & 0x01) != matchPattern[currentCharIndex]) {
					if (currentCharIndex == 0) {
						p1 = 1;
					} else if (currentCharIndex == 1) {
						p2 = 1;
					} else if (currentCharIndex == 2) {
						p3 = 1;
					} else if (currentCharIndex == 3) {
						p4 = 1;
					} else if (currentCharIndex == 4) {
						p5 = 1;
					} 
				}

				//Increment
				currentCharIndex++;
				currentBitChecking--;
				currentBitCount++;
			}

			//If none match, cancel the remaining check
			if (p1 == 1) {
				checking = 1;
			}
		}

		//Create the 3 chars for saving to the file
		char * repByte = (char *)malloc(3);
		memset(repByte, 0, sizeof(char));
		//Initalize all the bytes to 0
		for (int i = 0; i < 3; i++){
			for (int j = 7; 0 <= j; j--) {
				repByte[i] &= ~(1 << j);
			}
		}

		//Set the check byte to 1
		repByte[0] |= 1 << 7;

		char cTotal = '0';
		int total = 0;
		int amountOfChars;
		int individualChars = (totalBitCount / 5);

		//Calculate the total amount of repeating for bit values
		if (p1 == 0) {
			cTotal = '1';
			repByte[0] |= 1 << 6;
			amountOfChars = individualChars * 4;
			repByte[1] |= matchPattern[0] << 4;
			if (p2 == 0) {
				cTotal = '2';
				repByte[1] |= matchPattern[1] << 3;
				amountOfChars = individualChars * 3;
				if (p3 == 0) {
					cTotal = '3';
					repByte[1] |= matchPattern[2] << 2;
					amountOfChars = individualChars * 2;
					if (p4 == 0) {
						cTotal = '4';
						repByte[1] |= matchPattern[3] << 1;
						amountOfChars = individualChars * 1;
						if (p5 == 0) {
							cTotal = '5';
							repByte[1] |= matchPattern[4] << 0;
							amountOfChars = individualChars * 0;
						}
					}
				}
			}
		} else {
			amountOfChars = individualChars * 5;
		}

		if (amountOfChars % 8 > 0){
			total += 1;
		}
		total += (amountOfChars/8);

		//Set the amount for the multple values
		repByte[0] |= ((cTotal >> 2) & 0x01) << 2;
		repByte[0] |= ((cTotal >> 1) & 0x01) << 1;
		repByte[0] |= ((cTotal >> 0) & 0x01) << 0;

		//Set the pattern
		repByte[1] |= pattern[0] << 7;
		repByte[1] |= pattern[1] << 6;
		repByte[1] |= pattern[2] << 5;

		char * information = NULL;
		//Allocate the required amount of space + 50 buffer room
		information = (char *)malloc(total);
		memset(information, 0, sizeof(char));
		
		//Set all bits to 0
		for (int x = 0; x < total; x++) {
			for (int i = 7; i >= 0; i--) {
				information[x] = 0 << i;
			}
		}

		char * str = (char *)malloc(sizeof(char));
		sprintf(str, "%d", individualChars);
 		

		char * totalInBin = decToBin(individualChars);

 		//The total amount of bytes in the array
		repByte[2] |= ((totalInBin[0] >> 7) & 0x01) << 7;
		repByte[2] |= ((totalInBin[0] >> 6) & 0x01) << 6;
		repByte[2] |= ((totalInBin[0] >> 5) & 0x01) << 5;
		repByte[2] |= ((totalInBin[0] >> 4) & 0x01) << 4;
		repByte[2] |= ((totalInBin[0] >> 3) & 0x01) << 3;
		repByte[2] |= ((totalInBin[0] >> 2) & 0x01) << 2;
		repByte[2] |= ((totalInBin[0] >> 1) & 0x01) << 1;
		repByte[2] |= ((totalInBin[0] >> 0) & 0x01) << 0;

		free(totalInBin);
		free(str);

		//Save the information bytes to the file
		for (int x = 0; x < 3; x++) {
			fwrite(((void *)&repByte[x]), sizeof(char), 1, fileWritePtr);
		}

		int increment;
		int currentCheck = 7;
		int currentSave = 7;
		int currentCheckingChar = 0;
		int currentSavingChar = 0;
		int amountToSave = 0;
		int startingAmount;
		//Condense the information depending on the above
		switch(cTotal) {
			case '0':
				//No repeating bits found
				startingAmount = 0;
				amountToSave = 5;
				increment = 0;
				break;
			case '1':
				//1 Bit is repeating
				increment = 1;
				amountToSave = 4;
				startingAmount = 1;
				break;
			case '2':
				//2 Bit is repeating
				increment = 2;
				amountToSave = 3;
				startingAmount = 2;
				break;
			case '3':
				//3 Bit is repeating
				increment = 3;
				amountToSave = 2;
				startingAmount = 3;
				break;
			case '4':
				//All bits besides the last bit was repeating
				//loop through the amountofchars to print
				increment = 4;
				amountToSave = 1;
				startingAmount = 4;
				break;
			case '5':
				//All of the bits are repeating. 
				//No addition output is needed stores 3 bytes of information
				startingAmount = 5;
				break;
			default:
				startingAmount = 5;
				printf("No bits are repeating\n");
		}

		if (startingAmount != 5) {
			//Loop through all the required characters
			for (int x = 0; x < individualChars; x++) {
				//If we finished looping trough the current char byte
				while (increment != 0) {
					if (currentCheck < 0) {
						currentCheck = 7;
						currentCheckingChar++;
					}
					currentCheck--;
					increment--;
				}

				//Loop through the amount to save
				for (int i = 0; i < amountToSave; i++) {
					information[currentSavingChar] |= ((string[currentCheckingChar] >> currentCheck) & 0x01) << currentSave;
					currentSave--;
					currentCheck--;
					if (currentCheck < 0) {
						currentCheck = 7;
						currentCheckingChar++;
					}
					if (currentSave < 0) {
						currentSave = 7;
						currentSavingChar++;
					}
				}

				//Reset the increment
				increment = startingAmount;
			}

			//save to the file
			for (int x = 0; x < total; x++) {
				fwrite(((void *)&information[x]), sizeof(char), 1, fileWritePtr);
			}
		}

		if (information != NULL) {
			free(information);
		}
	} else if (amount == 1) {
		char byte = string[0];
		char * information = NULL;
		int total = 1;

		if (pattern[0] == 0) {
			//Since I found that typical text documents have a 0 for most of the characters
			// and other parts of text document, store them as is as the 0 will account for
			// the skip bit required
			information = (char *)malloc(1);

			for (int i = 7; i >= 0; i--) {
				information[0] = 0 << i;
			}
			
			//Add the information to the bits
			information[0] |= 0 << 7;
			information[0] |= pattern[1] << 6;
			information[0] |= pattern[2] << 5;
			information[0] |= ((string[0] >> 7) & 0x01) << 4;
			information[0] |= ((string[0] >> 6) & 0x01) << 3;
			information[0] |= ((string[0] >> 5) & 0x01) << 2;
			information[0] |= ((string[0] >> 4) & 0x01) << 1;
			information[0] |= ((string[0] >> 3) & 0x01) << 0;
		} else {

			//The only reason this part is here is for characters that have are on the higher end of the ascii table	
			// EX. €		
			information = (char *)malloc(2);

			for (int x = 0; x < 2; x++) {
				for (int i = 7; i >= 0; i--) {
					information[x] = 0 << i;
				}
			}

			//Set the first bit to a special check bit and the third bit to the unique character bit
			information[0] |= 1 << 7;
			information[0] |= 0 << 6;
			information[0] |= 1 << 5;
			information[0] |= 0 << 4;
			information[0] |= 0 << 3;
			information[0] |= 0 << 2;
			information[0] |= 0 << 1;
			information[0] |= 0 << 0;

			information[1] |= pattern[0] << 7;
			information[1] |= pattern[1] << 6;
			information[1] |= pattern[2] << 5;
			information[1] |= ((string[0] >> 7) & 0x01) << 4;
			information[1] |= ((string[0] >> 6) & 0x01) << 3;
			information[1] |= ((string[0] >> 5) & 0x01) << 2;
			information[1] |= ((string[0] >> 4) & 0x01) << 1;
			information[1] |= ((string[0] >> 3) & 0x01) << 0;

			total = 2;
		}

		//save to the file
		for (int x = 0; x < total; x++) {
			fwrite(((void *)&information[x]), sizeof(char), 1, fileWritePtr);
		}

		if (information != NULL) {
			free(information);
		}
	}
}

//Converts the decminal number to a binary number
char * decToBin(long int decimalNumber) {
	long int remainder,quotient;

	char * charRep = (char *)malloc(sizeof(char));

    int binaryNumber[100],i=1,j;

    quotient = decimalNumber;

    int current = 0;

	for(int i = 7; i >= 0; i--) {
    	charRep[0] |= 0 << i;
    }

    while(quotient!=0){

         charRep[0] |= (quotient % 2) << current;

         quotient = quotient / 2;

         current++;
    }

    return charRep;
}

//cPtrPtr -> pointer to pointer of characters
//iNumChars -> Amount of chars to trim from right
void trimRight(char ** cPtrPtr, int iNumChars) {
	int iDiff = 0;

	//Create a pointer to the original array
	char * cTempPtr = *(cPtrPtr);

	//Difference in length
	iDiff = strlen(cTempPtr) - iNumChars;

	//Allocate space for the new array
	*cPtrPtr = (char *)malloc((iDiff + 1) * sizeof(char));

	//Copy the data from the old string to the new string
	memcpy(*cPtrPtr, cTempPtr, iDiff * sizeof(char));

	//Set the null terminatior
	*(*cPtrPtr + iDiff) = '\0';
}

void readFileToExpand(char * fileName) {
	FILE * filePtr;
	FILE * fileWritePtr;
	
	//Open the read and write files
	if ((filePtr = fopen(fileName, "rb")) != NULL)
	{
		trimRight(&fileName, 6);
		if ((fileWritePtr = fopen(fileName, "w")) == NULL)
		{
			printf("Error creating uncompressed file.");
		} 

		int ch = getc(filePtr);
		int skip = 0;
		int readNextByteAsChar = 0;
		
		//While we have not hit the end of the file
		while (ch != EOF) {
			char byte = (char)ch;

			if (readNextByteAsChar == 0) {
				//Check the Check bit
				if (((byte >> 7) & 0x01) == 0) {
					//The check bit was set to 0
					//This means we can interperet the character literally
					fprintf(fileWritePtr, "%c", byte);
				}
				//A character that is on the higher end of the ascii table ex)€
				else if (((byte >> 7) & 0x01) == 1 && ((byte >> 5) & 0x01) == 1) {
					//Set the next byte read to be read as actual
					readNextByteAsChar = 1;
				} 
				//This is the compressed test to be extracted
				else {
					//Check the multiple bit
					int isMultiple = ((byte >> 6) & 0x01);
					int repeatingBitsAmount;
					int repeatingBits[5];
					int pattern[3];
					int amountOfChars = 0;

					//Grab the amount of repeating bits
					char numberOfRepeating = ' ';
					//To be safe, set all the bits to 0
					for (int x = 7; x >= 0; x--) {
						numberOfRepeating = 0 << x;
					}

					//This sets the char to by a number, the last 3 digits are stored in the information bit
					numberOfRepeating |= 1 << 5;
					numberOfRepeating |= 1 << 4;
					numberOfRepeating |= ((byte >> 2) & 0x01) << 2;
					numberOfRepeating |= ((byte >> 1) & 0x01) << 1;
					numberOfRepeating |= ((byte >> 0) & 0x01) << 0;

					//Convert from char to int
					repeatingBitsAmount = numberOfRepeating - '0';
					if (repeatingBitsAmount > 5) {
						printf("Invalid amount of repeating bits. Setting to default\n");
						repeatingBitsAmount = 0;
					}

					//Gather the next byte
					ch = getc(filePtr);
					byte = (char)ch;

					//Gathers the pattern from the 2nd bit
					for (int x = 7; x >= 5; x--) {
						pattern[7 - x] = ((byte >> x) & 0x01);
					}
					//Gathers the repeating bits from the second bit
					for (int x = 4; x >= 5 - repeatingBitsAmount; x--) {
						repeatingBits[4 - x] = ((byte >> x) & 0x01);
					}				

					//Gather the next byte
					ch = getc(filePtr);
					byte = (char)ch;

					//Convert the binary to decimal format
					if (((byte >> 7) & 0x01) == 1) {
						amountOfChars += 128;
					}
					if (((byte >> 6) & 0x01) == 1) {
						amountOfChars += 64;
					}
					if (((byte >> 5) & 0x01) == 1) {
						amountOfChars += 32;
					}
					if (((byte >> 4) & 0x01) == 1) {
						amountOfChars += 16;
					}
					if (((byte >> 3) & 0x01) == 1) {
						amountOfChars += 8;
					}
					if (((byte >> 2) & 0x01) == 1) {
						amountOfChars += 4;
					}
					if (((byte >> 1) & 0x01) == 1) {
						amountOfChars += 2;
					}
					if (((byte >> 0) & 0x01) == 1) {
						amountOfChars += 1;
					}

					//Now we have all the required parts to rebuild this line
					int currentChar = 0;
					int currentBit = 7;

					byte = (char)getc(filePtr);

					for (int x = 0; x < amountOfChars; x++) {
						//Create and set all bits to 0
						char outputChar = ' ';
						for (int i = 7; i >= 0; i--) {
							outputChar = 0 << x;
						}
						//Add the pattern
						for (int x = 7; x >= 5; x--) {
							outputChar |= pattern[7 - x] << x;
							//printf("%d", ((outputChar >> x) & 0x01));
						}	
						//Loop through all the repeating bits
						for (int x = 4; x >= 5 - repeatingBitsAmount; x--) {
							outputChar |= repeatingBits[4 - x] << x;
							//printf("%d", repeatingBits[4 - x]);
						}	
						//Grab the remaining non-repeating bits
						for (int y = 4 - repeatingBitsAmount; y >= 0; y--) {
							outputChar |= ((byte >> currentBit) & 0x01) << y;
							//printf("%d", (byte >> currentBit) & 0x01);
							currentBit--;
							if (currentBit < 0) {
								currentBit = 7;
								currentChar++;
								//If the current char we are at is less than the char we are looking for
								if (currentChar + 1 < amountOfChars) {
									byte = (char)getc(filePtr);
								}
							}
						}

						//Check to see if char == special space bit
						if (
							 ((outputChar >> 4) & 0x01) == 0 &&
							 ((outputChar >> 3) & 0x01) == 0 &&
							 ((outputChar >> 2) & 0x01) == 0 &&
							 ((outputChar >> 1) & 0x01) == 0 &&
							 ((outputChar >> 0) & 0x01) == 0
							) {
							fprintf(fileWritePtr, "%c", ' ');
						} else {
							fprintf(fileWritePtr, "%c", outputChar);
						}
					}
				}
			} else {
				//Read as literal
				fprintf(fileWritePtr, "%c", byte);
			}

			ch = getc(filePtr);
		}
	}
	fclose(filePtr);
	fclose(fileWritePtr);
}
