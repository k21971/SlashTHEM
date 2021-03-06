/*	SCCS Id: @(#)mkroom.c	3.4	2001/09/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Entry points:
 *	mkroom() -- make and stock a room of a given type
 *	nexttodoor() -- return TRUE if adjacent to a door
 *	has_dnstairs() -- return TRUE if given room has a down staircase
 *	has_upstairs() -- return TRUE if given room has an up staircase
 *	courtmon() -- generate a court monster
 *	douglas_adams_mon() -- create a Douglas-Adams_Monster
 *	save_rooms() -- save rooms into file fd
 *	rest_rooms() -- restore rooms from file fd
 */

#include "hack.h"

#ifdef OVLB
STATIC_DCL boolean FDECL(isbig, (struct mkroom *));
STATIC_DCL struct mkroom * FDECL(pick_room,(BOOLEAN_P));
STATIC_DCL void NDECL(mkshop), FDECL(mkzoo,(int)), NDECL(mkswamp);
STATIC_DCL void NDECL(mktemple);
STATIC_DCL void FDECL(mkgarden, (struct mkroom *));
STATIC_DCL coord * FDECL(shrine_pos, (int));
STATIC_DCL struct permonst * NDECL(morguemon);
STATIC_DCL struct permonst * NDECL(douglas_adams_mon);
STATIC_DCL struct permonst * NDECL(squadmon);
STATIC_DCL struct permonst * NDECL(fungus);
STATIC_DCL void NDECL(mktraproom);
STATIC_DCL void NDECL(mkpoolroom);
//STATIC_DCL void NDECL(mkstatueroom);
STATIC_DCL void FDECL(save_room, (int,struct mkroom *));
STATIC_DCL void FDECL(rest_room, (int,struct mkroom *));
#endif /* OVLB */

#define sq(x) ((x)*(x))

extern const struct shclass shtypes[];	/* defined in shknam.c */

#ifdef OVLB

STATIC_OVL boolean
isbig(sroom)
register struct mkroom *sroom;
{
	register int area = (sroom->hx - sroom->lx + 1)
			   * (sroom->hy - sroom->ly + 1);
	return((boolean)( area > 20 ));
}

void
mkroom(roomtype)
/* make and stock a room of a given type */
int	roomtype;
{
    if (roomtype >= SHOPBASE)
	mkshop();	/* someday, we should be able to specify shop type */
    else switch(roomtype) {
	case COURT:	mkzoo(COURT); break;
	case ZOO:	mkzoo(ZOO); break;
	case BEEHIVE:	mkzoo(BEEHIVE); break;
	case MORGUE:	mkzoo(MORGUE); break;
	case BARRACKS:	mkzoo(BARRACKS); break;
	case REALZOO:   mkzoo(REALZOO); break;
	case BADFOODSHOP: mkzoo(BADFOODSHOP); break;
	case DOUGROOM:	mkzoo(DOUGROOM); break;
	case DRAGONLAIR: mkzoo(DRAGONLAIR); break;
	case GIANTCOURT: mkzoo(GIANTCOURT); break;
	case SWAMP:	mkswamp(); break;
	case GARDEN:	mkgarden((struct mkroom *)0); break;
	case TEMPLE:	mktemple(); break;
	case LEPREHALL:	mkzoo(LEPREHALL); break;
	case COCKNEST:	mkzoo(COCKNEST); break;
	case ANTHOLE:	mkzoo(ANTHOLE); break;
	case LEMUREPIT: mkzoo(LEMUREPIT); break;
	case MIGOHIVE:  mkzoo(MIGOHIVE); break;
	case FUNGUSFARM: mkzoo(FUNGUSFARM); break;
	case CLINIC: mkzoo(CLINIC); break;
	case TERRORHALL: mkzoo(TERRORHALL); break;
	case COINHALL: mkzoo(COINHALL); break;
	/*case GRUEROOM: mkzoo(GRUEROOM); break;*/
	case TRAPROOM:  mktraproom(); break;
	case POOLROOM:  mkpoolroom(); break;
	case ARMORY: mkzoo(ARMORY); break;
	default:	impossible("Tried to make a room of type %d.", roomtype);
    }
}

STATIC_OVL void
mkshop()
{
	register struct mkroom *sroom;
	int i = -1, j;
#ifdef WIZARD
	char *ep = (char *)0;	/* (init == lint suppression) */

	/* first determine shoptype */
	if(wizard){
#ifndef MAC
		ep = nh_getenv("SHOPTYPE");
		if(ep){
			if(*ep == 'z' || *ep == 'Z'){
				mkzoo(ZOO);
				return;
			}
			if(*ep == 'm' || *ep == 'M'){
				mkzoo(MORGUE);
				return;
			}
			if(*ep == 'b' || *ep == 'B'){
				mkzoo(BEEHIVE);
				return;
			}
			if (*ep == 'p' || *ep == 'P') {
				mkzoo(LEMUREPIT);
				return;
			}
			if (*ep == 'i' || *ep == 'I') {
				mkzoo(MIGOHIVE);
				return;
			}
			if (*ep == 'f' || *ep == 'F') {
				mkzoo(FUNGUSFARM);
				return;
			}
			if(*ep == 't' || *ep == 'T' || *ep == '\\'){
				mkzoo(COURT);
				return;
			}
			if(*ep == 's' || *ep == 'S'){
				mkzoo(BARRACKS);
				return;
			}
			if(*ep == 'a' || *ep == 'A'){
				mkzoo(ANTHOLE);
				return;
			}
			if(*ep == 'c' || *ep == 'C'){
				mkzoo(COCKNEST);
				return;
			}
			if(*ep == 'r' || *ep == 'R'){
				mkzoo(ARMORY);
				return;
			}
			if(*ep == 'l' || *ep == 'L'){
				mkzoo(LEPREHALL);
				return;
			}
			if(*ep == '_'){
				mktemple();
				return;
			}
			if(*ep == 'n' || *ep == 'N'){
				mkgarden((struct mkroom *)0);
				return;
			}
			if(*ep == '}'){
				mkswamp();
				return;
			}
			if (*ep == 'd' || *ep == 'D') {
				mkzoo(DOUGROOM);
				return;
			}
			j = -1;
			for(i=0; shtypes[i].name; i++)
				if(*ep == def_oc_syms[(int)shtypes[i].symb]) {
					if (j < 0) j = i;
					if (!strcmp(ep + 1, shtypes[i].name))
						break;
				}
			if(*ep == 'g' || *ep == 'G')
				i = 0;
			else
				i = j;
		}
#endif
	}
#endif
	for(sroom = &rooms[0]; ; sroom++){
		if(sroom->hx < 0) return;
		if(sroom - rooms >= nroom) {
			pline("rooms not closed by -1?");
			return;
		}
		if(sroom->rtype != OROOM) continue;
		if(has_dnstairs(sroom) || has_upstairs(sroom))
			continue;
		if(
#ifdef WIZARD
		   (wizard && ep && sroom->doorct != 0) ||
#endif
			sroom->doorct == 1) break;
	}
	if (!sroom->rlit) {
		int x, y;

		for(x = sroom->lx - 1; x <= sroom->hx + 1; x++)
		for(y = sroom->ly - 1; y <= sroom->hy + 1; y++)
			levl[x][y].lit = 1;
		sroom->rlit = 1;
	}

	if(i < 0) {			/* shoptype not yet determined */
	    /* pick a shop type at random */
	    for (j = rnd(100), i = 0; (j -= shtypes[i].prob) > 0; i++)
		continue;

	    /* big rooms cannot be wand or book shops,
	     * - so make them general stores
	     */
	    if(isbig(sroom) && (shtypes[i].symb == WAND_CLASS
				|| shtypes[i].symb == SPBOOK_CLASS)) i = 0;
	}
	sroom->rtype = SHOPBASE + i;

	/* set room bits before stocking the shop */
#ifdef SPECIALIZATION
	topologize(sroom, FALSE); /* doesn't matter - this is a special room */
#else
	topologize(sroom);
#endif

	/* stock the room with a shopkeeper and artifacts */
	stock_room(i, sroom);
}

STATIC_OVL struct mkroom *
pick_room(strict)
register boolean strict;
/* pick an unused room, preferably with only one door */
{
	register struct mkroom *sroom;
	register int i = nroom;

	for(sroom = &rooms[rn2(nroom)]; i--; sroom++) {
		if(sroom == &rooms[nroom])
			sroom = &rooms[0];
		if(sroom->hx < 0)
			return (struct mkroom *)0;
		if(sroom->rtype != OROOM)	continue;
		if(!strict) {
		    if(has_upstairs(sroom) || (has_dnstairs(sroom) && rn2(3)))
			continue;
		} else if(has_upstairs(sroom) || has_dnstairs(sroom))
			continue;
		if(sroom->doorct == 1 || (!rn2(5) && !strict)
#ifdef WIZARD
						|| (wizard && !strict)
#endif
							)
			return sroom;
	}
	return (struct mkroom *)0;
}

STATIC_OVL void
mkzoo(type)
int type;
{
	register struct mkroom *sroom;

	if (type == BADFOODSHOP) {
	   if ((sroom = pick_room(TRUE)) != 0) {
		sroom->rtype = type;
		fill_zoo(sroom);
	   }
	}
	else if ((sroom = pick_room(FALSE)) != 0) {
		sroom->rtype = type;
		fill_zoo(sroom);
	}
}

void
mk_zoo_thronemon(x,y)
int x,y;
{
    int i = rnd(level_difficulty());
    int pm = (i > 9) ? PM_OGRE_KING
        : (i > 5) ? PM_ELVENKING
        : (i > 2) ? PM_DWARF_KING
        : PM_GNOME_KING;
    struct monst *mon = makemon(&mons[pm], x, y, NO_MM_FLAGS);

    if (mon) {
        mon->msleeping = 1;
        mon->mpeaceful = 0;
        set_malign(mon);
        /* Give him a sceptre to pound in judgment */
        (void) mongets(mon, ORNATE_MACE);
    }
}

void
fill_zoo(sroom)
struct mkroom *sroom;
{
	struct monst *mon;
	struct monst *randomon;

	register int sx,sy,i;
	int sh, tx, ty, goldlim, type = sroom->rtype;
	int rmno = (sroom - rooms) + ROOMOFFSET;
	coord mm;

	int moreorless;

#ifdef GCC_WARN
	tx = ty = goldlim = 0;
#endif

	sh = sroom->fdoor;
	switch(type) {
	    case GARDEN:
		mkgarden(sroom);
		/* mkgarden() sets flags and we don't want other fillings */
		return; 
	    case COURT:
	    case GIANTCOURT:
		if(level.flags.is_maze_lev) {
		    for(tx = sroom->lx; tx <= sroom->hx; tx++)
			for(ty = sroom->ly; ty <= sroom->hy; ty++)
			    if(IS_THRONE(levl[tx][ty].typ))
				goto throne_placed;
		}

		i = 100;
		do {	/* don't place throne on top of stairs */
			(void) somexy(sroom, &mm);
			tx = mm.x; ty = mm.y;
		} while (occupied((xchar)tx, (xchar)ty) && --i > 0);
	    throne_placed:
		if (type == COURT) mk_zoo_thronemon(tx, ty);
		break;
	    case BEEHIVE:
	    case MIGOHIVE:
		tx = sroom->lx + (sroom->hx - sroom->lx + 1)/2;
		ty = sroom->ly + (sroom->hy - sroom->ly + 1)/2;
		if(sroom->irregular) {
		    /* center might not be valid, so put queen elsewhere */
		    if ((int) levl[tx][ty].roomno != rmno ||
			    levl[tx][ty].edge) {
			(void) somexy(sroom, &mm);
			tx = mm.x; ty = mm.y;
		    }
		}
		break;
	    case ZOO:
	    case LEPREHALL:
		case COINHALL:
		goldlim = 500 * level_difficulty();
		break;
		case CLINIC:
		case TERRORHALL:
		/*case GRUEROOM:*/
		    break;
	    case DRAGONLAIR:
		goldlim = 1500 * level_difficulty();
		break;
	}

	moreorless = (rnd(10) + 1);

	for(sx = sroom->lx; sx <= sroom->hx; sx++)
	    for(sy = sroom->ly; sy <= sroom->hy; sy++) {
		if(sroom->irregular) {
		    if ((int) levl[sx][sy].roomno != rmno ||
			  levl[sx][sy].edge ||
			  (sroom->doorct &&
			   distmin(sx, sy, doors[sh].x, doors[sh].y) <= 1))
			continue;
		} else if(!SPACE_POS(levl[sx][sy].typ) ||
			  (sroom->doorct &&
			   ((sx == sroom->lx && doors[sh].x == sx-1) ||
			    (sx == sroom->hx && doors[sh].x == sx+1) ||
			    (sy == sroom->ly && doors[sh].y == sy-1) ||
			    (sy == sroom->hy && doors[sh].y == sy+1))))
		    continue;
		/* don't place monster on explicitly placed throne */
		if(type == COURT && IS_THRONE(levl[sx][sy].typ))
		    continue;
               /* armories (and clinics) don't contain as many monsters */
		if (((type != ARMORY && type != CLINIC) || rn2(2)) &&
				!(Role_if(PM_NOBLEMAN) && In_quest(&u.uz))){ mon = makemon(
		    (type == COURT) ? courtmon() :
		    (type == BARRACKS) ? squadmon() :
		    (type == CLINIC) ?
			(sx == tx && sy == ty ? &mons[PM_HEALER] :
			 &mons[PM_NURSE]) :
		    (type == TERRORHALL) ? mkclass(S_UMBER,0) :
		    (type == COINHALL) ? mkclass(S_BAD_COINS,0) :
		    /*(type == GRUEROOM) ? mkclass(S_GRUE,0) :*/
		    (type == MORGUE) ? morguemon() :
		    (type == FUNGUSFARM) ? fungus() :
		    (type == BEEHIVE) ?
			(sx == tx && sy == ty ? &mons[PM_QUEEN_BEE] :
			 &mons[PM_KILLER_BEE]) :
		    (type == DOUGROOM) ? douglas_adams_mon() : 
		    (type == LEPREHALL) ? /*&mons[PM_LEPRECHAUN]*/mkclass(S_LEPRECHAUN,0) :
		    (type == COCKNEST) ? 
		    	(rn2(4) ? &mons[PM_COCKATRICE] :
		    	 &mons[PM_CHICKATRICE]) :
                    (type == ARMORY) ? (rn2(10) ? mkclass(S_RUSTMONST,0) :
			&mons[PM_BROWN_PUDDING]) :
                    (type == ANTHOLE) ? 
		        (sx == tx && sy == ty ? &mons[PM_QUEEN_ANT] :
			 antholemon()) :
		    (type == DRAGONLAIR) ? mkclass(S_DRAGON,0) :
		    (type == LEMUREPIT)? 
		    	(!rn2(10)? &mons[PM_HORNED_DEVIL] : 
			           &mons[PM_LEMURE]) :
		    (type == MIGOHIVE)?
		      (sx == tx && sy == ty? &mons[PM_MIGO_QUEEN] :
	              (rn2(2)? &mons[PM_MIGO_DRONE] : &mons[PM_MIGO_WARRIOR])) :
		    (type == BADFOODSHOP) ? mkclass(S_BAD_FOOD,0) :
		    (type == REALZOO) ? realzoomon() :
		    (type == GIANTCOURT) ? mkclass(S_GIANT,0) :
		    (struct permonst *) 0,
		   sx, sy, NO_MM_FLAGS);
		} else mon = ((struct monst *)0);
/* some rooms can spawn new monster variants now --Amy */
		if(mon) {
			mon->msleeping = 1;
			if (/*type==COURT && */mon->mpeaceful) { /*enemies in these rooms will always be hostile now --Amy*/
				mon->mpeaceful = 0;
				set_malign(mon);
			}
		}
		switch(type) {
		    case ZOO:
		    case DRAGONLAIR:
		    case LEPREHALL:
			if(sroom->doorct)
			{
			    int distval = dist2(sx,sy,doors[sh].x,doors[sh].y);
			    i = sq(distval);
			}
			else
			    i = goldlim;
			if(i >= goldlim) i = 5*level_difficulty();
			goldlim -= i;
			(void) mkgold((long) rn1(i, 10), sx, sy);
			break;
		    case MORGUE:
			if(!(Role_if(PM_NOBLEMAN) && In_quest(&u.uz) )){
			if(!rn2(5))
			    (void) mk_tt_object(CORPSE, sx, sy);
			if(!rn2(10))	/* lots of treasure buried with dead */
			    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST,
					     sx, sy, TRUE, FALSE);
			if (!rn2(5))
			    make_grave(sx, sy, (char *)0);
			} else if(rn2(2)) make_grave(sx, sy, (char *)0);
			break;
		    case BEEHIVE:
			if(!rn2(3))
			    (void) mksobj_at(LUMP_OF_ROYAL_JELLY,
					     sx, sy, TRUE, FALSE);
			break;
		    case FUNGUSFARM:
			if (!rn2(3))
			    (void) mksobj_at(SLIME_MOLD, sx, sy, TRUE, FALSE);
			break;
		    case MIGOHIVE:
			switch (rn2(10)) {
			    case 9:
				mksobj_at(DIAMOND, sx, sy, TRUE, FALSE);
				break;
			    case 8:
				mksobj_at(RUBY, sx, sy, TRUE, FALSE);
				break;
			    case 7:
			    case 6:
				mksobj_at(AGATE, sx, sy, TRUE, FALSE);
				break;
			    case 5:
			    case 4:
				mksobj_at(FLUORITE, sx, sy, TRUE, FALSE);
				break;
			    default:
				break;
			}
			break;
		    case BARRACKS:
			if(!rn2(20))	/* the payroll and some loot */
			    (void) mksobj_at((rn2(3)) ? LARGE_BOX : CHEST,
					     sx, sy, TRUE, FALSE);
			if (!rn2(5))
			    make_grave(sx, sy, (char *)0);
			break;
		    case CLINIC: /* 5lo: Rare rooms, lets give them a lot of healing objects. */
			if(!rn2(5))
			    (void) mksobj_at(POT_HEALING,sx,sy,TRUE,FALSE);
			if(!rn2(10))
			    (void) mksobj_at(POT_EXTRA_HEALING,sx,sy,TRUE,FALSE);
			if(!rn2(20))
			    (void) mksobj_at(POT_FULL_HEALING,sx,sy,TRUE,FALSE);
			if(!rn2(30))
			    (void) mksobj_at(POT_RECOVERY,sx,sy,TRUE,FALSE);
			/* Now some wands... */
			if(!rn2(10))
			    (void) mksobj_at(WAN_HEALING,sx,sy,TRUE,FALSE);
			if(!rn2(20))
			    (void) mksobj_at(WAN_EXTRA_HEALING,sx,sy,TRUE,FALSE);
			/* And for misc healing objects */
			if(!rn2(10))
			    (void) mksobj_at(PILL,sx,sy,TRUE,FALSE);
			if(!rn2(40))
			    (void) mksobj_at(MEDICAL_KIT,sx,sy,TRUE,FALSE);
			break;
		    case COCKNEST:
			if(!rn2(3)) {
			    struct obj *sobj = mk_tt_object(STATUE, sx, sy);

			    if (sobj) {
				for (i = rn2(5); i; i--)
				    (void) add_to_container(sobj,
						mkobj(RANDOM_CLASS, FALSE));
				sobj->owt = weight(sobj);
			    }
			}
			break;
		    case ARMORY:
			{
				struct obj *otmp;
				if (!rn2(5)) { /* sorry Patrick, but the quantity of those items needs to be lower. --Amy */
					if (rn2(2))
						otmp = mkobj_at(WEAPON_CLASS, sx, sy, FALSE);
					else
						otmp = mkobj_at(ARMOR_CLASS, sx, sy, FALSE);
					otmp->spe = 0;
					if (is_rustprone(otmp)) otmp->oeroded = rn2(4);
					else if (is_rottable(otmp)) otmp->oeroded2 = rn2(4);
				}
			}
			break;
		    case ANTHOLE:
			if(!rn2(3))
			    (void) mkobj_at(FOOD_CLASS, sx, sy, FALSE);
			break;
		}
	    }
	switch (type) {
	      case COURT:
	      case GIANTCOURT:
		{
		  struct obj *chest;
		  levl[tx][ty].typ = THRONE;
		  (void) somexy(sroom, &mm);
		  (void) mkgold((long) rn1(50 * level_difficulty(),10), mm.x, mm.y);
		  /* the royal coffers */
		  chest = mksobj_at(CHEST, mm.x, mm.y, TRUE, FALSE);
		  chest->spe = 2; /* so it can be found later */
		  level.flags.has_court = 1;
		  break;
		}
	      case BARRACKS:
		  level.flags.has_barracks = 1;
		  break;
	      case REALZOO:              
	      case ZOO:
		  level.flags.has_zoo = 1;
		  break;
	      case MORGUE:
		  level.flags.has_morgue = 1;
		  break;
	      case SWAMP:
		  level.flags.has_swamp = 1;
		  break;
	      case BEEHIVE:
		  level.flags.has_beehive = 1;
		  break;
	      case DOUGROOM:
		  level.flags.has_zoo = 1;
		  break;
	      case LEMUREPIT:
		  level.flags.has_lemurepit = 1;
		  break;
	      case MIGOHIVE:
		  level.flags.has_migohive = 1;
		  break;
	      case FUNGUSFARM:
		  level.flags.has_fungusfarm = 1;
		  break;
            case CLINIC:
              level.flags.has_clinic = 1;
              break;
            case TERRORHALL:
              level.flags.has_terrorhall = 1;
              break;
            case COINHALL:
              level.flags.has_coinhall = 1;
              break;
            case TRAPROOM:
              level.flags.has_traproom = 1;
              break;
#if 0 /* Deferred for now */
            case GRUEROOM:
              level.flags.has_grueroom = 1;
              break;
#endif
            case POOLROOM:
              level.flags.has_poolroom = 1;
              break;
	}
}

/* make a swarm of undead around mm */
void
mkundead(mm, revive_corpses, mm_flags)
coord *mm;
boolean revive_corpses;
int mm_flags;
{
	int cnt = 1;
	if (!rn2(2)) cnt = (level_difficulty() + 1)/10;
	if (!rn2(5)) cnt += rnd(5);
	if (cnt < 1) cnt = 1;
	struct permonst *mdat;
	struct obj *otmp;
	coord cc;

	while (cnt--) {
	    mdat = morguemon();
	    if (enexto(&cc, mm->x, mm->y, mdat) &&
		    (!revive_corpses ||
		     !(otmp = sobj_at(CORPSE, cc.x, cc.y)) ||
		     !revive(otmp)))
		(void) makemon(mdat, cc.x, cc.y, mm_flags);
	}
	level.flags.graveyard = TRUE;	/* reduced chance for undead corpse */
}

/*void
mkundeadboo(mm, revive_corpses, mm_flags)
coord *mm;
boolean revive_corpses;
int mm_flags;
{
	struct monst *nmonst;

	int cnt = 1;
	if (!rn2(2)) cnt = (level_difficulty() + 1)/10;
	if (!rn2(5)) cnt += rnd(5);
	if (cnt < 1) cnt = 1;
	int mdat;
	struct obj *otmp;
	coord cc;

	while (cnt--) {
	    mdat = PM_UNDEAD_ARCHEOLOGIST + rn2(PM_UNDEAD_WIZARD - PM_UNDEAD_ARCHEOLOGIST + 1);
	    if (enexto(&cc, mm->x, mm->y, youmonst.data) &&
		    (!revive_corpses ||
		     !(otmp = sobj_at(CORPSE, cc.x, cc.y)) ||
		     !revive(otmp)))

		nmonst = makemon(&mons[mdat], cc.x, cc.y, mm_flags);
		tt_mname(nmonst);
	}
}*/

STATIC_OVL struct permonst *
morguemon()
{
	register int i = rn2(100), hd = rn2(level_difficulty());

	if(hd > 10 && i < 10)
		/* 5lo: Generate liches instead of demons for chaotic quest */
		return(Is_chaotic_quest(&u.uz) ? mkclass(S_LICH,0) :
			(Inhell || In_endgame(&u.uz)) ? mkclass(S_DEMON,0) :
					&mons[ndemon(A_NONE)]);
	if(hd > 8 && i > 85)
		return(mkclass(S_VAMPIRE,0));

	return((i < 20) ? &mons[PM_GHOST]
			: (i < 40) ? mkclass(S_WRAITH,0) : (i < 70) ? mkclass(S_MUMMY,0) : mkclass(S_ZOMBIE,0));
} /* added mummies, enabled all of S_wraith type monsters --Amy */

struct permonst *
antholemon()
{
	int mtyp;

	/* Same monsters within a level, different ones between levels */
	switch ((level_difficulty() + ((long)u.ubirthday)) % 4) {
	default:	mtyp = PM_GIANT_ANT; break;
	case 0:		mtyp = PM_SOLDIER_ANT; break;
	case 1:		mtyp = PM_BLACK_ANT; break;
	case 2:		mtyp = PM_FIRE_ANT; break;
	case 3:		mtyp = PM_SNOW_ANT; break;
	}
	return ((mvitals[mtyp].mvflags & G_GONE) ?
			(struct permonst *)0 : &mons[mtyp]);
}


STATIC_OVL struct permonst *
fungus()
{
	register int i, hd = level_difficulty(), mtyp = 0;

	i = rn2(hd > 20 ? 17 : hd > 12 ? 14 : 12);

	switch (i) {
	case 0:
	case 1: mtyp = PM_LICHEN; 		break;	
	case 2: mtyp = PM_BROWN_MOLD;		break;
	case 3: mtyp = PM_YELLOW_MOLD;		break;
	case 4: mtyp = PM_GREEN_MOLD;		break;
	case 5: mtyp = PM_RED_MOLD;		break;
	case 6: mtyp = PM_SHRIEKER;		break;
	case 7: mtyp = PM_VIOLET_FUNGUS;	break;
	case 8: mtyp = PM_BLUE_JELLY;		break;
	case 9: mtyp = PM_DISGUSTING_MOLD;	break;
	case 10: mtyp = PM_BLACK_MOLD;		break;
	case 11: mtyp = PM_GRAY_OOZE;		break;
	/* Following only after level 12... */
	case 12: mtyp = PM_SPOTTED_JELLY;	break;
	case 13: mtyp = PM_BROWN_PUDDING;	break;
	/* Following only after level 20... */
	case 14: mtyp = PM_GREEN_SLIME;		break;
	case 15: mtyp = PM_BLACK_PUDDING;	break;
	case 16: mtyp = PM_OCHRE_JELLY;		break;
	}

	return ((mvitals[mtyp].mvflags & G_GONE) ?
			(struct permonst *)0 : &mons[mtyp]);
}

/** Create a special room with trees, fountains and nymphs.
 * @author Pasi Kallinen
 */
STATIC_OVL void
mkgarden(croom)
struct mkroom *croom; /* NULL == choose random room */
{
    register int tryct = 0;
    boolean maderoom = FALSE;
    coord pos;
    register int i, tried;

    while ((tryct++ < 25) && !maderoom) {
	register struct mkroom *sroom = croom ? croom : &rooms[rn2(nroom)];
	
	if (sroom->hx < 0 || (!croom && (sroom->rtype != OROOM ||
	    !sroom->rlit || has_upstairs(sroom) || has_dnstairs(sroom))))
	    	continue;

	sroom->rtype = GARDEN;
	maderoom = TRUE;
	level.flags.has_garden = 1;

	tried = 0;
	i = rn1(5,3);
	while ((tried++ < 50) && (i >= 0) && somexy(sroom, &pos)) {
	    struct permonst *pmon;
	    if (!MON_AT(pos.x, pos.y) && (pmon = mkclass(S_NYMPH,0))) {
		struct monst *mtmp = makemon(pmon, pos.x,pos.y, NO_MM_FLAGS);
		mtmp->msleeping = 1;
		i--;
	    }
	}
	tried = 0;
	i = rn1(3,3);
	while ((tried++ < 50) && (i >= 0) && somexy(sroom, &pos)) {
	    if (levl[pos.x][pos.y].typ == ROOM && !MON_AT(pos.x,pos.y) &&
		!nexttodoor(pos.x,pos.y)) {
		if (rn2(3))
		  levl[pos.x][pos.y].typ = TREE;
		else {
		    levl[pos.x][pos.y].typ = FOUNTAIN;
		    level.flags.nfountains++;
		}
		i--;
	    }
	}
    }
}


STATIC_OVL void
mkswamp()	/* Michiel Huisjes & Fred de Wilde */
{
	register struct mkroom *sroom;
	register int sx,sy,i,eelct = 0;

	for(i=0; i<5; i++) {		/* turn up to 5 rooms swampy */
		sroom = &rooms[rn2(nroom)];
		if(sroom->hx < 0 || sroom->rtype != OROOM ||
		   has_upstairs(sroom) || has_dnstairs(sroom))
			continue;

		/* satisfied; make a swamp */
		sroom->rtype = SWAMP;
		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if(!OBJ_AT(sx, sy) &&
		   !MON_AT(sx, sy) && !t_at(sx,sy) && !nexttodoor(sx,sy)) {
		    if((sx+sy)%2) {
			levl[sx][sy].typ = POOL;
			if(!eelct || !rn2(4)) {
			    /* mkclass() won't do, as we might get kraken */
/* comment by Amy - low-level players shouldn't move close to water anyway, so I will totally spawn everything here! */
			    (void) makemon(rn2(3) ? mkclass(S_EEL,0)
						  : rn2(5) ? &mons[PM_GIANT_EEL]
						  : rn2(2) ? &mons[PM_PIRANHA]
						  : &mons[PM_ELECTRIC_EEL],
						sx, sy, NO_MM_FLAGS);
			    eelct++;
			}
		    } else {
			levl[sx][sy].typ = PUDDLE;
			if(!rn2(4))	/* swamps tend to be moldy */
			    (void) makemon(mkclass(S_FUNGUS,0),
						sx, sy, NO_MM_FLAGS);
		    else if(!rn2(16)){
			struct monst * mtmp = makemon(rn2(3) ? &mons[PM_WILL_O__WISP] 
			  : &mons[PM_GUIDE],
			  sx, sy, NO_MM_FLAGS);
			if (mtmp->data == &mons[PM_GUIDE]){
			    mongets(mtmp, WATER_WALKING_BOOTS);
			    m_dowear(mtmp, TRUE);
			}
		    }
		    } 
		}
		level.flags.has_swamp = 1;
	}
}

STATIC_OVL coord *
shrine_pos(roomno)
int roomno;
{
	static coord buf;
	struct mkroom *troom = &rooms[roomno - ROOMOFFSET];

	buf.x = troom->lx + ((troom->hx - troom->lx) / 2);
	buf.y = troom->ly + ((troom->hy - troom->ly) / 2);
	return(&buf);
}

STATIC_OVL void
mktemple()
{
	register struct mkroom *sroom;
	coord *shrine_spot;
	register struct rm *lev;

	if(!(sroom = pick_room(TRUE))) return;

	/* set up Priest and shrine */
	sroom->rtype = TEMPLE;
	/*
	 * In temples, shrines are blessed altars
	 * located in the center of the room
	 */
	shrine_spot = shrine_pos((sroom - rooms) + ROOMOFFSET);
	lev = &levl[shrine_spot->x][shrine_spot->y];
	lev->typ = ALTAR;
	lev->altarmask = induced_align(80);
	priestini(&u.uz, sroom, shrine_spot->x, shrine_spot->y, FALSE);
	lev->altarmask |= AM_SHRINE;
	level.flags.has_temple = 1;
}

boolean
nexttodoor(sx,sy)
register int sx, sy;
{
	register int dx, dy;
	register struct rm *lev;
	for(dx = -1; dx <= 1; dx++) for(dy = -1; dy <= 1; dy++) {
		if(!isok(sx+dx, sy+dy)) continue;
		if(IS_DOOR((lev = &levl[sx+dx][sy+dy])->typ) ||
		    lev->typ == SDOOR)
			return(TRUE);
	}
	return(FALSE);
}

boolean
has_dnstairs(sroom)
register struct mkroom *sroom;
{
	if (sroom == dnstairs_room)
		return TRUE;
	if (sstairs.sx && !sstairs.up)
		return((boolean)(sroom == sstairs_room));
	return FALSE;
}

boolean
has_upstairs(sroom)
register struct mkroom *sroom;
{
	if (sroom == upstairs_room)
		return TRUE;
	if (sstairs.sx && sstairs.up)
		return((boolean)(sroom == sstairs_room));
	return FALSE;
}

#endif /* OVLB */
#ifdef OVL0

int
somex(croom)
register struct mkroom *croom;
{
	return rn2(croom->hx-croom->lx+1) + croom->lx;
}

int
somey(croom)
register struct mkroom *croom;
{
	return rn2(croom->hy-croom->ly+1) + croom->ly;
}

boolean
inside_room(croom, x, y)
struct mkroom *croom;
xchar x, y;
{
	return((boolean)(x >= croom->lx-1 && x <= croom->hx+1 &&
		y >= croom->ly-1 && y <= croom->hy+1));
}

boolean
somexy(croom, c)
struct mkroom *croom;
coord *c;
{
	int try_cnt = 0;
	int i;

	if (croom->irregular) {
	    i = (croom - rooms) + ROOMOFFSET;

	    while(try_cnt++ < 100) {
		c->x = somex(croom);
		c->y = somey(croom);
		if (!levl[c->x][c->y].edge &&
			(int) levl[c->x][c->y].roomno == i)
		    return TRUE;
	    }
	    /* try harder; exhaustively search until one is found */
	    for(c->x = croom->lx; c->x <= croom->hx; c->x++)
		for(c->y = croom->ly; c->y <= croom->hy; c->y++)
		    if (!levl[c->x][c->y].edge &&
			    (int) levl[c->x][c->y].roomno == i)
			return TRUE;
	    return FALSE;
	}

	if (!croom->nsubrooms) {
		c->x = somex(croom);
		c->y = somey(croom);
		return TRUE;
	}

	/* Check that coords doesn't fall into a subroom or into a wall */

	while(try_cnt++ < 100) {
		c->x = somex(croom);
		c->y = somey(croom);
		if (IS_WALL(levl[c->x][c->y].typ))
		    continue;
		for(i=0 ; i<croom->nsubrooms;i++)
		    if(inside_room(croom->sbrooms[i], c->x, c->y))
			goto you_lose;
		break;
you_lose:	;
	}
	if (try_cnt >= 100)
	    return FALSE;
	return TRUE;
}

/*
 * Search for a special room given its type (zoo, court, etc...)
 *	Special values :
 *		- ANY_SHOP
 *		- ANY_TYPE
 */

struct mkroom *
search_special(type)
schar type;
{
	register struct mkroom *croom;

	for(croom = &rooms[0]; croom->hx >= 0; croom++)
	    if((type == ANY_TYPE && croom->rtype != OROOM) ||
	       (type == ANY_SHOP && croom->rtype >= SHOPBASE) ||
	       croom->rtype == type)
		return croom;
	for(croom = &subrooms[0]; croom->hx >= 0; croom++)
	    if((type == ANY_TYPE && croom->rtype != OROOM) ||
	       (type == ANY_SHOP && croom->rtype >= SHOPBASE) ||
	       croom->rtype == type)
		return croom;
	return (struct mkroom *) 0;
}

#endif /* OVL0 */
#ifdef OVLB

struct permonst *
courtmon()
{
	int     i = rn2(60) + rn2(3*level_difficulty());
	if (i > 200)            return(mkclass(S_DRAGON,0));
	else if (i > 130)       return(mkclass(S_GIANT,0));
	else if (i > 85)	return(mkclass(S_TROLL,0));
	else if (i > 75)	return(mkclass(S_CENTAUR,0));
	else if (i > 60)	return(mkclass(S_ORC,0));
	else if (i > 45)	return(&mons[PM_BUGBEAR]);
	else if (i > 30)	return(&mons[PM_HOBGOBLIN]);
	else if (i > 15)	return(mkclass(S_GNOME,0));
	else			return(mkclass(S_KOBOLD,0));
}

struct permonst *
douglas_adams_mon()
{
	int     i = rn2(60);
	if (i > 55) return(&mons[PM_RAVENOUS_BUGBLATTER_BEAST_OF_TRAAL]);
	else if (i > 54)        return(&mons[PM_MARVIN]);
	else if (i > 46)        return(&mons[PM_CREEPING___]);
	else if (i > 26)        return(&mons[PM_MICROSCOPIC_SPACE_FLEET]);
	else if (i > 20)        return(&mons[PM_VOGON]);
	else if (i > 19)        return(&mons[PM_VOGON_LORD]);
	else if (i > 2)        return(&mons[PM_BABELFISH]);
	else                    return(&mons[PM_ALGOLIAN_SUNTIGER]);
}

struct permonst *
realzoomon()
{
	int     i = rn2(60) + rn2(3*level_difficulty());
	if (i > 175)    return(&mons[PM_JUMBO_THE_ELEPHANT]);
	else if (i > 115)       return(&mons[PM_MASTODON]);
	else if (i > 85)        return(&mons[PM_PYTHON]);
	else if (i > 70)        return(&mons[PM_MUMAK]);
	else if (i > 55)        return(&mons[PM_TIGER]);
	else if (i > 45)        return(&mons[PM_PANTHER]);
	else if (i > 25)        return(&mons[PM_JAGUAR]);
	else if (i > 15)        return(&mons[PM_APE]);
	else                    return(&mons[PM_MONKEY]);
}

#define NSTYPES (PM_CAPTAIN - PM_SOLDIER + 1)

static struct {
    unsigned	pm;
    unsigned	prob;
} squadprob[NSTYPES] = {
    {PM_SOLDIER, 80}, {PM_SERGEANT, 15}, {PM_LIEUTENANT, 4}, {PM_CAPTAIN, 1}
};

STATIC_OVL struct permonst *
squadmon()		/* return soldier types. */
{
	int sel_prob, i, cpro, mndx;

	sel_prob = rnd(80+level_difficulty());

	cpro = 0;
	for (i = 0; i < NSTYPES; i++) {
	    cpro += squadprob[i].prob;
	    if (cpro > sel_prob) {
		mndx = squadprob[i].pm;
		goto gotone;
	    }
	}
	mndx = squadprob[rn2(NSTYPES)].pm;
gotone:
	if (!(mvitals[mndx].mvflags & G_GONE)) return(&mons[mndx]);
	else			    return((struct permonst *) 0);
}

/*
 * save_room : A recursive function that saves a room and its subrooms
 * (if any).
 */

STATIC_OVL void
save_room(fd, r)
int	fd;
struct mkroom *r;
{
	short i;
	/*
	 * Well, I really should write only useful information instead
	 * of writing the whole structure. That is I should not write
	 * the subrooms pointers, but who cares ?
	 */
	bwrite(fd, (genericptr_t) r, sizeof(struct mkroom));
	for(i=0; i<r->nsubrooms; i++)
	    save_room(fd, r->sbrooms[i]);
}

/*
 * save_rooms : Save all the rooms on disk!
 */

void
mktraproom()
{
    struct mkroom *sroom;
    struct rm *lev;
    int area, ttyp, ntraps;
    int idx = (level_difficulty() + ((long)u.ubirthday)) % 9;

    if(!(sroom = pick_room(TRUE))) return;

    sroom->rtype = TRAPROOM;

    if (!rn2(10)) idx = rn2(10); /* occasionally give anything if called twice on same level */

    area = ((sroom->hx - sroom->lx + 1) * (sroom->hy - sroom->ly + 1));
    ntraps = rn2(area/3) + (area/4);

    while (ntraps-- > 0) {
	switch (idx) {
	default: ttyp = LANDMINE; break;
	case 0:  ttyp = ROLLING_BOULDER_TRAP; ntraps--; break;
	case 1:  ttyp = rn2(2) ? PIT : SPIKED_PIT; break;
	case 2:  ttyp = WEB; break;
	case 3:  ttyp = rn2(3) ? ROCKTRAP : COLLAPSE_TRAP; break;
	case 4:  ttyp = FIRE_TRAP; break;
	case 5:  ttyp = rn2(5) ? TRAPDOOR : HOLE; break;
	case 6:  ttyp = STATUE_TRAP; break;
	case 7:  ttyp = rn2(2) ? DART_TRAP : ARROW_TRAP; break;
	}
	mktrap(ttyp, 0, sroom, NULL);
    }

}

void
mkpoolroom()
{
    struct mkroom *sroom;
    schar typ;
    register int sx,sy = 0;

    if (!(sroom = pick_room(TRUE))) return;

    if(sroom->rtype != OROOM || has_upstairs(sroom) || has_dnstairs(sroom)) return;

    sroom->rtype = POOLROOM;

    typ = !rn2(5) ? POOL : LAVAPOOL;

    for(sx = sroom->lx; sx <= sroom->hx; sx++)
    for(sy = sroom->ly; sy <= sroom->hy; sy++)
	if(!OBJ_AT(sx, sy) &&
	   !MON_AT(sx, sy) && !t_at(sx,sy) && !nexttodoor(sx,sy))
		levl[sx][sy].typ = typ;

	level.flags.has_poolroom = 1;
}
#if 0
void
mkstatueroom()
{
    struct mkroom *sroom;
    schar typ;
	register int sx,sy,i = 0;

    if (!(sroom = pick_room(TRUE))) return;

	if(sroom->rtype != OROOM || has_upstairs(sroom) || has_dnstairs(sroom)) return;

    sroom->rtype = STATUEROOM;

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		if(!OBJ_AT(sx, sy) &&
		   !MON_AT(sx, sy) && !t_at(sx,sy) /*&& !nexttodoor(sx,sy)*/) {
		    if(rn2(2)) 
				(void) maketrap(sx, sy, STATUE_TRAP);
		}

		for(sx = sroom->lx; sx <= sroom->hx; sx++)
		for(sy = sroom->ly; sy <= sroom->hy; sy++)
		    if(rn2(2)) 
			{
			    struct obj *sobj = mksobj_at(STATUE, sx, sy, TRUE, FALSE);

			    if (sobj) {
				for (i = rn2(5); i; i--)
				    (void) add_to_container(sobj,
						mkobj(RANDOM_CLASS, FALSE));
				sobj->owt = weight(sobj);
			    }
			}

	level.flags.has_statueroom = 1;

}
#endif
void
save_rooms(fd)
int fd;
{
	short i;

	/* First, write the number of rooms */
	bwrite(fd, (genericptr_t) &nroom, sizeof(nroom));
	for(i=0; i<nroom; i++)
	    save_room(fd, &rooms[i]);
}

STATIC_OVL void
rest_room(fd, r)
int fd;
struct mkroom *r;
{
	short i;

	mread(fd, (genericptr_t) r, sizeof(struct mkroom));
	for(i=0; i<r->nsubrooms; i++) {
		r->sbrooms[i] = &subrooms[nsubroom];
		rest_room(fd, &subrooms[nsubroom]);
		subrooms[nsubroom++].resident = (struct monst *)0;
	}
}

/*
 * rest_rooms : That's for restoring rooms. Read the rooms structure from
 * the disk.
 */

void
rest_rooms(fd)
int	fd;
{
	short i;

	mread(fd, (genericptr_t) &nroom, sizeof(nroom));
	nsubroom = 0;
	for(i = 0; i<nroom; i++) {
	    rest_room(fd, &rooms[i]);
	    rooms[i].resident = (struct monst *)0;
	}
	rooms[nroom].hx = -1;		/* restore ending flags */
	subrooms[nsubroom].hx = -1;
}
#endif /* OVLB */

/*mkroom.c*/
