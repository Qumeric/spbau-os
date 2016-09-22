#ifndef __PIT_H__
#define __PIT_H__

#include <memory.h>
#include <ioport.h>

void initialize_pit()
{
    uint8_t data = 0;
    
    // timer type
    data += (2 << 1);

    // init size
    data += (3 << 4);

    // 0 channel
    data += (0 << 6);

    out8(pit_command_port, data);
    
    // sending initial value
    out8(pit_0_data_port, (1 << 8) - 1);
    out8(pit_0_data_port, (1 << 8) - 1);    

    // reveal pit to master
    out8(master_data_port, ((1 << 8) - 1) - 1);
}

#endif /* PIT_H */

