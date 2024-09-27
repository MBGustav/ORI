class Tabela:
    def __init__(self, nome, num_colunas):
        self.nome = nome
        self.num_colunas = num_colunas
        self.parametros = {}

    def adicionar_parametro(self, chave, valor):
        self.parametros[chave] = valor

    def print_tabela(self):
        info = f"Nome da tabela: {self.nome}\nNúmero de colunas: {self.num_colunas}\n"
        for chave, valor in self.parametros.items():
            info += f"{chave}: {valor}\n"
        return info
    
    def save_to_file(self):
        with open(f"{self.nome}.txt", "w") as file:
            file.write(self.print_tabela())

    def read_from_file(self):
        with open(f"{self.nome}.txt", "r") as file:
            lines = file.readlines()
            self.nome = lines[0].split(":")[1].strip()
            self.num_colunas = int(lines[1].split(":")[1].strip())
            for line in lines[2:]:
                chave, valor = line.split(":")
                self.parametros[chave.strip()] = valor.strip()
