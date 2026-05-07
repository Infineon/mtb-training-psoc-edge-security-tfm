/*****************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for non-secure
*                    application in the CM33 CPU
*
* Related Document : See README.md
*
*******************************************************************************
# \copyright
# (c) 2024-2026, Infineon Technologies AG, or an affiliate of Infineon
# Technologies AG.  SPDX-License-Identifier: Apache-2.0
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/

#include <stdio.h>
#include "cybsp.h"
#include "cy_pdl.h"
#include "tfm_config/custom_partitions/arot_example/arot_example_api.h"
#include "tfm_ns_interface.h"
#include "os_wrapper/common.h"
#include "psa/crypto.h"
#include "ifx_platform_api.h"
#include "arot_example_api.h"


/*******************************************************************************
* Macros
*******************************************************************************/

#define SHA_256_SIZE                  (32U)

#define EC_KEY_BITS                   ((size_t) 256)

#define EC_SIGNATURE_SIZE             (2*(EC_KEY_BITS/8))

#define AES_KEY_SIZE                  ((size_t) 128)

#define AES_BUFF_SIZE                 (32U)

#define NONCE_SIZE                    ((size_t) 12)

#define PRNT_BYTES_PER_LINE           (16u)

/* The timeout value in microseconds used to wait for CM55 core to be booted */
#define CM55_BOOT_WAIT_TIME_USEC      (10U)

/* App boot address for CM55 project */
#define CM55_APP_BOOT_ADDR            (CYMEM_CM33_0_m55_nvm_START + \
                                        CYBSP_MCUBOOT_HEADER_SIZE)


/*******************************************************************************
* Global Variables
*******************************************************************************/


/*******************************************************************************
* Function Prototypes
*******************************************************************************/


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function of the CM33 non-secure application. 
*
* It initializes the TF-M NS interface to communicate with TF-M FW. The app
* calls PSA APIs to use the Internal Trusted Storage secure services
* offered by TF-M.

* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    uint32_t rslt;
    psa_status_t status;
    const unsigned char input_data[] = "Hello World";
    uint8_t hash[SHA_256_SIZE];
    uint8_t signature[EC_SIGNATURE_SIZE];
    size_t signature_len;
    size_t hash_len;
    psa_key_id_t ec_key_id;
    psa_key_id_t aead_key_id;
    uint8_t additional_data[] = {0x01, 0x02};
    uint8_t enc_data[AES_BUFF_SIZE];
    uint8_t dec_data[AES_BUFF_SIZE];
    uint8_t nonce[NONCE_SIZE];
    size_t enc_data_len = 0;
    size_t dec_data_len = 0;
    psa_key_attributes_t ec_key_attributes;
    psa_key_attributes_t aead_key_attributes;
    unsigned char out_buf[256];
    int buf_size;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize TF-M interface */
    rslt = tfm_ns_interface_init();
    if(rslt != OS_WRAPPER_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    buf_size = sprintf((char*)out_buf, "\x1b[2J\x1b[;H"
                "******* "
                "PSOC Edge MCU: Basic Trusted Firmware-M (TF-M) Crypto Application "
                "******* \r\n\n");
    ifx_platform_log_msg(out_buf, buf_size);
    
    psa_crypto_init();

    buf_size = sprintf((char*)out_buf, "\r\n*** TF-M SHA256 Cryptography service ***\r\n");
    ifx_platform_log_msg(out_buf, buf_size);

    /* Start of SHA256 code */
    buf_size = sprintf((char*)out_buf, "\r\nCalculating SHA256 of message ...\r\n");
    ifx_platform_log_msg(out_buf, buf_size);

    status = psa_hash_compute(PSA_ALG_SHA_256, input_data, sizeof(input_data),
                                  hash, sizeof(hash), &hash_len);
    if(status != PSA_SUCCESS)
    {
       CY_ASSERT(0);
    }

    buf_size = sprintf((char*)out_buf, "SHA256 calculated: \r\n");
    ifx_platform_log_msg(out_buf, buf_size);
    
    /* Print SHA256 on UART console */
    for(int i = 0; i < ((hash_len/PRNT_BYTES_PER_LINE) + ((hash_len%PRNT_BYTES_PER_LINE) ? 1: 0)); i++)
    { 
        int j;
        /* Print 16 bytes per line */
        for(j = 0; j < PRNT_BYTES_PER_LINE; j++)
        {
            if((i*PRNT_BYTES_PER_LINE + j) >= hash_len)
            {
                break;
            }
            sprintf((char*)(out_buf + 5*j), "0x%02x ", hash[(i*PRNT_BYTES_PER_LINE + j)]);
        }
        buf_size = sprintf((char*)(out_buf + 5*j), "\r\n");
        ifx_platform_log_msg(out_buf, ((j*5) + buf_size));
    }
    /* End of SHA256 code */

    /* Start of ECDSA code */
    buf_size = sprintf((char*)out_buf, "\r\n\n*** TF-M EC Cryptography service ***\r\n");
    ifx_platform_log_msg(out_buf, buf_size);

    buf_size = sprintf((char*)out_buf, "\r\nGenerating EC key...\r\n");
    ifx_platform_log_msg(out_buf, buf_size);

    /* Set key attributes */
    psa_set_key_usage_flags(&ec_key_attributes,
              PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&ec_key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&ec_key_attributes,
            PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&ec_key_attributes, EC_KEY_BITS);

    psa_set_key_lifetime(&ec_key_attributes, PSA_KEY_LIFETIME_VOLATILE);

    status = psa_generate_key(&ec_key_attributes, &ec_key_id);
    if(status != PSA_SUCCESS)
    {
        CY_ASSERT(0);
    }

    buf_size = sprintf((char*)out_buf, "Signing message EC private key...\r\n");
    ifx_platform_log_msg(out_buf, buf_size);

    status = psa_sign_message(ec_key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                              input_data, sizeof(input_data), signature,
                              sizeof(signature), &signature_len);
    if(status != PSA_SUCCESS)
    {
        CY_ASSERT(0);
    }

    buf_size = sprintf((char*)out_buf, "Calculated signature: \r\n");
    ifx_platform_log_msg(out_buf, buf_size);
    
    /* Print on UART console */
    for(int i = 0; i < ((signature_len/PRNT_BYTES_PER_LINE) + ((signature_len%PRNT_BYTES_PER_LINE) ? 1: 0)); i++)
    { 
        int j;
        /* Print 16 bytes per line */
        for(j = 0; j < PRNT_BYTES_PER_LINE; j++)
        {
            if((i*PRNT_BYTES_PER_LINE + j) >= signature_len)
            {
                break;
            }
            sprintf((char*)(out_buf + 5*j), "0x%02x ", signature[(i*PRNT_BYTES_PER_LINE + j)]);
        }
        buf_size = sprintf((char*)(out_buf + 5*j), "\r\n");
        ifx_platform_log_msg(out_buf, ((j*5) + buf_size));
    }

    buf_size = sprintf((char*)out_buf, "Verifying signature...\r\n");
    ifx_platform_log_msg(out_buf, buf_size);
    status = psa_verify_message(ec_key_id, PSA_ALG_ECDSA(PSA_ALG_SHA_256),
               input_data, sizeof(input_data), signature, signature_len);
    if(status != PSA_SUCCESS)
    {
        CY_ASSERT(0);
    }

    buf_size = sprintf((char*)out_buf, "Signature verification successful\r\n");
    ifx_platform_log_msg(out_buf, buf_size);
    /* End of ECDSA code */

    /* Start of AES AEAD code */
    buf_size = sprintf((char*)out_buf, "\r\n*** TF-M AEAD CCM Cryptography service ***\r\n");
    ifx_platform_log_msg(out_buf, buf_size);

    /* Set key attributes */
    psa_set_key_usage_flags(&aead_key_attributes,
                               PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&aead_key_attributes, PSA_ALG_CCM);
    psa_set_key_type(&aead_key_attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&aead_key_attributes, AES_KEY_SIZE);
    psa_set_key_lifetime(&aead_key_attributes, PSA_KEY_LIFETIME_VOLATILE);


    buf_size = sprintf((char*)out_buf, "\r\nGenerating AES key...\r\n");
    ifx_platform_log_msg(out_buf, buf_size);
    status = psa_generate_key(&aead_key_attributes, &aead_key_id);
    if(status != PSA_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Generate a random number for nonce */
    status = psa_generate_random(nonce, sizeof(nonce));
    if(status != PSA_SUCCESS)
    {
        CY_ASSERT(0);
    }

    buf_size = sprintf((char*)out_buf, "Encrypting data...\r\n");
    ifx_platform_log_msg(out_buf, buf_size);

    /* Encrypt data with AEAD single part cipher */
    status = psa_aead_encrypt(aead_key_id, PSA_ALG_CCM, nonce, sizeof(nonce),
                                additional_data, sizeof(additional_data),
                              input_data, sizeof(input_data), enc_data,
                              sizeof(enc_data), &enc_data_len);
    if(status != PSA_SUCCESS)
    {
        CY_ASSERT(0);
    }

    buf_size = sprintf((char*)out_buf, "Encrypted message: \r\n");
    ifx_platform_log_msg(out_buf, buf_size);
    
    /* Print encrypted message on UART console */
    for(int i = 0; i < ((enc_data_len/PRNT_BYTES_PER_LINE) + ((enc_data_len%PRNT_BYTES_PER_LINE) ? 1: 0)); i++)
    {
        int j;
        /* Print 16 bytes per line */
        for(j = 0; j < PRNT_BYTES_PER_LINE; j++)
        {
            if((i*PRNT_BYTES_PER_LINE + j) >= enc_data_len)
            {
                break;
            }
            sprintf((char*)(out_buf + 5*j), "0x%02x ", enc_data[(i*PRNT_BYTES_PER_LINE + j)]);
        }
        buf_size = sprintf((char*)(out_buf + 5*j), "\r\n");
        ifx_platform_log_msg(out_buf, ((j*5) + buf_size));
    }

    buf_size = sprintf((char*)out_buf, "Decrypting data...\r\n");
    ifx_platform_log_msg(out_buf, buf_size);

    /* Decrypt data */
    status = psa_aead_decrypt(aead_key_id, PSA_ALG_CCM, nonce, sizeof(nonce),
                              additional_data, sizeof(additional_data),
                              enc_data, enc_data_len, dec_data,
                              sizeof(dec_data), &dec_data_len);
    if(status != PSA_SUCCESS)
    {
        CY_ASSERT(0);
    }

    buf_size = sprintf((char*)out_buf, "Decrypted message - %s\r\n", dec_data);
    ifx_platform_log_msg(out_buf, buf_size);
    /* End of AES AEAD code */

    uint32_t input=1;
    uint32_t output[256];

    arot_example_print(input, output);

    /* Enable CM55. */
    /* CY_CM55_APP_BOOT_ADDR must be updated if CM55 memory layout is changed.*/
    Cy_SysEnableCM55(MXCM55, CM55_APP_BOOT_ADDR, CM55_BOOT_WAIT_TIME_USEC);

    for (;;)
    {

        /* Receive and forward the IPC requests from M55 to TF-M. 
         * M55 can request security aware PDL and TF-M for secure services,
         * and these requests are sent from M55 to M33 NS using Secure Request
         * Framework (SRF) over IPC.
         */
        result = mtb_srf_ipc_receive_request(&cybsp_mtb_srf_relay_context, MTB_IPC_NEVER_TIMEOUT);
        if(result != CY_RSLT_SUCCESS)
        {
            CY_ASSERT(0);
        }
        result =  mtb_srf_ipc_process_pending_request(&cybsp_mtb_srf_relay_context);
        if(result != CY_RSLT_SUCCESS)
        {
            CY_ASSERT(0);
        }
    }
}
/* [] END OF FILE */
