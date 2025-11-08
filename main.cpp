#include "booth2_mul.h"
#include "booth3_mul.h"

int sc_main(int argc, char **argv) {
  for (int s_a = 0; s_a < 2; s_a++) {
    for (int s_b = 0; s_b < 2; s_b++) {
      for (int a = 0; a < 256; a++) {
        for (int b = 0; b < 256; b++) {
          int golden = (1 - 2 * s_a) * (1 - 2 * s_b) * a * b;
          sc_uint<17> c0 = booth2_mul(sc_uint<1>(s_a), sc_uint<1>(s_b), sc_uint<8>(a), sc_uint<8>(b));
          sc_uint<17> c1 = booth3_mul(sc_uint<1>(s_a), sc_uint<1>(s_b), sc_uint<8>(a), sc_uint<8>(b));
          if (golden != sc_int<17>(c0).to_int()) {
            printf("booth2_mul error: s_a=%d, s_b=%d, a=%d, b=%d, golden=0x%x, rst=0x%x\n", s_a, s_b, a, b, golden, sc_int<17>(c0).to_int());
          }
          if (golden != sc_int<17>(c1).to_int()) {
            printf("booth3_mul error: s_a=%d, s_b=%d, a=%d, b=%d, golden=0x%x, rst=0x%x\n", s_a, s_b, a, b, golden, sc_int<17>(c1).to_int());
          }
        }
      }
    }
  }

  return 0;
}