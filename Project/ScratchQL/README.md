# Apresentação:

Neste projeto, nosso objetivo é aplicar diversos conceitos de Recuperação e Organização da Informação, com foco na implementação prática de um sistema baseado em SQL. O projeto será dividido em duas frentes principais de desenvolvimento: o Interpretador e o SQLTableManager.

- Interpretador: Esta parte do sistema será responsável por lidar com a entrada do usuário, que pode ser fornecida como texto simples ou comandos via linha de comando. O Interpretador será encarregado de processar essas entradas, transformando em comandos SQL para o sistema. Isso envolve a interpretação de consultas, comandos de inserção e exclusão de dados, garantindo que as instruções do usuário sejam traduzidas para operações SQL.

- SQLTableManager: Esta componente gerenciará as interações com as tabelas SQL, incluindo a criação, atualização e exclusão de tabelas. O SQLTableManager deve ser capaz de manter e manipular a estrutura das tabelas, bem como gerenciar os dados que elas contêm. Isso inclui operações como definir esquemas de tabelas, aplicar modificações na estrutura. 



## IMPLEMENTAÇÃO - CLASSE SQLTable(antiga SimpleTableHandler)

Esta classe tem como intuito representar o objeto tabela que, por sua vez, lida com o gerenciamento e atualização dos arquivos binários. A partir da criação de uma tabela, são criados dois arquivos binários complementares: **header** e **data**, onde este apresenta as informações adicionadas à tabela, enquanto naquele contém informações de construção. 

Para maior detalhamento, observar diagrama UML da classe mencionada:

<!-- TODO: incluir UML SQLTable -->







<!-- ### Ideas for _TableHandler_
1. A Class to File(making it more easier to read/write rows and collumns with the same pointer)[FileHandler]

2. A structure to deal with primary/secondary/n-ary keys, and TableHandler just intermediate the situation.
3. **Develop _delete_ after search is done.**
3. Busca pela chave primária (CPF)
4. Busca por chave secundaria - CIDADE por exemplo. 
    - implementa no começo sequencial.
5. Implenentação de Inserção/Remoção - logica e fisica.

 -->


## PROXIMOS PASSOS

|IMPLEMENTED ?| Unit-Tested ? | Descr.|
|-|-|-|
|✅| -| Binary Scheme Organization to read/write file|
|❌| -| **(just an idea)** mutex to control and manage file registry|
|❌|❌| DataInterface Operations(+,++,-, --, *, / and others)|
|✅|✅| DataInterface class and its abstractions|
|✅|✅| HeaderHandler class |
|✅|❌| TableHandler class**|


<!-- ✅ -->
<!-- ❌ -->

** I could do an data structure related to Threes and List/Vectorized. This idea allows me to improve from both sides: search and inclusion/exclusion. With this, data selection/filtering becomes fast and pratical. However, how should I do it ? 




## REFERENCIAS
- [Comandos SQL - separados por tópico](https://algodaily.com/lessons/useful-sql-commands-with-examples)

