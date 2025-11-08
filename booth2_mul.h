#pragma once

#include "systemc.h"

// 一个n位二进制数B的补码表示为：B = -b[n-1]*2^(n-1) + b[n-2]*2^(n-2) + ... + b[1]*2^1 + b[0]*2^0
// 比如，4位二进制数-5的补码表示为：1011 = -1*2^3 + 0*2^2 + 1*2^1 + 1*2^0
//      4位二进制数+5的补码表示为：0101 = -0*2^3 + 1*2^2 + 0*2^1 + 1*2^0
// booth2乘法(Radix-4)，对于B进行拆分重组：参考https://zhuanlan.zhihu.com/p/19316252580
// 对应《FAST MULTIPLICATION ALGORITHMS AND IMPLEMENTATION.pdf》中的Figure 2.4: 16 bit Booth 2 multiply; 推导过程中的b[-1]对应LSB补0

// 模拟bf16(1-8-7)中的乘法器
// 被乘数a和乘数b的符号位分别提取出来，记为s_a和s_b
// 被乘数a和乘数b的尾数位分别提取出来，记为a和b（包含隐藏位）
// c = (s_a*s_b)*a*b, 17bit = 1bit + 8bit + 8bit
inline sc_uint<17> booth2_mul(sc_uint<1> s_a, sc_uint<1> s_b, sc_uint<8> a, sc_uint<8> b) {
  sc_uint<17> c;
  sc_uint<1> res_sign = s_a ^ s_b;  // 通过符号位来翻转booth_code来实现*1或*-1
  sc_uint<3> booth_code[5];         // 用于部分积查表的Multiplier Bits
  sc_uint<9> booth_pp[5];           // 部分积结果
  sc_uint<1> booth_inv[5];          // 对应补码取反后+1部分
  sc_uint<17> pp[6];                // 华莱士压缩结果
  sc_uint<17> sum0, sum1;           // 用于4-2压缩
  sc_uint<17> carry0, carry1;
  booth_code[0] = (b[1], b[0], sc_uint<1>(0)) ^ (res_sign, res_sign, res_sign);
  booth_code[1] = (b[3], b[2], b[1]) ^ (res_sign, res_sign, res_sign);
  booth_code[2] = (b[5], b[4], b[3]) ^ (res_sign, res_sign, res_sign);
  booth_code[3] = (b[7], b[6], b[5]) ^ (res_sign, res_sign, res_sign);
  booth_code[4] = (sc_uint<1>(0), sc_uint<1>(0), b[7]) ^ (res_sign, res_sign, res_sign);
  sc_uint<8> not_a = ~a;
  for (int i = 0; i < 5; i++) {
    switch (booth_code[i]) {
      case 0:
      case 7:  // +/- 0*a
        booth_pp[i] = 0;
        booth_inv[i] = 0;
        break;
      case 1:
      case 2:  // +1*a
        booth_pp[i] = (sc_uint<1>(0), a);
        booth_inv[i] = 0;
        break;
      case 3:  // +2*a
        booth_pp[i] = (a, sc_uint<1>(0));
        booth_inv[i] = 0;
        break;
      case 4:  // -2*a
        booth_pp[i] = (not_a, sc_uint<1>(1));
        booth_inv[i] = 1;
        break;
      default:  // -1*a
        booth_pp[i] = (sc_uint<1>(1), not_a);
        booth_inv[i] = 1;
        break;
    }
  }
  pp[0] = (sc_uint<5>(0), !booth_inv[0], booth_inv[0], booth_inv[0], booth_pp[0]);                                 // 0 0 0 0 0 S_bar S S PP (5+3+9=17)
  pp[1] = (sc_uint<4>(0), sc_uint<1>(1), !booth_inv[1], booth_pp[1], sc_uint<1>(0), booth_inv[0]);                 // 0 0 0 0 1 S_bar PP 0 S (4+2+9+2=17bit)
  pp[2] = (sc_uint<2>(0), sc_uint<1>(1), !booth_inv[2], booth_pp[2], sc_uint<1>(0), booth_inv[1], sc_uint<2>(0));  // 0 1 S_bar PP 0 S 0 0 (2+2+9+4=17bit)
  pp[3] = (sc_uint<1>(1), !booth_inv[3], booth_pp[3], sc_uint<1>(0), booth_inv[2], sc_uint<4>(0));                 // 1 S_bar PP 0 S 0 0 0 0 (2+9+6=17bit)
  pp[4] = (booth_pp[4], sc_uint<1>(0), booth_inv[3], sc_uint<6>(0));                                               // PP 0 S 0 0 0 0 0 0 (9+8=17bit)
  pp[5] = (sc_uint<7>(0), sc_uint<1>(0), booth_inv[4], sc_uint<8>(0));                                             // 0... 0 S 0 0 0 0 0 0 0 0 (7+10=17bit)
  // 两次3-2压缩组成一次4-2压缩
  sum0 = pp[0] ^ pp[1] ^ pp[2];
  carry0 = ((pp[0] & pp[1]) ^ (pp[2] & (pp[0] ^ pp[1]))) << 1;
  sum1 = sum0 ^ carry0 ^ pp[3];
  carry1 = ((sum0 & carry0) ^ (pp[3] & (sum0 ^ carry0))) << 1;
  // 两次3-2压缩组成一次4-2压缩
  sum0 = sum1 ^ carry1 ^ pp[4];
  carry0 = ((sum1 & carry1) ^ (pp[4] & (sum1 ^ carry1))) << 1;
  sum1 = sum0 ^ carry0 ^ pp[5];
  carry1 = ((sum0 & carry0) ^ (pp[5] & (sum0 ^ carry0))) << 1;
  // adder
  c = (sum1 + carry1);

  return c;
}