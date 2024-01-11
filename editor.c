/*
A basic text editor written as an exercise in coding
a terminal-based editor in the C language.
*/

#include <stdio.h>

#define LIMIT 25000 // the limit for line length / number of lines

// function definitions
// command functions
int read_input(char s[]); // read the input to parse commands
// string modification functions
void strip_newline(char s[]); // remove newline characters from a given input string
int squeeze(char s[], char c); // remove given character from a string
int line_squeeze(char s1[], char s2[]); // remove from s1 all characters occurring in s2
void condense(char s[]); // removes any recurring instance of any character in the given string
// file functions
void save_file(FILE *fptr);
// misc functions
int count_chars(char s[]);
void print_lines();
void print_lines_numbered();

// enumerators
enum Mode { // mode the text editor is currently operating within
  INSERT,
  PROMPT
};
enum IsActive { // whether the application should keep the core input loop alive
  TRUE,
  FALSE
};

// global variable definitions
FILE *f; // pointer to our file
char *path = "test_output.txt"; // path to our file (temporarily set here for testing)
char *prompt = ">"; // the command prompt
int mode = INSERT;
int active = TRUE;
char lines[LIMIT][LIMIT];
int line_idx = 1; // line we are currently on; we use 1 so we can use '0' to prepend the first line of the file
int total_lines = 0; // the total number of lines with data on them

int main() {
  while (active == TRUE) {
    printf("%s", prompt);
    fgets(lines[line_idx], LIMIT, stdin);
    strip_newline(lines[line_idx]); // remove newline chars
    int parsed = read_input(lines[line_idx]); // check if a command was input
    if (!parsed) {
      line_idx++;
      total_lines++;
    }
  }

  return 0;
}

// read the input string to parse and execute user commands
int read_input(char s[]) {
  int command_parsed = 0;
  
  // was the command prefix specified?
  if (s[0] == '.') {
    // check for a blank command prompt
    if (s[1] == '\0') {
      printf("?BLANK COMMAND?\n");
    } else {
      // check following characters
      for (int i = 1; s[i] != '\0'; i++) {
        switch (s[i]) {
          case 'q':
            active = FALSE;
            break;
          case 'w':
            save_file(f);
            break;
          case 'p':
            print_lines();
            break;
          case 'n':
            print_lines_numbered();
            break;
          default:
            printf("?COMMAND %d NOT RECOGNIZED?\n", i);
            break;
        }
      }
    }
    command_parsed = 1;
  }
  
  return command_parsed; // 1 if successful
}

// count the number of characters in a given string and return the int
int count_chars(char s[]) {
  int count;
  for (count = 0; s[count] != '\0'; count++) ;
  return count;
}

// strip all newline characters from the input string
void strip_newline(char s[]) {
  int i, j;

  for (i = j = 0; s[i] != '\0'; i++) {
    if (s[i] != '\n') {
      s[j++] = s[i];
    }
  }
  s[j] = '\0';
}

// save the open file to disk
void save_file(FILE *fptr) {
  fptr = fopen(path, "w"); // open our file in write mode
  // write the text in memory to our file
  for (int i = 1; i <= total_lines; i++) {
    fprintf(fptr, "%s\n", lines[i]);
  }
  fclose(fptr); // close our open file (free mem, unlock it for other programs)

  printf("File saved to %s.\n", path);
}

// print the document
void print_lines() {
  for (int i = 1; i <= total_lines; i++) {
    printf("%s\n", lines[i]);
  }
}

// print the document with prepended numbers
void print_lines_numbered() {
  for (int i = 1; i <= total_lines; i++) {
    printf("%5d %s\n", i, lines[i]);
  }
}