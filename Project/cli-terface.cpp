#include <iostream> 
#include <string> 
#include <vector>
#include <set>
#include <filesystem>
#include <map>
#include <iomanip>

using std::string, std::map, std::setw;
using std::vector, std::cout, std::getline, std::cin;
namespace  fs = std::filesystem;

//%%%%%%%%%%% -- DEFINITIONS -- %%%%%%%%%%%
#define _DEBUG




#ifdef _DEBUG
#define DBG_COUT(message) do { std::cout << (message) << std::endl; } while (0)
#else
#define DBG_COUT(message) do {} while (0)
#endif


//%%%%%%%%%%% -- PREDECLARATIONS -- %%%%%%%%%%%
typedef string SQLTable;
const  std::set<string> datas = {"INT", "DATE", "STRING", "BOOL", "FLOAT"};
const bool data_exist(string tp){return datas.count(tp) != 0;}

void list_tables(const map<string, SQLTable> &sql_manager);
void delete_table(string name, map<string, SQLTable> &sql_manager); 
void read_input_command(string args,map<string, SQLTable> &sql_manager);
void insert_row(vector<string> args,map<string, SQLTable> &sql_manager ); 
void help_console();
void create_table(vector<string> args,map<string, SQLTable> &sql_manager);
void search_pkey(vector<string> args, map<string, SQLTable> &sql_manager);
void strUpper(string &str){for (auto & c: str) c = toupper(c);}



int main() {
    string command, tmp;
    map<string, SQLTable> sql_manager;
    while (true) {
        command.clear();  
        cout << "SimpleQL $> ";

        while (true) {
            tmp.clear();
            getline(cin, tmp);
            // Append line to command, removing the backslash
            if (!tmp.empty() && tmp.find('\\')!=std::string::npos) {
                command += tmp.substr(0, tmp.find('\\') - 1) + ' ';  
            } else {
                command += tmp; 
                break;  // Exit the inner loop 
            }
        }
        strUpper(command);
        // Check if the command is "EXIT" to break the outer loop
        if (command == "EXIT") break;
        
        // Process the command
        read_input_command(command, sql_manager);
    }

    return 0;
}

void read_input_command(string args,map<string, SQLTable> &sql_manager){
    DBG_COUT(("COMMAND:" + args));

    if(args == "HELP"){
        help_console();
    }
    
    vector<string> param;
    size_t prev = 0;
    size_t pos = 0;

    while ((pos = args.find(' ', prev)) != string::npos) {
        param.push_back(args.substr(prev, pos - prev));
        prev = pos + 1; // Move past the space
    }
    // Add the last token
    param.push_back(args.substr(prev));
    
    // debug printer
    for (const auto& p : param) { DBG_COUT( "param - " + p);};
    

    if(args.find("CREATE-TABLE")!= string::npos){
        create_table(param, sql_manager);
        return;
    }
    if(args.find("LIST-TABLE")!= string::npos){
        list_tables(sql_manager);
        return;
    }
    if(args.find("DELETE-TABLE")!= string::npos){
        delete_table(args, sql_manager);
        return;
    }

    cout << "\n --  COMMAND UNDEFINED ! --\n";
}


void help_console(){
    cout << "Main functions for SQLTable \n";
    cout << "- CREATE-TABLE <name> [<name_entity>=<data_type>]\n";
    cout << "- DELETE-TABLE \n";
    cout << "- INSERT-ROW   <table_name> (<row >)  \n";
    cout << "- LIST-TABLES  \n";

    cout << " -----------------\n";
    cout << "- CREATE-TABLE - truncate/write new table.\n";
    cout << "- DELETE-TABLE - delete the table\n";
    cout << "- INSERT-ROW   - insert the row - use between ().\n";
    cout << "- LIST-TABLES  - list the tables created.";

    cout << "\nData_types availables: ";
    for(auto const &c: datas)
        cout << c << ", ";
     cout << " \n";

}

void create_table(vector<string> args,map<string, SQLTable> &sql_manager){
    size_t sz = args.size();
    if(sz < 3){
        cout << "\n-- TABLE NOT CREATED, MISSING PARAMETERS -- \n";
        return;
    }
    vector<string> entities_name, entities_type;
    string arg, name, type;
    size_t pos =0, len;
    string table_name = args[1];

    // correctness parameters
    for(auto i = 2; i <sz; i++){
        arg = args[i];
        len = arg.size();

        if(((pos = arg.find('=')) == string::npos) ||
            !data_exist(type = arg.substr(pos+1, len))){
            cout << "TABLE NOT CREATED, INVALID PARAMETER : " << args[i] <<" -- \n";
            return;
        }
        
        name = arg.substr(0,   pos);
        entities_type.push_back(type);
        entities_name.push_back(name);
        
        DBG_COUT(name + ", " + type);
    }


    //CRIACÃO/REESCRITA DE TABELA

    sql_manager[table_name] = "hello";
    
}
void insert_row(vector<string> args,map<string, SQLTable> &sql_manager){
    //constraint: operation, table_name, <row>
    size_t sz = args.size();
    if(sz < 3){
        cout << "\n-- TABLE NOT CREATED, MISSING PARAMETERS -- \n";
        return;
    }

    //get the name of table
    string name = args[1];
    //get the row data
    vector<string> row =vector<string> (args.begin()+2, args.end());
    
    //open table ?
    // SimpleTableHandler T(name);

    
    
} 
void delete_table(string name, map<string, SQLTable> &sql_manager){
    
}
void list_tables(const map<string, SQLTable> &sql_manager){
    int num =1;
    for(auto it = sql_manager.begin(); it != sql_manager.end(); ++it){
        cout << "["<<setw(3)<<num++ << " ] - ";
        cout << it->first << "\n";
    }
}
void search_pkey(vector<string> args, map<string, SQLTable> &sql_manager){
    
}
