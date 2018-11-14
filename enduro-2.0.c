// Projeto do jogo Enduro - Atari  // gcc enduro.c -lm -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -o enduro

//Bibliotecas necessárias 
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <time.h>

// Definir o número de carros adversários
#define NUMCARS 10

// Constantes como FPS, largura, altura da tela,e variáveis do carro, das rodas, dos pontos do cenário, de theta
const float FPS = 60;
const int LARGURA_TELA = 1000;
const int ALTURA_TELA = 640;

int ALTURA_CEU;

float LARGURA_CARRO = 100;
float ALTURA_CARRO = 20;
float LARGURA_RODA = 30;
float ALTURA_RODA = 40;

// Largura superior e inferior da pista
float LARGURA_PISTA_SUP;
float LARGURA_PISTA_INF;

// Pontos restantes da pista = Lado Esquerdo
float PISTA_SUP_LEFT_X;
float PISTA_SUP_LEFT_Y;
float PISTA_INF_LEFT_X;
float PISTA_INF_LEFT_Y;

// Pontos restantes da pista = Lado Direito
float PISTA_SUP_RIGHT_X;
float PISTA_SUP_RIGHT_Y;
float PISTA_INF_RIGHT_X;
float PISTA_INF_RIGHT_Y;

// Theta para ajudar nas contas
float THETA;

//
float delta_dx, delta_speedy;

// Variável para guardar pontuação
int score = 0;

// Criando uma estrutura Car para guardar todas as variáveis necessárias dele
typedef struct {
	// Coordenadas
	float x, y, roda1_x, roda1_y, roda2_x, roda2_y; //x centro, y centro
	float xoffset; //distancia do x para a margem esquerda da pista (ajuda em alguns calculos)
	//
	float delta_speedy;
	int accel, breaking, right, left;
	int status;
	int id;
	int delay;
	ALLEGRO_COLOR cor;
} Car;

// Criando procedimento para iniciar as variáveis na função principal
void inicia_variaveis_globais() {
	
	ALTURA_CEU = ALTURA_TELA / 4;                                     // 160 

	LARGURA_PISTA_SUP = LARGURA_TELA / 20;                            // 50
	LARGURA_PISTA_INF = LARGURA_TELA / 1.2;                           // 833.3

	PISTA_SUP_LEFT_X = LARGURA_TELA / 2 - LARGURA_PISTA_SUP / 2;      // x1 = 487.5
	PISTA_SUP_LEFT_Y = ALTURA_CEU;                                    // y1 = 160
	PISTA_INF_LEFT_X = LARGURA_TELA / 2 - LARGURA_PISTA_INF / 2;      // X3 = 83.35
	PISTA_INF_LEFT_Y = ALTURA_TELA;                                   // y3 = 640

	PISTA_SUP_RIGHT_X = LARGURA_TELA / 2 + LARGURA_PISTA_SUP / 2;     // x2 = 512.5
	PISTA_SUP_RIGHT_Y = ALTURA_CEU;                                   // y2 = 160
	PISTA_INF_RIGHT_X = LARGURA_TELA / 2 + LARGURA_PISTA_INF / 2;     // x4 = 916.65
	PISTA_INF_RIGHT_Y = ALTURA_TELA;                                  // y4 = 640

	// Ângulo da pista -> atan = arcotangente
	THETA = atan((PISTA_INF_LEFT_Y - PISTA_SUP_LEFT_Y) / (PISTA_SUP_LEFT_X - PISTA_INF_LEFT_X));//480/411.65 = 1.16

	// Variações no eixo x e y da tela
	delta_dx = LARGURA_TELA / 200;             // 5
}

// Definindo um vetor para guardar as cores    
ALLEGRO_COLOR cor_aleatoria[NUMCARS];// = al_map_rgb(100 + rand()%155,100 + rand()%155,100 + rand()%155);

// Procedimento para desenhar o cenário na função principal
void desenha_cenario(ALLEGRO_DISPLAY *display) {

	// Desenha e define a cor da grama = cor da grama enduro original
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(0,66,0);
	al_set_target_bitmap(al_get_backbuffer(display));
	al_clear_to_color(BKG_COLOR);

	// Desenha e colore o retângulo do céu = dimensões do retângulo = cor do céu enduro original
	al_draw_filled_rectangle(0,0, LARGURA_TELA, ALTURA_CEU, al_map_rgb(24,24,165));

	// Desenhando as bordas da pista na cor Branca
	// coordenadas dos dois pontos(x1,y1), (x3,y3) = Borda esquerda
	al_draw_line(PISTA_SUP_LEFT_X, PISTA_SUP_LEFT_Y, PISTA_INF_LEFT_X, PISTA_INF_LEFT_Y, al_map_rgb(255,255,255), 7);
	// coordenadas dos dois pontos (x2,y2), (x4,y4) = Borda direita
	al_draw_line(PISTA_SUP_RIGHT_X, PISTA_SUP_RIGHT_Y, PISTA_INF_RIGHT_X, PISTA_INF_RIGHT_Y, al_map_rgb(255,255,255), 7);

}

// Função para fazer a proporção com o y', que é cada valor da descida da pista
float get_ratio(float y){
	float tam, ratio;

	tam = ((y - ALTURA_CEU) / THETA) * 2;
	ratio = tam / LARGURA_PISTA_INF;

	return ratio; 
}

// Função preenche o Carro Principal
void preenche_mycar(Car *meu_carro){
	// Configurando as coordenadas do meu carro principal pela estrutura Car
	// Meu Carro
	meu_carro-> x = LARGURA_TELA / 2 - LARGURA_CARRO / 2;
	meu_carro-> y = ALTURA_TELA - (ALTURA_CARRO * 1.5);
	// Rodas
	meu_carro-> roda1_x = LARGURA_TELA / 2 - LARGURA_CARRO / 2;
	meu_carro-> roda1_y = ALTURA_TELA - ALTURA_RODA;
	meu_carro-> roda2_x = LARGURA_TELA / 2 + LARGURA_CARRO / 2;
	meu_carro-> roda2_y = ALTURA_TELA - ALTURA_RODA;
	//
	meu_carro-> cor = al_map_rgb(255, 7, 7);
	meu_carro-> id = 0;
}

// Função que preenche um vetor de NUMCARS carros
void preenche_cars(Car *vet_carros, int i){
		// Coordenadas
		vet_carros -> x = (475 + rand() % 50);
		vet_carros -> y = ALTURA_CEU;
		vet_carros -> roda1_x = vet_carros -> x;
		vet_carros -> roda1_y = vet_carros -> y-2.5;// Pequena difença para roda ficar um pouco atrás do corpo
		vet_carros -> roda2_x = vet_carros -> x;	// do carro
		vet_carros -> roda2_y = vet_carros -> y-2.5;
		//
		vet_carros -> delta_speedy = ((rand() % 201) / 100);
		//
		vet_carros -> cor = cor_aleatoria[i];
		vet_carros -> id = i + 1;
}

// Função para desenhar os outros carros, recebendo uma struct também
void draw_carros(ALLEGRO_BITMAP *carro, ALLEGRO_DISPLAY *display, Car *vet_carros){	
	carro = al_create_bitmap((get_ratio(vet_carros -> y) * LARGURA_CARRO), (get_ratio(vet_carros -> y) * ALTURA_CARRO));	// Largura e altura do carro
	al_set_target_bitmap(carro);
	al_clear_to_color(vet_carros -> cor);               
	al_set_target_bitmap(al_get_backbuffer(display));       
	al_draw_bitmap(carro, vet_carros->x, vet_carros->y, 0);         // coordenada x e y dos carros
	//
	// Verificação para impedir que a variável y do carro principal também desça
	if(vet_carros -> id == 0)
		return;
	// Implementando o eixo y para descida
	vet_carros -> y += vet_carros -> delta_speedy;
}

// Função para desenhar as rodas
void draw_my_rodas(ALLEGRO_BITMAP *roda, ALLEGRO_DISPLAY *display, Car *c) {
	roda = al_create_bitmap(LARGURA_RODA,ALTURA_RODA);
	al_set_target_bitmap(roda);
	al_clear_to_color(al_map_rgb(0,0,0));
	al_set_target_bitmap(al_get_backbuffer(display));
	al_draw_bitmap(roda, c -> roda1_x, c -> roda1_y, 0); // Roda Esquerda
	al_draw_bitmap(roda, c -> roda2_x, c -> roda2_y, 0); // Roda Direita
}

// Função para desenhar as rodas dos outros carros
void draw_rodas(ALLEGRO_BITMAP *roda, ALLEGRO_DISPLAY *display, Car *vet_carros){
 
	 	roda = al_create_bitmap((get_ratio(vet_carros -> y) * LARGURA_RODA), (get_ratio(vet_carros -> y) * ALTURA_RODA));
	 	vet_carros -> roda2_x = vet_carros -> x + (get_ratio(vet_carros -> y) * (LARGURA_CARRO * 0.9));
	 	al_set_target_bitmap(roda);
	 	al_clear_to_color(al_map_rgb(0,0,0));
	 	al_set_target_bitmap(al_get_backbuffer(display));
	 	al_draw_bitmap(roda, vet_carros -> roda1_x, vet_carros -> roda1_y, 0); // Roda Esquerda
	 	al_draw_bitmap(roda, vet_carros -> roda2_x, vet_carros -> roda2_y, 0); // Roda Direita

	 	vet_carros -> roda1_y += vet_carros -> delta_speedy;     // Velocidade das rodas
	 	vet_carros -> roda2_y += vet_carros -> delta_speedy;

}

// Função Principal                <------------------------ MAIN ----------------------------->
int main(int argc, char **argv) {

	//srand(time(NULL)); Para variar a semente e assim ir mudando o modo como os carros aparecem
	srand(2);                                      // Para ficar igual ao do Pedro

	// Iniciando os ponteiros como Nulo
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	// Criando um bitmap para poder desenhar carro principal e rodas
	ALLEGRO_BITMAP *carro = NULL, *roda = 0;
	// Declaração de structs Car
	Car meu_carro;
	Car vet_carros[NUMCARS];      // Vetor de 10 carros

	// Chamando a função que inicia as variáveis globais
	inicia_variaveis_globais();

	// Rotinas de inicialização
	if(!al_init()) {                           // fprintf para escrever erros na saída padrão
		fprintf(stderr,"Falha ao iniciar a biblioteca Allegro.h\n");
		return -1;
	}

	if(!al_init_primitives_addon()) {
		fprintf(stderr,"Falha ao iniciar a biblioteca Primitives.h\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);                                 // Criar o timer == 1.0 / 60
	if(!timer) {
		fprintf(stderr,"Falha ao iniciar o timer\n");
		return -1;
	}

	display = al_create_display(LARGURA_TELA, ALTURA_TELA);             // Criar o tamanho da tela
	if(!display) {
		fprintf(stderr, "Falha ao iniciar a tela\n");
		al_destroy_timer(timer);                                        // Caso timer inicie e display não
		return -1;
	}

	// Inicializando o módulo de allegro que carrega as fontes
	al_init_font_addon();
	// Inicializando o módulo que entende arquivos ttf de fontes
	al_init_ttf_addon();

	// Carrega o arquivo arial.ttf, Fonte Arial, na pasta atual e define que será usado o tamanho 32
	ALLEGRO_FONT *size_32 = al_load_font("arial.ttf", 32, 1);

	// // Definindo as cores dos carrinhos      
	// ALLEGRO_COLOR cor_aleatoria[NUMCARS];// = al_map_rgb(100 + rand()%155,100 + rand()%155,100 + rand()%155);

	preenche_mycar(&meu_carro);

	// Vetor de NUMCARS cores, retirando o verde
	for(unsigned i = 0; i < NUMCARS; i++){
	  	cor_aleatoria[i] = al_map_rgb(1 + rand() % 255, 1 + rand() % 255,1 + rand() % 255);
	}

	// Criamos a nossa fila de eventos e guardamos em nosso ponteiro
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr,"Falha ao iniciar a fila de eventos\n");
		al_destroy_display(display);    	     // Caso haja algum erro em event_queue, destruir display e timer
		al_destroy_timer(timer);
		return -1;
	}

	// Inicializando o módulo do teclado
	al_install_keyboard();

	// Registra na fila de eventos todas as alterações da tela
	al_register_event_source(event_queue, al_get_display_event_source(display));
	// Registra na fila de eventos as alterações de tempo t para t + 1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	// Registra na fila de eventos as alterações nas teclas
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	// Reinicializa a tela
	al_flip_display();
	// Inicializa o temporizador
	al_start_timer(timer);

	// Declaração de novas Variáveis
	int playing = 1, move_direita = 0, move_esquerda = 0;

	// Vetor que preenche os NUMCARS
	for(unsigned i = 0; i < NUMCARS; i++){
		preenche_cars(&vet_carros[i], i+1);
	}

	// Enquanto playing for verdadeiro...
	while(playing == 1) {
		ALLEGRO_EVENT ev;
		// Aguarda por um evento e o armazena na variável de evento ev
		al_wait_for_event(event_queue, &ev); // Passando o endereço de memória de ev

		// Verifica se as flags de movimentação foram setadas como 1
		if(move_esquerda){
			if(meu_carro.roda1_x > PISTA_INF_LEFT_X + LARGURA_RODA / 2){
				meu_carro.x -= delta_dx;
				meu_carro.roda1_x -= delta_dx;
				meu_carro.roda2_x -= delta_dx;
			}
		}

		if(move_direita){
			if(meu_carro.roda2_x < PISTA_INF_RIGHT_X - LARGURA_RODA){
				meu_carro.x += delta_dx;
				meu_carro.roda1_x += delta_dx;
				meu_carro.roda2_x += delta_dx;
			}
		}

		// Verifica se o evento foi uma tecla apertada...
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//Verifica qual a tecla
			switch (ev.keyboard.keycode) {
				// se a tecla for w
				case ALLEGRO_KEY_W:

					break;
				// se a tecla for s
				case ALLEGRO_KEY_S:

					break;
				// se a tecla for a
				case ALLEGRO_KEY_A:
					move_esquerda = 1;
					break;
				// se a tecla for d
				case ALLEGRO_KEY_D:
					move_direita = 1;
					break;
				// se for escape, sai do jogo
				case ALLEGRO_KEY_ESCAPE:
					playing = 0;
					break;
			}
		}

		// Agora verifica se o evento é uma tecla levantada...
		if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			// Verifica qual foi a tecla...
			switch (ev.keyboard.keycode) {
				// se a tecla for w
				case ALLEGRO_KEY_W:

					break;
				// se a tecla for s
				case ALLEGRO_KEY_S:

					break;
				// se a tecla for a
				case ALLEGRO_KEY_A:
					move_esquerda = 0;
					break;
				// se a tecla for d
				case ALLEGRO_KEY_D:
					move_direita = 0;
					break;
				// Não precisa verificar a tecla escape na subida
			}
		}

		// Se o tipo de evento for um temporizador, ou seja, se o tempo passou de t para t + 1
		else if(ev.type == ALLEGRO_EVENT_TIMER){
			// Função para desenhar o cenário na tela
			desenha_cenario(display);

			// Função para desenhar o carro principal na tela
			draw_carros(carro, display, &meu_carro);
			draw_my_rodas(roda, display, &meu_carro);
			
			for(unsigned i = 0; i < NUMCARS; i++){
				draw_carros(carro, display, &vet_carros[i]);
				draw_rodas(roda, display, &vet_carros[i]);
				//al_rest(0.01);
			}

			// Função para reinicializar a tela
			al_flip_display();
		}

		// Se o tipo de evento for um fechamento da tela, ou seja, jogador aperta o x da janela para sair
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
			// sai do loop
			playing = 0;
		}

	} // Final do while	
		
	//colore toda a tela de preto
	al_clear_to_color(al_map_rgb(0,0,0));
	//if(collision)
		//al_draw_text(size_32, al_map_rgb(200, 0, 30), SCREEN_W/3, SCREEN_H/2, 0, "Perdeu :(");
	//else {
	al_draw_textf(size_32, al_map_rgb(0, 200, 30), LARGURA_TELA / 7, ALTURA_TELA / 1.5, 0, " Nº Carros Ultrapassados = %d", score);
	//}

	// Reinicializa a tela
	al_flip_display();
	al_rest(2);                             // Travar a tela por um tempo

	// Destrói os ponteiros
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_destroy_bitmap(carro);
	al_destroy_bitmap(roda);
	al_destroy_font(size_32);


	return 0;	
}
