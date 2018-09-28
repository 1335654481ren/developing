#ifndef SE_TOKEN_H_
#define SE_TOKEN_H_

#include <iostream>
#include <cstdlib>   
#include <time.h>  
#include <stdio.h>

std::string create_signed_token(int exp_time);
int validating_token(std::string str_token);

#endif