/*
   Functions responsible for taking a MIDI file as an input and producing the 
   corresponding PIANOREADER specific format that encodes lights with timing
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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
#define MAX_SONG_NAME           256
#define HEADER_ASCII            0x4D546864
#define TRACK_ASCII             0x4D54726B

////////////////////////////////////////////////////////////////////////////
///////////////            Misc. Defines           /////////////////////////
////////////////////////////////////////////////////////////////////////////
#define FILE_NAME               "/home/pandaman/Downloads/for_elise_by_beethoven.mid"


////////////////////////////////////////////////////////////////////////////
///////////////              Struct Defs           /////////////////////////
////////////////////////////////////////////////////////////////////////////

uint32_t file_len;
float ticks_per_qn = 0;
char song_name[MAX_SONG_NAME];
char midi_buffered[MAX_MIDI_SIZE];
uint64_t run_time=0;

note hands[4096];

uint32_t cnt_on = 0;
uint32_t cnt_off = 0;

int main(){
  FILE* midi_handle;

  // Open file in read mode
  midi_handle=fopen(FILE_NAME, "rb");
  if(midi_handle){
    uint32_t header_word, len;
    uint32_t time_scale;

    // Read entire file into a buffer for parsing and null terminate it
    file_len = fread(midi_buffered, sizeof(char), MAX_MIDI_SIZE, midi_handle);
    if (ferror(midi_handle) != 0){
      fprintf(stderr, "Error reading %s", FILE_NAME);
      return BAD_READ;
    }
    else{
      midi_buffered[file_len++] = '\0';
    }

    // Parse through the buffered file and then do cleanup
    parse_midi_buffer();
    fclose(midi_handle);
    //print_buffer_string(midi_buffered, file_len);
  }
  else{
    fprintf(stderr, "Error opening midi file!\n");
    return FILE_NOT_FOUND;
  }

  int i;

  printf("Cnt on %d\n", cnt_on);
  printf("Cnt off %d\n", cnt_off);

  for(i=0; i < (cnt_on + cnt_off - 2); i++){
    printf("N: %s, T: %f, D: %d\n", key_decode[(hands[i]).key], hands[i].time, hands[i].dir);
  }
}

void print_buffer_string(char* str, uint32_t len){
  for(int i=0; i<len; i++){
    fprintf(stderr, "%c", str[i]);
  }
}

// Function for getting 32 bits of data from a buffered midi file
// the offset into the buffer is updated and the word is returned
uint32_t get_word(uint32_t* offset){
  uint32_t word = ((uint8_t) midi_buffered[(*offset)] << 24) | ((uint8_t) midi_buffered[(*offset)+1] << 16) | ((uint8_t) midi_buffered[(*offset)+2] << 8) | ((uint8_t) midi_buffered[(*offset)+3]);


  (*offset) += 4;
  return word;
}

uint16_t get_half_word(uint32_t* offset){
  uint16_t half = ((uint8_t) midi_buffered[(*offset)] << 8) | ((uint8_t) midi_buffered[(*offset)+1]);
  (*offset) += 2;
  return half;
}

// Reads the variable length format designated by the midi specification
// returns the corresponding value and then updates the offset
uint64_t get_event_length(uint32_t* file_offset){
  bool length_parsing = true;
  uint64_t event_length = 0;

  while(length_parsing){
    uint32_t variable_len = midi_buffered[*file_offset];
  
    if (variable_len <= 0x80){
      length_parsing = false;
    }

    event_length = (event_length << 7) | (variable_len & 0x7f);
    (*file_offset)++;
  }

  return event_length;
}

uint8_t parse_midi_buffer(){
  int offset = 0;
  header hdr;

  // Get Midi File Header
  uint32_t first_word = get_word(&offset);
  if (first_word != HEADER_ASCII){
    return BAD_HEADER;
  }
  uint32_t header_len = get_word(&offset);

  if (header_len != 6){
    return BAD_HEADER;
  }

  hdr.format = get_half_word(&offset);
  hdr.tracks = get_half_word(&offset);
  hdr.div = get_half_word(&offset);

  if (!(hdr.div & 0x8000)){
    ticks_per_qn = hdr.div & 0x7FFF;
  }

  track tracks[16] = {0};
  uint8_t find_pos = 1;
  uint32_t tmp_offset = offset;
  uint32_t data = 0;
  int index = 0;

  printf("file len %d\n", file_len);
  while (find_pos && (tmp_offset < file_len)){
    uint8_t ind = midi_buffered[tmp_offset];
    if (ind == 0x4d){
      data = get_word(&tmp_offset);
      if (data == TRACK_ASCII){
        tracks[index].pos = tmp_offset;
        tracks[index].len = get_word(&tmp_offset);
        index++;
        if (index == hdr.tracks){
          find_pos = 0; 
        }
      }
    }
    else{
      tmp_offset++;
    }
  }

  int event_length = 0;
  for (int i=0; i < hdr.tracks; i++){
    printf("Parsing Track %d, len %d\n", i, tracks[i].len);
    tmp_offset = tracks[i].pos + 4;
    while(tmp_offset < (tracks[i].len + tracks[i].pos)){
      run_time += get_event_length(&tmp_offset);

      uint8_t dat = midi_buffered[tmp_offset];
      tmp_offset++;

      if (dat == 0xFF){
        parse_meta_event(&tmp_offset);
      }
      else if (dat == 0xF0 || dat == 0xF7){
        parse_sys_event(&tmp_offset);
      }
      else{
        parse_midi_event(&tmp_offset, dat);
      }
    }
  }
}

void parse_meta_event(uint32_t* offset){
  uint8_t type = midi_buffered[*offset];
  int len = 0;
  (*offset)++;

  switch (type) {
    case 0x00:
      (*offset)++;
      len = 2;
      break;
    case 0x01:
      len = get_event_length(offset);
      break;
    case 0x02:
      len = get_event_length(offset);
      break;
    case 0x03:
      len = get_event_length(offset);
      memcpy(song_name, (midi_buffered + (*offset)), len);
      printf("Title %s\n", song_name);
      break;
    case 0x04:
      len = get_event_length(offset);
      break;
    case 0x05:
      len = get_event_length(offset);
      break;
    case 0x06:
      len = get_event_length(offset);
      break;
    case 0x07:
      len = get_event_length(offset);
      break;
    case 0x20:
      (*offset)++;
      len = 0x01;
      break;
    case 0x2F:
      (*offset)++;
      len = 0x00;
      break;
    case 0x51:
      (*offset)++;
      len = 0x03;
      break;
    case 0x54:
      (*offset)++;
      len = 0x05;
      break;
    case 0x58:
      (*offset)++;
      len = 0x04;
      break;
    case 0x59:
      (*offset)++;
      len = 0x02;
      break;
    case 0x7F:
      len = get_event_length(offset);
      break;
  }
  (*offset) += len;

}

void parse_sys_event(uint32_t* offset){
  int len = get_event_length(offset);
  (*offset) += len;
}

void parse_midi_event(uint32_t* offset, uint8_t type){
  int len = 0;
  static int temp_time = 0;

  if (!temp_time) {
    temp_time = run_time;
  }
  int dif = temp_time - run_time;
  temp_time = run_time;

  switch (type & 0xF0){
    case 0x80:
      len = 2;
      int key_off = midi_buffered[*offset];
      hands[cnt_on + cnt_off].time = dif/ticks_per_qn;
      hands[cnt_on + cnt_off].dir = 0; 
      hands[cnt_on + cnt_off].key = key_off;
      hands[cnt_on + cnt_off].hand = key_off > 60 ? 1 : 2;

      cnt_off++;
      break;
    case 0x90:
      len = 2;
      int key = midi_buffered[*offset];
      hands[cnt_on + cnt_off].time = dif/ticks_per_qn;
      hands[cnt_on + cnt_off].dir = 1; 
      hands[cnt_on + cnt_off].key = key;
      hands[cnt_on + cnt_off].hand = key > 60 ? 1 : 2;

      cnt_on++;
      break;
    case 0xA0:
      len = 2;
      break;
    case 0xB0:
      len = 2;
      break;
    case 0xC0:
      len = 1;
      break;
    case 0xD0:
      len = 1;
      break;
    case 0xE0:
      len = 2;
      break;

  }
  (*offset) += len;
}
