#pragma once
#include <sys/time.h>
#include <stdio.h>

namespace utils
{
  inline long ms(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
          
    long ms = tp.tv_sec*1000 + tp.tv_usec/1000;
    return ms;
  }     

  inline long tic(long& start){
    long now = ms();
    long dt = now - start;
    start = now;
    printf("tic: %ld\n", dt);
    return dt;
  }
}
