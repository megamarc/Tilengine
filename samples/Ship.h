#ifndef _SHIP_H
#define _SHIP_H

#include "Shooter.h"

Actor* CreateShip (void);
Actor* CreateClaw (int id);
Actor* CreateShot (int type, int x, int y);

#endif
