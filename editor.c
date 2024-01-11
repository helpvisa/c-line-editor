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
void copy_string(char from[], char to[]); // copy a string to another string
void strip_newline(char s[]); // remove newline characters from a given input string
int squeeze(char s[], char c); // remove given character from a string
int line_squeeze(char s1[], char s2[]); // remove from s1 all characters occurring in s2
void condense(char s[]); // removes any recurring instance of any character in the given string
void shift_lines_down(int from, int to); // shifts all lines down starting at a point and ending at another point
// file functions
void save_file(FILE *fptr);
void read_file(FILE *fptr);
// misc functions
int count_chars(char s[]);
void print_lines();
void print_lines_numbered();
void print_help();
/* --> will have to include a function which checks all lines up to current total line count
in order to account for 'deleting' a line at the end of the file and leaving blank lines behind */

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
char *path; // path to our file (temporarily set here for testing)
char *prompt = ">"; // the command prompt
char command = '.'; // the character which prompts command parsing
int mode = PROMPT;
int active = TRUE;
char lines[LIMIT][LIMIT];
char order[LIMIT]; // the last order input at the prompt
int line_idx = 1; // line we are currently on; we use 1 so we can use '0' to prepend the first line of the file
int total_lines = 0; // the total number of lines with data on them

int main(int argc, char *argv[]) {
  // process args
  printf("Welcome to %s!\nType 'h' for a list of commands.\n", argv[0]);
  if (argc > 1) {
    path = argv[1];
    read_file(f);
  } else {
    path = "untitled.txt";
    printf("No file specified. Editing untitled.txt\nUse 'f' to set filename.\n");
  }

  while (active == TRUE) {
    switch (mode) {
      case INSERT:
        fgets(order, LIMIT, stdin);
        strip_newline(order); // remove newline chars
        int parsed = read_input(order); // check if a command was input
        if (!parsed) {
          // copy order string to line index
          copy_string(order, lines[line_idx]);
          // we run this check first, as it is in consideration of our current line, not the next one
          if (line_idx > total_lines) {
            total_lines = line_idx; // accounts for blank lines
          }
          line_idx++;
        }
        break;
      default: // we default to PROMPT mode if no other mode is set
        printf("%s", prompt);
        fgets(order, LIMIT, stdin);
        strip_newline(order);
        read_input(order); // check if a command was input
        break;
    }
  }

  return 0;
}

//---functions---///
// copy one string to another
void copy_string(char from[], char to[]) {
  int i;
  for (i = 0; from[i] != '\0'; i++) {
    to[i] = from[i];
  }
  to[i] = '\0'; // EOF null character
}

// read the input string to parse and execute user commands
int read_input(char s[]) {
  int command_parsed = 0;
  
  // was the command prefix specified in insert mode?
  if (mode == INSERT && s[0] == command && s[1] == '\0') {
    mode = PROMPT;
    command_parsed = 1;
    printf("Entering prompt mode...\n");
    return command_parsed;
  } else if (mode == PROMPT) {
    // check for a blank command prompt
    if (s[0] == '\0') {
      printf("? Blank command.\n");
    } else if (s[0] != command) {
      int stop_parsing = 0; // if 1, do not execute next command
      // check following characters
      for (int i = 0; s[i] != '\0' && stop_parsing == 0; i++) {
        switch (s[i]) {
          case 'i':
            mode = INSERT;
            printf("Entering insert mode...\n");
            break;
          case 'd':
            lines[line_idx][0] = '\0';
              if (line_idx <= total_lines) {
              shift_lines_down(line_idx,total_lines);
              total_lines--; // we have deleted one line
              printf("Current line (%d) deleted.\n", line_idx);
              if (line_idx > total_lines+1)
                line_idx = total_lines+1;
            } else printf("Current line (%d) is already empty.\n", line_idx);
            break;
          case 'q':
            active = FALSE;
            break;
          case 'w':
            save_file(f);
            break;
          case 'r':
            read_file(f);
            break;
          case 'p':
            print_lines();
            break;
          case 'n':
            print_lines_numbered();
            break;
          case 'c':;
            int count = count_chars(lines[line_idx]);
            printf("%d character%s on current line (%d).\n", count, (count > 1 || count < 1) ? "s" : "", line_idx);
            printf("%s\n", lines[line_idx]);
            break;
          case 'l':
            printf("%d total lines in document.\n", total_lines);
            break;
          case 'f':;
            char new_name[LIMIT];
            int length = 0;
            for (int j = i+1; s[j] != '\0' && s[j] != '\n'; j++) {
              new_name[j-i-1] = s[j];
              length++;
            }
            if (length > 0) {
              path = new_name;
              printf("Changed filename to %s\n", path);
            } else printf("No filename entered!\nEditing %s\n", path);
            stop_parsing = 1;
            break;
          case 'g':;
            int goto_line = 0; // the line we target
            for (int j = i+1; s[j] >= '0' && s[j] <= '9'; j++) {
              goto_line *= 10;
              goto_line += s[j] - '0'; // convert char to real int
            }
            if (goto_line > -1 && goto_line < LIMIT) {
              line_idx = goto_line;
            } else {
              line_idx = 0;
            }
            printf("Sitting on line %d...\n", line_idx);
            break;
          default:
            if (!(s[i] >= '0' && s[i] <= '9'))
              printf("? Command '%c' not recognized.\n", s[i]);
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

// shift all lines down from the start point up to the end point
void shift_lines_down(int from, int to) {
  for (int i = from; i < to; i++) {
    copy_string(lines[i+1], lines[i]);
  }
}

// save the open file to disk
void save_file(FILE *fptr) {
  int total_characters = 0; // total number of characters saved to disk

  fptr = fopen(path, "w"); // open our file in write mode
  // write the text in memory to our file
  for (int i = 1; i <= total_lines; i++) {
    total_characters += count_chars(lines[i]) + 1; // we add an extra 1 to account for the missing 'newline' added by our fprintf
    fprintf(fptr, "%s\n", lines[i]);
  }
  fclose(fptr); // close our open file (free mem, unlock it for other programs)

  printf("\033[33m%d bytes saved to disk at %s.\033[0m\n", total_characters, path);
}

// open and read the specified file
void read_file(FILE *fptr) {
  fptr = fopen(path, "r");
  if (fptr != NULL) {
    line_idx = 1; // we start reading from line 1
    while (fgets(lines[line_idx], LIMIT, fptr)) {
      strip_newline(lines[line_idx]);
      total_lines = line_idx;
      line_idx++;
    }
    printf("Editing %s\n", path);
    fclose(fptr);
  } else {
    printf("File %s does not exist!\n", path);
  }
}

// print the document
void print_lines() {
  for (int i = 1; i <= total_lines; i++) {
    printf("\033[32m%s\033[0m\n", lines[i]);
  }
}

// print the document with prepended numbers
void print_lines_numbered() {
  for (int i = 1; i <= total_lines; i++) {
    printf("\033[31m%5d\033[0m \033[34m%s\033[0m\n", i, lines[i]);
  }
}