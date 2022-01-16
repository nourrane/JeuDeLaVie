#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#define R 50
#define t 8

SDL_Surface* ecran;
SDL_Surface * rectangle;
SDL_Event event;
long tick;
int gameRunning;

void printTab(int** T, int n, int m)
{
    /*
    Affiche un tableau 2D d'entier
    */
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
            printf("|  %d  ", T[i][j]);
		}
		printf("|\n");
	}
    printf("\n---------------------------\n");
}
int** callocTabInt(int n, int m)
{
    int** T = (int**)malloc(sizeof(int*) * n);
    if (T == NULL)
    {
        fprintf(stderr, "Erreur in memory allocation : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++)
    {
        T[i] = (int*)calloc(sizeof(int), m);
        if (T[i] == NULL)
        {
            fprintf(stderr, "Erreur in memory allocation : %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
    return T;
}

void replaceChar(char* S, char toReplace, char replacement, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (S[i] == toReplace) S[i] = replacement;
    }
}


void saveMap(int** T, int n, int m)
{
    char S[100];
    printf("Entrer le nom du fichier :\n");
    scanf("%s", S);
    int sizeStr = strlen(S) + 10;
    char * fileName = (char*) calloc(sizeStr, sizeof(char));
    sprintf(fileName, "save/%s", S);
    replaceChar(fileName, ':', '-', sizeStr);

    FILE * F = fopen(fileName, "w+");

    if (F == NULL)
    {
        fprintf(stderr, "Erreur in loading file : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }


    fprintf(F, "[%d][%d]", n, m);
    for (int i = 0; i < n; i++)
    {
        fprintf(F, "\n");
        for (int j = 0; j < m; j++)
        {
            fprintf(F, "{%d}", T[i][j]);
        }
    }

    free(fileName);
    fclose(F); 
    printf("Map save !\n");
}

int ** mooveBlock(int ** T, int n)
{
    int i,j;
    int ** New=callocTabInt(n,n);
    int voisin;
    for (i=1;i<n-1;i++)
    {
        for (j=1;j<n-1;j++)
        {
            voisin = T[i-1][j-1]+T[i-1][j]+T[i-1][j+1]+T[i][j-1]+T[i][j+1]+T[i+1][j-1]+T[i+1][j]+T[i+1][j+1];

            if (voisin==3)
            {
                New[i][j]=1;
            }
            else if (T[i][j]==1 && voisin<2){
                New[i][j]=0;
            }
            else {
                New[i][j]=T[i][j];
            }
        }
    }
    T=New;
    printTab(T,n,n);
    return T;
}
void get_input(int ** T, int n, int m)
{
    /*
    r�cup�re les diff�rents event dont les entr�s clavier 
    et la croix pour fermer
    */

    SDL_WaitEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
        printf("Voulez vous sauvegarder ? O/N\n");
        char c;
        scanf("%c", &c);
        if (c != 'o' && c!='n') scanf("%c", &c);
        if(c == 'O' || c == 'o' || c == '0')
            saveMap(T, n, m);
        gameRunning = 0;
        break;
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_SPACE:
            mooveBlock(T, n);
            break;
        case SDLK_s:
            saveMap(T, n, m);
            break;
        case SDLK_l:
            //freeTabInt(T, n, m);
            //T = loadMap(&n, &m);
            break;

        default:
            break;
        }
    default:
        break;
    }
}


void update(int** T, int n, int m)
{
    get_input(T, n, m);
    if (gameRunning == 0) { return; }
}

void draw(int** T, int n, int m)
{
    /*
    dessine les elemments sur la fenetre 
    */
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));

    rectangle=NULL;

	rectangle= SDL_CreateRGBSurface(SDL_HWSURFACE ,R,R,32,0,0,0,0);

	SDL_Rect position;
    int i;
	for (i=0;i<t;i++)
    {
		for (int j=0;j<t;j+=2)
        {	
				position.x=i*R;
				position.y=j*R;
				SDL_BlitSurface(rectangle,NULL,ecran,&position);
			}
        i++;
        for (int j=1;j<t;j+=2)
        {	
				position.x=i*R;
				position.y=j*R;
				SDL_BlitSurface(rectangle,NULL,ecran,&position);
		}
	}
	
		
    SDL_Flip(ecran);
}

void freeTabInt(int** T, int nbrLine, int nbrColumn)
{
    /*
    Lib�re un tableau 2D d'entier
    */
	for (int i = 0; i < nbrLine; i++)
	{
		free(T[i]);
	}
	free(T);
}

void unload(int ** T, int n, int m)
{
    /*
    Lib�re toute la m�moire � la fin du code.
    */
    SDL_FreeSurface(ecran);
    freeTabInt(T, n, m);
}


int ** loadMap(int* n, int* m)
{
    char S[100];
    printf("Entrer le nom du fichier :\n");
    scanf("%s", S);
    char* fileName = (char*)calloc(strlen(S)+10, sizeof(char));
    sprintf(fileName, "save/%s", S);
    FILE* F = fopen(fileName, "r");

    if (F == NULL)
    {
        fprintf(stderr, "Erreur in loading file : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    fscanf(F, "[%d][%d]", n, m);
    printf("%d, %d\n", *n, *m);

    int ** T = callocTabInt(*n, *m);
    for (int i = 0; i < *n; i++)
    {
        fscanf(F, "\n{%d}", &T[i][0]);
        for (int j = 1; j < *m; j++)
        {
            fscanf(F, "{%d}", &T[i][j]);
        }
    }
    
    fclose(F);
    free(fileName);

    printf("Map load!\n");
    printTab(T, *n, *m);
    printf("---------------------------\n\n\n");
    return T;
}
int ** load(int n) 
{
    /*
    Initialise la fen�tre la boucle de jeu et la table de jeu
    � la taille donn�.
    */
    printf("initializing the game\n");
    tick = 0;
    gameRunning = 1;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetCaption(__TIME__, NULL);

    ecran = NULL;

    if ((ecran = SDL_SetVideoMode(R*t, R*t, 32, SDL_HWSURFACE)) == NULL)
    {
        fprintf(stderr, "Erreur VideoMode %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    printf("n: commencer une nouvelle partie \nc: continuer une partie enregistrée\n");
    int** T;
    char ch;
    scanf("%c", &ch);

    if (ch=='n' || ch=='N')
    {
    T = callocTabInt(n, n);  
    }
    else{

        T=loadMap(&n,&n);
    }

    /*
    generateNewBlock(T, n, m, int((n + 0.5) / 2));
    printf("Point = %d\n", point);
    printTab(T, n, m);
    printf("---------------------------\n\n\n");
    */
    return T;
}

void logic()
{
    /*
    boucle de jeu 
    */
    int n = t;
    int m = n;
    int ** T = load(n);
    while (gameRunning)
    {   
        tick++;
        update(T, n, m);
        draw(T, n, m);
    }
    printf("Unload\n");
    unload(T, n, m);
}

int main() { 

    logic(R);
 
	return 0; 
} 