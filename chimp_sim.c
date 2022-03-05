/*
 *Chimps On Typewriters Simulator.
 *Generates random chars, then tries to match them with English words.
 *
 *Last Modified: 24 Jan 19
 *Ver 1.02.65
 *ADBeta
 * 
 *Buffer:
 *[GENERATE] large buffer with fully random 8bit chars
 *[REMOVE] all chars that are not a-z and A-Z
 *[SHRINK] buffer to remove blank spaces from previous step
 *[CONVERT] to all caps
 *
 *Dictionary:
 *[POPULATE] arrays with all words of 2, 3, 4, etc chars length
 *[COMPARE] all words from dictionary wih buffer array, incrementing through all lengths
 *
 *Output:
 *[STORE] all matching words in a new array
 *[OUTPUT] to text file
 *
 * Code is cheesed in multiple places because I can't be fucked.
 *
 * todo
 * 0x0D is present at the end of all dictionary words. this is a CR signal from windows.
 * build a better function that removes these cause they suck ass.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//Random String Variables.
char rand_char_buffer[999999];
int rand_string_length = 999999;

//Dictionary Variables.
FILE *dictionary_file;
//Make Large 2D array for dictionary data.
char dictionary_array[500000][40];
unsigned int dictionary_word_count = 0;

//Output file Variables.
FILE *output_file;

//Matching Variables.
char matching_words[50000][40];

//Generate random number between 0x20 and 0x7F.
char random_char() {
	return (rand() % (0x7F - 0x20 + 1)) + 0x20;
}

//random array generator.
void gen_random_string(char output_buffer[], int input_length) {
	for(int buffer_position = 0; buffer_position < input_length; buffer_position++) {
		output_buffer[buffer_position] = random_char();
	}
}

//clean array of all non a-z/A-Z chars.
void purge_string(char input_buffer[], int input_length) {
	//Stage 1: Remove all non-matching chars, replace them with 0x1A (SUBTITUTE).
	for(int buffer_position = 0; buffer_position < input_length; buffer_position++) {
		//Remove complete outliers. 
		if(input_buffer[buffer_position] < 0x41 || input_buffer[buffer_position] > 0x7A) {
			input_buffer[buffer_position] = 0x1A;
		}
		//Remove unwanted chars between the a-z A-Z range.
		if(input_buffer[buffer_position] < 0x61 && input_buffer[buffer_position] > 0x5A) {
			input_buffer[buffer_position] = 0x1A;
		}
	}

	//Stage 2: Shorten the array, move all chars down until there are no 0x1A's present.
	int replace_mode = 1; //0 = done, 1 = replace.
	while(replace_mode == 1) {
		for(int buffer_position = 0; buffer_position < input_length; buffer_position++) {
			if(input_buffer[buffer_position] == 0x1A) {
				//If there is any 0x1A, go into replace mode.
				replace_mode = 1;
				int replace_position = buffer_position;
				//From where the error occured, go to the end.
				while(replace_position < input_length) {
					input_buffer[replace_position] = input_buffer[replace_position + 1];
					replace_position ++;
				}
				//If fault occurs, break after while loop.
				break;
			}
			//If no error occurs, go back to finished mode.
			replace_mode = 0;
		}
	}

	//Stage 3: Change the buffer length to match true length.
	rand_string_length = 0;
	while(input_buffer[rand_string_length] != 0) {
		rand_string_length++;
	}
}

//Capitalize all chars in the string.
void capitalize_string(char input_string[], int input_length) {
	for(int string_position = 0; string_position < input_length; string_position++) {
		//If char is within a-z range.
		if(input_string[string_position] >= 0x61 && input_string[string_position] <= 0x7A) {
			//Compound subtract 0x20 to get it within A-Z range.
			input_string[string_position] -= 0x20;
		}
		if(input_string[string_position] == 0x00) {
			break;
		}
	}
}

int get_word_length(char input_string[]) {
	int word_length = 0;
	
	while(input_string[word_length] >= 0x20) {
		word_length++;
	}
	
	return word_length;
}

int compare_strings(char input_string_one[], char input_string_two[]) {
	//For length of string only including normal chars.
	for(int string_position = 0; string_position < get_word_length(input_string_one); string_position++) {
		//If the two char at [index] don't match.
		if(input_string_one[string_position] != input_string_two[string_position]) {
			//Return non-match flag.
			return 1;
		}
	}
	//If completes without interrupt, return match flag.
	return 0;
}

unsigned int get_dictionary_word_count() {
	unsigned int word_counter = 0;
	
	while(dictionary_array[word_counter][0] != 0x00) {
		word_counter++;
	}
	
	return word_counter;
}

void generate_dictionary_array() {
	//Set all dictionary data to 0.
	memset(dictionary_array, 0x00, sizeof dictionary_array);

	int char_counter = 0;
	unsigned long word_counter = 0;
	char current_char;	

	//while there is data in the file.
	while((current_char = fgetc(dictionary_file)) != EOF) {
		if(current_char != '\n') {
			//If isn't a newline, add it char-by-char to array.
			dictionary_array[word_counter][char_counter] = current_char;
			char_counter++;
		} else {
			//Else, go to next word in array and start from position 0.
			word_counter++;
			char_counter = 0;
		}
	}
	//Count the ammount of words in the array for later.
	dictionary_word_count = get_dictionary_word_count();
}

void find_matching_words() {
	memset(matching_words, 0x00, sizeof matching_words);
	int found_match = 0;
	long matching_array_position = 0;
	char word_under_test[40];
	char random_string_buffer[40];
	for(long current_word = 0; current_word < dictionary_word_count; current_word++) {
		found_match = 0;
		//Blank the word buffer.
		memset(word_under_test, 0x00, 40);

		strcpy(word_under_test, dictionary_array[current_word]);
		capitalize_string(word_under_test, 40);
		for(long random_string_pos = 0; random_string_pos < (rand_string_length - get_word_length(word_under_test)); random_string_pos++) {
			memset(random_string_buffer, 0x00, 40);
			strncpy(random_string_buffer, rand_char_buffer + random_string_pos, get_word_length(word_under_test));
			if(compare_strings(random_string_buffer, word_under_test) == 0 && found_match == 0) {
				strcpy(matching_words[matching_array_position], random_string_buffer);
				matching_array_position++;
				found_match = 1;
				//printf("%s\n", random_string_buffer);
				}
			}
		
	}
}

int main() {
	//Seed the random generator with time variable.
	srand(time(NULL));

	//Open dictionary file for reading. If doesn't exist, show error.
	if((dictionary_file = fopen("dict_alpha.txt", "r")) == NULL) {
		printf("Error opening dictionary file.");
	}

	//Open output file for writing.
	output_file = fopen("words_found.txt", "w");

	//Generate the completely random string.
	gen_random_string(rand_char_buffer, rand_string_length);
	//Purge it of all unwanted chars. This will change rand_string_length.
	purge_string(rand_char_buffer, rand_string_length);
	//Capitalize string.
	capitalize_string(rand_char_buffer, rand_string_length);

	//Generate the dictionary array.
	generate_dictionary_array();
	dictionary_word_count = get_dictionary_word_count();
	//Find all matching words in the array.
	find_matching_words();

	int temp_var = 0;
	while(matching_words[temp_var][0] != 0) {
		fprintf(output_file, "%s\n", matching_words[temp_var]);
		temp_var++;
	}

	//Close file sessions.
	fclose(dictionary_file);
	fclose(output_file);
	return 0;
} 
