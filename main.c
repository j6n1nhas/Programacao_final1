#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <sqlite3.h>

#define CADEIRAS 5

typedef struct
{
    char nome[50];
    float nota;
} Disciplina;

typedef struct
{
    Disciplina cadeira[CADEIRAS];
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
int criar_curso();
int modificar_curso();
int eliminar_curso();
int menu_alunos();
int criar_aluno();
int modificar_aluno();
int eliminar_aluno();
int menu_consultas();
int menu_notas();
int mostra_cursos();
int mostra_alunos();
int save_curso(Curso);
int find_aluno(Aluno);
int find_curso(Curso);
int matricular_aluno(Aluno*);
int desmatricular_aluno();


int main(int argc, char* argv[])
{
    int op;
    setlocale(LC_ALL, "pt_PT.UTF-8");
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
    printf("%s\t%s\n", __TIME__, __DATE__);
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
    int op;
    do
    {
        puts("SISTEMA DE REGISTO ACADÉMICO ISLA\n");
        puts("GESTÃO DE CURSOS");
        puts("1 - CRIAR CURSO");
        puts("2 - MODIFICAR CURSO");
        puts("3 - ELIMINAR CURSO");
        puts("0 - VOLTAR");
        scanf("%d", &op);
        getchar();
        if(op>3 || op<0)
            puts("OPÇÃO INVÁLIDA");
    } while(op<0 || op>3);
    switch(op)
    {
        case 1:
            if(!criar_curso())
            {
                puts("NAO FOI POSSIVEL CRIAR CURSO COM SUCESSO");
                return 0;
            }
            return 1;
        case 2:
            if(!modificar_curso())
            {
                puts("NAO FOI POSSIVEL EDITAR CURSO COM SUCESSO");
                return 0;
            }
            return 1;
        case 3:
            if(!eliminar_curso())
            {
                puts("NÃO FOI POSSIVEL ELIMINAR CURSO COM SUCESSO");
                return 0;
            }
            return 1;
        case 0:
            return 1;
    }
}

int criar_curso()
{
    Curso curso;
    FILE *fcursos;
    int index;
    printf("Qual o nome do curso que quer criar: ");
    gets(curso.nome);
    // Verificar se o curso já existe com esse nome
    if(!find_curso(curso))
    {
        for(index=0; index<CADEIRAS; index++)
        {
            printf("Qual o nome da %da disciplina: ", index+1);
            gets(curso.cadeira[index].nome);
        }
        save_curso(curso);
        return 1;
    }
    return 0;
}

int modificar_curso()
{
    int total_cursos, num_curso, index;
    Curso curso, novo_curso;
    Aluno aluno;
    FILE *fcursos, *falunos;
    puts("\nMODIFICAR CURSO");
    total_cursos = mostra_cursos();
    if(!total_cursos)
        return 0;
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
    fseek(fcursos, -(sizeof(curso)), SEEK_CUR); // Posicionamo-nos no início do registo que vamos alterar
    printf("CURSO: %s\n", curso.nome);
    printf("NOVO NOME DE CURSO (ENTER PARA MANTER): ");
    gets(novo_curso.nome);
    for(index=0; index<CADEIRAS; index++)
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
            for(index=0; index<CADEIRAS; index++)
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
    // Se o nome de curso foi alterado, temos de o atualizar
    if(strcmp(novo_curso.nome, "\0"))
        strcpy(curso.nome, novo_curso.nome);
    for(index=0; index<CADEIRAS; index++)
    {   // O mesmo para as disciplinas
        if(strcmp(novo_curso.cadeira[index].nome, "\0"))
            strcpy(curso.cadeira[index].nome, novo_curso.cadeira[index].nome);
    }
    // Gravar e fechar o ficheiro
    fwrite(&curso, sizeof(curso), 1, fcursos);
    fclose(fcursos);
    return 1;
}

int eliminar_curso()
{
    int num_curso, index, total_cursos = mostra_cursos();
    FILE *fin, *fout;
    Curso curso;
    Aluno aluno;
    if(!total_cursos)
        return 0;
    printf("Qual o curso que pretende eliminar? ");
    scanf("%d", &num_curso);
    if(num_curso<1 || num_curso>total_cursos)
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
        if(index == num_curso)
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
        if(index != num_curso)
            fwrite(&curso, sizeof(curso), 1, fout);
        index++;
    }
    fclose(fin);
    fclose(fout);
    remove("cursos.bin");
    rename("temp1.bin", "cursos.bin");
    puts("CURSO ELIMINADO");
    return 1;
}

//MENU ALUNOS
int menu_alunos()
{
    int op, total_alunos, num_aluno;
    Aluno aluno_novo;
    char c;
    FILE *falunos;
    do
    {
        puts("SISTEMA DE REGISTO ACADÉMICO ISLA\n");
        puts("GESTÃO DE ALUNOS");
        puts("1 - CRIAR ALUNO");
        puts("2 - MODIFICAR ALUNO");
        puts("3 - ELIMINAR ALUNO");
        puts("4 - MATRICULAR ALUNO");
        puts("5 - DESMATRICULAR ALUNO");
        puts("0 - VOLTAR");
        scanf("%d", &op);
        if(op>3 && op<0)
            puts("OPÇÃO INVÁLIDA");
    } while(op<0 && op<4);
    switch (op)
    {
    case 1:
        if(!criar_aluno())
        {
            puts("NAO FOI POSSIVEL CRIAR ALUNO COM SUCESSO");
            return 0;
        }
        return 1;
    case 2:
        if(!modificar_aluno())
        {
            puts("NAO FOI POSSIVEL EDITAR ALUNO COM SUCESSO");
            return 0;
        }
        return 1;
    case 3:
        if(!eliminar_aluno())
        {
            puts("NAO FOI POSSIVEL ELIMINAR ALUNO COM SUCESSO");
            return 0;
        }
        return 1;
    case 4:
        total_alunos = mostra_alunos();
        printf("Aluno numero a matricular: ");
        scanf("%d", &num_aluno);
        if(num_aluno<1 || num_aluno>total_alunos)
        {
            puts("ALUNO NAO EXISTENTE");
            return 0;
        }
        falunos = fopen("alunos.bin", "r+b");
        if(falunos == NULL)
        {
            puts("ERRO A ABRIR O FICHEIRO DE ALUNOS");
            return 0;
        }
        fseek(falunos, sizeof(aluno_novo)*(num_aluno-1), SEEK_SET);
        fread(&aluno_novo, sizeof(aluno_novo), 1, falunos);
        // Se o aluno já estiver inscrito num curso, perguntar se quer alterar o curso neste aluno
        if(strlen(aluno_novo.curso.nome))
        {
            printf("O ALUNO %s JA ESTA INSCRITO NO CURSO %s. PRETENDE MATRICULAR O ALUNO NOUTRO CURSO? (s/n): ", aluno_novo.nome, aluno_novo.curso.nome);
            scanf(" %c", &c);
            if(c=='n')
            {
                puts("OPERACAO ABORTADA");
                return 0;
            }
        }
        if(matricular_aluno(&aluno_novo))
        {
            fseek(falunos, -(sizeof(aluno_novo)), SEEK_CUR);
            fwrite(&aluno_novo, sizeof(aluno_novo), 1, falunos);
            fclose(falunos);
            puts("ALUNO MATRICULADO COM SUCESSO");
            return 1;
        }
        return 0;
    case 5:
        if(!desmatricular_aluno())
        {
            puts("NAO FOI POSSIVEL DESMATRICULAR ALUNO COM SUCESSO");
            return 0;
        }
        return 1;
    }
}

int criar_aluno()
{
    Aluno aluno_novo;
    char c;
    int index;
    FILE *falunos;
    printf("Qual o número do aluno: ");
    scanf("%d", &aluno_novo.numero);
    getchar();
    printf("Qual o nome do aluno: ");
    gets(aluno_novo.nome);
    // Verificação de aluno existente em registo, ou com o mesmo número ou mesmo nome
    if(find_aluno(aluno_novo) == 1) // Função find_aluno devolve 1 se aluno já existe e 0 senão
    {
        printf("JA EXISTE UM ALUNO COM O NUMERO %d OU NOME %s\n", aluno_novo.numero, aluno_novo.nome);
        return 0;
    }
    printf("Pretende inscrever já o aluno num curso? (s/n): ");
    scanf(" %c", &c);
    if(c=='s')
    {
        if(matricular_aluno(&aluno_novo) == 0)
        {
            puts("\nErro:\nProcessamento de aluno invalido");
            return 0;
        }
        puts("\nEstrutura do aluno:");
        printf("Numero: %d\tNome: %s\tCurso: %s\n", aluno_novo.numero, aluno_novo.nome, aluno_novo.curso.nome);
        printf("Disciplinas:\n");
        for(index=0; index<CADEIRAS; index++)
            printf("%s\n", aluno_novo.curso.cadeira[index].nome);
    }
    else
    {
        strcpy(aluno_novo.curso.nome, "\0");
        for(index=0; index<CADEIRAS; index++)
        {
            strcpy(aluno_novo.curso.cadeira[index].nome, "\0");
            aluno_novo.curso.cadeira->nota = 0.0;
        }
        puts("\nEstrutura do aluno:");
        printf("Numero: %d\tNome: %s\n", aluno_novo.numero, aluno_novo.nome);
    }
    puts("");
    printf("CONFIRMA DADOS PARA GRAVAR? (s/n): ");
    scanf(" %c", &c);
    if(c=='n')
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
}

int modificar_aluno()
{
    int index, total_alunos, num_aluno;
    Aluno aluno, novo_aluno;
    FILE *falunos;
    puts("\nMODIFICAR ALUNO");
    total_alunos = mostra_alunos();
    if(!total_alunos)
    {
        puts("NAO HA ALUNOS EM SISTEMA");
        return 0;
    }
    printf("Qual o numero de aluno que pretende modificar: ");
    scanf("%d", &num_aluno);
    getchar();
    if(num_aluno<1 || num_aluno>total_alunos)
    {
        puts("ALUNO INEXISTENTE");
        return 0;
    }
    falunos = fopen("alunos.bin", "r+b");
    if(falunos==NULL)
    {
        puts("ERRO NA ABERTURA DO FICHEIRO DE ALUNOS");
        return 0;
    }
    //percorrer o ficheiro de alunos e parar no index do aluno que queremos
    index = 0;
    while(fread(&aluno, sizeof(aluno), 1, falunos))
    {
        index++;
        if(index!=num_aluno)
            continue;
        break;
    }
    printf("ALUNO: %s\n", aluno.nome);
    printf("NOVO NOME DE ALUNO (ENTER PARA MANTER): ");
    gets(novo_aluno.nome);
    printf("Numero de aluno: %d\n", aluno.numero);
    printf("NOVO NUMERO DE ALUNO (ZERO PARA MANTER): ");
    scanf("%d", &novo_aluno.numero);

    // Se o nome de aluno foi alterado, temos de o atualizar
    if(strcmp(novo_aluno.nome, "\0"))
    {   // Vamos verificar se já existe algum aluno com o mesmo nome
        rewind(falunos);
        index = 0;
        while(fread(&aluno, sizeof(aluno), 1, falunos))
        {
            index++;
            if(index==num_aluno)
                continue;
            if(!strcmp(aluno.nome, novo_aluno.nome))
            {
                printf("Ja existe um aluno com o mesmo nome: \"%s\"\nOPERACAO ABORTADA\n", novo_aluno.nome);
                fclose(falunos);
                return 0;
            }
        }
    }
    // Se o numero de aluno foi alterado, temos de o atualizar
    if(novo_aluno.numero != 0)
    {   // Vamos verificar se já existe algum aluno com o mesmo número
        rewind(falunos);
        index = 0;
        while(fread(&aluno, sizeof(aluno), 1, falunos))
        {
            index++;
            if(index==num_aluno)
                continue;
            if(aluno.numero == novo_aluno.numero)
            {
                printf("Ja existe um aluno com o mesmo numero: %d\nOPERACAO ABORTADA\n", aluno.numero);
                fclose(falunos);
                return 0;
            }
        }
    }
    // Colocamo-nos no início da estrutura aluno a alterar
    fseek(falunos, sizeof(aluno)*(num_aluno-1), SEEK_SET);
    // Lemos os dados para a estrutura aluno
    fread(&aluno, sizeof(aluno), 1, falunos);
    // Fazemos as alterações
    if(strcmp(novo_aluno.nome, "\0"))
        strcpy(aluno.nome, novo_aluno.nome);
    if(novo_aluno.numero != 0)
        aluno.numero = novo_aluno.numero;
    // Colocamo-nos novamente no início da estrutura
    fseek(falunos, -(sizeof(aluno)), SEEK_CUR);
    // Escrevemos os novos dados
    fwrite(&aluno, sizeof(aluno), 1, falunos);
    // Fechamos o ficheiro
    fclose(falunos);
    return 1;
}

int eliminar_aluno()
{
    int num_aluno, index, total_alunos = mostra_alunos();
    FILE *ficheiro1, *ficheiro2;
    Aluno aluno_remover;
    char c='s';
    ficheiro1 = fopen("alunos.bin", "rb");
    ficheiro2 = fopen("temp.bin", "wb");
    if(!total_alunos)
        return 0;
    if(ficheiro1 == NULL)
    {
        puts("\nErro:\nNão foi possível abrir o ficheiro de alunos");
        return 0;
    }
    printf("Qual o aluno a apagar: ");
    scanf("%d", &num_aluno);
    if(num_aluno<1 || num_aluno>total_alunos)
    {
        puts("ALUNO INEXISTENTE");
        return 0;
    }
    index = 0;
    while(fread(&aluno_remover, sizeof(aluno_remover), 1, ficheiro1))
    {
        index++;
        //apenas escreve no ficheiro temporario, os alunos que sao diferentes do id inserido
        //na pratica é uma remoçao do id que o user deu
        if(index != num_aluno)
            fwrite(&aluno_remover, sizeof(aluno_remover), 1, ficheiro2);
        else
        {
            if(strlen(aluno_remover.curso.nome))
            {
                printf("O ALUNO %s ESTA MATRICULADO NO CURSO %s. PRENTENDE ELIMINAR TODOS OS DADOS REFERENTES A ESTE ALUNO (s/n)? ", aluno_remover.nome, aluno_remover.curso.nome);
                scanf(" %c", &c);
                if(c=='s')
                    continue;
                fwrite(&aluno_remover, sizeof(aluno_remover), 1, ficheiro2);
            }
        }        
    }
    fclose(ficheiro1);
    fclose(ficheiro2);
    //remove o ficheiro antigo de alunos
    remove("alunos.bin");
    // Muda o nome do ficheiro para alunos.bin
    rename("temp.bin", "alunos.bin");
    if(c=='s')
    {
        puts("ALUNO ELIMINADO");
        return 1;
    }
    return 0;
}

int matricular_aluno(Aluno* aluno)
{
    FILE *fcursos;
    int num, index, num_curso;
    Curso current_curso;
    fcursos = fopen("cursos.bin", "rb");
    if(fcursos==NULL)
    {
        puts("\nERRO A ABRIR O FICHEIRO DE CURSOS");
        return 0;
    }
    num = mostra_cursos();
    printf("\nQual o num de curso que pretende matricular: ");
    scanf("%d", &num_curso);
    // TODO: Fazer aqui a validação do número do curso introduzido
    if(num_curso<1 || num_curso>num)
    {
        puts("CURSO INEXISTENTE");
        return 0;
    }
    index = 0; //Variável de controlo para iterar sobre cursos no ficheiro
    while(fread(&current_curso, sizeof(current_curso), 1, fcursos))
    {
        index++;
        if(index == num_curso)
        {
            strcpy(aluno->curso.nome, current_curso.nome);
            for(index=0; index<CADEIRAS; index++)
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

int desmatricular_aluno()
{
    FILE *falunos;
    int index, total_alunos, num_aluno;
    Aluno aluno;
    puts("\nDESMATRICULAR ALUNO\n");
    total_alunos = mostra_alunos();
    falunos = fopen("alunos.bin", "r+b");
    if(falunos == NULL)
    {
        puts("ERRO AO ABRIR O FICHEIRO DE ALUNOS");
        return 0;
    }
    if(total_alunos==0)
        return 0;
    printf("Qual o aluno numero que pretende desmatricular: ");
    scanf("%d", &num_aluno);
    if(num_aluno<0 || num_aluno>total_alunos)
    {
        puts("ALUNO INEXISTENTE");
        return 0;
    }
    index = 0;
    while(fread(&aluno, sizeof(aluno), 1, falunos))
    {
        index++;
        if(index!=num_aluno)
            continue;
        if(!strlen(aluno.curso.nome))
        {
            printf("O ALUNO %s NAO SE ENCONTRA MATRICULADO A NENHUM CURSO\n", aluno.nome);
            fclose(falunos);
            return 0;
        }
        break;
    }
    fseek(falunos, -(sizeof(aluno)), SEEK_CUR);
    strcpy(aluno.curso.nome, "");
    for(index=0; index<CADEIRAS; index++)
    {
        strcpy(aluno.curso.cadeira[index].nome, "");
        aluno.curso.cadeira[index].nota = 0.0;
    }
    fwrite(&aluno, sizeof(aluno), 1, falunos);
    fclose(falunos);
    puts("ALUNO DESMATRICULADO COM SUCESSO");
    return 1;
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

//MENU DE LANÇAMENTO DE NOTAS
int menu_notas()
{
    Aluno aluno;
    FILE *falunos;
    falunos = fopen("alunos.bin", "r+b");
    int num_aluno, index1, index2, found;
    if(falunos == NULL)
    {
        puts("\nErro:\nNão foi possível abrir o ficheiro de alunos");
        return 0;
    }
    puts("\nLANÇAMENTO DE NOTAS");
    puts("ALUNOS MATRICULADOS:");
    index1 = 0; // index1 vai dar o número de alunos matriculados
    while(fread(&aluno, sizeof(aluno), 1, falunos))
    {   // Só mostra os alunos matriculados verificando se o nome do curso do aluno não está vazio
        if(strlen(aluno.curso.nome))
        {
            index1++;
            printf("%d - %s\n", index1, aluno.nome);
        }
    }
    if(index1 < 1)
    {
        puts("NAO HA ALUNOS MATRICULADOS");
        return 0;
    }
    printf("Qual o número de aluno que quer lançar notas: ");
    scanf("%d", &num_aluno);
    if(num_aluno<1 || num_aluno > index1)
    {
        puts("ALUNO INEXISTENTE");
        return 0;
    }
    rewind(falunos);
    index2 = 0;
    while(fread(&aluno, sizeof(aluno), 1, falunos))
    {
        if(strlen(aluno.curso.nome))
        {
            index2++;
            if(index1==index2)
            {
                found = 1;
                break;
            }
        }
    }
    if(found==1)
    {
        printf("ALUNO:\nNum: %d\tNome: %s\n", aluno.numero, aluno.nome);
        for(index1=0; index1<CADEIRAS; index1++)
        {
            printf("\nDisciplina: %s\tNota: %.2f\t\tNova nota: ", aluno.curso.cadeira[index1].nome, aluno.curso.cadeira[index1].nota);
            scanf("%f", &aluno.curso.cadeira[index1].nota);
        }
        fseek(falunos, -(sizeof(aluno)), SEEK_CUR);
        fwrite(&aluno, sizeof(aluno), 1, falunos);
        fclose(falunos);
        return 1;
    }
    return 0;
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
        for(index=0; index<CADEIRAS; index++)
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
    int index, num=0;
    falunos = fopen("alunos.bin", "rb");
    if(falunos==NULL)
    {
        printf("Não foi possível abrir o ficheiro \"alunos.bin\"\n");
        return 0;
    }
    puts("ALUNOS EM SISTEMA:");
    while(fread(&aluno, sizeof(aluno), 1, falunos))
    {
        num++;
        printf("\nALUNO %d:\n", num);
        printf("Nome: %s\tNum: %d\t Curso: %s\n", aluno.nome, aluno.numero, aluno.curso.nome);
        printf("Disciplina\tNota\n");
        for(index=0; index<CADEIRAS; index++)
            printf("%s\t%.2f\n", aluno.curso.cadeira[index].nome, aluno.curso.cadeira[index].nota);
        printf("\n");
    }
    if(num==0)
        puts("NAO HA ALUNOS GUARDADOS");
    return num;
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
    while(fread(&search_aluno, sizeof(search_aluno), 1, falunos))
    {
        if(search_aluno.numero == aluno.numero || !strcmp(aluno.nome, search_aluno.nome))
        {
            fclose(falunos);
            return 1;
        }
    }
    return 0;
}

// Função para procurar existência de curso em ficheiro. Se já existir curso x, a função devolve 1, caso contrário devolve 0
int find_curso(Curso curso)
{
    FILE *fcursos;
    Curso search_curso;
    fcursos = fopen("cursos.bin", "rb");
    if(fcursos == NULL)
    {
        puts("Não é possível abrir o ficheiro de cursos");
        return 0;
    }
    while(fread(&search_curso, sizeof(search_curso), 1, fcursos))
    {
        if(!strcmp(search_curso.nome, curso.nome))
        {
            fclose(fcursos);
            return 1;
        }
    }
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
