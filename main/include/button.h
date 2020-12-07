//
// Created by derk on 21-9-20.
//

#ifndef BUTTON_H
#define BUTTON_H

#define RESET_CONNECTION_BUTTON_GPIO GPIO_NUM_0

void setup_reset_button(void);
void wait_until_reset_button_pressed(void);

#endif //BUTTON_H
