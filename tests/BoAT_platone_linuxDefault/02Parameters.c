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

BoatPlatoneWallet *g_platone_wallet_ptr;
BoatPlatoneWalletConfig wallet_config = {0};

BOAT_RESULT platoneWalletPrepare(void)
{
    BOAT_RESULT index;

    //set user private key context
    memset(&wallet_config, 0, sizeof(wallet_config));
        
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

    index = BoatWalletCreate(BOAT_PROTOCOL_PLATONE, NULL, &wallet_config, sizeof(BoatPlatoneWalletConfig));

    if (index == BOAT_ERROR)
    {
        return BOAT_ERROR;
    }
    
    g_platone_wallet_ptr = BoatGetWalletByIndex(index);
    if (g_platone_wallet_ptr == NULL)
    {
        return BOAT_ERROR;
    }

    return BOAT_SUCCESS;
}

START_TEST(test_004ParametersInit_0001TxInitSuccess)
{
    BOAT_RESULT rtnVal;
    BoatPlatoneTx tx_ptr = {0};

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert(rtnVal == BOAT_SUCCESS);

    rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_SUCCESS);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_004ParametersInit_0002TxInitFailureNullParam)
{
    BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    rtnVal = BoatPlatoneTxInit(NULL, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);

	rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, NULL, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
				   
	rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, NULL, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
				   
	rtnVal = BoatPlatoneTxInit(NULL, NULL, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
					   
	rtnVal = BoatPlatoneTxInit(NULL, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, NULL, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, NULL, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, NULL, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
						   
    rtnVal = BoatPlatoneTxInit(NULL, NULL, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, NULL, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_004ParametersInit_0003TxInitSuccessNullGasPrice)
{
	BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

	rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, NULL, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);	
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_004ParametersInit_0004TxInitFailureErrorGasPriceHexFormat)
{
    BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);
	rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, "0x123G", 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_004ParametersInit_0005TxInitSuccessGasPriceHexNullOx)
{
	BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);
	rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, "A", 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);	
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_004ParametersInit_0006TxInitFailureGasLimitErrorHexFormat)
{
	BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);
    rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   "0x123G", TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
	ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_004ParametersInit_0007TxInitSuccessGasLimitHexNullOx)
{
    BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   "333333", TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_004ParametersInit_0008TxInitFailureRecipientErrorHexFormat)
{
    BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, "0xABCDG", BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
	ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_004ParametersInit_0009TxInitFailureRecipientLongLength)
{
	BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

	rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, "0xAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA", BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
	ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_004ParametersInit_0010TxInitFailureFailureTxtype)
{
	BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, 0x10);

    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_005ParametersSet_0001GetNonceFromNetworkSuccess)
{
    BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);
	
    rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_SUCCESS);

	rtnVal = BoatPlatoneTxSetNonce(&tx_ptr, BOAT_PLATONE_NONCE_AUTO);	
    ck_assert(rtnVal == BOAT_SUCCESS);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_005ParametersSet_0002SetNonceSuccess)
{
	BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

	rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_SUCCESS);
	
	rtnVal = BoatPlatoneTxSetNonce(&tx_ptr, 0xA1);	
	BoatFieldMax32B NONCE;
    memset(NONCE.field, 0, 32);
	NONCE.field_len = UtilityHexToBin(NONCE.field, 32, "0xA1",
	 				                  TRIMBIN_LEFTTRIM, BOAT_TRUE);
    ck_assert(rtnVal == BOAT_SUCCESS);
	ck_assert_str_eq(tx_ptr.rawtx_fields.nonce.field, NONCE.field);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_005ParametersSet_0003SetNonceFailureNullTx)
{
	BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

	rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_SUCCESS);
	
	rtnVal = BoatPlatoneTxSetNonce(NULL, 0xA1);	
	ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_005ParametersSet_0004SetValueSuccess)
{
	BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

	rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
		                   TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_SUCCESS);

	BoatFieldMax32B value;
    value.field_len = UtilityHexToBin(value.field, 32, "0x2386F26FC10000",
									  TRIMBIN_LEFTTRIM, BOAT_TRUE);
	rtnVal = BoatPlatoneTxSetValue(&tx_ptr, &value);
    ck_assert(rtnVal == BOAT_SUCCESS);

	ck_assert_str_eq(tx_ptr.rawtx_fields.value.field, value.field);
    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_005ParametersSet_0005SetValueFailureNullTx)
{
    BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    ck_assert_int_eq(strlen(TEST_RECIPIENT_ADDRESS), 42);

    rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
                           TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_SUCCESS);

    

    BoatFieldMax32B value;
    value.field_len = UtilityHexToBin(value.field, 32, "0x2386F26FC10000",
                                      TRIMBIN_LEFTTRIM, BOAT_TRUE);
    rtnVal = BoatPlatoneTxSetValue(NULL, &value);
    ck_assert(rtnVal == BOAT_ERROR_COMMON_INVALID_ARGUMENT);

    BoatIotSdkDeInit();
}
END_TEST

START_TEST(test_005ParametersSet_0006SetValueSuccessNullvalue)
{
    BSINT32 rtnVal;
    BoatPlatoneTx tx_ptr;

    BoatIotSdkInit();

    rtnVal = platoneWalletPrepare();
    ck_assert_int_eq(rtnVal, BOAT_SUCCESS);

    rtnVal = BoatPlatoneTxInit(g_platone_wallet_ptr, &tx_ptr, TEST_IS_SYNC_TX, TEST_GAS_PRICE, 
                           TEST_GAS_LIMIT, TEST_RECIPIENT_ADDRESS, BOAT_PLATONE_TX_TYPE_CONTRACT_NULL_TERMED_STR);
    ck_assert(rtnVal == BOAT_SUCCESS);

    rtnVal = BoatPlatoneTxSetValue(&tx_ptr, NULL);
    ck_assert(rtnVal == BOAT_SUCCESS);
    ck_assert_int_eq(tx_ptr.rawtx_fields.value.field_len, 0);

    BoatIotSdkDeInit();
}
END_TEST

Suite *make_parameters_suite(void)
{
    /* Create Suite */
    Suite *s_param = suite_create("param");


    /* Create test cases */
    TCase *tc_param_api = tcase_create("param_api");

    tcase_set_timeout(tc_param_api,50);       

    /* Add a test case to the Suite */
    suite_add_tcase(s_param, tc_param_api);      
 
    /* Test cases are added to the test set */
    tcase_add_test(tc_param_api, test_004ParametersInit_0001TxInitSuccess); 
    tcase_add_test(tc_param_api, test_004ParametersInit_0002TxInitFailureNullParam); 
    tcase_add_test(tc_param_api, test_004ParametersInit_0003TxInitSuccessNullGasPrice); 
    tcase_add_test(tc_param_api, test_004ParametersInit_0004TxInitFailureErrorGasPriceHexFormat); 
    tcase_add_test(tc_param_api, test_004ParametersInit_0005TxInitSuccessGasPriceHexNullOx); 
    tcase_add_test(tc_param_api, test_004ParametersInit_0006TxInitFailureGasLimitErrorHexFormat); 
    tcase_add_test(tc_param_api, test_004ParametersInit_0007TxInitSuccessGasLimitHexNullOx); 
    tcase_add_test(tc_param_api, test_004ParametersInit_0008TxInitFailureRecipientErrorHexFormat); 
    tcase_add_test(tc_param_api, test_004ParametersInit_0009TxInitFailureRecipientLongLength); 
    tcase_add_test(tc_param_api, test_004ParametersInit_0010TxInitFailureFailureTxtype); 

    tcase_add_test(tc_param_api, test_005ParametersSet_0001GetNonceFromNetworkSuccess); 
    tcase_add_test(tc_param_api, test_005ParametersSet_0002SetNonceSuccess); 
    tcase_add_test(tc_param_api, test_005ParametersSet_0003SetNonceFailureNullTx); 
    tcase_add_test(tc_param_api, test_005ParametersSet_0004SetValueSuccess); 
    tcase_add_test(tc_param_api, test_005ParametersSet_0005SetValueFailureNullTx); 
    tcase_add_test(tc_param_api, test_005ParametersSet_0006SetValueSuccessNullvalue); 

    return s_param;
}