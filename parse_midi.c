/*
   Functions responsible for taking a MIDI file as an input and producing the 
   corresponding PIANOREADER specific format that encodes lights with timing
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "midi.h"

////////////////////////////////////////////////////////////////////////////
///////////////            Error Defines          //////////////////////////
////////////////////////////////////////////////////////////////////////////
#define FILE_NOT_FOUND          -1
#define BAD_HEADER              -2
#define BAD_READ                -3

////////////////////////////////////////////////////////////////////////////
///////////////           Standard Defines         /////////////////////////
////////////////////////////////////////////////////////////////////////////
#define MAX_MIDI_SIZE           322146
#define HEADER_ASCII            0x4D546864

////////////////////////////////////////////////////////////////////////////
///////////////            Misc. Defines           /////////////////////////
////////////////////////////////////////////////////////////////////////////
#define FILE_NAME               "/home/pandaman/Downloads/for_elise_by_beethoven.mid"


////////////////////////////////////////////////////////////////////////////

int main(){
  FILE* midi_handle;

  midi_handle=fopen(FILE_NAME, "rb");
  if(midi_handle){
    char midi_buffered[MAX_MIDI_SIZE];
    uint32_t header_word, len;
    uint32_t time_scale;

    size_t file_len = fread(midi_buffered, sizeof(char), MAX_MIDI_SIZE, midi_handle);
    if (ferror(midi_handle) != 0){
      fprintf(stderr, "Error reading %s", FILE_NAME);
      return BAD_READ;
    }
    else{
      midi_buffered[file_len++] = '\0';
    }
    parse_midi_buffer(midi_handle);
    fclose(midi_handle);
    print_buffer_string(midi_buffered, MAX_MIDI_SIZE);
  }
  else{
    fprintf(stderr, "Error opening midi file!\n");
    return FILE_NOT_FOUND;
  }
}

void print_buffer_string(char* str, uint32_t len){
  for(int i=0; i<len; i++){
    fprintf(stderr, "%c", str[i]);
  }
}

uint32_t get_word(const char* midi_buffered, uint32_t* offset){
  uint32_t word = ((const int*) midi_buffered)[*offset];
  (*offset) += 4;
  return word;
}

uint32_t get_event_length(const char* midi_file, uint32_t* file_offset){
  bool length_parsing = true;
  uint32_t event_length = 0;

  while(length_parsing){
    uint32_t variable_len = midi_file[*file_offset];
  
    if (variable_len <= 80){
      length_parsing = false;
    }

    event_length = (event_length << 7) | (variable_len & 0x7f);
    (*file_offset)++;
  }

  return event_length;
}

uint8_t parse_midi_buffer(FILE* midi_handle){

}
