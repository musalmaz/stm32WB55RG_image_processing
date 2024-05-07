/*
 * ard_test.c
 *
 *  Created on: Apr 29, 2024
 *      Author: musa
 */

#include "ard_test.h"

const regval_list qvga_ov7670[] = {

  { REG_COM14, 0x19 },

  { 0x72, 0x11 },

  { 0x73, 0xf1 },


  { REG_HSTART, 0x16 },

  { REG_HSTOP, 0x04 },

  { REG_HREF, 0xa4 },

  { REG_VSTART, 0x02 },

  { REG_VSTOP, 0x7a },

  { REG_VREF, 0x0a },


  { 0xff, 0xff }, /* END MARKER */

};


const regval_list yuv422_ov7670[] = {

  { REG_COM7, 0x0 },  /* Selects YUV mode */

  { REG_RGB444, 0 },  /* No RGB444 please */

  { REG_COM1, 0 },

  { REG_COM15, COM15_R00FF },

  { REG_COM9, 0x6A }, /* 128x gain ceiling; 0x8 is reserved bit */

  { 0x4f, 0x80 },   /* "matrix coefficient 1" */

  { 0x50, 0x80 },   /* "matrix coefficient 2" */

  { 0x51, 0 },    /* vb */

  { 0x52, 0x22 },   /* "matrix coefficient 4" */

  { 0x53, 0x5e },   /* "matrix coefficient 5" */

  { 0x54, 0x80 },   /* "matrix coefficient 6" */

  { REG_COM13, COM13_UVSAT },// COM13_UVSAT

  { 0xff, 0xff },   /* END MARKER */

};


const regval_list ov7670_default_regs1[] = {//from the linux driver

  { REG_COM7, COM7_RESET },

  { REG_TSLB, 0x04 }, /* OV */ //0x04 idi

  { REG_COM7, 0 },  /* VGA */

  /*

  * Set the hardware window.  These values from OV don't entirely

  * make sense - hstop is less than hstart.  But they work...

  */

  { REG_HSTART, 0x13 }, { REG_HSTOP, 0x01 },

  { REG_HREF, 0xb6 }, { REG_VSTART, 0x02 },

  { REG_VSTOP, 0x7a }, { REG_VREF, 0x0a },


  { REG_COM3, 0 }, { REG_COM14, 0 },

  /* Mystery scaling numbers */

  { 0x70, 0x3a }, { 0x71, 0x35 },

  { 0x72, 0x11 }, { 0x73, 0xf0 },

  { 0xa2,/* 0x02 changed to 1*/1 }, { REG_COM10, 0x0 },

  /* Gamma curve values */

  { 0x7a, 0x20 }, { 0x7b, 0x10 },

  { 0x7c, 0x1e }, { 0x7d, 0x35 },

  { 0x7e, 0x5a }, { 0x7f, 0x69 },

  { 0x80, 0x76 }, { 0x81, 0x80 },

  { 0x82, 0x88 }, { 0x83, 0x8f },

  { 0x84, 0x96 }, { 0x85, 0xa3 },

  { 0x86, 0xaf }, { 0x87, 0xc4 },

  { 0x88, 0xd7 }, { 0x89, 0xe8 },

  /* AGC and AEC parameters.  Note we start by disabling those features,

  then turn them only after tweaking the values. */

  { REG_COM8, COM8_FASTAEC | COM8_AECSTEP },

  { REG_GAIN, 0 }, { REG_AECH, 0 },

  { REG_COM4, 0x40 }, /* magic reserved bit */

  { REG_COM9, 0x18 }, /* 4x gain + magic rsvd bit */

  { REG_BD50MAX, 0x05 }, { REG_BD60MAX, 0x07 },

  { REG_AEW, 0x95 }, { REG_AEB, 0x33 },

  { REG_VPT, 0xe3 }, { REG_HAECC1, 0x78 },

  { REG_HAECC2, 0x68 }, { 0xa1, 0x03 }, /* magic */

  { REG_HAECC3, 0xd8 }, { REG_HAECC4, 0xd8 },

  { REG_HAECC5, 0xf0 }, { REG_HAECC6, 0x90 },

  { REG_HAECC7, 0x94 },

  { REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_AGC | COM8_AEC },

  { 0x30, 0 }, { 0x31, 0 },//disable some delays

  /* Almost all of these are magic "reserved" values.  */

  { REG_COM5, 0x61 }, { REG_COM6, 0x4b },

  { 0x16, 0x02 }, { REG_MVFP, 0x07 },

  { 0x21, 0x02 }, { 0x22, 0x91 },

  { 0x29, 0x07 }, { 0x33, 0x0b },

  { 0x35, 0x0b }, { 0x37, 0x1d },

  { 0x38, 0x71 }, { 0x39, 0x2a },

  { REG_COM12, 0x78 }, { 0x4d, 0x40 },

  { 0x4e, 0x20 }, { REG_GFIX, 0 },

  /*{0x6b, 0x4a},*/{ 0x74, 0x10 },

  { 0x8d, 0x4f }, { 0x8e, 0 },

  { 0x8f, 0 }, { 0x90, 0 },

  { 0x91, 0 }, { 0x96, 0 },

  { 0x9a, 0 }, { 0xb0, 0x84 },

  { 0xb1, 0x0c }, { 0xb2, 0x0e },

  { 0xb3, 0x82 }, { 0xb8, 0x0a },


  /* More reserved magic, some of which tweaks white balance */

  { 0x43, 0x0a }, { 0x44, 0xf0 },

  { 0x45, 0x34 }, { 0x46, 0x58 },

  { 0x47, 0x28 }, { 0x48, 0x3a },

  { 0x59, 0x88 }, { 0x5a, 0x88 },

  { 0x5b, 0x44 }, { 0x5c, 0x67 },

  { 0x5d, 0x49 }, { 0x5e, 0x0e },

  { 0x6c, 0x0a }, { 0x6d, 0x55 },

  { 0x6e, 0x11 }, { 0x6f, 0x9e }, /* it was 0x9F "9e for advance AWB" */

  { 0x6a, 0x40 }, { REG_BLUE, 0x40 },

  { REG_RED, 0x60 },

  { REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_AGC | COM8_AEC | COM8_AWB },


  /* Matrix coefficients */

  { 0x4f, 0x80 }, { 0x50, 0x80 },

  { 0x51, 0 },    { 0x52, 0x22 },

  { 0x53, 0x5e }, { 0x54, 0x80 },

  { 0x58, 0x9e },


  { REG_COM16, COM16_AWBGAIN }, { REG_EDGE, 0 },

  { 0x75, 0x05 }, { REG_REG76, 0xe1 },

  { 0x4c, 0 },     { 0x77, 0x01 },

  { REG_COM13, /*0xc3*/0x48 }, { 0x4b, 0x09 },

  { 0xc9, 0x60 },   /*{REG_COM16, 0x38},*/

  { 0x56, 0x40 },


  { 0x34, 0x11 }, { REG_COM11, COM11_EXP | COM11_HZAUTO },

  { 0xa4, 0x82/*Was 0x88*/ }, { 0x96, 0 },

  { 0x97, 0x30 }, { 0x98, 0x20 },

  { 0x99, 0x30 }, { 0x9a, 0x84 },

  { 0x9b, 0x29 }, { 0x9c, 0x03 },

  { 0x9d, 0x4c }, { 0x9e, 0x3f },

  { 0x78, 0x04 },


  /* Extra-weird stuff.  Some sort of multiplexor register */

  { 0x79, 0x01 }, { 0xc8, 0xf0 },

  { 0x79, 0x0f }, { 0xc8, 0x00 },

  { 0x79, 0x10 }, { 0xc8, 0x7e },

  { 0x79, 0x0a }, { 0xc8, 0x80 },

  { 0x79, 0x0b }, { 0xc8, 0x01 },

  { 0x79, 0x0c }, { 0xc8, 0x0f },

  { 0x79, 0x0d }, { 0xc8, 0x20 },

  { 0x79, 0x09 }, { 0xc8, 0x80 },

  { 0x79, 0x02 }, { 0xc8, 0xc0 },

  { 0x79, 0x03 }, { 0xc8, 0x40 },

  { 0x79, 0x05 }, { 0xc8, 0x30 },

  { 0x79, 0x26 },

  { 0xff, 0xff }, /* END MARKER */

};

uint8_t writeReg(uint8_t reg_address, uint8_t data)
{
	  uint8_t check = 1;	// Check I2C operation status //
	  uint8_t counter = 0;	// Counter used for maximum numbers of retries //

	  // Continue until we reach max number of tries is reached or success //
	  while ((check != HAL_OK) && (counter != MAX_I2C_RETRIES_1))
	  {
	    check = HAL_I2C_Mem_Write(&hi2c1, camAddr_WR, reg_address, I2C_MEMADD_SIZE_8BIT, &data, 1, MAX_I2C_TIMEOUT_1);
	    counter++;
	  }

	  // Delay needed based on manual //
	  HAL_Delay(100);

	  // Check if we succeeded or not //
	  if (counter == MAX_I2C_RETRIES_1)
	    return HAL_ERROR;
	  else
	    return check;
}

uint8_t readReg(uint8_t reg)
{
    uint8_t receivedData = 0;
    HAL_StatusTypeDef status;

    // Transmit the register address you want to read from
    // Assume DEVICE_ADDRESS is the 7-bit I2C address of the device, shifted left by 1 for HAL library
    status = HAL_I2C_Master_Transmit(&hi2c1, camAddr_WR << 1, &reg, 1, MAX_I2C_TIMEOUT_1);
    if(status != HAL_OK)
    {
        // Error handling for transmit failure
        printf("I2C Transmit Error\n");
        return 0; // Return an error code or invalid data
    }

    // Receive data from the register
    status = HAL_I2C_Master_Receive(&hi2c1, camAddr_RD << 1, &receivedData, 1, MAX_I2C_TIMEOUT_1);
    if(status != HAL_OK)
    {
        // Error handling for receive failure
        printf("I2C Receive Error\n");
        return 0; // Return an error code or invalid data
    }

    return receivedData;
}

static void writeSensorRegs8_8(const regval_list reglist[])
{
    const regval_list *next = reglist;

    while (1) {
        uint8_t reg_addr = next->reg_num;
        uint8_t reg_val = next->value;

        // Check for termination condition
        if (reg_addr == 255 && reg_val == 255)
            break;

        // Write the register value
        wrReg(reg_addr, reg_val);

        // Move to the next register-value pair
        next++;
    }
}

void cameraInit(void)
{
	 writeReg(0x12, 0x80);

	  HAL_Delay(100);

	  writeSensorRegs8_8(ov7670_default_regs1);

	 writeReg(REG_COM10, 32);//PCLK does not toggle on HBLANK.
}

void setResolution(void){

 writeReg(REG_COM3, 4); // REG_COM3 enable scaling

  writeSensorRegs8_8(qvga_ov7670);

}

void setColor(void){

  writeSensorRegs8_8(yuv422_ov7670);

 // wrSensorRegs8_8(qvga_ov7670);

}

void setupCam(){

  cameraInit();

  setResolution();

  setColor();

 writeReg(0x11, 10); //Earlier it had the value:writeReg(0x11, 12); New version works better for me :) !!!!

}
