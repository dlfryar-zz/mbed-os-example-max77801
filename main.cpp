/**********************************************************************
* Copyright (C) 2017 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
**********************************************************************/

#include "mbed.h"
#include "max77801.h"

Serial uart_main(MAIN_TX, MAIN_RX, 115200);
DigitalInOut cell_power_control(CELL_PWR_EN, PIN_OUTPUT, PullDown, 0);

int main()
{
    int c;
    int32_t rData;
    double vout_l = 3.3000;
    double vout_h = 3.7500;

    DigitalOut rLED(STATUS_LED, 0);
    DigitalOut gLED(STATUS_LED, 0);
    DigitalOut bLED(STATUS_LED, 0);

    I2C i2cBus(DCDC_I2C_SDA, DCDC_I2C_SCL);
    i2cBus.frequency(400000);

    MAX77801 max77801(&i2cBus);
    wait_ms(100);

    cell_power_control.write(1); // turn on max77801
    wait(1);

    rData = max77801.init();
    if (rData < 0)
    {
        uart_main.printf("MAX77801 Fail to Init. Stopped\r\n");
        rLED = true;
    }
    else
    {
        uart_main.printf("MAX77801 Init Done\r\n");
    }

    while (1)
    {
        gLED = true;
        rLED = false;
        uart_main.printf("----- [ MAX77801 DEMO ] Usage -----  \r\n");
        uart_main.printf("i : Set Registers to POR Staus \r\n");
        uart_main.printf("+ : Increase VOUT L&H \r\n");
        uart_main.printf("- : Dncrease VOUT L&H \r\n");
        uart_main.printf("s : Read Status Registser \r\n");
        uart_main.printf("e : Enable Buck Boost \r\n");
        uart_main.printf("d : Disable Buck Boost \r\n");
        uart_main.printf("\r\nInput >> ");

        c = uart_main.getc();

        gLED = false;
        rLED = true;
        switch (c)
        {
        // ReInit
        case 'i':
            uart_main.printf("Init to POR Status\r\n");
            //Set to POR Status
            max77801.init();
            vout_l = 3.3000;
            rData = max77801.write_register(MAX77801::REG_VOUT_DVS_L, 0x38);
            if (rData < 0)
                uart_main.printf("Error: to access data\r\n");
            vout_h = 3.7500; //WLP
            rData = max77801.write_register(MAX77801::REG_VOUT_DVS_H, 0x40);
            if (rData < 0)
                uart_main.printf("Error: to access data\r\n");
            break;

        //Control VOUT
        case '+':
            uart_main.printf("Increase VOUT\r\n");
            vout_l += 0.0125;
            rData = max77801.set_vout(vout_l, MAX77801::VAL_LOW);
            if (rData < 0)
                uart_main.printf("Error: to access data\r\n");
            vout_h += 0.0125;
            rData = max77801.set_vout(vout_h, MAX77801::VAL_HIGH);
            if (rData < 0)
                uart_main.printf("Error: to access data\r\n");
            break;

        case '-':
            uart_main.printf("Decrease VOUT\r\n");
            vout_l -= 0.0125;
            rData = max77801.set_vout(vout_l, MAX77801::VAL_LOW);
            if (rData < 0)
                uart_main.printf("Error: to access data\r\n");
            vout_h -= 0.0125;
            rData = max77801.set_vout(vout_h, MAX77801::VAL_HIGH);
            if (rData < 0)
                uart_main.printf("Error: to access data\r\n");
            break;

        //Read Status Register
        case 's':
            uart_main.printf("Read Status\r\n");
            rData = max77801.get_status();
            if (rData < 0)
                uart_main.printf("Error: to access data\r\n");
            else
                uart_main.printf("Status 0x%2X\r\n", rData);
            break;

        //Buck Boost Enable/Disable
        case 'e':
            uart_main.printf("Enable Buck Boost\r\n");
            rData = max77801.config_enable(MAX77801::BUCK_BOOST_OUTPUT,
                                           MAX77801::VAL_ENABLE);
            if (rData < 0)
                uart_main.printf("Error: to access data\r\n");
            break;

        case 'd':
            uart_main.printf("Disable Buck Boost\r\n");
            rData = max77801.config_enable(MAX77801::BUCK_BOOST_OUTPUT,
                                           MAX77801::VAL_DISABLE);
            if (rData < 0)
                uart_main.printf("Error: to access data\r\n");
            break;

        default:
            uart_main.printf("Unknown Command\r\n");
            break;
        }

        wait_ms(1000);
    }
}
