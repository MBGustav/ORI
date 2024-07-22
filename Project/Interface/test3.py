import tkinter as tk
from tkinter import messagebox
from table import Tabela


def open_create_table_page():
    raise_frame(create_table_frame)

def open_show_tables_page():
    raise_frame(show_tables_frame)

def inicial_page():
    raise_frame(inicial_frame)

def raise_frame(frame):
    frame.tkraise()

def show_tables(table : Tabela):
    messagebox.showinfo("Table: " + table.nome, "" + table.print_tabela())

def send_data_to_cpp():
    messagebox.showinfo("Info", "Dados enviados para o backend em C++ (ainda não implementado).")

def content_show_tables_page():
    tk.Label(show_tables_frame, text="Página de Mostrar Tabelas").pack()
    tk.Label(show_tables_frame, text="Digite o nome da tabela que deseja visualizar:").pack()
    table_name_entry = tk.Entry(show_tables_frame)
    table_name_entry.pack()
    def show_table():
        try:
            table_name = table_name_entry.get()
            table = Tabela(table_name, 0)
            table.read_from_file()
            show_tables(table)
        except FileNotFoundError:
            messagebox.showerror("Error", "Table not found.")
    tk.Button(show_tables_frame, text="Mostrar tabela", command=show_table).pack()

def cria_tabela(table_name_entry,num_columns_entry):
    table_name = table_name_entry.get()
    num_columns = num_columns_entry.get()
    if num_columns:
        int_num_columns = int(num_columns)
        tabela = Tabela(table_name, int_num_columns)
        after_create_table(tabela)
    else:
        messagebox.showerror("Error", "Please enter a valid number of columns.")
   

def content_create_table_page():
    initial_description = """
    Essa pagina foi criada para a criacao de tabelas. Coloque o nome da tabela e 
    a quantidade de colunas que deseja criar. Depois disso, insira o nome das colunas e informe o tipo de dado.
    Exemplo: Nome da coluna: id, Tipo de dado: INT , Nome da coluna: nome, Tipo de dado: STRING"""
    tk.Label(create_table_frame, text=initial_description, font=("Arial", 10), wraplength=300, justify="center").pack()
    tk.Label(create_table_frame, text="Nome da tabela:").pack()
    table_name_entry = tk.Entry(create_table_frame)
    table_name_entry.pack()
    tk.Label(create_table_frame, text="Quantidade de colunas:").pack()
    num_columns_entry = tk.Entry(create_table_frame)
    num_columns_entry.pack()
    criar_tabela_button =  tk.Button(create_table_frame, text="Criar tabela", command=lambda: cria_tabela(table_name_entry,num_columns_entry))
    criar_tabela_button.pack()
    

def after_create_table(tabela : Tabela):
    column_names = []
    data_types = []
    left_frame = tk.Frame(create_table_frame)
    left_frame.pack(side="left")
    right_frame = tk.Frame(create_table_frame)
    right_frame.pack(side="right")
    current_frame = left_frame
    for i in range(tabela.num_colunas):
        current_frame = left_frame if i % 2 == 0 else right_frame
        tk.Label(current_frame, text=f"Nome da coluna {i}:").pack()
        column_name = tk.Entry(current_frame)
        column_name.pack()
        column_names.append(column_name)
        tk.Label(current_frame, text=f"Tipo de dado {i}:").pack()
        data_type = tk.Entry(current_frame)
        data_type.pack(fill="x")
        data_types.append(data_type)
    def add_columns():
        for name_entry, type_entry in zip(column_names, data_types):
            tabela.adicionar_parametro(name_entry.get(), type_entry.get())
        messagebox.showinfo("Info", "Colunas adicionadas com sucesso!")
        tabela.save_to_file()
    
    # Botão para adicionar colunas
    tk.Button(create_table_frame, text="Adicionar colunas", command= add_columns).pack()
    tk.Button(create_table_frame, text="Enviar dados para o backend", command=send_data_to_cpp).pack()
    tk.Button(create_table_frame, text="Imprimir tabela", command=lambda: show_tables(tabela)).pack()



def content_inicial_page():
    # Texto descritivo para a página inicial
    initial_description = """
    Bem-vindo à nossa aplicação de gerenciamento de tabelas de Banco de Dados! 
    Aqui, você pode criar novas tabelas, visualizar tabelas existentes e muito mais.
    Simplifique o seu trabalho com nossa ferramenta poderosa e intuitiva.
    """

    tk.Label(inicial_frame, text=initial_description, font=("Arial", 10), wraplength=300, justify="center").pack()

window = tk.Tk()
window.title("Table Management")
window.geometry("800x700")
window.resizable(False, False)
window.configure(bg="white")

create_table_frame = tk.Frame(window, height=400, width=700)
show_tables_frame = tk.Frame(window , height=400, width=700)
inicial_frame = tk.Frame(window , height=400, width=700)

# Conteúdo para os frames
tk.Label(create_table_frame, text='Página de Criação de Tabelas').pack()
tk.Button(create_table_frame, text='Ir para Mostrar Tabelas', command=open_show_tables_page).pack()

tk.Label(show_tables_frame, text='Página de Mostrar Tabelas').pack()
tk.Button(show_tables_frame, text='Ir para Criar Tabela', command=open_create_table_page).pack()

tk.Label(inicial_frame, text='Página Inicial').pack()
tk.Button(inicial_frame, text='Ir para Criação de Tabela', command=open_create_table_page).pack()

# Posicione os frames
for frame in (create_table_frame, show_tables_frame, inicial_frame):
    frame.grid(row=0, column=0, sticky='news')

content_inicial_page()
content_create_table_page()
content_show_tables_page()
# Menu setup
menu_bar = tk.Menu(window)
window.config(menu=menu_bar)

file_menu = tk.Menu(menu_bar, tearoff=0)
menu_bar.add_cascade(label="Menu", menu=file_menu)
file_menu.add_command(label="Criar Tabela", command=open_create_table_page)
file_menu.add_command(label="Mostrar Tabelas", command=open_show_tables_page)
file_menu.add_command(label="Ir para pagina inicial", command=inicial_page)
file_menu.add_separator()
file_menu.add_command(label="Sair", command=window.quit)

# Mostra o frame inicial
raise_frame(inicial_frame)

window.mainloop()
