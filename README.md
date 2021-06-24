# Programacao_final1
## Projeto final de programação 2ª semestre no ISLA

Pretende-se que os alunos implementem um trabalho prático de sua autoria, e à sua escolha, podendo escolher um dos temas sugeridos ou apresentando uma ideia similar ao nível de exigência, desenvolvam e que no final defendam o seu trabalho desenvolvido.
Este trabalho tem um peso de 25% da nota final e é obrigatório para o processo de avaliação no decorrer do semestre.
Pretende-se que este trabalho englobe a maioria dos conteúdos abordados e se necessário os alunos podem fazer uso de outras bibliotecas, não abordadas em aula.
O trabalho pode ser realizado em grupos de 2 alunos (preferência) ou individual, deve ser entregue e defendido na última aula, durante o decorrer da aula.
As sugestões de trabalhos a desenvolver são:

### Tema escolhido:
Opção 2: Criar um programa que permita registar as notas dos alunos de uma instituição de ensino.
Deve haver uma estrutura de alunos e os dados devem ser guardados em ficheiros de texto. Cada aluno deve ter um ficheiro com as suas notas às diferentes disciplinas. O utilizador deve poder inserir alunos; visualizar alunos registados; lançar notas; remover alunos; etc…

---

O projeto que desenvolvemos tem uma constante de valor 5 que comanda o número de disciplinas que cada curso contém. A ideia partiu da criação de cursos com as respetivas disciplinas e depois ao criar alunos, permitir matricular alunos em determinados cursos, sendo as disciplinas que o compõem carregadas automáticamente para os dados do aluno.
O programa utiliza 3 estruturas:

**Disciplina{char nome[50]; float nota;}**

**Curso {char nome[50]; Disciplina cadeira[CADEIRAS];}**

**Aluno {Curso curso; char nome[100]; int numero;}**

A sequência lógica da programação aponta para a criação de cursos e respetivas disciplinas para que posteriormente possamos criar alunos e matriculá-los aos cursos já existentes.

O menú principal, oferece opções de:   
1. - Gestão de cursos   
	+ - Criar curso;   
	+ - Modificar curso;   
	+ - Eliminar curso.   
2. - Gerir alunos   
	+ - Criar aluno;   
	+ - Modificar aluno;   
	+ - Eliminar aluno;   
	+ - Matricular aluno;   
	+ - Desmatricular aluno.   
3. - Notas   
4. - Consultas   
 + - Consultar cursos;   
 + - Consultar alunos.   

 ---
Ao editar cursos, não é permitido mudar o nome para o nome de um curso já existente e depois de fazer essa validação, atualiza também o registo dos alunos que estão matriculados a esse curso.   
Ao eliminar curso, não permite eliminar nenhum curso que tenha pelo menos um aluno matriculado.   
Matricular aluno avisa para o facto do aluno já estar matriculado num curso e permite voltar atrás, caso contrário, perder-se-ão os dados. O mesmo acontece para desmatricular que depois de passar o aviso, os dados são perdidos.   
Ao lançar notas, só mostra os alunos matriculados porque só assim faz sentido.   
Consultar, apenas faz a listagem dos registos em ficheiro.