/*
 ==============================================================================
    Copyright (c) 2023 Franco Caspe
    All rights reserved.

    **BSD 3-Clause License**

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

 ==============================================================================

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================
*/

/*
File: algorithms.hpp
Defines oscillator routing for FM modulation.
*/

#ifndef SRC_ALGORITHMS_HPP_
#define SRC_ALGORITHMS_HPP_
#endif  // SRC_ALGORITHMS_HPP_

const int ALG1_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
                             0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG1_OUTM[6] = {1, 0, 1, 0, 0, 0};
const int ALG2_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
                             0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG2_OUTM[6] = {1, 0, 1, 0, 0, 0};
const int ALG3_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                             0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG3_OUTM[6] = {1, 0, 0, 1, 0, 0};
const int ALG4_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                             0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG4_OUTM[6] = {1, 0, 0, 1, 0, 0};
const int ALG5_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG5_OUTM[6] = {1, 0, 1, 0, 1, 0};
const int ALG6_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG6_OUTM[6] = {1, 0, 1, 0, 1, 0};
const int ALG7_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG7_OUTM[6] = {1, 0, 1, 0, 0, 0};
const int ALG8_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG8_OUTM[6] = {1, 0, 1, 0, 0, 0};
const int ALG9_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG9_OUTM[6] = {1, 0, 1, 0, 0, 0};
const int ALG10_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG10_OUTM[6] = {1, 0, 0, 1, 0, 0};
const int ALG11_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG11_OUTM[6] = {1, 0, 0, 1, 0, 0};
const int ALG12_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG12_OUTM[6] = {1, 0, 1, 0, 0, 0};
const int ALG13_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG13_OUTM[6] = {1, 0, 1, 0, 0, 0};
const int ALG14_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG14_OUTM[6] = {1, 0, 1, 0, 0, 0};
const int ALG15_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG15_OUTM[6] = {1, 0, 1, 0, 0, 0};
const int ALG16_MM[36] = {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG16_OUTM[6] = {1, 0, 0, 0, 0, 0};
const int ALG17_MM[36] = {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG17_OUTM[6] = {1, 0, 0, 0, 0, 0};
const int ALG18_MM[36] = {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG18_OUTM[6] = {1, 0, 0, 0, 0, 0};
const int ALG19_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG19_OUTM[6] = {1, 0, 0, 1, 1, 0};
const int ALG20_MM[36] = {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG20_OUTM[6] = {1, 1, 0, 1, 0, 0};
const int ALG21_MM[36] = {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG21_OUTM[6] = {1, 1, 0, 1, 1, 0};
const int ALG22_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG22_OUTM[6] = {1, 0, 1, 1, 1, 0};
const int ALG23_MM[36] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG23_OUTM[6] = {1, 1, 0, 1, 1, 0};
const int ALG24_MM[36] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG24_OUTM[6] = {1, 1, 1, 1, 1, 0};
const int ALG25_MM[36] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG25_OUTM[6] = {1, 1, 1, 1, 1, 0};
const int ALG26_MM[36] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG26_OUTM[6] = {1, 1, 0, 1, 0, 0};
const int ALG27_MM[36] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG27_OUTM[6] = {1, 1, 0, 1, 0, 0};
const int ALG28_MM[36] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG28_OUTM[6] = {1, 0, 1, 0, 0, 1};
const int ALG29_MM[36] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG29_OUTM[6] = {1, 1, 1, 0, 1, 0};
const int ALG30_MM[36] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG30_OUTM[6] = {1, 1, 1, 0, 0, 1};
const int ALG31_MM[36] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
const float ALG31_OUTM[6] = {1, 1, 1, 1, 1, 0};
const int ALG32_MM[36] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ALG32_OUTM[6] = {1, 1, 1, 1, 1, 1};
