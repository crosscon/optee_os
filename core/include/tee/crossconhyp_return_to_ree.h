#ifndef __crossconhyp_RETURN_TO_REE_H
#define __crossconhyp_RETURN_TO_REE_H

/*
* @brief Handles the return of the cold boot, standard call or RPC call
* @details Called from assembly only.
*/
void crossconhyp_return_to_ree(unsigned long a0, unsigned long a1, unsigned long a2,
			       unsigned long a3, unsigned long a4, unsigned long a5,
			       unsigned long a6, unsigned long a7);

#endif
