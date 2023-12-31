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
File: RMSProcessor.cpp
Computes block-wise input audio RMS over a window.
*/

#include "RMSProcessor.hpp"
#include <cstdlib>
#include <iostream>
#include <cmath>


constexpr float _REF_DB = 20.0f; //maximum reached by input (will be mapped to 1.0)
constexpr float _DB_RANGE = 70.0f; //minimum where pitch is tracked (will be mapped to 0.0)
constexpr float a_rms = 1.0f/(_DB_RANGE-_REF_DB);
constexpr float b_rms = _DB_RANGE/(_DB_RANGE-_REF_DB);

RMS_Processor::~RMS_Processor()
        {
        free(_rms_buffer);
        }

void RMS_Processor::init(int windowSize, int blockSize, bool linear_output)
    {
    std::cout << "[RMS] Called init winsize " << windowSize << " blocksize " << blockSize << std::endl;
    _n_entries = windowSize/blockSize;
    _rms_buffer = (float*) malloc( _n_entries * sizeof(float));
    _use_linear_output = linear_output;
    for(int i = 0; i < _n_entries;i++)
        _rms_buffer[i] = 0.0f;
    _measure_count = 0;
    _block_size = blockSize;
    }


float RMS_Processor::process(const float* frame)
    {
    /*Measure frame's RMS */
    float retval = 0.0f;
    float acc = 0.0f;
    for(int i = 0; i < _block_size;i++)
        {
        acc += frame[i]*frame[i];
        }
    acc /= (float)_block_size;
    /*Accumulate in buffer */
    _rms_buffer[_measure_count] = acc;
    _measure_count = ++_measure_count & (_n_entries-1); //Wrap counter
    
    /* Fetch complete RMS window */
    acc = 0.0f;
    for(int i = 0; i < _n_entries;i++)
        acc += _rms_buffer[i];
    acc /= (float)_n_entries;
    
    if(_use_linear_output == true)
        { // Linear RMS to amp feature, clamp at 2.
            const float lim = 2.0f;
            acc = acc * 32.0f; // Found by trial and error.
            retval = acc > lim? lim : acc;
        }
    else
        { // Logarithm RMS to amp feature
        /* Compute Squared RMS in dB */
        if (acc < 1e-20f) acc = 1e-20f; //Avoid log instabilities
        float rms_squared_db = 20.0*log10f(acc);
        //std::cout << "acc = " << acc << " rms_squared_db = " << rms_squared_db << std::endl;
        
        /* Normalize */
        // rms_norm = a * rms +  b; -> a = 1/60, b = 4/3
        if(rms_squared_db < -_DB_RANGE) rms_squared_db = -_DB_RANGE;

        retval = (a_rms * rms_squared_db + b_rms);
        }
    return retval;
    }