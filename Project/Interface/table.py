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
