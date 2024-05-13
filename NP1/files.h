#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define NAME_SIZE (20)
#define ID_SIZE (10)


typedef struct Registry
{
    char name[NAME_SIZE];
    char id[ID_SIZE];
    int  code;
}Registry;

size_t reg_size() {return sizeof(struct Registry);}

void file_err(){
    fprintf(stderr, "Error opening File\n");
    exit(-1);
}

void reg_err(){
    fprintf(stderr, "Error opening Register!\n");
    exit(-1);
}
void acc_err() 
{
    fprintf(stderr, "Error Accessing Data!\n");
    exit(-1);
}
struct Registry *new_reg(const char *name, const char *id, const int code) {
    struct Registry *reg = malloc(sizeof(struct Registry));
    if (!reg) {
        // Handle memory allocation failure
        return NULL;
    }
    if(!name || !id){
        free(reg);
        acc_err();
    } 

    reg->code = code;
    strncpy(reg->name, name, MIN(strlen(name), NAME_SIZE));
    reg->name[MIN(strlen(name), NAME_SIZE-1)] = '\0';

    strncpy(reg->id, id, MIN(strlen(id), ID_SIZE));
    reg->id[MIN(strlen(id), ID_SIZE-1)] = '\0';

  
    printf("%s\n, %li", reg->name, strlen(reg->name));
    printf("%s\n, %li", name, strlen(name));
    
    return reg;
}

Registry *param_check_register(Registry *R)
{
    if(!R) reg_err();
    //concat the strings names and sizes, reusing allocation
    return new_reg(R->name,R->id, R->code);
}
void display_registry(const struct Registry *R)
{
    if(R == NULL) reg_err();
    printf("\tName: %s\n\ttid:%s\n\tcode: %i\n", R->name, R->id, R->code);
}

void read_sequential(const char fname[], int register_id, struct Registry *output);
void display_all_regs(const char FILE_PATH[]);

void read_sequential(const char fname[], int register_id, struct Registry *output);
void read_direct(const char fname[], int register_id, struct Registry *output);

void write_sequential(const char *fname, bool reset_file, struct Registry *input);
void write_direct(const char *fname, bool reset_file, struct Registry *input);

void user_input(){}

void display_all_regs(const char FILE_PATH[])
{
    int r_id =0;
    FILE *file_r = fopen(FILE_PATH, "rb") ;

    struct Registry R;
    while(fread(&R, reg_size(), 1, file_r))
    {
        printf("Register_id:\t %i\n", r_id++);
        display_registry(&R);
    }
    printf("-------------\n");
}


void read_sequential(const char fname[], int register_id, struct Registry *output)
{ 

    int rd = 0;
    int idx = 0;
    if(!fname || !output) file_err();
    FILE *file = fopen(fname, "rb+");
    if (!file) file_err(); 

    struct Registry R;
    while((rd = fread(&R, reg_size(), 1, file)) != 0 && (idx < register_id))
        idx++;
    
    fclose(file);

    // in case is not read the Reg.
    if(idx != register_id || rd == 0) 
        reg_err();
    
    memcpy(output, &R, reg_size());
    
}


void write_sequential(const char *fname, bool reset_file, struct Registry *input )
{ 
    size_t total_elements = 0;
    struct Registry R;
    FILE *file = NULL;

    
    file = fopen(fname, reset_file ? "wb+" : "ab+");
    if(!file) file_err();   

    //iterate over all elements
    while (fread(&R, reg_size(), 1, file)) {
        total_elements++;
    }
    //then write in the file
    fwrite(input, reg_size(), 1, file);

    //fclose to save
    fclose(file);
}

void read_direct(const char fname[], int register_id, struct Registry *output)
{
    int idx = 0;
    if(!fname || !output) file_err();
    FILE *file = fopen(fname, "rb+");
    if(!file) file_err(); 


    if(fseek(file,register_id, SEEK_SET))
        reg_err();
    else
        fread(output, reg_size(), 1, file);
    
    fclose(file);
}
void write_direct(const char *fname, bool reset_file, struct Registry *input)
{
    size_t total_elements = 0;
    struct Registry R;
    FILE *file = NULL;
    
    if(!fname || !input) file_err();

    file = fopen(fname, reset_file ? "wb+" : "ab+");
    if(!file) file_err();   

    fseek(file, 0, SEEK_END);

    // check sizes before write 
    Registry *formatted_input =param_check_register(input);

    fwrite(input, reg_size(), 1, file);

    fclose(file);
}


#endif // FILES_H
