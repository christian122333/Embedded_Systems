#ifndef PLAYER_H
#define PLAYER_H

struct Player {
	unsigned long x;
	unsigned long y;
	unsigned long width;
	unsigned long height;
	unsigned long prev_x;
	const unsigned char *player_image;
	const unsigned char *explosion0_image;
	const unsigned char *explosion1_image;
	unsigned long explosion_frame;
	unsigned long life;
};

struct Bunker {
	unsigned long x;
	unsigned long y;
	unsigned long width;
	unsigned long height;
	const unsigned char *bunker0_image;
	const unsigned char *bunker1_image;
	const unsigned char *bunker2_image;
	const unsigned char *bunker3_image;
	unsigned long life;
};

struct Missile {
	unsigned long x;
	unsigned long y;
	unsigned long width;
	unsigned long height;
	const unsigned char *image;
	unsigned long life;
};


void Player_init(struct Player *p, struct Bunker *b, struct Missile *missile, int missile_length);
void Missile_init(struct Missile *missile, int missile_length);
void Player_move(struct Player *p, unsigned long data);
void Missile_move(struct Missile *missile, int missile_length);
void Player_shoot(struct Missile *missile, struct Player *player, int missile_length);


#endif /*PLAYER_H*/