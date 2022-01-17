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
    int ** N=callocTabInt(n,n);
    for (int i =0; i<n; i++){
        for (int j=0;j<n;j++){
            N[i][j]=T[i][j];
        }
    }
    int voisin;
    for (i=1;i<n-1;i++)
    {
        for (j=1;j<n-1;j++)
        {
            voisin = N[i-1][j-1]+N[i-1][j]+N[i-1][j+1]+N[i][j-1]+N[i][j+1]+N[i+1][j-1]+N[i+1][j]+N[i+1][j+1];

            if (voisin==3 && N[i][j]==0)
            {
                T[i][j]=1;
            }
            else if (N[i][j]==1 && (voisin<2 || voisin>3))
            {
                T[i][j]=0;
            }
        }
    }

    freeTabInt(N,n,n);
    return T;
}

void get_input(int ** T, int n, int m)
{
    /*
    r�cup�re les diff�rents event dont les entr�s clavier 
    et la croix pour fermer
    */
    int cont=1;
    while(cont){   
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
            cont=0;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_SPACE:
                T=mooveBlock(T, n);
                cont=0;
                break;
            case SDLK_s:
                saveMap(T, n, m);
                cont=0;
                break;
            default:
                break;
            }
        default:
            break;
        }
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
	for (i=0;i<n;i++)
    {
		for (int j=0;j<m;j++)
        {	
            position.y=i*R; 
			position.x=j*R;
            if (T[i][j]==1)
            {
				SDL_BlitSurface(rectangle,NULL,ecran,&position);
            }
		}
       
	}
	
		
    SDL_Flip(ecran);
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
    printf("fichiers disponibles: 1) clignotant\v 2) ruche\v 3) planneur\n");
    printf("Entrer le numéros du fichier :\n");
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

void CreatMap(int **T,int n)
{
    /*
   CREATION DE MAP SUR L'INTERFACE GRAPHIQUE
    */
    draw(T,n,n);
    int cont=1;
    int x,y;
    while(cont){   
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            printf("Voulez vous sauvegarder ? O/N\n");
            char c;
            scanf("%c", &c);
            if (c != 'o' && c!='n') scanf("%c", &c);
            if(c == 'O' || c == 'o' || c == '0')
                saveMap(T, n, n);
            gameRunning = 0;
            cont=0;
            break;
        case SDL_MOUSEBUTTONDOWN: // Click de souris 
            
            y=(event.button.x)/R;
            x=(event.button.y)/R;
            T[x][y]=1-T[x][y];
            draw(T,n,n);
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) 
            {
            case SDLK_RETURN:
                cont=0;
                printf("voulez vous sauvegarder cette map pour plus tard? O/N\n");
                char ch;
                do
                {
                    scanf("%c",&ch);
                } 
                while(ch!='o'&& ch!='O'&& ch!='0' && ch!='n' && ch!='N');

                if (ch=='o'|| ch=='O'||ch=='0')
                    saveMap(T,n,n);

                break;
            default:
                break;
            }
        default:
            break;
        }
    }
}

void createcran(int n)
{
    tick = 0;
    gameRunning = 1;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_WM_SetCaption(__TIME__, NULL);

    ecran = NULL;

    if ((ecran = SDL_SetVideoMode(n*R, n*R, 32, SDL_HWSURFACE)) == NULL)
    {
        fprintf(stderr, "Erreur VideoMode %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));
}

int ** load(int * n) 
{

    printf("n: commencer une nouvelle partie \nc: continuer une partie enregistrée\n");
    int** T;
    char ch;
    scanf("%c", &ch);

    if (ch=='n' || ch=='N')
    {

    printf("Quel taille de fenêtre souhaitez vous? (pour 5 pixels par exemple, tapez 5 )\n");
    scanf("%d",n);
    printf("Cliquer sur la surface pour créer votre figure, puis tapez échape lorqu'elle est finie\n");

    T = callocTabInt(*n, *n); 
    createcran(*n);
    CreatMap(T,*n);
    draw(T,*n,*n);
    }
    else
    { 
        T=loadMap(n,n);
        createcran(t);
        draw(T,t,t);
    }
    /*
    Initialise la fen�tre la boucle de jeu et la table de jeu
    � la taille donn�.
    */
    
    draw(T,*n,*n);
    return T;
}

void logic()
{
    /*
    boucle de jeu 
    */
    int n;
    int ** T = load(&n);
    printf("Cliquez sur la touche espace pour faire évoluer votre jeu\n");
    while (gameRunning)
    {   
        tick++;
        draw(T, n, n);
        update(T, n, n);
        printTab(T,n,n);
        

    }
    printf("Unload\n");
    unload(T, n, n);
}

int main() { 

    logic();
 
	return 0; 
} 