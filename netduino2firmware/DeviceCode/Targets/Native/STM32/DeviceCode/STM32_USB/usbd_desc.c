/**
  ******************************************************************************
  * @file    usbd_desc.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   This file provides the USBD descriptors and string formating method.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_req.h"
#include "usbd_conf.h"
#include "usb_regs.h"

#define USBD_VID                      0x22B1          // Secret Labs LLC
#define USBD_PID                      0x1000          // Netduino

#define USBD_LANGID_STRING            0x409
#define USBD_MANUFACTURER_STRING      "Secret Labs LLC"

#define USBD_PRODUCT_FS_STRING        "Netduino"
#define USBD_SERIALNUMBER_FS_STRING   "000000000666"  // TODO: Unique Device ID

#define USBD_CONFIGURATION_FS_STRING  "Netduino"
#define USBD_INTERFACE_FS_STRING      "Netduino"


#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* USB Standard Device Descriptor */
__ALIGN_BEGIN uint8_t USBD_DeviceDesc[USB_SIZ_DEVICE_DESC] __ALIGN_END =
{
    0x12,                       /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
    0x10,                       /* bcdUSB */
    0x01,
    0x00,                       /* bDeviceClass */
    0x00,                       /* bDeviceSubClass */
    0x00,                       /* bDeviceProtocol */
    USB_OTG_MAX_EP0_SIZE,       /* bMaxPacketSize */
    LOBYTE(USBD_VID),           /* idVendor */
    HIBYTE(USBD_VID),           /* idVendor */
    LOBYTE(USBD_PID),           /* idProduct */
    HIBYTE(USBD_PID),           /* idProduct */
    0x00,                       /* bcdDevice */
    0x01,
    USBD_IDX_MFC_STR,           /* iManufacturer */
    USBD_IDX_PRODUCT_STR,       /* iProduct */
    USBD_IDX_SERIAL_STR,        /* iSerialNumber */
    USBD_CFG_MAX_NUM            /* bNumConfigurations */
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* USB Standard Device Descriptor */
__ALIGN_BEGIN uint8_t USBD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,           // bLength
  USB_DESC_TYPE_DEVICE_QUALIFIER,       // bDescriptorType
  0x00,                                 // bcdUSB
  0x02,                                 //
  0x00,                                 // bDeviceClass
  0x00,                                 // bDeviceSubClass
  0x00,                                 // bDeviceProtocol
  0x40,                                 // bMaxPacketSize0
  0x01,                                 // bNumConfigurations
  0x00,                                 // Reserved, must be zero
};

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

/* USB Standard Device Descriptor */
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_SIZ_STRING_LANGID] __ALIGN_END =
{
  USB_SIZ_STRING_LANGID,
  USB_DESC_TYPE_STRING,
  LOBYTE(USBD_LANGID_STRING),
  HIBYTE(USBD_LANGID_STRING),
};

/////////////////////////////////////////////////////////////////////////////
// Descriptor callbacks

USBD_DEVICE USR_desc =
{
  USBD_USR_DeviceDescriptor,
  USBD_USR_LangIDStrDescriptor,
  USBD_USR_ManufacturerStrDescriptor,
  USBD_USR_ProductStrDescriptor,
  USBD_USR_SerialStrDescriptor,
  USBD_USR_ConfigStrDescriptor,
  USBD_USR_InterfaceStrDescriptor,
};

uint8_t* USBD_USR_DeviceDescriptor(uint8_t speed, uint16_t *length)
{
  *length = sizeof(USBD_DeviceDesc);
  return USBD_DeviceDesc;
}

uint8_t* USBD_USR_LangIDStrDescriptor(uint8_t speed, uint16_t *length)
{
  *length = sizeof(USBD_LangIDDesc);
  return USBD_LangIDDesc;
}

uint8_t* USBD_USR_ProductStrDescriptor(uint8_t speed, uint16_t *length)
{
  if(speed == USB_OTG_SPEED_HIGH)
  {
    //USBD_GetString(USBD_PRODUCT_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString(USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

uint8_t* USBD_USR_ManufacturerStrDescriptor(uint8_t speed, uint16_t *length)
{
  USBD_GetString(USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

uint8_t* USBD_USR_SerialStrDescriptor(uint8_t speed, uint16_t *length)
{
  if(speed == USB_OTG_SPEED_HIGH)
  {
    //USBD_GetString(USBD_SERIALNUMBER_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString(USBD_SERIALNUMBER_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

uint8_t* USBD_USR_ConfigStrDescriptor(uint8_t speed, uint16_t *length)
{
  if(speed == USB_OTG_SPEED_HIGH)
  {
    //USBD_GetString(USBD_CONFIGURATION_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString(USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

uint8_t* USBD_USR_InterfaceStrDescriptor(uint8_t speed, uint16_t *length)
{
  if(speed == USB_OTG_SPEED_HIGH)
  {
    //USBD_GetString(USBD_INTERFACE_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString(USBD_INTERFACE_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
