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
#include "tcase_venachain.h"

#define TEST_EIP155_COMPATIBILITY   BOAT_FALSE
#define TEST_VENACHAIN_CHAIN_ID      300

BoatVenachainNetworkConfig get_venachain_network_settings()
{
    g_venachain_network_config.chain_id             = TEST_VENACHAIN_CHAIN_ID;
    g_venachain_network_config.eip155_compatibility = TEST_EIP155_COMPATIBILITY;
    memset(g_venachain_network_config.node_url_str,0U,BOAT_VENACHAIN_NODE_URL_MAX_LEN);
    strncpy(g_venachain_network_config.node_url_str, TEST_VENACHAIN_NODE_URL, BOAT_VENACHAIN_NODE_URL_MAX_LEN - 1);

    return g_venachain_network_config;
}

BOAT_RESULT check_venachain_networkData(BoatVenachainNetworkData *networkData,BUINT8 networkIndex,BoatVenachainNetworkConfig *networkConfig)
{
    BOAT_RESULT ret = BOAT_ERROR;

    if((networkData->networkIndex != networkIndex) || \
        (networkData->chain_id != networkConfig->chain_id) || \
        (networkData->eip155_compatibility != networkConfig->eip155_compatibility) || \
        (0 != strncmp(networkData->node_url_str,networkConfig->node_url_str,strlen(networkData->node_url_str))) || \
        (strlen(networkData->node_url_str) != strlen(networkConfig->node_url_str)))
    {
        return ret;
    }

    return BOAT_SUCCESS;

}

START_TEST(test_003CreateNetwork_0001CreateOnetimeNetworkSuccess) 
{
    BOAT_RESULT ret;
    BOAT_RESULT networkIndex;
    BoatVenachainNetworkConfig networkConfig;

    BoatIotSdkInit();
    /* 1. Create network */
    networkConfig = get_venachain_network_settings();
    networkIndex = BoatVenachainNetworkCreate(&networkConfig,BOAT_STORE_TYPE_RAM);
    ck_assert_int_eq(networkIndex, 0);

    /* 2.Verify network */
    BoatVenachainNetworkData networkData;
    ret = BoATVenachain_GetNetworkByIndex(&networkData,networkIndex);
    ck_assert_int_eq(ret, BOAT_SUCCESS);

    ret = check_venachain_networkData(&networkData,networkIndex,&networkConfig);
    ck_assert_int_eq(ret, BOAT_SUCCESS);

    /* 3.Check keypair list */
    

    BoatIotSdkDeInit();
}
END_TEST


Suite *make_network_suite(void) 
{
    /* Create Suite */
    Suite *s_network = suite_create("network");

    /* Create test cases */
    TCase *tc_network_api = tcase_create("network_api");

    /* Add a test case to the Suite */
    suite_add_tcase(s_network, tc_network_api);       
    /* Test cases are added to the test set */
    tcase_add_test(tc_network_api, test_003CreateNetwork_0001CreateOnetimeNetworkSuccess);  


    return s_keypair;
}

