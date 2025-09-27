/*
 * standby_mode.h
 *
 *  Created on: 27-Sep-2025
 *      Author: vigneshv
 */

#ifndef STANDBY_MODE_H_
#define STANDBY_MODE_H_

#define READ_REG(REG)  ((REG))
#define WRITE_REG(REG, VAL)  				((REG) = (VAL))
#define READ_BIT(REG, BIT) 					((REG) & BIT)
#define MODIFY_REG(REG, CLEARMASK, SETMASK) WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

uint32_t get_wakeup_pin_state(void);
void set_power_mode(uint32_t pwr_mode);
void standby_wakeup_pin_setup(void);

#endif /* STANDBY_MODE_H_ */
