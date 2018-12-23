#include "MagRemote.h"

IRrecv* ir = NULL;

void initRemote(int irpin) {
  ir = new IRrecv(irpin);
  ir->enableIRIn();
}
unsigned long irRead() {
  decode_results rs;
  if(ir->decode(&rs)) {
    ir->resume();
    return rs.value;
  }
  return 0;
}