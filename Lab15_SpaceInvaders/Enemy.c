#include "Enemy.h"
#include "Random.h"
#include "Images.h"
#include <stdlib.h>

void Enemy_init(struct Enemy *enemy, unsigned long y, int direction, const unsigned char *image, int enemy_length){
	int i;
	for(i = 0; i < enemy_length; i++){
		enemy[i].x = (20*i);
		enemy[i].y = y;
		enemy[i].width = 16 - 1;
		enemy[i].height = 10 - 1;
		enemy[i].direction = direction;
		enemy[i].enemy_image = image;
		enemy[i].explosion0_image = SmallExplosion0;
		enemy[i].explosion1_image = SmallExplosion1;
		enemy[i].explosion_frame = 2;
		enemy[i].life = 1;
	}
}

void Enemy_laser_init(struct Enemy_laser *laser, int laser_length){
	int i;
	for(i = 0; i < laser_length; i++){
		laser[i].x = 0;
		laser[i].y = 0;
		laser[i].width = 2 - 1;
		laser[i].height = 9 - 1;
		laser[i].laser_image = Laser0;
		laser[i].life = 0;
	}
}

void Enemy_move(struct Enemy *enemy, struct Enemy_laser *laser, int enemy_length, int laser_length){
	int i;
	int randx;
	for(i = 0; i < enemy_length; i++){
		if(enemy[i].life > 0){
			/*Enemy is moving to the right*/
			if(enemy[i].direction == RIGHT){
				if(enemy[i].x > 66){
					enemy[i].y += 9;
					enemy[i].direction = LEFT;
				}
				else
					enemy[i].x += 2 * (enemy[i].direction);
			}
			/*Enemy is moving to the left*/
			else if(enemy[i].direction == LEFT){
				if(enemy[i].x < 2){
					enemy[i].y += 9;
					enemy[i].direction = RIGHT;
				}
				else
					enemy[i].x += 2 * (enemy[i].direction);
			}
			/*Enemy has random chance of shooting laser*/
			randx = rand()%100;
			if(randx >= 90)
				Enemy_shoot(&enemy[i], laser, laser_length);
		}
	}
	Lasers_move(laser, laser_length);
}

void Enemy_shoot(struct Enemy *enemy, struct Enemy_laser *laser, int laser_length){
	int i;
	for(i = 0; i < laser_length; i++){
		if(laser[i].life == 0){
			laser[i].x = enemy->x + 8;
			laser[i].y = enemy->y + 7;
			laser[i].life = 1;
			break;
		}
	}
}

void Lasers_move(struct Enemy_laser *laser, int laser_length){
	int i;
	for(i = 0; i < laser_length; i++){
		if(laser[i].life > 0)
			laser[i].y += 2;
	}
}