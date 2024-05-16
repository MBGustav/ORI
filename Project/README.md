
# Implementação de um Interpretador - ScratchQL

Neste projeto, será desenvolvido um projeto de estudo voltado no desenvolvimento da Linguagem SQL. Tal desenvolvimento possui duas frentes de produção: Interpretador e Gerenciamento de Tabelas.


# PASSOS DE DESENVOLVIMENTO
## Gerenciamento de Tabelas

Para Abstrairmos a entrada de diferentes tipos de dados, criamos uma estrutura fixa de entradas possíveis para uso na sua entrada, especificada com enumeração como `DataType`. Nele, limitamos os tipos de dados a serem trabalhados.

Em suma:
- `DataParameters`: classe para leitura do dado de forma correta
- `EntityProp`: Propriedades da Entidade
- `DataType`: Tipo de Dados(STRING, INT, DATA, FLOAT, BOOL)
<br><br><br>

```mermaid
classDiagram

    class DataParameters {
        - DataType tp
        + virtual string getValueStr() = 0
        + virtual int getValueInt() = 0
        + virtual float getValueFloat() = 0
        + virtual bool getValueBool() = 0
    }

    class EntityProp {
        - string Name
        - DataType type
    }

    class DataType {
        <<enumeration>>
        STRING
        INTEGER
        FLOAT
        BOOLEAN
    }

```

Para manipulação dos Arquivos: MetaData. Sua função é integrar o arquivo físico e a tabela. 
Nesta organização, temos muito oque melhorar: uso de Hash, pesquisa de dados e outros BTree..


```mermaid
classDiagram
    class MetaData {
        - filePointer streamTable
        - string _tablename
        - size_t _totalRows
        - size_t _headerOffset
        - array _entity
        
        -- Private Methods --
        - void OpenTable()
        - void CloseTable()
        - void Delete()
        - void write()
        - void read()
        
        + MetaData(string TableName, v_entity &Properties)
        + ~MetaData()
        + string get_name(bool fullName = false)
        + size_t get_offset_row(size_t idx)
        + size_t get_numRows()
        + v_entity get_entities()
        + size_t get_offset()
        + size_t get_sizeHeader()
        + int get_rowOffset()
        + size_t total_entities()
        + void set_name(string Name)
        + void set_entities(v_entity entity)
        + void set_totalRows(size_t rows)
        + void set_headerOffset(size_t offset)
        + void increase_elements()
        + void decrease_elements()
    }   

    class ErrorHandler

    MetaData "1" *-- "*" EntityProp : contém
    MetaData "1" *-- "1" ErrorHandler : chama
    MetaData "1" *-- "1" data_t : tem
```

## INTERPRETADOR - S~~crath~~QL

Esta etapa é feita em conjunto com a criação do gerenciador de tabelas. Nele, temos a chamada e criação de arquivos de tabelas para gerir todo o sistema. 

<img src="https://media.licdn.com/dms/image/D5622AQFSLOogRPf1RQ/feedshare-shrink_800/0/1708318964477?e=1718236800&v=beta&t=1TmxqlHWvcp8S-bVeGpAZFjzzGxN6jRZNhRMnsfIsJ8" width=900>

Na imagem, temos a síntese de todas as _keywords_ da Linguagem SQL. Para este exemplo, estamos desenvolvendo apenas a ponta do Iceberg - por isso, ScratchQL - a fim de entender os conceitos de organização de dados, bem como o desenvolvimento de um Interpretador.


## REFERENCIAS BIBLIOGRAFICAS
