/*
   Functions responsible for taking a MIDI file as an input and producing the 
   corresponding PIANOREADER specific format that encodes lights with timing
*/

#include <stdio.h>
#include <file.h>
#include <stlib.h>

////////////////////////////////////////////////////////////////////////////
///////////////            Error Defines          //////////////////////////
////////////////////////////////////////////////////////////////////////////
#define THROW_FILE_NOT_FOUND    -1
#define BAD_HEADER              -2

////////////////////////////////////////////////////////////////////////////
///////////////           Standard Defines         /////////////////////////
////////////////////////////////////////////////////////////////////////////
#define MAX_MIDI_SIZE           322146
#define HEADER_ASCII            0x4D546864

////////////////////////////////////////////////////////////////////////////
///////////////            Misc. Defines           /////////////////////////
////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////

int main(){
  FILE* midi_handle;

  midi_handle=fopen(FILE_NAME);
  if(midi_handle){

  }
}
