#include <stdio.h>
#include <time.h>
void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
  
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}
  
int main(int argc, char const *argv)
{
    printf("Test1\n");
    int PORTV = 1;
    int move_leds_counter = 2;
    int derection = 0;
    while (1)
    {
        // printf("hello");
        ++move_leds_counter;
        printf("PORTV: %i   move_leds_counter: %i   derection:%i\n", PORTV,move_leds_counter,derection);
        delay(1);
        if (derection == 0)
        {
            PORTV = PORTV << 1;
        }

        else
        {
            // move right
            PORTV = PORTV >> 1;

        }

        if (move_leds_counter == 3)
        {
            move_leds_counter = 0;
            if (derection == 0)
                PORTV = 8;
            derection = ~derection;
            printf("----flip derection----\n");
        }
    }
    return 0;
}