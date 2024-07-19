
#include <iostream>
#include "HeaderHandler.h"
#include "HeaderHandler_test.h"
#include "DataInterface_test.h"
#include "SimpleTableHandler.h"

#include "SimpleHandler_test.h"


using namespace std;


int main(){

    // create_tab();
    SimpleHandler_tst();

    using_pkey_validation();

    query_pkey_test();
    

    // DataInterface_test();
    // Test_HeaderHandler();
}