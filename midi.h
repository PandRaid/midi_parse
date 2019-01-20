/*
    This is the header file that contains all the function declarations for midi
    parsing functions and their corresponding data structures, as well as debug
    and logging information
*/

void print_buffer_string(char* buffer, uint32_t len);
uint16_t get_half_word(uint32_t* offset);
uint32_t get_word(uint32_t* offset);
uint64_t get_event_length(uint32_t* offset);
uint8_t parse_midi_buffer();
void parse_meta_event(uint32_t* offset);
void parse_sys_event(uint32_t* offset);
void parse_midi_event(uint32_t* offset, uint8_t type, uint64_t time);

typedef struct header {
  uint16_t format;
  uint16_t tracks;
  uint16_t div;
}header;

typedef struct track {
  uint32_t pos;
  uint32_t len;
  char title[128];
}track;

typedef struct note {
  uint64_t time;
  int key;
  uint8_t dir;
}note;

char * key_decode[128] = {
  "c-1", "c#-1", "d-1", "d#-1", "e-1", "f-1", "f#-1", "g-1", "g#-1", "a-1", "a#-1", "b-1",
  "c0", "c#0", "d0", "d#0", "e0", "f0", "f#0", "g0", "g#0", "a0", "a#0", "b0",
  "c1", "c#1", "d1", "d#1", "e1", "f1", "f#1", "g1", "g#1", "a1", "a#1", "b1",
  "c2", "c#2", "d2", "d#2", "e2", "f2", "f#2", "g2", "g#2", "a2", "a#2", "b2",
  "c3", "c#3", "d3", "d#3", "e3", "f3", "f#3", "g3", "g#3", "a3", "a#3", "b3",
  "c4", "c#4", "d4", "d#4", "e4", "f4", "f#4", "g4", "g#4", "a4", "a#4", "b4",
  "c5", "c#5", "d5", "d#5", "e5", "f5", "f#5", "g5", "g#5", "a5", "a#5", "b5",
  "c6", "c#6", "d6", "d#6", "e6", "f6", "f#6", "g6", "g#6", "a6", "a#6", "b6",
  "c7", "c#7", "d7", "d#7", "e7", "f7", "f#7", "g7", "g#7", "a7", "a#7", "b7",
  "c8", "c#8", "d8", "d#8", "e8", "f8", "f#8", "g8", "g#8", "a8", "a#8", "b8",
  "c9", "c#9", "d9", "d#9", "e9", "f9", "f#9", "g9" 
};
