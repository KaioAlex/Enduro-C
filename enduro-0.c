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

// Variável para guardar pontuação
int score = 0;

// Criando uma estrutura Car para guardar todas as variáveis necessárias dele
typedef struct {
	float x, y, roda1_x, roda1_y, roda2_x, roda2_y; //x centro, y centro
	float largura_carro;
	float altura_carro;
	float xoffset; //distancia do x para a margem esquerda da pista (ajuda em alguns calculos)
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

}

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

// Função para criar meu carro: Recebe a altura e largura do carro e as coordenadas x e y do carro
void draw_meu_carro(ALLEGRO_BITMAP *carro, ALLEGRO_DISPLAY *display, ALLEGRO_COLOR cor, float largura, float altura, float x, float y){
	carro = al_create_bitmap(largura, altura);		
	al_set_target_bitmap(carro);
	al_clear_to_color(cor);               // Cor vermelha
	al_set_target_bitmap(al_get_backbuffer(display));       // Resolver o erro, voltando o alvo p/ display
	al_draw_bitmap(carro, x, y, 0);                         // Posição do carro
}
// Função para criar os outros carros, recebendo uma struct
void draw_carros(ALLEGRO_BITMAP *carro, ALLEGRO_DISPLAY *display, Car *vet_carros){
	float a = LARGURA_PISTA_SUP, b = ALTURA_CEU;
	//        Começa em 50 até 833     Começa em 160 até 640
	if(a < LARGURA_PISTA_INF || b < ALTURA_CEU){
		carro = al_create_bitmap(regra_tres_largura_carro(a), regra_tres_altura_carro(b));	// Largura e altura do carro
		al_set_target_bitmap(carro);
		al_clear_to_color(vet_carros -> cor);               
		al_set_target_bitmap(al_get_backbuffer(display));       
		al_draw_bitmap(carro, vet_carros -> x, vet_carros -> y, 0);         // coordenada x e y dos carros
		//
		a+= 5.5;
		b+= 3.0;
		vet_carros -> y = b + delta_dy;
		//
		draw_carros(carro, display, &vet_carros);                    
	}

}

// Função para criar as rodas
void draw_rodas(ALLEGRO_BITMAP *roda, ALLEGRO_DISPLAY *display, float largura, float altura, float x1, float y1, float x2, float y2){
	roda = al_create_bitmap(largura, altura);
	al_set_target_bitmap(roda);
	al_clear_to_color(al_map_rgb(0,0,0));
	al_set_target_bitmap(al_get_backbuffer(display));
	al_draw_bitmap(roda, x1, y1, 0); // Roda Esquerda
	al_draw_bitmap(roda, x2, y2, 0); // Roda Direita
}

// Função regra de três das coordenadas dos carros
float regra_tres_largura_carro(float largura_pista){     // o que varia é o eixo x, começa em 25 e vai até 833
	return (largura_pista * 1.2) * LARGURA_CARRO / LARGURA_PISTA_INF;
}

float regra_tres_altura_carro(float altura_pista){       // o que varia aqui é o eixo y, começa em 160 até 640
	return altura_pista * ALTURA_CARRO / ALTURA_TELA; 
}

float regra_tres_largura_roda(float largura_pista){
	return largura_pista * LARGURA_RODA / LARGURA_PISTA_INF;
}

float regra_tres_altura_roda(float altura_pista){
	return altura_pista * ALTURA_RODA / ALTURA_TELA;
}


// Função Principal

int main(int argc, char **argv) {

	//srand(time(NULL)); Para variar a semente e assim ir mudando o modo como os carros aparecem
	srand(2);                                      // Para ficar igual ao do Pedro

	// Iniciando os ponteiros como Nulo
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	// Criando um bitmap para poder desenhar carro principal e rodas
	ALLEGRO_BITMAP *carro = NULL, *roda = 0;

	// Configurando as coordenadas do meu carro principal pela estrutura Car
	Car meu_carro;
	Car vet_carros[NUMCARS];      // Vetor de 10 carros
	// Carro
	meu_carro.x = LARGURA_TELA / 2 - LARGURA_CARRO / 2;
	meu_carro.y = ALTURA_TELA - (ALTURA_CARRO * 1.5);
	// Rodas
	meu_carro.roda1_x = LARGURA_TELA / 2 - LARGURA_CARRO / 2;
	meu_carro.roda1_y = ALTURA_TELA - ALTURA_RODA;
	meu_carro.roda2_x = LARGURA_TELA / 2 + LARGURA_CARRO / 2;
	meu_carro.roda2_y = ALTURA_TELA - ALTURA_RODA;

	// Configurando a variação do meu carro
	float delta_dx = LARGURA_TELA / 200.0, delta_dy = 3.0;         // Variando 5 pixels cada tecla apertada

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

	// Definindo as cores dos carrinhos
	ALLEGRO_COLOR cor_red = al_map_rgb(255, 7, 7);           
	ALLEGRO_COLOR cor_aleatoria[NUMCARS];// = al_map_rgb(100 + rand()%155,100 + rand()%155,100 + rand()%155);
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
	int playing = 1, move_direita = 0, move_esquerda = 0, vet_400_500[NUMCARS];
	float a = LARGURA_PISTA_SUP, b = ALTURA_CEU, c = LARGURA_PISTA_SUP, d = ALTURA_CEU, e = LARGURA_PISTA_SUP, f = ALTURA_CEU, g = LARGURA_PISTA_SUP, h = ALTURA_CEU, i = LARGURA_PISTA_SUP, j = ALTURA_CEU; // inicio coordenadas x e y
	float k = LARGURA_PISTA_SUP, l = ALTURA_CEU, m = LARGURA_PISTA_SUP, n = ALTURA_CEU, o = LARGURA_PISTA_SUP, p = ALTURA_CEU, q = LARGURA_PISTA_SUP, r = ALTURA_CEU, s = LARGURA_PISTA_SUP, t = ALTURA_CEU;

	// Guardando 10 coordenadas x entre 488 e 512 em um vetor e criando um vetor de 10 carros
	for(unsigned i = 0; i < NUMCARS; i++) {
		vet_carros[i].x = (475 + rand() % 50);
		vet_carros[i].y = ALTURA_CEU;
		vet_carros[i].largura_carro = LARGURA_CARRO;
		vet_carros[i].altura_carro = ALTURA_CARRO;
		vet_400_500[i] = (400 + rand() % 50);
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
			draw_meu_carro(carro, display, cor_red, LARGURA_CARRO, ALTURA_CARRO, meu_carro.x, meu_carro.y);
			draw_rodas(roda, display, LARGURA_RODA, ALTURA_RODA, meu_carro.roda1_x, meu_carro.roda1_y, meu_carro.roda2_x, meu_carro.roda2_y);

			// Função para desenhar os outros carros na tela
			//cria_carro(carro, display, regra_tres_largura_carro(833.3), regra_tres_altura_carro(640), c[0].x, ALTURA_TELA - (regra_tres_altura_carro(640) * 1.5));
			//draw_rodas(roda, display, regra_tres_largura_roda(833.3), regra_tres_altura_roda(640), c[0].x, ALTURA_TELA - regra_tres_altura_roda(640), c[0].x + (regra_tres_largura_carro(833.3) / 1.3), ALTURA_TELA - regra_tres_altura_roda(640));
			
			draw_carros(carro, display, &vet_carros[0]);


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
