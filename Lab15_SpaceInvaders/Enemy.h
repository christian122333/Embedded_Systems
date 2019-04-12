#ifndef ENEMY_H
#define ENEMY_H
enum DIRECTION {LEFT = -1,RIGHT = 1};
struct Enemy {
	unsigned long x;
	unsigned long y;
	unsigned long width;
	unsigned long height;
	unsigned long direction;
	const unsigned char *enemy_image;
	const unsigned char *explosion0_image;
	const unsigned char *explosion1_image;
	unsigned long explosion_frame;
	unsigned long life;
};

struct Enemy_laser {
	unsigned long x;
	unsigned long y;
	unsigned long width;
	unsigned long height;
	const unsigned char *laser_image;
	unsigned long life;
};

void Enemy_init(struct Enemy *enemy, unsigned long y, int direction, const unsigned char *image, int enemy_length);
void Enemy_laser_init(struct Enemy_laser *laser, int enemy_length);
void Enemy_move(struct Enemy *enemy, struct Enemy_laser *laser, int enemy_length, int laser_length);
void Enemy_shoot(struct Enemy *enemy, struct Enemy_laser *laser, int laser_length);
void Lasers_move(struct Enemy_laser *laser, int laser_length);
#endif /*ENEMY_H*/