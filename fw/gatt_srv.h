/**
 * Copyright (c) 2015 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef GATT_SRV_H
#define GATT_SRV_H

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#define SRV_UUID_BASE        { 0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, \
                               0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00 }
#define SRV_UUID_SERVICE     0x1523
#define SRV_UUID_TX_CHAR     0x1524
#define SRV_UUID_RX_CHAR     0x1525

#define RX_DATA_LEN          4  // length in bytes
#define TX_DATA_LEN          16 // length in bytes

typedef struct gatt_srv_s gatt_srv_t;
typedef void (*gatt_srv_write_handler_t) (uint16_t conn_handle, gatt_srv_t * p_lbs, const uint8_t *data, int len);
typedef struct
{
    gatt_srv_write_handler_t srv_write_handler;
} gatt_srv_init_t;


struct gatt_srv_s
{
    uint16_t                    service_handle;
    ble_gatts_char_handles_t    rx_char_handles;
    ble_gatts_char_handles_t    tx_char_handles;
    uint8_t                     uuid_type;
    gatt_srv_write_handler_t    srv_write_handler;
};

uint32_t gatt_srv_init(gatt_srv_t * p_lbs, const gatt_srv_init_t * p_lbs_init);

void gatt_srv_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

uint32_t gatt_srv_on_change(uint16_t conn_handle, gatt_srv_t *p_lbs, uint8_t data[TX_DATA_LEN]);


#endif
