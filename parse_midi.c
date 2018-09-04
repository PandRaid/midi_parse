/*
   Functions responsible for taking a MIDI file as an input and producing the 
   corresponding PIANOREADER specific format that encodes lights with timing
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "midi.h"

////////////////////////////////////////////////////////////////////////////
///////////////            Error Defines          //////////////////////////
////////////////////////////////////////////////////////////////////////////
#define FILE_NOT_FOUND          -1
#define BAD_HEADER              -2

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
    }
    else{
      midi_buffered[file_len++] = '\0';
    }
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
