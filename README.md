# discreta12petriautom

 ex12.c trata-se de uma simulação de uma rede de Petri, se utilizando dos recursos de dados abstratos e de threads. A entrada de dados será feita pela leitura dos valores nos arquivos entradas-petri.txt e os valores são armazena dos em variáveis, sendo os cinco primeiros valores representativos de quantidades (lugares, transições...), os demais não são variáveis únicas sendo necessário a utilização do sistema de lista para armazena-las. O paralelismo foi implementado para simular a aleatoriedade do programa, além de melhorar o processamento desta. Para cada thread haverá uma transição com listas de arcos que partem ou entram nela, denominadas listas de arco entram e de arco saem. Com base nessas listas arco é que a simulação funciona, retirando tokens, ativando transições e adicionando tokens em novos lugares ou no mesmo lugar. A rede de Petri é desenha para melhor visualização e para isso é utilizado a biblioteca allegro.h, na qual 3 funções derivam para desenhar tal rede, sendo elas (desenha_estados, desenha_transicao e desenha_arcos).


    Alunos: Arthur Carvalho de Albuquerque Cardoso
            Mateus Lenier Rezende
    Professor: Dr. Ruben Carlo Benante.
    Curso: Engenharia de Controle e Automação.
