/*
Q1. You are given a sample text file containing HEX values, which represents following data:
1. Temperature in deg C
2. x, y and z axis acceleration data in mg force.
The data is formatted as Type, Length, Value (TLV).
For example,
01 02 13 01 02 0C 2C 02 00 00 0D FC FF FF C4 00 00 00
0x01: Type for temperature
0x02: No of bytes
0x13 0x01: Temperature value in Little Endian (LE) format (Temperature in deg C is 275/10 = 27. 5)
0x02: Type for acceleration
0x0C: No of bytes
2C 02 00 00: X axis acceleration in LE format (Acceleration in mg force is 556 mg)
0D FC FF FF: Y axis acceleration in LE format (Acceleration in mg force is -1011 mg)
C4 00 00 00: Z axis acceleration in LE format (Acceleration in mg force is 196 mg)
Develop an application that reads the sample text file and outputs the temperature and/or
acceleration in X, Y and Z axis and the net acceleration.


NOTE: 
1. Little Endian == LSB .... MSB    (Given)
2. Big Endian == MSB ... LSB        (Used)
3. hex_len = 2*length   (length is in bytes && each byte is represented by two hexadecimal value)
*/


// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>


// MACROS
#define SEP_LEN		     (75)
// #define FILE_NAME        ("sample.txt")
#define FILE_NAME        ("test.txt")
#define READ_SIZE        (512)
#define TEMP_SIZE        (3)


// Testing Purpose Only
#ifdef DEBUG
    #define HEX_VALUE_1      (0x2C020000)
    #define HEX_VALUE_2      (0x0DFCFFFF)
    #define HEX_VALUE_3      (0xC4000000)
    #define HEX_VALUE_4      ((int16_t) 0x1301)
#endif


int16_t read_file(char*, char*);
size_t LE_to_BE(size_t, uint8_t);
size_t str_to_int(char*);
void sep(char, uint8_t);
// int32_t hex_to_int32_LE(int32_t);
// int16_t hex_to_int16_LE(int16_t);

// Actual Data Content Read from File
char file_content[READ_SIZE] = {0};
float temp_C = 0, a_net = 0;
int32_t a[3] = {0};
bool status[3] = {false, false, false};

// Main Function
int main(){
    sep('*', SEP_LEN);
    
    // Step 1: Read the file and store the data in the char* (replacing any '\n' or '\r' with ' ')
    const int16_t content_size = read_file(FILE_NAME, file_content);

    #ifdef DEGUG
        printf("Before File Content: %s\n", file_content);
    #endif

    // Replace newline characters with spaces
    for (int16_t i_rep = 0; i_rep < content_size; i_rep++) {
        if (file_content[i_rep] == '\n' || file_content[i_rep] == '\r') {
            file_content[i_rep] = ' ';
        }
    }

    #ifdef DEBUG
        printf("After File Content: %s\n", file_content);
    #endif

    // Step 2: Iterate through the data and store the data as per the requirements
    char temp_data[TEMP_SIZE] = {0};
    int16_t i = 0;
    uint8_t j = 0;
    int8_t type = -1, length = -1, dummy_val = -1;

    // Step 3: Process the values
    while(i < content_size){
        // Append the data until ' ' is encountered; i points at ' '
        if(status[1] != true){
            while(file_content[i] != ' ' && file_content[i] != '\0')
                temp_data[j++] = file_content[i++];
            i++;    // To point at non ' ' position
            dummy_val = str_to_int(temp_data);
            
            // Check for the condition
            #ifdef DEBUG
                printf("Status[0] = %d\nStatus[1] = %d\nStatus[2] = %d\n", , status[0], status[1], status[2]);
                printf("\nDummy Value [%d] = %d\n", i, dummy_val);
            #endif
            
            // Type
            if(status[0] == false){
                status[0] = true;
                type = dummy_val;
                if(type == 1)
                    printf("Temperature = ");
                else if(type == 2)
                    printf("Acceleration X = ");
            }
            // Length
            else if (status[1] == false){
                status[1] = true;
                length = dummy_val;
                #ifdef DEBUG
                    printf("Length = %d\n", length);
                #endif
            }
        }
        // Value
        else{
            // Store the proper value in character format
            char* temp_hex_value = (char*)calloc(2 * length + 1, sizeof(char));
            j = 0;
            for(int16_t index = i; index < i + (2*length + length - 1) ; index++){
                if(file_content[index] != ' '){
                    temp_hex_value[j++] = file_content[index];
                }
            }
            temp_hex_value[j] = 0;              // To ensure proper termination of the string (0 = '/0')
            i += (2*length + length + 1);    // To skip the space (+1)

            #ifdef DEBUG
                printf("Temp Data 1 [%d]: %s {%u}\n", i, temp_hex_value, strlen(temp_hex_value));
            #endif

            // Temperature
            if(type == 1){
                status[2] = true;       // Decoding Value (V)
                // Convert it into integer format
                #ifdef DEBUG
                    int16_t int_val_temp = str_to_int(temp_hex_value);
                    printf("LE Integer Value [%u] = 0x%.4X\n", sizeof(int_val_temp), int_val_temp);
                    int16_t int_val_temp = LE_to_BE(str_to_int(temp_hex_value), sizeof(int_val_temp));
                    printf("BE Integer Value = 0x%.4X\n", int_val_temp);
                #endif
                temp_C = ((float) LE_to_BE(str_to_int(temp_hex_value), length)) / 10.0;
                printf("%.1f *C\n", temp_C);
            }
            
            // Acceleration
            else if(type == 2){
                status[2] = true;       // Decoding Value (V)
                char* temp_a = (char*)calloc((2*length)/3 + 1, sizeof(char));

                #ifdef DEBUG
                    int32_t a_axis = str_to_int(temp_a);
                    a_axis = LE_to_BE(a_axis, sizeof(int32_t));
                #endif

                // Acceleration X
                memcpy(temp_a, temp_hex_value, 2*length/3);
                a[0] = LE_to_BE(str_to_int(temp_a), sizeof(str_to_int(temp_a)));
                memset(temp_a, 0, sizeof(temp_a));
                printf("%d mg\n", a[0]);

                // Acceleration Y
                memcpy(temp_a, (temp_hex_value + 2*length/3), 2*length/3);
                a[1] = LE_to_BE(str_to_int(temp_a), sizeof(str_to_int(temp_a)));
                memset(temp_a, 0, sizeof(temp_a));
                printf("Acceleration Y = %d mg\n", a[1]);

                // Acceleration Z
                memcpy(temp_a, (temp_hex_value + 2*(2*length)/3), 2*length/3);
                a[2] = LE_to_BE(str_to_int(temp_a), sizeof(str_to_int(temp_a)));
                memset(temp_a, 0, sizeof(temp_a));
                printf("Acceleration Z = %d mg\n", a[2]);

                // Net Acceleration
                a_net = sqrt(pow(a[0], 2) + pow(a[1], 2) + pow(a[2], 2));
                printf("Net Acceleration = %.4f mg\n", a_net);
                free(temp_a);
            }
            
            // Free the dynamically allocated memory
            free (temp_hex_value);

            // Status Reset
            if(status[0] == status[1] && status[1] == status[2]){
                status[0] = false;
                status[1] = false;
                status[2] = false;
            }
            sep('-', SEP_LEN);
        }
       
        // Reset condition
        j = 0;
        memset(temp_data, 0, TEMP_SIZE);
    }
    
    // Testing Purpose only
    #ifdef DEBUG
        printf("String Size: %u\n", content_size);
        printf("File Content: %s\n", file_content);
        printf("Hexadecimal Value: 0x%.8X\n", HEX_VALUE_1);
        printf("Integer Value: %d\n",hex_to_int32_LE(HEX_VALUE_1));
        sep('-', SEP_LEN);

        printf("Hexadecimal Value: 0x%.8X\n", HEX_VALUE_2);
        printf("Integer Value: %d\n",hex_to_int32_LE(HEX_VALUE_2));
        sep('-', SEP_LEN);

        printf("Hexadecimal Value: 0x%.8X\n", HEX_VALUE_3);
        printf("Integer Value: %d\n",hex_to_int32_LE(HEX_VALUE_3));
        sep('-', SEP_LEN);

        printf("Hexadecimal Value: 0x%.8X\n", HEX_VALUE_4);
        printf("Integer Value: %d\n",hex_to_int16_LE(HEX_VALUE_4));
    #endif

    sep('*', SEP_LEN);
    return 0;
}

/**
 * @brief Reads the provided file_name and stores the content in the provided string
 * @param file_name (char*) -> The name of the file inclusive of the path
 * @param file_content (char*) -> The content of the file read is stored here 
 * @return Size of the input provided string occupied (int8_t [3], int16_t [5], int32_t[])
 */
int16_t read_file(char* file_name, char* file_content){
    // Create a file handler
    FILE *p_file;
    // Open a 'txt' file in read mode
    p_file = fopen(FILE_NAME, "r");
    if(p_file == NULL){
        printf("Unable to open the file!\n");
        return -111;
    }
    // Store the actual value in the string
    // fgets(file_content, READ_SIZE, p_file);     // Issues with '\n'
    int16_t len = 0;
    while(!feof(p_file) && len < READ_SIZE){
        file_content [len++] = fgetc(p_file);
    }
    file_content[len] = 0;
    // Close the file handler
    fclose(p_file);
    // Return the size of the string occupied
    return len;
}


/**
 * @brief Converts the provided string into it's equivalent integer value
 * @param input_text (char*) -> Character array to be converted into integer value
 * @return Integer Value of the input string
 */
size_t str_to_int(char* input_text){
    size_t int_value = 0;
    
    for(uint8_t i_str = 0; i_str < strlen(input_text); i_str++){
       int8_t demo = toupper(input_text[i_str]);
       if (demo >= '0' && demo <= '9')
            demo -= '0'; 
       else if (demo >= 'A' && demo <= 'F')
            demo = (demo - 'A') + 10; 
       int_value += demo * pow(16, strlen(input_text) - i_str - 1); 
    }

    if(strlen(input_text) <= 3){
        return (int8_t)int_value;
    }
    else if (strlen(input_text) <= 5)
        return (int16_t)int_value;
    else if (strlen(input_text) <= 9)
        return (int32_t)int_value;  
    else
        return (int64_t)int_value; 
}


/**
 * @brief Returns integer value for input hexadecimal value for 8 (1 byte)
 * @param hexadecimal_value 8-bit hexadecimal value
 * @returns Converted 8-bit Big Endian (BE) Integer Value
 * @note Input is considered to be Little Endian (LE)
 */
/* int16_t hex_to_int16_LE(int16_t hex_value){
    int16_t* temp = (int16_t*) calloc(1, sizeof(int16_t));
    *temp = (((hex_value & 0x00FF) << 8) | ((hex_value & 0xFF00) >> 8));
    hex_value = *temp;
    free (temp);
    return hex_value;
}
*/

/**
 * @brief Returns integer value for input hexadecimal value for 32-bits (4 bytes)
 * @param hexadecimal_value 32-bit hexadecimal value
 * @returns Converted 32-bit Big Endian (BE) Integer Value
 * @note Input is considered to be Little Endian (LE)
 */
/*
int32_t hex_to_int32_LE(int32_t hex_value){
    int32_t* temp = (int32_t*) calloc(1, sizeof(int32_t));
    for(uint8_t i = 0; i < 4; i++)
        *temp |= (((hex_value >> 8 * i) & 0xFF) << (8 * (3 - i)));
    hex_value = *temp;
    free (temp);
    return hex_value;
}
*/

/**
 * @brief Converts the input Little Endian (LE) to it's equivalent Big Endian (BE)
 * @param hex_val LE value to be converted
 * @return Equivalent BE value 
 */
size_t LE_to_BE(size_t hex_val, uint8_t hex_len_byte){
    void* temp;
    size_t result = 0;
    if (hex_len_byte == 2){
        temp = (int16_t*)calloc(1, sizeof(int16_t));
        for(uint8_t byte = 0; byte < sizeof(hex_val); byte++){
            *((int16_t*)temp) |= ((hex_val >> 8 * byte) & 0xFF) << (8 * (hex_len_byte - 1 - byte));  
            #ifdef DEBUG  
                printf("Temp [%u] = %.4X\n", sizeof(hex_val), *((int16_t*)temp));
            #endif
        }
        result = *((int16_t*)temp);
    }
    else if(hex_len_byte == 4){
        temp = (int32_t*)calloc(1, sizeof(int32_t));
        for(uint8_t byte = 0; byte < sizeof(hex_val); byte++){
            *((int32_t*)temp) |= ((hex_val >> 8 * byte) & 0xFF) << (8 * (hex_len_byte - 1 - byte));
            #ifdef DEBUG    
                printf("Temp [%u] = %.8X\n", sizeof(hex_val), *((int32_t*)temp));
            #endif
        }
        result = *((int32_t*)temp);
    }
    free(temp);
    return result;
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