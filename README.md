# Linguagem Simp

## Features

- [x] Expressões Aritméticas
- [x] Expressões Relacionais
- [x] Variáveis
- [x] Condicionais (if/else)
- [x] Laço de repetição (while)
- [x] Procedimentos/Funções
- [x] Funções anônimas/lambdas
- [x] Closures/upvalues
- [x] IO (read/print)
- [x] Recursão (ver exemplo y.simp)
- [x] Erros/warnings de compilação
- [ ] Tailcall
- [ ] Arrays
- [ ] Strings
- [ ] Laço de repetição (for)

## Instruções para building

1. Instale as dependências do parser (bison e flex)
2. Baixe e faça build da versão 4 da linguagem lua

```bash
curl -R -O http://www.lua.org/ftp/lua-4.0.1.tar.gz
tar zxf lua-4.0.1.tar.gz
mv lua-4.0.1 lua
cd lua
make all
```

3. make

O executável será gerado em `output/simp`

## Uso

TODO
