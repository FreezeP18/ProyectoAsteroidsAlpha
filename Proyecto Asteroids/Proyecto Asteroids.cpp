#include <stdio.h>
#include <iostream>
#include <math.h>
#include <windows.h>
#include <stdlib.h>
#include <cstdlib>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>

using namespace std;
#pragma warning(disable:4996)
#define FPS 65.0
const float d = 5; 

struct Actor {
	float x;
	float y;
	float lado; 
	int dir; 
	ALLEGRO_BITMAP* sprite;
};

struct Bala {
	float x;
	float y;
	float velocidad;
	bool activa; 
	ALLEGRO_BITMAP* sprite;
	float dirX;
	float dirY;
};

struct Enemigo {
	float x;
	float y;
	float lado;
	bool activo;
	ALLEGRO_BITMAP* sprite;
};

void mover(Actor& actor)
{
	float bordeX = actor.x;
	float bordeY = actor.y;

	switch (actor.dir) {
	case 1:
		bordeX = actor.x + d;
		break;
	case -1: 
		bordeX = actor.x - d;
		break;
	case 2: 
		bordeY = actor.y - d;
		break;
	case -2: 
		bordeY = actor.y + d;
	}
	if (bordeX >= 0 && bordeX <= 800 - actor.lado) {
		actor.x = bordeX;
	}
	if (bordeY >= 0 && bordeY <= 600 - actor.lado) {
		actor.y = bordeY;
	}

	al_draw_bitmap(actor.sprite, actor.x, actor.y, NULL);
	
}

void disparar(Bala& bala, const Actor& actor) {
	if (!bala.activa) {
		bala.activa = true;
		bala.x = actor.x + actor.lado / 2;
		bala.y = actor.y;

		if (actor.dir == 2) { 
			bala.dirX = 0;
			bala.dirY = -1;
		}
		else if (actor.dir == -2) { 
			bala.dirX = 0;
			bala.dirY = 1;
		}
		else {
			bala.dirX = sin(actor.dir * 3.14159265358979323846 / 2);
			bala.dirY = -cos(actor.dir * 3.14159265358979323846 / 2);
		}
	}
}

void moverBala(Bala& bala) {

	if (bala.activa) {
		bala.x += bala.dirX * bala.velocidad;
		bala.y += bala.dirY * bala.velocidad;
		al_draw_bitmap(bala.sprite, bala.x, bala.y, NULL);
		if (bala.x < 0 || bala.x > 800 || bala.y < 0 || bala.y > 600) {
			bala.activa = false;
		}
	}
}

void moverEnemigos(Enemigo enemigos[], int cantidad) {
	for (int i = 0; i < cantidad; i++) {
		if (enemigos[i].activo) {
			enemigos[i].x += rand() % 2 ? d : -d;
			enemigos[i].y += rand() % 2 ? d : -d;
			if (enemigos[i].x < 0) enemigos[i].x = 0;
			if (enemigos[i].x > 800 - enemigos[i].lado) enemigos[i].x = 800 - enemigos[i].lado;
			if (enemigos[i].y < 0) enemigos[i].y = 0;
			if (enemigos[i].y > 600 - enemigos[i].lado) enemigos[i].y = 600 - enemigos[i].lado;
		}
	}
}

int main() {
	//Si no se inicializa Allegro se lanza el error
	if (!al_init()) {
		al_show_native_message_box(NULL, "Ventana Emergente", "Error", "No se puede inicializar Allegro", NULL, NULL);
		return -1;
	}

	//Se instalan los complementos y periféricos
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_install_keyboard();

	// Tamaño de pantalla
	const float W = 800; // ancho
	const float H = 600; // alto

	const float alto_texto = 90;

	// Se crea un display en ventana
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	ALLEGRO_DISPLAY* pantalla = al_create_display(W, H);

	al_set_window_title(pantalla, "Proyecto Algoritmos: Asteroids Invincible");//Se le pone un título a la ventana

	if (!pantalla)
	{
		//Si el display no se ejecuta se lanza el mensaje de error
		al_show_native_message_box(NULL, "Ventana Emergente", "Error", "No se puede crear la pantalla", NULL, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	// Timer de redibujado, a 60 Hz
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

	ALLEGRO_TIMER* timerMOV = al_create_timer(1.0 / 15);

	// Cola de eventos
	ALLEGRO_EVENT_QUEUE* cola_eventos = al_create_event_queue();

	// Tipo de letra para el texto
	ALLEGRO_FONT* fuente;
	fuente = al_load_font("Minecraft.ttf", 60, NULL);

	al_draw_text(fuente, al_map_rgb(255, 255, 255), W / 2, H / 2, ALLEGRO_ALIGN_CENTER, "Nivel completado");

	// Sprites
	ALLEGRO_BITMAP* sprite_enemigo = al_load_bitmap("sprites/enemigoPNG.png");

	ALLEGRO_BITMAP* sprite_balaV = al_load_bitmap("sprites/balaV.png");
	ALLEGRO_BITMAP* sprite_nave_U = al_load_bitmap("sprites/naveU.png");
	ALLEGRO_BITMAP* sprite_nave_A = al_load_bitmap("sprites/naveA.png");
	ALLEGRO_BITMAP* sprite_nave_I = al_load_bitmap("sprites/naveI.png");
	ALLEGRO_BITMAP* sprite_nave_D = al_load_bitmap("sprites/naveD.png");

	//Se registran las fuentes de eventos (timers y periféricos) en la cola de eventos
	al_register_event_source(cola_eventos, al_get_timer_event_source(timer));
	al_register_event_source(cola_eventos, al_get_timer_event_source(timerMOV));
	al_register_event_source(cola_eventos, al_get_display_event_source(pantalla));
	al_register_event_source(cola_eventos, al_get_keyboard_event_source());

	// Condición que termina el ciclo de juego
	bool continuar = true;
	bool dibujar = true;

	// Personajes
	Actor Nave, Borde;
	Borde.lado = 20;

	Nave.lado = 34;
	Nave.x = 400;
	Nave.y = 300;
	Nave.dir = 0;
	Nave.sprite = sprite_nave_A;

	Bala bala;
	bala.x = 0;
	bala.y = 0;
	bala.velocidad = 8;
	bala.activa = false;
	bala.sprite = sprite_balaV;

	const int MAX_ENEMIGOS = 10;
	Enemigo enemigos[MAX_ENEMIGOS];
	for (int i = 0; i < MAX_ENEMIGOS; i++) {
		enemigos[i].x = rand() % (int)(W - 2 * Borde.lado);
		enemigos[i].y = rand() % (int)(H - 2 * Borde.lado);
		enemigos[i].lado = 50;
		enemigos[i].activo = true;
		enemigos[i].sprite = sprite_enemigo;
	}

	ALLEGRO_KEYBOARD_STATE keyState;

	// Se dibujan los personajes en las posiciones iniciales
	al_draw_bitmap(Nave.sprite, Nave.x, Nave.y, NULL);
	//al_draw_bitmap(bala.sprite, bala.x, bala.y, NULL);

	al_start_timer(timer);
	al_start_timer(timerMOV);

	int enemigosDerrotados = 0;

	while (continuar) {
		ALLEGRO_EVENT eventos;
		al_wait_for_event(cola_eventos, &eventos);

		if (eventos.type == ALLEGRO_EVENT_TIMER) {

			if (eventos.timer.source == timer) {
				al_get_keyboard_state(&keyState);

				if (al_key_down(&keyState, ALLEGRO_KEY_DOWN)) {
					if (Nave.dir != -2) {
						Nave.dir = -2;
						Nave.sprite = sprite_nave_A;
					}
					mover(Nave);
				}

				if (al_key_down(&keyState, ALLEGRO_KEY_UP)) {
					if (Nave.dir != 2) {
						Nave.dir = 2;
						Nave.sprite = sprite_nave_U;
						bala.activa;
					}
					mover(Nave);
				}

				if (al_key_down(&keyState, ALLEGRO_KEY_RIGHT)) {
					if (Nave.dir != 1) {
						Nave.dir = 1;
						Nave.sprite = sprite_nave_D;
					}
					mover(Nave);
				}

				if (al_key_down(&keyState, ALLEGRO_KEY_LEFT)) {
					if (Nave.dir != -1) {
						Nave.dir = -1;
						Nave.sprite = sprite_nave_I;
					}
					mover(Nave);
				}

				if (al_key_down(&keyState, ALLEGRO_KEY_SPACE)) {
					disparar(bala, Nave);
				}

				// Mover balas
				moverBala(bala);

				// Colisiones de balas con enemigos
				for (int i = 0; i < MAX_ENEMIGOS; i++) {
					if (enemigos[i].activo && bala.activa &&
						bala.x >= enemigos[i].x && bala.x <= enemigos[i].x + enemigos[i].lado &&
						bala.y >= enemigos[i].y && bala.y <= enemigos[i].y + enemigos[i].lado) 
					{
						enemigos[i].activo = false;
						bala.activa = false;
						enemigosDerrotados++;
					}
					else if (enemigos[i].activo &&
						Nave.x >= enemigos[i].x && Nave.x <= enemigos[i].x + enemigos[i].lado &&
						Nave.y >= enemigos[i].y && Nave.y <= enemigos[i].y + enemigos[i].lado) 
					{
						enemigos[i].activo = false;
						enemigosDerrotados++;
					}
				}
				dibujar = true;

				// Verificar si se derrotaron los 10 enemigos
				if (enemigosDerrotados == 10) {
					enemigosDerrotados = 0;
					for (int i = 0; i < MAX_ENEMIGOS; i++)
					{
						enemigos[i].activo = true;
					}
				}
			}

			if (eventos.timer.source == timerMOV) {
				// Movimiento de los enemigos
				for (int i = 0; i < MAX_ENEMIGOS; i++) {
					if (enemigos[i].activo) {
						enemigos[i].x += rand() % 2 ? d : -d;
						enemigos[i].y += rand() % 2 ? d : -d;
						if (enemigos[i].x < 0) enemigos[i].x = 0;
						if (enemigos[i].x > W - enemigos[i].lado) enemigos[i].x = W - enemigos[i].lado;
						if (enemigos[i].y < 0) enemigos[i].y = 0;
						if (enemigos[i].y > H - enemigos[i].lado) enemigos[i].y = H - enemigos[i].lado;
					}
				}
				dibujar = true;
			}
		}
		else if (eventos.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			continuar = false;
		}

		if (dibujar && al_is_event_queue_empty(cola_eventos)) {
			al_clear_to_color(al_map_rgb(0, 0, 0));

			// Dibujar enemigos
			for (int i = 0; i < MAX_ENEMIGOS; i++) {
				if (enemigos[i].activo) {
					al_draw_bitmap(enemigos[i].sprite, enemigos[i].x, enemigos[i].y, NULL);
				}
			}

			// Dibujar balas
			moverBala(bala);

			// Dibujar Nave
			al_draw_bitmap(Nave.sprite, Nave.x, Nave.y, NULL);

			al_flip_display();
			dibujar = false;
		}
	}

	al_destroy_bitmap(sprite_balaV);
	al_destroy_bitmap(sprite_nave_U);
	al_destroy_bitmap(sprite_nave_A);
	al_destroy_bitmap(sprite_nave_I);
	al_destroy_bitmap(sprite_nave_D);
	al_destroy_bitmap(sprite_enemigo);
	al_destroy_font(fuente);
	al_destroy_timer(timer);
	al_destroy_timer(timerMOV);
	al_destroy_event_queue(cola_eventos);
	al_destroy_display(pantalla);

	return 0;
}


