# Compressor/Descompressor JPEG

## Projeto da disciplina de Multimídia @ ICMC-USP

## Realizado por:
- Felipe Carneiro Machado - 14569373    
- Augusto Cavalcante Barbosa Pereira - 14651531


Este repositório contém o proejto final para a disciplina de Multimídia, oferecida no ICMC-USP, 1˚ semestre de 2025, pelo Prof. Dr. Rudinei Goularte

O projeto consiste na implementação de um par compressor/descompressor para imagens .bmp, seguindo as etapas do padrão JPEG, são elas:
- Conversão para o espaço de cor YCrCb com subsampling
- Compressão com perdas com DCT (*Discrete Cosine Transform*)
- Quantização dos coeficientes da DCT
- Vetorização em Zig-Zag dos coeficientes AC
- *Run Length encoding* para coeficientes AC e codificação diferencial para DC
- Codificação de Huffman 

A informação gerada no final pelo compressor é salva em um arquivo binário de formato próprio.

### Sobre a organização do código

Segue a estrutura de diretórios

```
.
├── assets # Imagens para tests
├── executables # Diretorio saida para os executaveis, criado pelo Makefile
├── bin # Diretorio saida para os arquivos objeto, criado pelo Makefile
├── Makefile # Makefile do projeto
└── src # Codigo fonte
    ├── compressor.c # Compressor, possue funcao main
    ├── decompressor.c # Descompressor, possue funcao main
    ├── app # Implementa as funcoes de compressao/descompressao completas
    ├── imIO # IO de imagens e conversao de espaço de cor
    └── jpeg # Implementacao das etapas de compressao/descompressao
```

### Procedimentos de compilação e execução

Este projeto foi implementado e testado em ambientes Linux e Windows utilizando o compilador gcc sob o padrão C99, e o comando make para execução de comandos.

Para compilar o código, basta executar, estando no diretório raiz, o comando `make build`, isto compilará todos os arquivos `.c`, criará os diretórios `bin` e `executables`, e criará os executáveis `compressor` e `decompressor` dentro do diretório `executables`.

Para executar o compressor, execute (assumindo que está no diretório raiz do projeto, caso contrário utilize o caminho para o executável):

```
./executables/compressor <bmp_fonte> <arquivo_destino> <fator_de_compressao>
```

`<bmp_fonte>` e `<arquivo_destino>` devem ser os caminhos para o arquivo a ser comprimido e o arquivo onde deve ser armazenado o resultado da compressão. Não é necessário que o arquivo destino exista previamente, mas seus diretórios pais devem. `<fator_de_compressao>` deve ser um número `double` que atuará como fator de compressão na etapa de quantização. Maiores valores implicam maior compressão. Note que ao executar a compressão, o programa printa a taxa de compressão em porcentagem.

Para executar o descompressor, execute (assumindo que está no diretório raiz do projeto, caso contrário utilize o caminho para o executável):

```
./executables/compressor <bin_fonte> <arquivo_destino>
```

Analogamente ao compressor, `<bin_fonte>` é o arquivo binário gerado pelo compressor e `<arquivo_destino>` é o arquivo .bmp a ser gerado. Este é um bmp completo que pode ser aberto com software padrão para visualização de imagens.
