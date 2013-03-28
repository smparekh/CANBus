/* 
 * File:   CanDash.h
 * Author: David
 *
 * Created on March 19, 2013, 8:41 PM
 */

#ifndef CANDASH_H
#define	CANDASH_H
#define  CAN_CONFIG_MODE 0
#define  CAN_NORMAL_MODE 1
#ifdef	__cplusplus
extern "C" {
#endif

/*Status Functions that */
int GetCanMode(int canMode);
void SetCanConfigMode(int configmode);
void initialize_can(void);


#ifdef	__cplusplus
}
#endif

#endif	/* CANDASH_H */

