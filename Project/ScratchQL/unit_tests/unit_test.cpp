
#include <iostream>
#include "HeaderHandler.h"
#include "HeaderHandler_test.h"
#include "DataInterface_test.h"
#include "SimpleTableHandler.h"

#include "SimpleHandler_test.h"


using namespace std;


int main(){

    DataInterface_test();
    Test_HeaderHandler();
    SimpleHandler_tst();
}