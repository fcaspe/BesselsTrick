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

void RMS_Processor::init(int windowSize, int blockSize)
    {
    std::cout << "[RMS] Called init winsize " << windowSize << " blocksize " << blockSize << std::endl;
    _n_entries = windowSize/blockSize;
    _rms_buffer = (float*) malloc( _n_entries * sizeof(float));
    for(int i = 0; i < _n_entries;i++)
        _rms_buffer[i] = 0.0f;
    _measure_count = 0;
    _block_size = blockSize;
    }


float RMS_Processor::process(const float* frame)
    {
    /*Measure frame's RMS */
    float acc = 0.0f;
    for(int i = 0; i < _block_size;i++)
        {
        acc += frame[i]*frame[i];
        }
    acc /= (float)_block_size;
    /*Accumulate in buffer */
    _rms_buffer[_measure_count] = acc;
    _measure_count = ++_measure_count & (_n_entries-1); //Wrap counter
    //rt_printf("C: %i\t",_measure_count);

    
    /* Fetch complete RMS window */
    /* TODO:
        This will introduce a transient in the beggining,
        when the buffer is still not full. We could just account
        for the loaded samples until the buffer is full.
        Leave this for a later implementation. */
    
    acc = 0.0f;
    for(int i = 0; i < _n_entries;i++)
        acc += _rms_buffer[i];
    acc /= (float)_n_entries;
    
    if(_use_linear_output == true)
        {
            return acc;
        }
    else
        {
        /* Compute Squared RMS in dB */
        
        if (acc < 1e-20f) acc = 1e-20f; //Avoid log instabilities
        float rms_squared_db = 20.0*log10f(acc);
        //std::cout << "acc = " << acc << " rms_squared_db = " << rms_squared_db << std::endl;
        
        /* Normalize */
        // rms_norm = a * rms +  b; -> a = 1/60, b = 4/3
        if(rms_squared_db < -_DB_RANGE) rms_squared_db = -_DB_RANGE;

        return (a_rms * rms_squared_db + b_rms);
        }
    
    return 0; //never reach here
    }