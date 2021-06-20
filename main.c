#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char nome[50];
    float nota;
} Disciplina;

typedef struct
{
    Disciplina cadeira[5];
    char nome[50];
} Curso;

typedef struct
{
    Curso curso;
    char nome[100];
    int numero;
} Aluno;

int main_menu();
int menu_cursos();
int menu_alunos();
int menu_consultas();
int menu_notas();
int mostra_cursos();
int mostra_alunos();
int modificar_curso();
int save_curso(Curso);
int find_aluno(Aluno);
int find_curso(Curso);
int inscrever_aluno(Aluno*);

void main(int argc, char* argv[])
{
    int op;
    do
    {
        op = main_menu();
        switch (op)
        {
        case 1:
            menu_cursos();
            break;
        case 2:
            menu_alunos();
            break;
        case 3:
            menu_notas();
            break;
        case 4:
            menu_consultas();
            break;
        case 0:
            puts("OVER AND OUT...");
            break;
        default:
            puts("OPÇÃO INVÁLIDA");
            break;
        }
    } while(op >= 1 && op <= 4);
}

int main_menu()
{
    int op;
    puts("SISTEMA DE REGISTO ACADÉMICO ISLA\n");
    puts("1 - GERIR CURSOS");
    puts("2 - GERIR ALUNOS");
    puts("3 - NOTAS");
    puts("4 - CONSULTAS");
    puts("0 - SAIR\n");
    scanf("%d", &op);
    return op;
}

//MENU DE CURSOS
int menu_cursos()
{
    int op, index, var, total_cursos;
    FILE *fin, *fout;
    Curso curso, aux_curso;
    Aluno aluno;
    do
    {
        puts("SISTEMA DE REGISTO ACADÉMICO ISLA\n");
        puts("GESTÃO DE CURSOS");
        puts("1 - CRIAR CURSO");
        puts("2 - MODIFICAR CURSO");
        puts("3 - ELIMINAR CURSO");
        puts("0 - VOLTAR");
        scanf("%d", &op);
        if(op>3 || op<0)
            puts("OPÇÃO INVÁLIDA");
    } while(op<0 || op>3);
    switch(op)
    {
        case 1:
            getchar();
            printf("Qual o nome do curso que quer criar: ");
            gets(curso.nome);
            // Verificar se o curso já existe com esse nome
            fin = fopen("cursos.bin", "a+b");
            if(fin == NULL)
            {
                puts("\nErro:\nImpossível abrir ficheiro de cursos");
                return 0;
            }
            rewind(fin);
            while(fread(&aux_curso, sizeof(aux_curso), 1, fin))
            {
                if(!strcmp(aux_curso.nome, curso.nome))
                {
                    printf("\nO curso %s já existe em sistema\n", curso.nome);
                    return 0;
                }
            }
            fclose(fin);
            for(index=0; index<5; index++)
            {
                printf("Qual o nome da %da disciplina: ", index+1);
                gets(curso.cadeira[index].nome);
            }
            save_curso(curso);
            break;
        case 2:
            if(!modificar_curso())
                return 0;
            return 1;
        case 3:
            total_cursos = mostra_cursos();
            printf("Qual o curso que pretende eliminar? ");
            scanf("%d", &var);
            if(var<1 && var>total_cursos)
            {
                puts("\nCurso não existente");
                return 0;
            }
            // Verificar se existem alunos matriculados ao curso que se pretende eliminar para impedir de apagar cursos frequentados (até ao próximo fecho dos ficheiros fin1 e fin2)
            fin = fopen("alunos.bin", "rb");
            fout = fopen("cursos.bin", "r+b");
            if(fout == NULL)
            {
                puts("\nErro:\nNão foi possível abrir o ficheiro de cursos");
                return 0;
            }
            index = 1;
            while(fread(&curso, sizeof(curso), 1, fout))
            {
                if(index == var)
                    break;
            }
            if(fin == NULL)
            {
                puts("\nErro:\nNão foi possível abrir o ficheiro de alunos");
                return 0;
            }
            while(fread(&aluno, sizeof(aluno), 1, fin))
            {
                if(!strcmp(aluno.curso.nome, curso.nome))
                {
                    printf("Não é possível eliminar o curso %s porque existe pelo menos um aluno matriculado\n", curso.nome);
                    return 0;
                }
            }
            fclose(fin);
            fclose(fout);
            fin = fopen("cursos.bin", "rb");
            fout = fopen("temp1.bin", "wb");
            index = 1;
            while(fread(&curso, sizeof(curso), 1, fin))
            {
                if(index != var)
                    fwrite(&curso, sizeof(curso), 1, fout);
                index++;
            }
            fclose(fin);
            fclose(fout);
            remove("cursos.bin");
            rename("temp1.bin", "cursos.bin");
            puts("CURSO ELIMINADO");
            return op;
        case 0:
            return 1;
    }
}

int modificar_curso()
{
    int total_cursos, num_curso, index;
    Curso curso, novo_curso;
    Aluno aluno;
    FILE *fcursos, *falunos;
    puts("\nMODIFICAR CURSO");
    total_cursos = mostra_cursos();
    printf("Qual o numero de curso que pretende modificar: ");
    scanf("%d", &num_curso);
    getchar();
    if(num_curso<1 || num_curso>total_cursos)
    {
        puts("CURSO INEXISTENTE");
        return 0;
    }
    fcursos = fopen("cursos.bin", "r+b");
    if(fcursos==NULL)
    {
        puts("ERRO NA ABERTURA DO FICHEIRO DE CURSOS");
        return 0;
    }
    while(fread(&curso, sizeof(curso), 1, fcursos))
    {
        index++;
        if(index!=num_curso)
            continue;
        break;
    }
    fseek(fcursos, -sizeof(curso), SEEK_CUR); // Posicionamo-nos no início do registo que vamos alterar
    printf("CURSO: %s\n", curso.nome);
    printf("NOVO NOME DE CURSO (ENTER PARA MANTER): ");
    gets(novo_curso.nome);
    for(index=0; index<5; index++)
    {
        printf("DISCIPLINA %d: %s\n", index+1, curso.cadeira[index].nome);
        printf("NOVO NOME DE DISCIPLINA (ENTER PARA MANTER): ");
        gets(novo_curso.cadeira[index].nome);
    }
    // Agora aqui vamos ao ficheiro de alunos à procura de alunos matriculados neste curso para alterarmos os nomes também no ficheiro alunos.bin
    falunos = fopen("alunos.bin", "r+b");
    if(falunos==NULL)
    {
        fclose(fcursos);
        puts("ERRO AO ABRIR O FICHEIRO DE ALUNOS");
        return 0;
    }
    while(fread(&aluno, sizeof(aluno), 1, falunos))
    {   // Se o aluno está matriculado neste curso
        if(!strcmp(curso.nome, aluno.curso.nome))
        {   // Se o curso mudou de nome, atualizamos o nome do curso no aluno
            if(strcmp(novo_curso.nome, "\0"))
                strcpy(aluno.curso.nome, novo_curso.nome);
            for(index=0; index<5; index++)
            {   // Se o nome da disciplina não foi alterada, passamos à próxima
                if(!strcmp(novo_curso.cadeira[index].nome, "\0"))
                    continue;
                strcpy(aluno.curso.cadeira[index].nome, novo_curso.cadeira[index].nome);
            }
            fseek(falunos, -(sizeof(aluno)), SEEK_CUR);
            fwrite(&aluno, sizeof(aluno), 1, falunos);
        }
        else
            continue;
    }
    fclose(falunos);
    if(strcmp(novo_curso.nome, "\0"))
        strcpy(curso.nome, novo_curso.nome);
    for(index=0; index<5; index++)
    {
        if(strcmp(novo_curso.cadeira[index].nome, "\0"))
            strcpy(curso.cadeira[index].nome, novo_curso.cadeira[index].nome);
    }
    fwrite(&curso, sizeof(curso), 1, fcursos);
    fclose(fcursos);
    return 1;
}

int menu_alunos()
{
    int op, index, found, num=1;
    char c, carater;
    Aluno aluno_novo;
    FILE *falunos;
    do
    {
        puts("SISTEMA DE REGISTO ACADÉMICO ISLA\n");
        puts("GESTÃO DE ALUNOS");
        puts("1 - CRIAR ALUNO");
        puts("2 - MODIFICAR ALUNO");
        puts("3 - ELIMINAR ALUNO");
        puts("4 - INSCREVER ALUNO");
        puts("0 - VOLTAR");
        scanf("%d", &op);
        if(op>3 && op<0)
            puts("OPÇÃO INVÁLIDA");
    } while(op<0 && op<4);
    switch (op)
    {
    case 1:
        printf("Qual o número do aluno: ");
        scanf("%d", &aluno_novo.numero);
        printf("Qual o nome do aluno: ");
        scanf(" %s", aluno_novo.nome);
        // Verificação de aluno existente em registo, ou com o mesmo número ou mesmo nome
        if(find_aluno(aluno_novo) == 1) // Função find_aluno devolve 1 se aluno já existe e 0 senão
            return 0;
        printf("Pretende inscrever já o aluno num curso? (s/n): ");
        scanf(" %c", &c);
        if(c=='s')
        {
            if(inscrever_aluno(&aluno_novo) == 0)
            {
                puts("\nErro:\nProcessamento de aluno invalido");
                return 0;
            }
        }
        puts("\nEstrutura do aluno:\n");
        printf("Numero: %d\tNome: %s\tCurso: %s\n", aluno_novo.numero, aluno_novo.nome, aluno_novo.curso.nome);
        printf("Disciplinas:\n");
        for(index=0; index<5; index++)
            printf("%s\n", aluno_novo.curso.cadeira[index].nome);
        puts("");
        printf("CONFIRMA DADOS PARA GRAVAR? (s/n): ");
        scanf(" %c", &carater);
        if(carater=='n')
        {
            puts("DADOS PERDIDOS");
            return 0;
        }
        falunos = fopen("alunos.bin", "a+b");
        if(falunos==NULL)
        {
            printf("NAO FOI POSSIVEL ABRIR O FICHEIRO \"alunos.bin\"\n");
            return 0;
        }
        fwrite(&aluno_novo, sizeof(aluno_novo), 1, falunos);
        fclose(falunos);
        return 1;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        break;
    }
}

int inscrever_aluno(Aluno* aluno)
{
    FILE *fcursos;
    int num, index;
    Curso current_curso;
    fcursos = fopen("cursos.bin", "rb");
    if(fcursos==NULL)
    {
        puts("\nNAO EXISTE FICHEIRO DE REGISTO DE CURSOS");
        return 0;
    }
    puts("\nCURSOS DISPONIVEIS:");
    num = 1;
    while(fread(&current_curso, sizeof(current_curso), 1, fcursos))
    {
        printf("%d - %s\t\tDisciplinas: ", num, current_curso.nome);
        for(index=0; index<5; index++)
        {
            printf("%s\t", current_curso.cadeira[index].nome);
        }
        printf("\n");
        num++;
    }
    printf("\nQual o num de curso que pretende matricular: ");
    scanf("%d", &num);
    // TODO: Fazer aqui a validação do número do curso introduzido
    rewind(fcursos);
    index = 0; //Variável de controlo para iterar sobre cursos no ficheiro
    while(fread(&current_curso, sizeof(current_curso), 1, fcursos))
    {
        index++;
        if(index == num)
        {
            strcpy(aluno->curso.nome, current_curso.nome);
            for(index=0; index<5; index++)
            {
                strcpy(aluno->curso.cadeira[index].nome, current_curso.cadeira[index].nome);
                aluno->curso.cadeira[index].nota = 0.0;
            }
            fclose(fcursos);
            return 1;
        }
    }
    fclose(fcursos);
    return 0;
}

int menu_consultas()
{
    int op, index;
    do
    {
        puts("CONSULTAS");
        puts("1 - CONSULTAR CURSOS");
        puts("2 - CONSULTAR ALUNOS");
        puts("0 - VOLTAR");
        scanf("%d", &op);
        if(op>2 && op<0)
            puts("OPÇÃO INVÁLIDA");
    }while(op<0 && op>2);
    switch(op)
    {
        case 1:
            mostra_cursos();
            break;
        case 2:
            mostra_alunos();
            break;
        case 0:
            return 1;
    }

}

int menu_notas()
{
    Aluno aluno;
    FILE *falunos;
    falunos = fopen("alunos.bin", "r+");
    int num_aluno, found=0;
    if(falunos == NULL)
    {
        puts("\nErro:\nNão foi possível abrir o ficheiro de alunos");
        return 0;
    }
    puts("\nLANÇAMENTO DE NOTAS");
    puts("\nALUNOS REGISTADOS:");
    while(fread(&aluno, sizeof(aluno), 1, falunos))
        printf("Numero: %d\tNome: %s\n", aluno.numero, aluno.nome);
    printf("Qual o número de aluno que quer lançar notas: ");
    scanf("%d", &num_aluno);
    rewind(falunos);
    while(fread(&aluno, sizeof(aluno), 1, falunos))
    {
        if(num_aluno == aluno.numero)
        {
            found = 1;
            break;
        }
    }
    if(found == 0)
    {
        printf("O número de aluno %d não se encontra no ficheiro de registo\n", num_aluno);
        return 0;
    }
    printf("ALUNO:\nNum: %d\tNome: %s\n", aluno.numero, aluno.nome);
    for(found=0; found<5; found++)
    {
        printf("\nDisciplina: %s\tNota: %.2f\t\tNova nota: ", aluno.curso.cadeira[found].nome, aluno.curso.cadeira[found].nota);
        scanf("%f", &aluno.curso.cadeira[found].nota);
    }
    fseek(falunos, sizeof(aluno), 1);
    fwrite(&aluno, sizeof(aluno), 1, falunos);
    fclose(falunos);
    return 1;
}

int mostra_cursos()
{
    int num=0, index;
    Curso curso;
    FILE *fcursos;
    fcursos = fopen("cursos.bin", "rb");
    if(fcursos==NULL)
    {
        printf("\nERRO A ABRIR O FICHEIRO \"cursos.bin\"\n");
        return 0;
    }
    puts("\nLISTA DE CURSOS EXISTENTES");
    while(fread(&curso, sizeof(curso), 1, fcursos))
    {
        num++;
        printf("%d - %s\t\tDisciplinas:\t", num, curso.nome);
        for(index=0; index<5; index++)
            printf("%s\t", curso.cadeira[index].nome);
        puts("");
    }
    puts("\n");
    fclose(fcursos);
    return num;
}

int mostra_alunos()
{
    FILE *falunos;
    Aluno aluno;
    int index;
    falunos = fopen("alunos.bin", "rb");
    if(falunos==NULL)
    {
        printf("Não foi possível abrir o ficheiro \"alunos.bin\"\n");
        return 0;
    }
    puts("ALUNOS EM FICHEIRO:");
    while(fread(&aluno, sizeof(aluno), 1, falunos))
    {
        printf("Nome: %s\tNum: %d\t Curso: %s\n", aluno.nome, aluno.numero, aluno.curso.nome);
        printf("Disciplinas:\n");
        for(index=0; index<5; index++)
            printf("%s\t%.2f\n", aluno.curso.cadeira[index].nome, aluno.curso.cadeira[index].nota);
        printf("\n");
    }
    return 1;
}

// Função para procurar existência de aluno em ficheiro. Se já existir aluno x com número y a função devolve 1, caso contrário devolve 0
int find_aluno(Aluno aluno)
{
    FILE *falunos;
    Aluno search_aluno;
    falunos = fopen("alunos.bin", "rb");
    if(falunos == NULL)
    {
        puts("Erro:\nNão é possível abrir o ficheiro de alunos");
        return 0;
    }
    while(fread(&search_aluno, sizeof(search_aluno), 1, falunos) == 1)
    {
        if(search_aluno.numero == aluno.numero || !strcmp(aluno.nome, search_aluno.nome))
        {
            printf("Já existe um aluno com o nome %s ou numero %d em ficheiro\n", aluno.nome, aluno.numero);
            return 1;
        }
    }
    printf("Não foi encontrado o aluno %s com o número %d\nPode prosseguir\n", aluno.nome, aluno.numero);
    return 0;
}

// Função para procurar existência de curso em ficheiro. Se já existir curso x, a função devolve 1, caso contrário devolve 0
int find_curso(Curso curso)
{
    FILE *fcursos;
    Curso search_curso;
    fcursos = fopen("cursos.bin", "rb");
    while(fread(&search_curso, sizeof(search_curso), 1, fcursos))
    {
        if(!strcmp(search_curso.nome, curso.nome))
        {
            printf("O curso %s já existe em ficheiro\n", curso.nome);
            return 1;
        }
    }
    printf("O curso %s ainda não está registado\n", curso.nome);
    return 0;
}

int save_curso(Curso novo_curso)
{
    FILE *fp;
    fp = fopen("cursos.bin", "a+b");
    if(fp == NULL)
    {
        puts("NAO FOI POSSIVEL ABRIR O FICHEIRO DE CURSOS");
        return 0;
    }
    fwrite(&novo_curso, sizeof(novo_curso), 1, fp);
    fclose(fp);
    return 1;
}