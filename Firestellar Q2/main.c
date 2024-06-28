/*
Q2. A bus runs from point A to B with 5 stops in between for picking passengers only and all the
passengers get down at B. Because of social distancing norm, the bus can only have a maximum of
10 passengers. When the passenger count reaches 10, it does not accept new passengers.
A passenger on reaching the bus stop needs to notify the origin bus stop number via a mobile app.
The bus before starting from point A fetches the passenger count and maximum time a passenger
has been waiting for all the stops.
Develop a program to help the bus plan the route to minimize the number of trips and stops and not
exceed maximum passenger waiting time to more than 20 minutes.
For example,
Input:
Bus stop Passenger count Max passenger waiting time
A 4 2 mins
1 1 17 mins
2 3 4 mins
3 2 1 min
4 1 3 mins
5 2 2 mins
B
Output:
4 stops: A, 1, 2, 5
In this case, the bus stops at 4 points
A because maximum number of passengers
1 because maximum passenger waiting time close to 20 mins
2 Second maximum number of passengers
5 Passenger waiting time is higher for the two stops with same number of passengers
*/

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

// MACROS
#define SIZE(X)               ((uint8_t)(sizeof(X)/sizeof((X)[0])))
#define PASS_LIMIT            ((uint8_t)10)
#define WAIT_LIMIT            ((uint8_t)20)
#define SEP_LEN               ((uint8_t)75)
#define UPDATE_SEATS(X)       (available_seats -= (uint8_t)(X))
#define GET_PASS_FROM_INDEX(X)     (stops[X].pass_count)

// #define DEBUG

// Structure consisting the information about bus_stop
typedef struct {
    char name;
    uint8_t pass_count;
    uint8_t wait_time;
    int8_t og_index;
} bus_stop_t;

void sort_bus_stop(bus_stop_t*, uint8_t, bool);
uint8_t find_max_val(bus_stop_t*, uint8_t, bool, uint8_t*, bool*, bool*);
void traverse_bus_stops(const bus_stop_t*, uint8_t);
void print_route(bus_stop_t*, uint8_t, bool*);
uint8_t static inline find_curr_index(bus_stop_t*, uint8_t, uint8_t);
void sep(char, uint8_t);

// Main Function
int main(){
    // Input Display
    sep('*', SEP_LEN);
    
    /*
    // Actual Case: Optimal Bus Route: [ A -> 1 -> 2 -> 5 -> B ] ===> Total Stops = 4
    bus_stop_t stops[6] = {
        {'A', 4, 2, 0},
        {'1', 1, 17, 1},
        {'2', 3, 4, 2},
        {'3', 2, 1, 3},
        {'4', 1, 3, 4},
        {'5', 2, 2, 5}
    };

    // Test Case 1: Optimal Bus Route: [ A -> 1 -> B ] ===> Total Stops = 2
    bus_stop_t stops[6] = {
        {'A', 7, 2, 0},
        {'1', 3, 1, 1},
        {'2', 0, 4, 2},
        {'3', 0, 1, 3},
        {'4', 0, 3, 4},
        {'5', 0, 2, 5}
    };    

    // Test Case 2: Optimal Bus Route: [ A -> 1 -> 2 -> 3 -> 5 -> B ] ===> Total Stops = 5
    bus_stop_t stops[6] = {
        {'A', 2, 2, 0},
        {'1', 1, 19, 1},
        {'2', 3, 4, 2},
        {'3', 2, 1, 3},
        {'4', 1, 3, 4},
        {'5', 2, 2, 5}
    };

    // Test Case 3: Optimal Bus Route: [ A -> 1 -> 2 -> 4 -> B ] ===> Total Stops = 4
    bus_stop_t stops[6] = {
        {'A', 4, 2, 0},
        {'1', 2, 17, 1},
        {'2', 3, 4, 2},
        {'3', 2, 1, 3},
        {'4', 1, 3, 4},
        {'5', 2, 2, 5}
    };
    */

    // Step 1: Structure Definition
    bus_stop_t stops[6] = {
        {'A', 4, 2, 0},
        {'1', 2, 17, 1},
        {'2', 3, 4, 2},
        {'3', 2, 1, 3},
        {'4', 1, 3, 4},
        {'5', 2, 2, 5}
    };
    
    // Displaying input information
    #ifdef DEBUG
        printf("Before Sorting:\n");
        traverse_bus_stops(stops, SIZE(stops));
    #endif

    // Step 2: Sorting the input structure array based upon passengers count
    sort_bus_stop(stops, SIZE(stops), true);

    #ifdef DEBUG
        printf("After Sorting:\n");
        traverse_bus_stops(stops, SIZE(stops));
    #endif

    // Step 3: Updating the route as per the requirements (MAIN)
    bool* route_index = (bool*)calloc(6, sizeof(bool));         // Points to OG index being used or not
    memset(route_index, false, sizeof(route_index));
    bool* traversed_index = (bool*)calloc(6, sizeof(bool));     // Points to index being traversed or not
    memset(traversed_index, false, sizeof(traversed_index));
    uint8_t max_pass_count_index = 255, max_wait_time_index = 255;
    uint8_t curr_max_pass_count = 100, curr_max_wait_time = 200, available_seats = PASS_LIMIT;

    // Loop until seats are available
    while(available_seats){

        // Current iteration's max passenger count
        curr_max_pass_count = find_max_val(stops, SIZE(stops), true, &max_pass_count_index, route_index, traversed_index);

        // If seats are more than current bus stop's max passenger
        if(curr_max_pass_count <= available_seats){
            route_index[stops[max_pass_count_index].og_index] = true;       // Route Developed
            traversed_index[stops[max_pass_count_index].og_index] = true;   // Traversed
            UPDATE_SEATS(curr_max_pass_count);                              // Seats Updated
        }
        // If not, find another bus stop which satisfies the criteria
        else{
            traversed_index[stops[max_pass_count_index].og_index] = true;
        }

        // Current iteration's max wait time
        curr_max_wait_time = find_max_val(stops, SIZE(stops), false, &max_wait_time_index, route_index, traversed_index);

        // If wait time is very close to WAIT_LIMIT in comparision to max passengers wrt PASS_LIMIT
        if((WAIT_LIMIT - curr_max_wait_time) < (PASS_LIMIT - curr_max_pass_count)){
            // Check if number of passenger can be accommodated or not
            if(GET_PASS_FROM_INDEX(max_wait_time_index) <= available_seats){
                route_index[stops[max_wait_time_index].og_index] = true;       // Route Developed
                traversed_index[stops[max_wait_time_index].og_index] = true;   // Traversed
                UPDATE_SEATS(GET_PASS_FROM_INDEX(max_wait_time_index));        // Seats Updated
            }
        }
        
        #ifdef DEBUG
            printf("Current Max Passengers = %u\t", curr_max_pass_count);
            printf("Current Max Wait Time = %u\t", curr_max_wait_time);
            printf("Current Available Seat(s) = %u\n", available_seats);
        #endif

    }

    // Step 4: Print the Final Route
    print_route(stops, SIZE(stops), route_index);
    sep('*', SEP_LEN);
    free (traversed_index);
    free (route_index);
    return 0;
}

/**
 * @brief Prints the final optimized route
 * @param input (bus_stop_t*) - Address of array of bus_stop_t (struct)
 * @param length (uint8_t) - Length of the input bus_stop_t array
 * @param status (bool*) - The status array which defines whether the bus_stop_t element was used or not
 */
void print_route(bus_stop_t* input, uint8_t length, bool* status){
    printf("Optimal Bus Route: [ ");
    uint8_t* stops = (uint8_t*)calloc(1, sizeof(uint8_t));
    // Print the character
    for(uint8_t ind = 0; ind < length; ind++){
        if(status[ind] == true){
            printf("%c %s ", input[find_curr_index(input, length, ind)].name, "->");
            *stops+=1;
        }
    }
    printf("B ] ===> Total Stops = %u\n", *stops);
    free (stops);
}

/**
 * @brief Provides the current index after sorting the values based upon pass_count
 * @param input (bus_stop_t*) - Address of array of bus_stop_t (struct)
 * @param length (uint8_t) - Length of the input bus_stop_t array
 * @param passed_og_index (uint8_t) - Comparison Original Index allocated before sorting
 * @return current sorted index (uint8_t)
 */
uint8_t static inline find_curr_index(bus_stop_t* input, uint8_t length, uint8_t passed_og_index){
    uint8_t curr_index = 0;
    for(curr_index; curr_index < length; curr_index++){
        if(input[curr_index].og_index == passed_og_index)
            return curr_index;
    }
}

/**
 * @brief Provides unused max value or next consecutive max value depending upon the parameter and also updates the index value at which the max value was found in the input index variable
 * @param input (bus_stop_t*) - Address of array of bus_stop_t (struct)
 * @param length (uint8_t) - Length of the input bus_stop_t array
 * @param parameter (bool) - true => pass_count | false => wait_time
 * @param index (uint8_t*) - Index at which max value was found (Skipping Index)
 * @return max_value (uint8_t) found in the array of the structure
 */
uint8_t find_max_val(bus_stop_t* input, uint8_t length, bool parameter, uint8_t* index, bool* route_index, bool* traversed_index) {
  uint8_t max = 0;
  for (uint8_t i = 0; i < length; i++) {
    // Skip stops already processed or excluded by route_index
    if (traversed_index[input[i].og_index] || route_index[input[i].og_index] || i == *index) {
      continue;
    }
    else{
        if (parameter){
            if (input[i].pass_count > max){
                max = input[i].pass_count;
                *index = i;
            }
        }
        else{
            if (input[i].wait_time > max){
                max = input[i].wait_time;
                *index = i;
            }
        }
        
    }
  }
  return max;
}

/**
 * @brief Sorts the input bus_stop_t array of given length based upon parameter provided
 * @param input (bus_stop_t*) - Address of array of bus_stop_t (struct)
 * @param length (uint8_t) - Length of the input bus_stop_t array
 * @param parameter (bool) - true => pass_count | false => wait_time 
 * @note In case of same value, other parameter which is greater is given priority, in case other parameter are also equal then the element having lower OG index is chosen
 */
void sort_bus_stop(bus_stop_t* input, uint8_t length, bool parameter){
    if (parameter){
        for(uint8_t k = 0; k < length - 1; k ++){
            bool swapped = false;
            for(uint8_t l = 0; l < length - 1 - k; l++){
                if(input[l].pass_count < input[l+1].pass_count){
                    swapped = true;
                    bus_stop_t temp = input[l];
                    input[l] = input[l+1];
                    input[l+1] = temp;
                }
                else if(input[l].pass_count == input[l+1].pass_count){
                    if(input[l].wait_time < input[l+1].wait_time){
                        swapped = true;
                        bus_stop_t temp = input[l];
                        input[l] = input[l+1];
                        input[l+1] = temp;
                    }
                    else if(input[l].wait_time == input[l+1].wait_time){
                        bus_stop_t temp = (input[l].og_index < input[l+1].og_index)? input[l] : input[l + 1];
                        if(temp.og_index != input[l].og_index){
                            swapped = true;
                            input[l] = input[l+1];
                            input[l+1] = temp;
                        }
                        
                    }
                }
            }
            if(!swapped)
                break;
        }
    }
    else{
        for(uint8_t k = 0; k < length - 1; k ++){
            bool swapped = false;
            for(uint8_t l = 0; l < length - 1 - k; l++){
                if(input[l].wait_time < input[l+1].wait_time){
                    swapped = true;
                    bus_stop_t temp = input[l];
                    input[l] = input[l+1];
                    input[l+1] = temp;
                }
                else if(input[l].wait_time == input[l+1].wait_time){
                    if(input[l].pass_count < input[l+1].pass_count){
                        swapped = true;
                        bus_stop_t temp = input[l];
                        input[l] = input[l+1];
                        input[l+1] = temp;
                    }
                    else if(input[l].pass_count == input[l+1].pass_count){
                        bus_stop_t temp = (input[l].og_index < input[l+1].og_index)? input[l] : input[l + 1];
                        if(temp.og_index != input[l].og_index){
                            swapped = true;
                            input[l] = input[l+1];
                            input[l+1] = temp;
                        }
                        
                    }
                }
            }
            if(!swapped)
                break;
        }
    }
}

/**
 * @brief Perfect Visualization Tool for current Structure
 * @param stops (bus_stops_t*) - Address to the array of the bus_stop_t (struct)
 * @param length (uint8_t) - Length of the array of bus_stop_t
 */
void traverse_bus_stops(const bus_stop_t* stops, uint8_t length){
    sep('-', SEP_LEN - 9);
    printf("| Bus Stop Name | Passenger Count |  Wait Time\t| Original Index |\n");
    for(uint8_t i = 0; i < length; i++){
        sep('-', SEP_LEN - 9);
        const bus_stop_t *current_stop = &stops[i];
        printf("| \t%c\t", current_stop->name);
        printf("| \t%.2u  \t", current_stop->pass_count);
        printf("  | \t%.2u\t", current_stop->wait_time);
        printf("|\t%.2u\t |\t\t\n", current_stop->og_index);
    }
    sep('-', SEP_LEN - 9);
}

/**
 * @brief Custom Display
 * @param c (char) -> Character to be displayed
 * @param a (uint8_t) -> Total no of times the character has to be displayed
*/
void sep(char c, uint8_t a){
    for(uint8_t i = 0; i < a; i++)
        printf("%c", c);
    printf("\n");
}