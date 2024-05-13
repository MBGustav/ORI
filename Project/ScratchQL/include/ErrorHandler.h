#ifndef ErrorHandler_H
#define ErrorHandler_H

#include <iostream>
using namespace std;



void inline Name_error()
{
    cerr << "File Name Not Suported!"<<endl;
    exit(EXIT_FAILURE);   
}

void inline File_error()
{
    cerr << "File Not Opened!"<<endl;
    exit(EXIT_FAILURE); 
}

void inline bound_error()
{
    cerr << "Index out of Bounds!!"<<endl;
    exit(EXIT_FAILURE); 
}

void inline Type_error() 
{
    cerr << "File Type not Recognized!" << endl;
    exit(EXIT_FAILURE);
}


void inline Delete_Error()
{
    cerr << "Delete File/Table Not Occured!" << endl;
    exit(EXIT_FAILURE);
}

#endif /*ErrorHandler*/