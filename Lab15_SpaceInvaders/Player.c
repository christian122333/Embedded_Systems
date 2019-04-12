#include "Player.h"
#include "Images.h"
#include <stdlib.h>

void Player_init(struct Player *player, struct Bunker *bunker, struct Missile missile[], int missile_length){
		player->x = 0;
		player->y = 47;
		player->width = 18 - 1;
		player->height = 8 - 1;
	  player->prev_x = 0;
	  player->player_image = PlayerShip0;
		player->explosion0_image = BigExplosion0;
		player->explosion1_image = BigExplosion1;
		player->explosion_frame = 2;
	  player->life = 1;
		
		bunker->x = 33;
		bunker->y = 47 - PLAYERH;
		bunker->width = 18 - 1;
		bunker->height = 5 - 1;
	  bunker->bunker0_image = Bunker0;
	  bunker->bunker1_image = Bunker1;
		bunker->bunker2_image = Bunker2;
		bunker->bunker3_image = Bunker3;
	  bunker->life = 10;
		Missile_init(missile, missile_length);
}

void Missile_init(struct Missile *missile, int missile_length){
	int i;
	for(i = 0; i < missile_length; i++){
		missile[i].x = 0;
		missile[i].y = 0;
		missile[i].width = 4 - 1;
		missile[i].height = 9 - 1;
		missile[i].image = Missile0;
		missile[i].life = 0;
	}
}
void Player_move(struct Player *player, unsigned long data){
	unsigned long x = data;
	if(x != player->prev_x){	//Update the Player's value if it has moved
		player->prev_x = x;
		player->x = x;
	}
}

void Missile_move(struct Missile *missile, int missile_length){
	int i;
	for(i=0; i<missile_length; i++){
		if(missile[i].life > 0)
			missile[i].y -= 2;
	}
	
}

void Player_shoot(struct Missile *missile, struct Player *player, int missile_length){
	int i;
	for(i=0; i<missile_length; i++){
		if(missile[i].life == 0){
			missile[i].x = player->x + 8;
			missile[i].y = player->y - 8;
			missile[i].life = 1;
			break;
		}
	}
}