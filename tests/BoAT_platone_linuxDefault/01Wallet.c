/******************************************************************************
 * Copyright (C) 2018-2021 aitos.io
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/
#include "tcase_platone.h"

#define EXCEED_STR_MAX_LEN          4097
#define TEST_EIP155_COMPATIBILITY   BOAT_FALSE
#define TEST_PLATONE_CHAIN_ID       1
#define TEST_GAS_LIMIT              "0x6691B7"
#define TEST_GAS_PRICE              "0x4A817C800"
#define TEST_IS_SYNC_TX             BOAT_TRUE
#define TEST_RECIPIENT_ADDRESS      "0xaac9fb1d70ee0d4b5a857a28b9c3b16114518e45"

BUINT8 binFormatKey[32];

BOAT_RESULT check_platone_wallet(BoatPlatoneWallet *wallet_ptr)
{
    BOAT_RESULT result = BOAT_SUCCESS;

	result = strncmp(wallet_ptr->account_info.prikeyCtx.extra_data.value, platone_private_key_buf, strlen(platone_private_key_buf));
    if (result != 0) 
    {
        return result;
    }
	
    result = strncmp(wallet_ptr->network_info.node_url_ptr, TEST_PLATONE_NODE_URL, strlen(TEST_PLATONE_NODE_URL));
    if (result != 0)
    {
        return result;
    }

    return BOAT_SUCCESS;
}

BoatPlatoneWalletConfig get_platone_wallet_settings()
{
    //set user private key context
    BoatPlatoneWalletConfig wallet_config;
    if (TEST_KEY_TYPE == BOAT_WALLET_PRIKEY_FORMAT_NATIVE)
    {
        wallet_config.prikeyCtx_config.prikey_format  = BOAT_WALLET_PRIKEY_FORMAT_NATIVE;
        UtilityHexToBin(binFormatKey, 32, platone_private_key_buf, TRIMBIN_TRIM_NO, BOAT_FALSE);
        wallet_config.prikeyCtx_config.prikey_content.field_ptr = binFormatKey;
        wallet_config.prikeyCtx_config.prikey_content.field_len = 32;
    }
    else
    {
        wallet_config.prikeyCtx_config.prikey_format  = BOAT_WALLET_PRIKEY_FORMAT_PKCS;
        wallet_config.prikeyCtx_config.prikey_content.field_ptr = (BUINT8 *)platone_private_key_buf;
	    wallet_config.prikeyCtx_config.prikey_content.field_len = strlen(platone_private_key_buf) + 1;
    }
	wallet_config.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_EXTERNAL_INJECTION;
	wallet_config.prikeyCtx_config.prikey_type	  = BOAT_WALLET_PRIKEY_TYPE_SECP256K1;


	wallet_config.chain_id             = TEST_PLATONE_CHAIN_ID;
    wallet_config.eip155_compatibility = TEST_EIP155_COMPATIBILITY;
    strncpy(wallet_config.node_url_str, TEST_PLATONE_NODE_URL, BOAT_PLATONE_NODE_URL_MAX_LEN - 1);
	
    return wallet_config;
}

START_TEST(test_001CreateWallet_0001CreateOneTimeWalletSuccess) 
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *g_platone_wallet_ptr = NULL;
    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 0);

     /* 1. execute unit test */
	rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 1);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == true);

    g_platone_wallet_ptr = BoatGetWalletByIndex(rtnVal);
    ck_assert(g_platone_wallet_ptr != NULL);
    //ck_assert(check_ethereum_wallet(g_ethereum_wallet_ptr) == BOAT_SUCCESS);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0002CreateOneTimeWalletFailureNullConfig) 
{
    BSINT32 rtnVal;
    extern BoatIotSdkContext g_boat_iot_sdk_context;

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, NULL, sizeof(BoatPlatoneWalletConfig));
    
    /* 3. verify test result */
    /* 3-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    /* 3-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == false);
}
END_TEST

START_TEST(test_001CreateWallet_0003CreatePersistWalletSuccess) 
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *g_platone_wallet_ptr = NULL;

    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, "platone", &wallet, sizeof(BoatPlatoneWalletConfig));

    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 0);
    ck_assert_int_eq(access("platone", F_OK), 0);

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, "platone", &wallet, sizeof(BoatPlatoneWalletConfig));
    
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 1);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == true);

    g_platone_wallet_ptr = BoatGetWalletByIndex(rtnVal);
    ck_assert(g_platone_wallet_ptr != NULL);
    //ck_assert(check_platon_wallet(g_platon_wallet_ptr) == BOAT_SUCCESS);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0004CreateLoadWalletSuccess) 
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *g_platone_wallet_ptr = NULL;
    BoatIotSdkInit();    
    extern BoatIotSdkContext g_boat_iot_sdk_context;

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, "platone", NULL, sizeof(BoatPlatoneWalletConfig));
    
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 0);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);

    g_platone_wallet_ptr = BoatGetWalletByIndex(rtnVal);
    ck_assert(g_platone_wallet_ptr != NULL);
    //ck_assert(check_platon_wallet(g_platon_wallet_ptr) == BOAT_SUCCESS);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0005CreateLoadWalletFailureNoExist) 
{
    BSINT32 rtnVal;
    extern BoatIotSdkContext g_boat_iot_sdk_context;

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, "platone_no_exist", NULL, sizeof(BoatPlatoneWalletConfig));
    
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_PERSISTER_READ_FAIL);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == false);
    //ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == false);
}
END_TEST

START_TEST(test_001CreateWallet_0006CreateOneTimeWalletFailureShortSize) 
{
    BSINT32 rtnVal;
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig) - 10);
    
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == false);
}
END_TEST

START_TEST(test_001CreateWallet_0007CreateOneTimeWalletSuccessLongSize) 
{
    BSINT32 rtnVal;
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig) + 10);
    
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == false);
}
END_TEST

START_TEST(test_001CreateWallet_0008CreateOneTimeWalletFailureProtocolUnknown)
{
    BSINT32 rtnVal;
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_UNKNOWN, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == false);
}
END_TEST

START_TEST(test_001CreateWallet_0009CreateWalletWithInternalGeneration)
{
    BSINT32 rtnVal;
    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    wallet.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 0);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0010CreateSixWallet)
{
    BSINT32 rtnVal;
    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    wallet.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 0);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 1);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 2);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 3);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 4);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 5);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[2].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[3].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[4].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[5].is_used == true);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0011CreateSevenWallet)
{
    BSINT32 rtnVal;
    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    wallet.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 0);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 1);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 2);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 3);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 4);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 5);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, BOAT_ERROR_WALLET_NUM_EXCEED);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[2].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[3].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[4].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[5].is_used == true);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0012CreateOnetimeWalletWithLoadExistedWallet)
{
    BSINT32 rtnVal;
    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    wallet.prikeyCtx_config.load_existed_wallet = BOAT_TRUE;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 0);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0013UnloadWalletSuccess)
{
    BSINT32 rtnVal;
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    wallet.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 0);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);

    BoatWalletUnload(0);
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == false);
}
END_TEST

START_TEST(test_001CreateWallet_0014UnloadInexistentWallet)
{
    BSINT32 rtnVal;
    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    wallet.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 0);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);

    BoatWalletUnload(1);
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);

    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0015CreateSixWalletUnloadOneSuccess)
{
    BSINT32 rtnVal;
    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    wallet.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 0);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 1);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 2);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 3);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 4);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 5);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[2].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[3].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[4].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[5].is_used == true);

    BoatWalletUnload(1);
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == false);
    ck_assert(g_boat_iot_sdk_context.wallet_list[2].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[3].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[4].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[5].is_used == true);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0016CreateSixWalletUnloadOneCreateOne)
{
    BSINT32 rtnVal;
    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    wallet.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 0);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 1);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 2);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 3);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 4);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 5);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[2].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[3].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[4].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[5].is_used == true);

    BoatWalletUnload(3);
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 3);

    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[2].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[3].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[4].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[5].is_used == true);

    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0017CreateSixWalletUnloadTwoCreateOne)
{
    BSINT32 rtnVal;
    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;
    wallet.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 0);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 1);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 2);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 3);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 4);

    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 5);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[2].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[3].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[4].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[5].is_used == true);

    BoatWalletUnload(2);
    BoatWalletUnload(4);
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet, sizeof(BoatPlatoneWalletConfig));
    ck_assert_int_eq(rtnVal, 2);

    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[2].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[3].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[4].is_used == false);
    ck_assert(g_boat_iot_sdk_context.wallet_list[5].is_used == true);

    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_001CreateWallet_0018DeletePersistWalletSuccess) 
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *g_platone_wallet_ptr = NULL;

    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, "platone", &wallet, sizeof(BoatPlatoneWalletConfig));
    
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 0);

    rtnVal = BoatWalletDelete("platone");

    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, "platone", NULL, sizeof(BoatPlatoneWalletConfig));
    
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_PERSISTER_READ_FAIL);

    /* 2-2. verify the global variables that be affected */
    ck_assert(g_boat_iot_sdk_context.wallet_list[0].is_used == true);
    ck_assert(g_boat_iot_sdk_context.wallet_list[1].is_used == false);

    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_002InitWallet_0001SetEIP155CompSuccess)
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *wallet_ptr = BoatMalloc(sizeof(BoatPlatoneWallet));
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();

    ck_assert_ptr_ne(wallet_ptr, NULL);    
    
    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletSetEIP155Comp(wallet_ptr, wallet.eip155_compatibility);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    /* 2-2. verify the global variables that be affected */
    ck_assert(wallet_ptr->network_info.eip155_compatibility == wallet.eip155_compatibility);
    BoatFree(wallet_ptr);
}
END_TEST

START_TEST(test_002InitWallet_0002SetEIP155CompFailureNullParam)
{
    BSINT32 rtnVal;
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    
    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletSetEIP155Comp(NULL, wallet.eip155_compatibility);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    /* 2-2. verify the global variables that be affected */
}
END_TEST

START_TEST(test_002InitWallet_0003SetChainIdSuccess)
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *wallet_ptr = BoatMalloc(sizeof(BoatPlatoneWallet));
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();

    ck_assert_ptr_ne(wallet_ptr, NULL);
    
    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletSetChainId(wallet_ptr, wallet.chain_id);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    /* 2-2. verify the global variables that be affected */
    ck_assert(wallet_ptr->network_info.chain_id == wallet.chain_id);
    BoatFree(wallet_ptr); 
}
END_TEST

START_TEST(test_002InitWallet_0004SetChainIdFailureNullParam)
{
    BSINT32 rtnVal;
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    
    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletSetChainId(NULL, wallet.chain_id);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_COMMON_INVALID_ARGUMENT);

}
END_TEST

START_TEST(test_002InitWallet_0005SetNodeUrlSuccess)
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *wallet_ptr = BoatMalloc(sizeof(BoatPlatoneWallet));
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    
    /* 1. execute unit test */
    wallet_ptr->network_info.node_url_ptr = NULL;
    rtnVal = BoatPlatoneWalletSetNodeUrl(wallet_ptr, wallet.node_url_str);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    /* 2-2. verify the global variables that be affected */
    ck_assert_str_eq(wallet_ptr->network_info.node_url_ptr, wallet.node_url_str);
    BoatPlatoneWalletDeInit(wallet_ptr);
}
END_TEST

START_TEST(test_002InitWallet_0006SetNodeUrlFailureNullParam)
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *wallet_ptr = BoatMalloc(sizeof(BoatPlatoneWallet));
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    
    /* 1. execute unit test */
    wallet_ptr->network_info.node_url_ptr = NULL;
    rtnVal = BoatPlatoneWalletSetNodeUrl(NULL, wallet.node_url_str);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    /* 2-2. verify the global variables that be affected */
    ck_assert(wallet_ptr->network_info.node_url_ptr == NULL);

    /* 1. execute unit test */
    wallet_ptr->network_info.node_url_ptr = NULL;
    rtnVal = BoatPlatoneWalletSetNodeUrl(wallet_ptr, NULL);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    /* 2-2. verify the global variables that be affected */
    ck_assert(wallet_ptr->network_info.node_url_ptr == NULL);

    /* 1. execute unit test */
    wallet_ptr->network_info.node_url_ptr = NULL;
    rtnVal = BoatPlatoneWalletSetNodeUrl(NULL, NULL);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    /* 2-2. verify the global variables that be affected */
    ck_assert(wallet_ptr->network_info.node_url_ptr == NULL);
    BoatFree(wallet_ptr); 
}
END_TEST

START_TEST(test_002InitWallet_0007SetNodeUrlFailureErrorNodeUrlFormat)
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *wallet_ptr = BoatMalloc(sizeof(BoatPlatoneWallet));
    BoatPlatoneWalletConfig wallet;
    
    /* 1. execute unit test */
    strncpy(wallet.node_url_str, "abcd", strlen("abcd"));
    wallet_ptr->network_info.node_url_ptr = NULL;
    rtnVal = BoatPlatoneWalletSetNodeUrl(wallet_ptr, wallet.node_url_str);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    /* 2-2. verify the global variables that be affected */
    ck_assert(wallet_ptr->network_info.node_url_ptr == NULL);
    BoatFree(wallet_ptr);
}
END_TEST

START_TEST(test_002InitWallet_0008SetNodeUrlFailureNodeUrlOutOfLimit)
{
    BSINT32 rtnVal;
    BoatPlatoneWallet *wallet_ptr = BoatMalloc(sizeof(BoatPlatoneWallet));
    char error_platone_node_url[EXCEED_STR_MAX_LEN];
    
    for (int i = 0; i < EXCEED_STR_MAX_LEN; i++)
    {
        error_platone_node_url[i] = ':';
    }

    /* 1. execute unit test */
    wallet_ptr->network_info.node_url_ptr = NULL;
    rtnVal = BoatPlatoneWalletSetNodeUrl(wallet_ptr, error_platone_node_url);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    /* 2-2. verify the global variables that be affected */
    ck_assert(wallet_ptr->network_info.node_url_ptr == NULL);
    BoatFree(wallet_ptr);
}
END_TEST

START_TEST(test_002InitWallet_0009InitPlatoneWalletWithNullConfig)
{
    BoatPlatoneWallet *rtnVal;

    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletInit(NULL, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_ptr_eq(rtnVal,NULL);

    /* 2-2. verify the global variables that be affected */ 
}
END_TEST

START_TEST(test_002InitWallet_0010InitPlatoneWalletWithSmallerSize)
{
    BoatPlatoneWallet *rtnVal;
    BoatPlatoneWalletConfig walletConfig = {0};

    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletInit(&walletConfig, sizeof(BoatPlatoneWalletConfig) - 1);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_ptr_eq(rtnVal,NULL);

    /* 2-2. verify the global variables that be affected */ 
}
END_TEST

START_TEST(test_002InitWallet_0011InitPlatoneWalletWithBiggerSize)
{
    BoatPlatoneWallet *rtnVal;
    BoatPlatoneWalletConfig walletConfig = {0};

    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletInit(&walletConfig, sizeof(BoatPlatoneWalletConfig) + 1);
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_ptr_eq(rtnVal,NULL);

    /* 2-2. verify the global variables that be affected */ 
}
END_TEST

START_TEST(test_002InitWallet_0012InitPlatoneWalletSuccess)
{
    BoatPlatoneWallet *rtnVal;
    BoatPlatoneWalletConfig walletConfig = get_platone_wallet_settings();

    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletInit(&walletConfig, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_ptr_ne(rtnVal,NULL);

    /* 2-2. verify the global variables that be affected */ 
}
END_TEST

START_TEST(test_002InitWallet_0013InitPlatoneWalletGenerationKey)
{
    BoatPlatoneWallet *rtnVal;
    BoatPlatoneWalletConfig walletConfig;

    walletConfig.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    walletConfig.prikeyCtx_config.prikey_format = BOAT_WALLET_PRIKEY_FORMAT_NATIVE;
    walletConfig.prikeyCtx_config.prikey_type = BOAT_WALLET_PRIKEY_TYPE_SECP256K1;
    walletConfig.eip155_compatibility = BOAT_FALSE;

    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletInit(&walletConfig, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_ptr_ne(rtnVal,NULL);
    BoatPlatoneWalletDeInit(rtnVal);
    /* 2-2. verify the global variables that be affected */ 
}
END_TEST

START_TEST(test_002InitWallet_0014InitPlatoneWalletWithWrongGenMode)
{
    BoatPlatoneWallet *rtnVal;
    BoatPlatoneWalletConfig walletConfig;

    walletConfig.prikeyCtx_config.prikey_genMode = 3;
    walletConfig.prikeyCtx_config.prikey_format = BOAT_WALLET_PRIKEY_FORMAT_NATIVE;
    walletConfig.prikeyCtx_config.prikey_type = BOAT_WALLET_PRIKEY_TYPE_SECP256K1;
    walletConfig.eip155_compatibility = BOAT_FALSE;
    walletConfig.load_existed_wallet = BOAT_FALSE;

    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletInit(&walletConfig, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_ptr_eq(rtnVal,NULL);
}
END_TEST

START_TEST(test_002InitWallet_0015InitPlatoneWalletWithWrongKeyFormat)
{
    BoatPlatoneWallet *rtnVal;
    BoatPlatoneWalletConfig walletConfig;

    walletConfig.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    walletConfig.prikeyCtx_config.prikey_format = 4;
    walletConfig.prikeyCtx_config.prikey_type = BOAT_WALLET_PRIKEY_TYPE_SECP256K1;
    walletConfig.eip155_compatibility = BOAT_FALSE;
    walletConfig.load_existed_wallet = BOAT_FALSE;

    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletInit(&walletConfig, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_ptr_eq(rtnVal,NULL);
}
END_TEST

START_TEST(test_002InitWallet_0016InitPlatoneWalletWithWrongType)
{
    BoatPlatoneWallet *rtnVal;
    BoatPlatoneWalletConfig walletConfig;

    walletConfig.prikeyCtx_config.prikey_genMode = BOAT_WALLET_PRIKEY_GENMODE_INTERNAL_GENERATION;
    walletConfig.prikeyCtx_config.prikey_format = BOAT_WALLET_PRIKEY_FORMAT_NATIVE;
    walletConfig.prikeyCtx_config.prikey_type = 3;
    walletConfig.eip155_compatibility = BOAT_FALSE;
    walletConfig.load_existed_wallet = BOAT_FALSE;

    /* 1. execute unit test */
    rtnVal = BoatPlatoneWalletInit(&walletConfig, sizeof(BoatPlatoneWalletConfig));
    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_ptr_eq(rtnVal,NULL);
}
END_TEST

START_TEST(test_003DeleteWallet_0001DeleteWalletFailureNullFleName) 
{
    BSINT32 rtnVal;

    BoatIotSdkInit();
    BoatPlatoneWalletConfig wallet = get_platone_wallet_settings();
    extern BoatIotSdkContext g_boat_iot_sdk_context;

    /* 1. execute unit test */
    rtnVal = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, "platone", &wallet, sizeof(BoatPlatoneWalletConfig));

    /* 2. verify test result */
    /* 2-1. verify the return value */
    ck_assert_int_eq(rtnVal, 0);

    BoatWalletDelete(NULL);
    ck_assert_int_eq(access("platone", F_OK), 0);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_003DeleteWallet_0002DeleteWalletFailureNoExistingFile) 
{
    BoatWalletDelete("platone_no_exist");
    ck_assert_int_eq(access("platone", F_OK), 0);
}
END_TEST

START_TEST(test_003DeleteWallet_0003DeleteWalletSucessExistingFile) 
{
    BoatWalletDelete("platone");
    ck_assert_int_eq(access("platone", F_OK), -1);
}
END_TEST

Suite *make_wallet_suite(void) 
{
    /* Create Suite */
    Suite *s_wallet = suite_create("wallet");

    /* Create test cases */
    TCase *tc_wallet_api = tcase_create("wallet_api");

    /* Add a test case to the Suite */
    suite_add_tcase(s_wallet, tc_wallet_api);       
    /* Test cases are added to the test set */
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0001CreateOneTimeWalletSuccess);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0002CreateOneTimeWalletFailureNullConfig);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0003CreatePersistWalletSuccess);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0004CreateLoadWalletSuccess);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0005CreateLoadWalletFailureNoExist);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0006CreateOneTimeWalletFailureShortSize);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0007CreateOneTimeWalletSuccessLongSize);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0008CreateOneTimeWalletFailureProtocolUnknown);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0009CreateWalletWithInternalGeneration);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0010CreateSixWallet);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0011CreateSevenWallet);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0012CreateOnetimeWalletWithLoadExistedWallet);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0013UnloadWalletSuccess);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0014UnloadInexistentWallet);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0015CreateSixWalletUnloadOneSuccess);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0016CreateSixWalletUnloadOneCreateOne);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0017CreateSixWalletUnloadTwoCreateOne);  
    tcase_add_test(tc_wallet_api, test_001CreateWallet_0018DeletePersistWalletSuccess);  

    tcase_add_test(tc_wallet_api, test_002InitWallet_0001SetEIP155CompSuccess);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0002SetEIP155CompFailureNullParam);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0003SetChainIdSuccess);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0004SetChainIdFailureNullParam);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0005SetNodeUrlSuccess);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0006SetNodeUrlFailureNullParam);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0007SetNodeUrlFailureErrorNodeUrlFormat);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0008SetNodeUrlFailureNodeUrlOutOfLimit);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0009InitPlatoneWalletWithNullConfig);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0010InitPlatoneWalletWithSmallerSize);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0011InitPlatoneWalletWithBiggerSize);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0012InitPlatoneWalletSuccess);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0013InitPlatoneWalletGenerationKey);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0014InitPlatoneWalletWithWrongGenMode);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0015InitPlatoneWalletWithWrongKeyFormat);  
    tcase_add_test(tc_wallet_api, test_002InitWallet_0016InitPlatoneWalletWithWrongType);  

    tcase_add_test(tc_wallet_api, test_003DeleteWallet_0001DeleteWalletFailureNullFleName);  
    tcase_add_test(tc_wallet_api, test_003DeleteWallet_0002DeleteWalletFailureNoExistingFile);  
    tcase_add_test(tc_wallet_api, test_003DeleteWallet_0003DeleteWalletSucessExistingFile);  

    return s_wallet;
}

