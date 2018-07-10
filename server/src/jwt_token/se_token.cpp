/*
https://github.com/pokowaka/jwt-cpp
Signing tokens 
For example we can create a signed token with HS256 as follows:
 */

#include <iostream>
#include <cstdlib>   
#include <time.h>  
#include <stdio.h> 
#include "jwt/jwt_all.h"

using json = nlohmann::json;

std::string create_signed_token(int exp_time)
{
    // Setup a signer
    HS256Validator signer("secret!");
    time_t now;
    time(&now);// 等同于now = time(NULL)
    printf("now time is %ld\n", now);
    // Create the json payload that expires 01/01/2017 @ 12:00am (UTC)
    json payload = {{"sub", "subject"}, {"exp", now}};

    // Let's encode the token to a string
    auto token = JWT::Encode(signer, payload);

    std::cout << token << std::endl;

    return token;
}
/*
Validating tokens
Validation is straightforward:
*/
int validating_token(std::string str_token)
{
    ExpValidator exp;
    HS256Validator signer("secret!");

    // Now let's use these validators to parse and verify the token we created
    // in the previous example
    // std::string str_token =
    //     "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9."
    //     "eyJpc3MiOiJmb28iLCJleHAiOjE0ODMyMjg4MDB9."
    //     "u3JTUx1tJDo601olQv0rHk4kGkKadIj3cvy-DDZKVRo";
    try {
        // Decode and validate the token
        ::json header, payload;
        std::tie(header, payload) = JWT::Decode(str_token, &signer, &exp);
        std::cout << "Header: " << header << std::endl;
        std::cout << "Payload: " << payload << std::endl;
    } catch (InvalidTokenError &tfe) {
        // An invalid token
        std::cout << "Validation failed: " << tfe.what() << std::endl;
        return -1;
    }
    return 0;
}

