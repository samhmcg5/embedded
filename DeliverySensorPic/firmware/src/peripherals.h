#ifndef _PERIPHERALS_H
#define _PERIPHERALS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

#define MAGNET_STATE_OFF 0
#define MAGNET_STATE_ON 1
#define ADC_STATE_OFF false
#define ADC_STATE_ON true

typedef enum
{
	/* Application's state machine's initial state. */
	PERIPHERALS_STATE_INIT=0,
	PERIPHERALS_STATE_SERVICE_TASKS,

	/* TODO: Define states used by the application state machine. */

} PERIPHERALS_STATES;


typedef struct
{
    /* The application's current state */
    PERIPHERALS_STATES state;

    /* TODO: Define any additional data used by the application. */

} PERIPHERALS_DATA;


void PERIPHERALS_Initialize ( void );
void PERIPHERALS_Tasks( void );

bool adc_state;
int objDist;
bool objHeld;

#endif /* _PERIPHERALS_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

