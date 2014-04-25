#ifndef STM32_USB_H
#define STM32_USB_H

#define RCC_AHB2Periph_OTG_FS ((uint32_t)0x00000080)

struct STM32_USB_Driver
{
    static USB_CONTROLLER_STATE * GetState( int Controller );
    static HRESULT Initialize( int Controller );
    static HRESULT Uninitialize( int Controller );
    static BOOL StartOutput( USB_CONTROLLER_STATE* State, int endpoint );
    static BOOL RxEnable( USB_CONTROLLER_STATE* State, int endpoint );
    static BOOL GetInterruptState();
    static BOOL ProtectPins( int Controller, BOOL On );
};

#endif