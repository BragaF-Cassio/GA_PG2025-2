## Processamento Gráfico: Fundamentos - Grau A, 2025/2, Unisinos
> Este programa foi desenvolvido como parte da disciplina *Processamento Gráfico* com o objetivo de aplicar conceitos de OpenGL moderna e C++ na criação de uma aplicação interativa. O projeto explora a utilização de shaders, transformações, projeções e detecção de colisões em um ambiente gráfico 2D/3D.

## Jogo: Sai de Baixo!
- Objetivo: desviar das plataformas em queda.

## Como jogar:
- O jogo pode ser iniciado executando o play.bat
- Use as setas para se mover para a esquerda e para a direita
- Desvie das plataformas em queda
- Desviando de uma plataforma, você ganha 1 ponto
- Ao ser atingido por uma plataforma, o jogo acaba
- As plataformas surgem aleatóriamente
- A velocidade em que as plataformas caem aumenta com o tempo, deixando o jogo progressivamente mais difícil

## Integrantes: 
- Cássio Ferreira Braga
- Gabriel Cezar Walber
- Patrícia Nagel

## Profª:
- Rossana Baptista Queiroz

## Informações Técnicas

- **Linguagem:** C++ (C++11 ou superior)  
- **API Gráfica:** OpenGL 3.3+ (core profile)  
- **Dependências:** GLFW, GLAD, stb_image, glm 
- **IDE/Compilador:** Visual Studio Code / MinGW / CMake (ou equivalente)  
- **Plataforma-alvo:** Windows / Linux / MacOS  

## Checklist de Requisitos

- [x] Criação de janela e contexto OpenGL  
- [x] Configuração de shaders e pipeline programável  
- [X] Implementação de transformações (model, view, projection)  
- [X] Controle de movimentação do personagem/câmera  
- [X] Renderização de múltiplos objetos  
- [X] Detecção de colisão

## Estrutura do Repositório

```plaintext
📂 GA_PG2025-2/
├── 📂 common/                # Código reutilizável entre os projetos
│   └── glad.c                # Implementação da GLAD
├── 📂 include/               # Cabeçalhos e bibliotecas de terceiros
│   └── 📂 glad/              # Cabeçalhos da GLAD (OpenGL Loader)
│       ├── 📂 KHR/           # Diretório com cabeçalhos da Khronos (GLAD)
│          └── khrplatform.h
│       └── glad.h
├── 📂 src/                   # Pasta do código-fonte
│   └── 📂 Jogo/
│       └── 📂 SaiDeBaixo/
│           └── SaiDeBaixo.cpp # Arquivo com o cógido fonte do jogo
├── 📂 build/                  # Diretório gerado pelo CMake (não incluído no repositório)
├── 📄 CMakeLists.txt          # Configuração do CMake para compilar os projetos
├── 📄 README.md               # Este arquivo, com a documentação do repositório
└── 📄 play.bat                # Script para iniciar o jogo
```
