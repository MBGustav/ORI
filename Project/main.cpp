#include <iostream>
#include <filesystem>
#include <vector>

using namespace std;


// bool fileExists(const std::string& filename) {
//     std::ifstream file(string(filename + TABLE_EXT).c_str());
//     return file.good();
// }

string full_path(string local){
    return std::filesystem::absolute(local).string(); 
}


int main(int argc, char **argv)
{
    std::cout << "Hello\n";
    return 0;
}

