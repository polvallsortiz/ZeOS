#include "include/asm.h"

ENTRY (keyboard_handler) :
      SAVE_ALL
      EOI
      CALL keyboard_routine
      RESTORE_ALL
      iret
