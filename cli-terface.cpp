#include <iostream> 
#include <string> 
#include <vector>
#include <set>
#include <filesystem>
#include <map>
#include <iomanip>


#include "SimpleTableHandler.h"


using std::string, std::map, std::setw, std::left;
using std::vector, std::cout, std::getline, std::cin;
namespace  fs = std::filesystem;

//%%%%%%%%%%% -- DEFINITIONS -- %%%%%%%%%%%
//#define _VERBOSE_OUTPUT


#ifndef DIR_TABLES
#define DIR_TABLES "ScratchQL/FolderTables"
#endif

#ifdef _VERBOSE_OUTPUT
#define DBG_COUT(message) do { std::cout << (message) << std::endl; } while (0)
#else
#define DBG_COUT(message) do {} while (0)
#endif


//%%%%%%%%%%% -- SMALL FUNCTIONS  DECL -- %%%%%%%%%%%
bool valid_args(const vector<string>& args, int min_val) {
    if (args.size() < min_val) {
        cout << "\n[ERROR] --  Missing parameters.\n";
        return false;
    }
    return true;
}
void strUpper(string &str){for (auto & c: str) c = toupper(c);}

//%%%%%%%%%%% -- PREDECLARATIONS -- %%%%%%%%%%%
typedef SimpleTableHandler SQLTable;
const  std::set<string> datas = {"INT", "DATE", "STRING", "BOOL", "FLOAT"};
const bool data_exist(string tp){return datas.count(tp) != 0;}

void read_input_command(string args,/*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager);
void delete_table(vector<string> args, /*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager); 
void insert_row(vector<string> args,/*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager ); 
void create_table(vector<string> args,/*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager);
void search_pkey(vector<string> args, /*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager);
void search_skey(vector<string> args, /*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager);
void update_list_tables(/*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager);
void list_properties(string args, const std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager);
void display_tables(vector<string> args, std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager);

void help_console();
void list_tables(const /*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager);


void delte_row(vector<string> vector1, map<std::string, std::shared_ptr<SQLTable>> &map);

void update_table(vector<string> vector1, map<std::string, std::shared_ptr<SQLTable>> &map);

int main() {
    string command, tmp;
    std::map<std::string, std::shared_ptr<SQLTable>> sql_manager;
    cout << "-- LOADING TABLES --\n";
    update_list_tables(sql_manager);
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

void read_input_command(string args,/*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager){
    DBG_COUT(("COMMAND:" + args));

    if(args == "HELP"){
        help_console();
        return;
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
    // for (const auto& p : param) { DBG_COUT( "param - " + p);};

    if(args.find("DISPLAY")!= string::npos){
        display_tables(param, sql_manager);
        return;
    }
    
    if(args.find("CREATE-TABLE")!= string::npos){
        create_table(param, sql_manager);
        return;
    }

    if(args.find("LIST-TABLE")!= string::npos){
        list_tables(sql_manager);
        return;
    }

    if(args.find("DELETE-TABLE")!= string::npos){
        delete_table(param, sql_manager);
        return;
    }

        if(args.find("INSERT-ROW")!= string::npos){
        insert_row(param, sql_manager);
        return;
    }

    if(args.find("QUERY-PKEY")!= string::npos){
        search_pkey(param, sql_manager);
        return;
    }
    if(args.find("QUERY-SKEY")!= string::npos){
        search_skey(param, sql_manager);
        return;
    }

    if(args.find("UPDATE-DB")!= string::npos){
        update_list_tables(sql_manager);
        return;
    }
    if(args.find("DELETE * FROM")!= string::npos){
        delte_row(param, sql_manager);
        return;
    }

    if(args.find("UPDATE * FROM")!= string::npos){
        update_table(param, sql_manager);
        return;
    }




    if(args.size()  != 0)
        cout << "\n --  COMMAND UNDEFINED ! --\n";
}

void update_table(vector<string> vector1, map<std::string, std::shared_ptr<SQLTable>> &map) {
    //UPDATE * FROM <table_name> SET <entity> = <value> WHERE id = ?
    // constraints <cmd> <table_name> <entity> <value> <key>
    if(!valid_args(vector1, 6)) return ;

    string name = vector1[3];
    string ent_key = vector1[5];
    string key = vector1[11];
    string value = vector1[7];
    cout << key;
    if(map.count(name) == 0){
        cout << "\n-- TABLE ( "<< name <<" )NOT FOUND -- \n";
        return;
    }

    vector<size_t> size_ent(map[name]->get_total_entities(), 0);
    int idx = 0;
    size_t total = map[name]->total_items();
    size_t total2print = std::min((size_t)10, total);
    vector<vector<DataInterface *>> vis_table;
    vector<DataInterface *> row;
    auto columns = map[name]->get_entities();
    vector<DataInterface *> row2(map[name]->get_total_entities(), nullptr); // Inicializar row2 com o tamanho correto e valores nulos

    while (idx < total2print) {
        row = map[name]->read_row(idx++);

        if (row.size() != map[name]->get_total_entities()) break;
        vis_table.push_back(row);

        // Atualiza o tamanho máximo dos dados por coluna
        for (int i = 0; i < map[name]->get_total_entities(); i++) {
            size_ent[i] = std::max(size_ent[i], (row[i]->toString()).size());
        }

        // Procurar pela coluna com o nome especificado
        for (int i = 0; i < map[name]->get_total_entities(); i++) {
            if (key == row[i]->toString()) {
                // Registro encontrado
                int numero_coluna = 1;
                cout << "Registro encontrado: " << std::endl;
                for (auto &data : row) {
                    cout << "Valor da Coluna " << numero_coluna << " == " << data->toString() << " ";
                    cout << std::endl;

                    // Atualizar ou manter o valor da coluna em row2
                    if(columns[numero_coluna-1].name == ent_key){
                        // Atualiza o valor da entidade especificada
                        row2[numero_coluna-1] = dt_alloc(columns[numero_coluna-1].type, value);
                    } else {
                        // Mantém o valor original
                        row2[numero_coluna-1] = data;
                    }

                    numero_coluna++;
                }

                cout << std::endl;
                cout << "Registro atualizado: " << std::endl;
                for(auto &data : row2){
                    cout << data->toString() << " ";
                }
                cout << std::endl;

                // Atualizar a linha no mapa
                map[name]->update(idx-1, row2);

                break;  // Saia do loop quando encontrar a correspondência
            }
        }
    }

}

void delte_row(vector<string> vector1, map<std::string, std::shared_ptr<SQLTable>>& map) {
    //DELETE * FROM <table_name> WHERE id = ?
    // constraints <cmd> <table_name> <key>
    if(!valid_args(vector1, 4)) return ;

    string name = vector1[3];
    std::string key = vector1[7];
    cout << key;
    if(map.count(name) == 0){
        cout << "\n-- TABLE ( "<< name <<" )NOT FOUND -- \n";
        return;
    }


    map[name]->delete_row(key,name);
    map[name]->display();
    cout<<std::endl;
}

void update_list_tables(/*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager)
{
    sql_manager.clear();
    fs::path path = DIR_TABLES; // Caminho da pasta
    string file_ext = ".hbin"; 
        // Verificar se o diretório existe
    if (!fs::is_directory(path)) {
        cout << "\n -- ERROR: " << path << " NOT FOUND -- \n";
        return;
    }

    // search for header extension
    for ( auto & entry : fs::directory_iterator(path)) {
        auto filename = entry.path().filename();
        string ext = filename.extension().string();
        string stem = filename.stem().string();
        if(ext == file_ext){
            auto table = std::make_shared<SQLTable>(stem, false);
            sql_manager[stem] = table;
            
            DBG_COUT("found -- " + stem + "  --with ext: " + ext);
        }
    }
}



void create_table(vector<string> args, /*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager){
    size_t sz = args.size();
    if(sz < 3){
        cout << "\n-- TABLE NOT CREATED, MISSING PARAMETERS -- \n";
        return;
    }
    vector<string> entities_name, entities_type;
    vector<EntityProperties> entities;
    string arg, name, type;
    size_t pos =0, len;
    string table_name = args[1];
    DBG_COUT("Creating table link -- " + name);
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
        entities.push_back(EntityProperties(name, str2type(type), i-2));
        DBG_COUT(name + ", " + type);
    }
    auto table = std::make_shared<SQLTable>(table_name, entities, false);
    sql_manager[table_name] = table;

    //CRIACÃO/REESCRITA DE TABELA
    
}
void insert_row(vector<string> args,/*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager){
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
    
    // check if table exists
    if(sql_manager.count(name) == 0){
        cout << "\n-- TABLE NOT FOUND -- \n";
        return;
    }

    if(!sql_manager[name]->insert(row))
        cout << "\n--[ERROR] ROW NOT INSERTED -- \n";

    
} 
void delete_table(vector<string> args, /*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager){
    //constraint - <cmd> <name> 
    size_t sz = args.size();
    if(sz < 2){
        cout << "\n-- TABLE NOT CREATED, MISSING PARAMETERS -- \n";
        return;
    }
    string name = args[1];
    
    if(sql_manager.count(name) == 0){
        cout << "\n-- TABLE ( "<< name <<" )NOT FOUND -- \n";
        return;
    }   

    sql_manager[name]->drop();
    sql_manager.erase(name);
}
void list_tables(const /*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager) {
    int num = 1;
    const int width = 30; // Definindo largura fixa 


    cout << "╔══════════════════════════════════════╗\n";
    cout << "║            Table Listing             ║\n";
    cout << "╚══════════════════════════════════════╝\n";
    cout << "╔═════╦════════════════════════════════╗\n";
    cout << "║ No. ║ Table Name                     ║\n";
    cout << "╠═════╬════════════════════════════════╣\n";

    for (const auto &entry : sql_manager) {
        cout << "║ " << setw(3) << left << num++ << " ║ " 
             << setw(width) << left << entry.first << " ║\n";
    }
    cout << "╚═════╩════════════════════════════════╝\n";


    // list_properties(sql_manager);
}

void list_properties(string arg, std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager) {
    const int width = 15; // Define a fixed width for each column

    
    // for (const auto& entry : sql_manager) {
        // Get the entities (columns) of the table
        vector<EntityProperties> entities = sql_manager[arg]->get_entities();

        // Header for each table
        cout << "╔════════════════════════════════════════════════╗\n";
        cout << "║  Properties from : " << std::setw(25) << arg << " ║\n";
        cout << "╠════════════════╦═══════════════════════════════╣\n";
        cout << "║ Column         ║ Type                          ║\n";
        cout << "╠════════════════╬═══════════════════════════════╣\n";

        // Print each column name and its type
        for (const auto& entity : entities) {
            cout << "║ " << std::setw(14) << left << entity.name
                 << "║ " << std::setw(30) << left << type2str(entity.type) << " ║\n";
        }

        // End of the table for each entity
        cout << "╚════════════════╩═══════════════════════════════╝\n\n";
    // }
}

void search_pkey(vector<string> args, std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager){
    // constraints <cmd> <table_name> <key>
    if(!valid_args(args, 3)) return ;

    string name = args[1];
    string key = args[2];
    cout << key;
    if(sql_manager.count(name) == 0){
        cout << "\n-- TABLE ( "<< name <<" )NOT FOUND -- \n";
        return;
    }



    vector<size_t> size_ent(sql_manager[name]->get_total_entities(), 0);
    int idx =0;
    //Collect size to print
    size_t total = sql_manager[name]->total_items();
    size_t total2print = std::min((size_t)10, total);
    vector<vector<DataInterface *>> vis_table;
    vector<DataInterface *> row;
    while(idx < total2print){
        row = sql_manager[name]->read_row(idx++);

        if(row.size()!= sql_manager[name]->get_total_entities()) break;
        vis_table.push_back(row);


        for(int i=0; i < sql_manager[name]->get_total_entities();i++) {
            size_ent[i] = std::max(size_ent[i], (row[i]->toString()).size());
        }

        for(auto &data : row){
            if(key == data->toString()){
                int numero_coluna = 1;
                cout <<"╠════════════════╦═══════════════════════════════╣\n";
                cout << "Registro encontrado: " << std::endl;
                for(auto &data : row){
                    cout << "║ Coluna " << std::setw(14) << left << numero_coluna
                         << "║ " << std::setw(30) << left << data->toString() << " ║\n";
                    numero_coluna++;
                }
                cout << "╚════════════════╩╦═════════════════════════════╝\n";
                break;
            }
            cout << std::endl;
        }
    }
    if (row.empty()) {
        cout << "Nenhum registro encontrado" << std::endl;
    }

}

void search_skey(vector<string> args, /*map<string, SQLTable> &sql_manager*/ std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager){
    // constraints <cmd> <table_name> <entity> <key>
    if(!valid_args(args, 4)) return ;

    string name    = args[1];
    string ent_key = args[2];
    string key     = args[3];

    if(sql_manager.count(name) == 0){
        cout << "\n-- TABLE ( "<< name <<" )NOT FOUND -- \n";
        return;
    }

    vector<size_t> size_ent(sql_manager[name]->get_total_entities(), 0);
    int idx = 0;

// Coletar o tamanho para impressão
    size_t total = sql_manager[name]->total_items();
    size_t total2print = std::min((size_t)10, total);
    vector<vector<DataInterface *>> vis_table;
    vector<DataInterface *> row;
    auto columns = sql_manager[name]->get_entities();

    while (idx < total2print) {
        row = sql_manager[name]->read_row(idx++);

        if (row.size() != sql_manager[name]->get_total_entities()) break;
        vis_table.push_back(row);

        // Atualiza o tamanho máximo dos dados por coluna
        for (int i = 0; i < sql_manager[name]->get_total_entities(); i++) {
            size_ent[i] = std::max(size_ent[i], (row[i]->toString()).size());
        }

        // Procurar pela coluna com o nome especificado
        for (int i = 0; i < sql_manager[name]->get_total_entities(); i++) {
            if (ent_key == columns[i].name && key == row[i]->toString()) {
                // Registro encontrado
                int numero_coluna = 1;
                cout << "Registro encontrado: " << std::endl;
                for (auto &data : row) {
                    cout << "Valor da Coluna " << numero_coluna << " == " << data->toString() << " ";
                    numero_coluna++;
                }
                cout << std::endl;
                break;  // Saia do loop quando encontrar a correspondência
            }
        }
    }
    if (row.empty()) {
        cout << "Nenhum registro encontrado" << std::endl;
    }


}



void help_console() {
    // Cabeçalho elegante
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║                Main Functions for SimpleQL               ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n";
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║ Commands                                                 ║\n";
    cout << "╠══════════════════════════════════════════════════════════╣\n";
    cout << "║ - CREATE-TABLE <name> [<name_entity>=<data_type>]        ║\n";
    cout << "║ - DELETE-TABLE <table_name>                              ║\n";
    cout << "║ - INSERT-ROW   <table_name> (<row>)                      ║\n";
    cout << "║ - LIST-TABLES                                            ║\n";
    cout << "║ - QUERY-PKEY   <table_name> <PKEY>                       ║\n";
    cout << "║ - QUERY-SKEY   <table_name> <entity_name> <PKEY>         ║\n";
    cout << "║ - UPDATE-DB                                              ║\n";
    cout << "║ - DELETE * FROM <table_name> WHERE id = ?                ║\n";
    cout << "║ - UPDATE * FROM <table_name> SET <entity> = <value> WHERE id = ? ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n";
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║ Description                                              ║\n";
    cout << "╠══════════════════════════════════════════════════════════╣\n";
    cout << "║ - CREATE-TABLE : Truncate/write a new table.             ║\n";
    cout << "║ - DELETE-TABLE : Delete the table.                       ║\n";
    cout << "║ - INSERT-ROW   : Insert a row, dont use space on string. ║\n";
    cout << "║ - LIST-TABLES  : List the tables created.                ║\n";
    cout << "║ - QUERY-PKEY   : search for data by pkey and shows       ║\n";
    cout << "║ - QUERY-SKEY   : search for data by skey and shows       ║\n";
    cout << "║ - UPDATE-DB    : Force update to read tables             ║\n";
    cout << "║ - DELETE FROM : Delete a row from table                 ║\n";
    cout << "║ - UPDATE * FROM : Update a row from table                ║\n";
    cout << "╠══════════════════════════════════════════════════════════╣\n";
    cout << "║ Data Types Available :                                   ║\n";
    // cout << "╠══════════════════════════════════════════════════════════╣\n";
    cout << "║ ";
    int  i = 0;
    for(auto const &c: datas){
        cout << c ;
        if (i++ != datas.size() - 1) cout << ", ";
    }
    cout << "                           ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n";

}

void display_tables(vector<string> args, std::map<std::string, std::shared_ptr<SQLTable>>& sql_manager){
    // constraint <cmd> <table1> <table2> ...
    if(!valid_args(args, 2)){return;}

    for(int i=1; i <args.size(); i++){
        string name = args[i];
        if(sql_manager.count(name) != 0){
            cout << "Table Name -- " << name << "\n";
            sql_manager[name]->display();
            list_properties(name, sql_manager);
        } else {
            cout << "Table  -- " << name << " not found\n";
            return;
        }
    }


}