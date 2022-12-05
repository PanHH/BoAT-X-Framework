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

/*!@brief hwbcs wallet API for BoAT IoT SDK

@file
api_hw_bcs.c defines the hwbcs wallet API for BoAT IoT SDK.
*/

/* self-header include */
#include "boatinternal.h"
#if PROTOCOL_USE_HWBCS == 1
#include "boatprotocols.h"
#include "http2intf.h"
/* protos include */
#include "peer/proposal_response.pb-c.h"
#include "orderer/cluster.pb-c.h"
#include "common/transaction.pb-c-hwbcs.h"

#if (BOAT_HWBCS_TLS_SUPPORT == 1)
#include "mbedtls/x509_crt.h"
#include "mbedtls/oid.h"
#include "boatutility.h"
#endif

__BOATSTATIC size_t boat_find_oid_value_in_name(const mbedtls_x509_name *name, const char* target_short_name, char *value, size_t value_length)
{
    const char *short_name = NULL;
    bool found = false;
    size_t retval = 0;

    while((name != NULL) && !found)
    {
        // if there is no data for this name go to the next one
        if(!name->oid.p)
        {
            name = name->next;
            continue;
        }

        int ret = mbedtls_oid_get_attr_short_name(&name->oid, &short_name);
        if((ret == 0) && (strcmp(short_name, target_short_name) == 0))
        {
            found = true;
        }

        if(found)
        {
            size_t bytes_to_write = (name->val.len >= value_length) ? value_length - 1 : name->val.len;

            for(size_t i = 0; i < bytes_to_write; i++)
            {
                char c = name->val.p[i];
                // if (c < 32 || c == 127 || (c > 128 && c < 160))
				if (c < 32 || c == 127 )
                {
                    value[i] = '?';
                } else
                {
                    value[i] = c;
                }
            }

            // null terminate
            value[bytes_to_write] = 0;

            retval = name->val.len;
        }

        name = name->next;
    }

    return retval;
}

/*!****************************************************************************
 * @brief Access to the specified node 
 * 
 * @param tx_ptr 
 *   huawei chain transaction structure pointer
 *
 * @param nodeInfo 
*    The first element address of nodeinfo structure.
 *  
 * @param nodeMaxNum 
 *   Support maxium node number.
 *   \n For endorser node, this param is equeal with the macro #BOAT_HWBCS_ENDORSER_MAX_NUM,
 *   for orderer node, this param is equeal with the macro #BOAT_HWBCS_ORDERER_MAX_NUM.
 *
 * @return 
 *   Return \c BOAT_SUCCESS if set successed, otherwise return a failed code.
 ******************************************************************************/
__BOATSTATIC BOAT_RESULT BoatHwbcsTxExec(BoatHwbcsTx *tx_ptr,
										 BoatHwbcsNodesCfg nodeCfg, BoatHwbcsFunType funType)
{
	BOAT_RESULT result = BOAT_SUCCESS;
	BoatHwbcsEndorserResponse *parsePtr = NULL;
	Http2Response fabricHttp2res = {0,NULL};
	Common__Transaction *proposalResponse = NULL;
	Common__Response *resData = NULL;
	Common__TxPayload *resPayload = NULL;
	Common__CommonTxData *commondTxData = NULL;

	// Orderer__SubmitResponse *submitResponse = NULL;
	BUINT8 valid_node_quantities;
	BUINT32 datalen = 0;
	boat_try_declare;
	// DiscoverRes discooverRes = tx_ptr->discooverRes;
	int i, j, k;

	if (tx_ptr == NULL)
	{
		BoatLog(BOAT_LOG_CRITICAL, "Arguments cannot be NULL.");
		return BOAT_ERROR_COMMON_INVALID_ARGUMENT;
	}
	if(tx_ptr->evaluateRes.httpResLen != 0){
		if(tx_ptr->evaluateRes.http2Res != NULL){
			BoatFree(tx_ptr->evaluateRes.http2Res);
		}
		tx_ptr->evaluateRes.httpResLen = 0;
	}


	result = hwbcsProposalTransactionPacked(tx_ptr);
	if (result != BOAT_SUCCESS)
	{
		BoatLog(BOAT_LOG_CRITICAL, "[%s]:packed failed.", tx_ptr->var.args.args[0]);
		boat_throw(BOAT_ERROR_COMMON_PROTO_PACKET_FAIL, BoatHwbcsTxProposal_exception);
	}
	if (tx_ptr->var.type == (BoatHlfabricType)HWBCS_TYPE_PROPOSAL)
	{
		for (i = 0; i < nodeCfg.endorserLayoutNum; i++)
		{
			for (j = 0; j < nodeCfg.layoutCfg[i].endorserGroupNum; j++)
			{
				valid_node_quantities = 0;
				for (k = 0; k < nodeCfg.layoutCfg[i].groupCfg[j].endorserNumber; k++)
				{
					((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->nodeUrl = nodeCfg.layoutCfg[i].groupCfg[j].endorser[k].nodeUrl;
#if (BOAT_HWBCS_TLS_SUPPORT == 1)

					if (((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain != NULL)
					{
						if (((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr != NULL)
						{
							BoatFree(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr);
						}
						((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_len = 0;
						BoatFree(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain);
					}

					((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->hostName = nodeCfg.layoutCfg[i].groupCfg[j].endorser[k].hostName;
					((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain = BoatMalloc(sizeof(BoatFieldVariable));
					if(NULL == ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain){
						BoatLog(BOAT_LOG_CRITICAL, " Fail to malloc tlsCAchain memory");
						boat_throw(BOAT_ERROR_COMMON_OUT_OF_MEMORY, BoatHwbcsTxProposal_exception);
					}
					((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_len = nodeCfg.layoutCfg[i].groupCfg[j].tlsOrgCertContent.length + 1;
					((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr = BoatMalloc(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_len);
					if(NULL == ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr){
						BoatLog(BOAT_LOG_CRITICAL, " Fail to malloc tlsCAchain field_ptr memory");
						boat_throw(BOAT_ERROR_COMMON_OUT_OF_MEMORY, BoatHwbcsTxProposal_exception);
					}
					memset(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr, 0x00, ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_len);
					memcpy(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr, nodeCfg.layoutCfg[i].groupCfg[j].tlsOrgCertContent.content, nodeCfg.layoutCfg[i].groupCfg[j].tlsOrgCertContent.length);
// 			BoatLog_hexasciidump(BOAT_LOG_NORMAL, "tlsCAchain  :",
// 			 ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr,
// 			 ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_len);
#endif
					((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->type = tx_ptr->var.type;

					// ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->parseDataPtr = &tx_ptr->endorserResponse;

					((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->parseDataPtr = &(fabricHttp2res);

					((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->pathTmp = "/nodeservice.Contract/Invoke";
					result = http2SubmitRequest(tx_ptr->wallet_ptr->http2Context_ptr);
					if (result != BOAT_SUCCESS)
					{
						continue;
					}

					proposalResponse = common__transaction__unpack(NULL, fabricHttp2res.httpResLen - 5, fabricHttp2res.http2Res + 5);
					if (fabricHttp2res.httpResLen != 0)
					{
						BoatFree(fabricHttp2res.http2Res);
					}
					fabricHttp2res.httpResLen = 0;

					if ((proposalResponse != NULL) && (proposalResponse->n_approvals != 0) && (proposalResponse->approvals != NULL))
					{

						resData = common__response__unpack(NULL, proposalResponse->payload.len, proposalResponse->payload.data);
						if (resData->status != COMMON__STATUS__HWBCS__SUCCESS)
						{
							continue;
						}

						if ((resData->payload.data[1] & 0x80) == 0x80)
						{
							datalen = (resData->payload.data[1] & 0x7F) + (resData->payload.data[2] << 7);
							resPayload = common__tx_payload__unpack(NULL, datalen, resData->payload.data + 3);
						}
						else
						{
							datalen = resData->payload.data[1];
							resPayload = common__tx_payload__unpack(NULL, datalen, resData->payload.data + 2);
						}
						commondTxData = common__common_tx_data__unpack(NULL, resPayload->data.len, resPayload->data.data);
						if (commondTxData->response->status != COMMON__STATUS__HWBCS__SUCCESS)
						{
							continue;
						}

						parsePtr->response[parsePtr->responseCount].contentPtr = proposalResponse;
						parsePtr->response[parsePtr->responseCount].responseType = (BoatHlfabricType)HWBCS_TYPE_PROPOSAL;
						parsePtr->response[parsePtr->responseCount].payload.field_len = resData->payload.len;
						parsePtr->response[parsePtr->responseCount].payload.field_ptr = BoatMalloc(resData->payload.len);
						if(NULL == parsePtr->response[parsePtr->responseCount].payload.field_ptr){
							BoatLog(BOAT_LOG_CRITICAL, " Fail to malloc payload field_ptr memory");
							boat_throw(BOAT_ERROR_COMMON_OUT_OF_MEMORY, BoatHwbcsTxProposal_exception);
						}						
						memcpy(parsePtr->response[parsePtr->responseCount].payload.field_ptr, resData->payload.data, resData->payload.len);
						tx_ptr->evaluateRes.httpResLen = commondTxData->response->payload.len;
						tx_ptr->evaluateRes.http2Res = BoatMalloc(tx_ptr->evaluateRes.httpResLen);
						if(NULL == tx_ptr->evaluateRes.http2Res){
							BoatLog(BOAT_LOG_CRITICAL, " Fail to malloc http2Res memory");
							boat_throw(BOAT_ERROR_COMMON_OUT_OF_MEMORY, BoatHwbcsTxProposal_exception);
						}	
						memcpy(tx_ptr->evaluateRes.http2Res,commondTxData->response->payload.data,commondTxData->response->payload.len);

						// parsePtr->http2res.httpResLen = commondTxData->response->payload.len;
						// parsePtr->http2res.http2Res = BoatMalloc(parsePtr->http2res.httpResLen);
						// memcpy(parsePtr->http2res.http2Res, commondTxData->response->payload.data, commondTxData->response->payload.len);
						// parsePtr->http2Res =  commondTxData->response->payload.data;
						parsePtr->responseCount++;
						common__common_tx_data__free_unpacked(commondTxData, NULL);
						common__response__free_unpacked(resData, NULL);
					}
					else
					{
						continue;
					}
					if (funType == HWBCS_FUN_EVALUATE)
					{
						return result;
					}
					valid_node_quantities++;
					if (valid_node_quantities >= nodeCfg.layoutCfg[i].groupCfg[j].quantities)
					{
						if (nodeCfg.layoutCfg[i].endorserGroupNum == (j + 1))
						{
							return BOAT_SUCCESS;
						}
						break;
					}
				}
				if (valid_node_quantities < nodeCfg.layoutCfg[i].groupCfg[j].quantities)
				{
					BoatLog(BOAT_LOG_CRITICAL, "http2SubmitRequest failed ");
					boat_throw(BOAT_ERROR, BoatHwbcsTxProposal_exception);
				}
			}
		}
	}
	else
	{
		for (i = 0; i < nodeCfg.orderCfg.endorserNumber; i++)
		{
			((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->nodeUrl = nodeCfg.orderCfg.endorser[i].nodeUrl;

#if (BOAT_HWBCS_TLS_SUPPORT == 1)

			((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->hostName = nodeCfg.orderCfg.endorser[i].hostName;
			// ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain = nodeCfg.orderCfg.tlsOrgCertContent.content;
			if (((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain != NULL)
			{
				if (((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr != NULL)
				{
					BoatFree(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr);
				}
				((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_len = 0;
				BoatFree(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain);
			}
			((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain = BoatMalloc(sizeof(BoatFieldVariable));
			if(NULL == ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain){
				BoatLog(BOAT_LOG_CRITICAL, " Fail to malloc tlsCAchain memory");
				boat_throw(BOAT_ERROR_COMMON_OUT_OF_MEMORY, BoatHwbcsTxProposal_exception);
			}
			((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_len = nodeCfg.orderCfg.tlsOrgCertContent.length + 1;
			((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr = BoatMalloc(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_len);
			if(NULL == ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr){
				BoatLog(BOAT_LOG_CRITICAL, " Fail to malloc tlsCAchain field_ptr memory");
				boat_throw(BOAT_ERROR_COMMON_OUT_OF_MEMORY, BoatHwbcsTxProposal_exception);
			}			
			memset(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr, 0x00, ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_len);
			memcpy(((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->tlsCAchain[0].field_ptr, nodeCfg.orderCfg.tlsOrgCertContent.content, nodeCfg.orderCfg.tlsOrgCertContent.length);

#endif

			((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->type = tx_ptr->var.type;

			((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->parseDataPtr = &fabricHttp2res;

			// BoatLog_hexasciidump(BOAT_LOG_NORMAL, "http2SubmitRequest 111 send :",
			// 					 ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->sendBuf.field_ptr,
			// 					 ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->sendBuf.field_len);
			parsePtr = ((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->parseDataPtr;
			((http2IntfContext*)(tx_ptr->wallet_ptr->http2Context_ptr))->pathTmp = "/nodeservice.TransactionSender/SendTransaction";
			result = http2SubmitRequest(tx_ptr->wallet_ptr->http2Context_ptr);
			if (result != BOAT_SUCCESS)
			{
				BoatLog(BOAT_LOG_CRITICAL, "[http2]http2SubmitRequest failed.");
				continue;
			}

			resData = common__response__unpack(NULL, fabricHttp2res.httpResLen - 7, fabricHttp2res.http2Res + 7);
			BoatLog(BOAT_LOG_NORMAL, "[http2]common__response__unpack respond status : %d .", resData->status);
			if (fabricHttp2res.httpResLen != 0)
			{
				BoatFree(fabricHttp2res.http2Res);
			}
			fabricHttp2res.httpResLen = 0;
			if (resData->status == COMMON__STATUS__HWBCS__SUCCESS)
			{
				result = BOAT_SUCCESS;
				break;
			}
			else
			{
				BoatLog(BOAT_LOG_CRITICAL, "[http2]orderer respond unpacked failed.");
				result = BOAT_ERROR;
				continue;
			}
		}
	}

	//! boat catch handle
	boat_catch(BoatHwbcsTxProposal_exception)
	{
		BoatLog(BOAT_LOG_CRITICAL, "Exception: %d", boat_exception);
		result = boat_exception;
	}

	return result;
}

BOAT_RESULT BoatHwbcsWalletSetAccountInfo(BoatHwbcsWallet *wallet_ptr,
										  const BoatWalletPriKeyCtx_config prikeyCtx_config,
										  const BoatHwbcsCertInfoCfg certContent)
{
	return BoatHlfabricWalletSetAccountInfo(wallet_ptr, prikeyCtx_config, certContent);
}

#if (BOAT_HWBCS_TLS_SUPPORT == 1) && (BOAT_HWBCS_TLS_IDENTIFY_CLIENT == 1)
BOAT_RESULT BoatHwbcsWalletSetTlsClientInfo(BoatHwbcsWallet *wallet_ptr,
											const BoatWalletPriKeyCtx_config prikeyCtx_config,
											const BoatHwbcsCertInfoCfg certContent)
{
	BOAT_RESULT result = BOAT_SUCCESS;
	boat_try_declare;

	if (wallet_ptr == NULL)
	{
		BoatLog(BOAT_LOG_CRITICAL, "wallet_ptr should not be NULL.");
		return BOAT_ERROR_COMMON_INVALID_ARGUMENT;
	}

	/* initialization */
	memset(&wallet_ptr->tlsClinet_info.prikeyCtx, 0, sizeof(BoatWalletPriKeyCtx));
	wallet_ptr->tlsClinet_info.cert.field_ptr = NULL;
	wallet_ptr->tlsClinet_info.cert.field_len = 0;

	/* prikey context assignment */
	memcpy(&wallet_ptr->tlsClinet_info.prikeyCtx,
		   &prikeyCtx_config.private_KeyCtx, sizeof(BoatWalletPriKeyCtx));

	/* cert assignment */
	wallet_ptr->tlsClinet_info.cert.field_ptr = BoatMalloc(certContent.length);
	if (wallet_ptr->tlsClinet_info.cert.field_ptr == NULL)
	{
		BoatLog(BOAT_LOG_CRITICAL, "BoatMalloc failed.");
		boat_throw(BOAT_ERROR_COMMON_OUT_OF_MEMORY, BoatHwbcsWalletSetTlsInfo_exception);
	}
	memcpy(wallet_ptr->tlsClinet_info.cert.field_ptr, certContent.content, certContent.length);
	wallet_ptr->tlsClinet_info.cert.field_len = certContent.length;

	/* boat catch handle */
	boat_catch(BoatHwbcsWalletSetTlsInfo_exception)
	{
		BoatLog(BOAT_LOG_CRITICAL, "Exception: %d", boat_exception);
		result = boat_exception;
		/* free malloc param Deinit */
		BoatFree(wallet_ptr->tlsClinet_info.cert.field_ptr);
		wallet_ptr->tlsClinet_info.cert.field_len = 0;
	}

	return result;
}
#endif

#if (BOAT_HWBCS_TLS_SUPPORT == 1)
BOAT_RESULT BoatHwbcsWalletSetRootCaInfo(BoatHwbcsWallet *wallet_ptr,
										 const BoatHwbcsCertInfoCfg *rootCaContent,
										 BUINT32 rootCaNumber)
{
	return BoatHlfabricWalletSetRootCaInfo(wallet_ptr, rootCaContent, rootCaNumber);
}
#endif

BOAT_RESULT BoatHwbcsWalletSetNetworkInfo(BoatHwbcsWallet *wallet_ptr,
										  const BoatHwbcsNodesCfg endorserInfo_ptr)
{

	return BoatHlfabricWalletSetNetworkInfo(wallet_ptr, endorserInfo_ptr);
}

BoatHwbcsWallet *BoatHwbcsWalletInit(const BoatHwbcsWalletConfig *config_ptr,
									 BUINT32 config_size)
{
	return BoatHlfabricWalletInit(config_ptr, config_size);
}

void BoatHwbcsWalletDeInit(BoatHwbcsWallet *wallet_ptr)
{
	BoatHlfabricWalletDeInit(wallet_ptr);
}

BOAT_RESULT BoatHwbcsTxInit(BoatHwbcsTx *tx_ptr,
							const BoatHwbcsWallet *wallet_ptr,
							const BCHAR *chaincodeId_path_str,
							const BCHAR *chaincodeId_name_str,
							const BCHAR *chaincodeId_version_str,
							const BCHAR *channelId_str,
							const BCHAR *orgName_str,
							const BCHAR *contract_name)
{
	BUINT32 stringLen;
	BCHAR *paramSrcList[6];
	BCHAR **paramDstList[6];
	BUINT16 i = 0;
	BOAT_RESULT result = BOAT_SUCCESS;

	boat_try_declare;

	if ((tx_ptr == NULL) || (wallet_ptr == NULL))
	{
		BoatLog(BOAT_LOG_CRITICAL, "Arguments 'tx_ptr' or 'wallet_ptr' cannot be NULL.");
		return BOAT_ERROR_COMMON_INVALID_ARGUMENT;
	}

	/* tx_ptr instance reset */
	/* ----->tx_ptr->wallet_ptr reset */
	tx_ptr->wallet_ptr = NULL;
	/* ----->tx_ptr->var reset */
	tx_ptr->var.type = HLFABRIC_TYPE_IDLE;
	tx_ptr->var.timestamp.sec = 0;
	tx_ptr->var.timestamp.nanos = 0;
	tx_ptr->var.nonce.field_len = 0;
	for (i = 0; i < 24; i++)
	{
		tx_ptr->var.nonce.field[i] = 0;
	}
	tx_ptr->var.chaincodeId.name = NULL;
	tx_ptr->var.chaincodeId.path = NULL;
	tx_ptr->var.chaincodeId.version = NULL;
	tx_ptr->var.args.nArgs = 0;
	for (i = 0; i < BOAT_HLFABRIC_ARGS_MAX_NUM; i++)
	{
		tx_ptr->var.args.args[i] = NULL;
	}
	tx_ptr->var.channelId = NULL;
	tx_ptr->var.orgName = NULL;
	/* ----->tx_ptr->endorserResponse reset */
	tx_ptr->endorserResponse.responseCount = 0;
	for (i = 0; i < BOAT_HLFABRIC_ENDORSER_MAX_NUM; i++)
	{
		tx_ptr->endorserResponse.response[i].contentPtr = NULL;
		tx_ptr->endorserResponse.response[i].responseType = HLFABRIC_TYPE_IDLE;
		tx_ptr->endorserResponse.response[i].payload.field_len = 0;
		tx_ptr->endorserResponse.response[i].payload.field_ptr = NULL;
		tx_ptr->endorserResponse.response[i].endorser.field_len = 0;
		tx_ptr->endorserResponse.response[i].endorser.field_ptr = NULL;
		tx_ptr->endorserResponse.response[i].signature.field_len = 0;
		tx_ptr->endorserResponse.response[i].signature.field_ptr = NULL;
	}

	/* wallet assignment */
	tx_ptr->wallet_ptr = (BoatHlfabricWallet *)wallet_ptr;

	/* paramList assignment */
	paramSrcList[0] = (BCHAR *)chaincodeId_path_str;
	paramSrcList[1] = (BCHAR *)chaincodeId_name_str;
	paramSrcList[2] = (BCHAR *)chaincodeId_version_str;
	paramSrcList[3] = (BCHAR *)channelId_str;
	paramSrcList[4] = (BCHAR *)orgName_str;
	paramSrcList[5] = (BCHAR *)contract_name;
	paramDstList[0] = &tx_ptr->var.chaincodeId.path;
	paramDstList[1] = &tx_ptr->var.chaincodeId.name;
	paramDstList[2] = &tx_ptr->var.chaincodeId.version;
	paramDstList[3] = &tx_ptr->var.channelId;
	paramDstList[4] = &tx_ptr->var.orgName;
	paramDstList[5] = &tx_ptr->var.contract_name;

	for (i = 0; i < sizeof(paramSrcList) / sizeof(paramSrcList[0]); i++)
	{
		if ((paramSrcList[i] != NULL) && ((stringLen = strlen(paramSrcList[i])) > 0))
		{
			*paramDstList[i] = BoatMalloc(stringLen + 1);
			if (*paramDstList[i] == NULL)
			{
				BoatLog(BOAT_LOG_CRITICAL, "BoatMalloc failed.");
				boat_throw(BOAT_ERROR_COMMON_OUT_OF_MEMORY, BoatHlfabricTxInit_exception);
			}
			memcpy(*paramDstList[i], paramSrcList[i], stringLen + 1);
		}
	}
#if (BOAT_HWBCS_TLS_SUPPORT == 1)
	mbedtls_x509_crt m_certificate;
	mbedtls_x509_crt_init(&m_certificate);
	uint32_t status = mbedtls_x509_crt_parse(&m_certificate, (const unsigned char *)tx_ptr->wallet_ptr->account_info.cert.field_ptr, tx_ptr->wallet_ptr->account_info.cert.field_len);
	if(status != 0){
		BoatLog(BOAT_LOG_CRITICAL, "certificate parse failed.");
		boat_throw(BOAT_ERROR, BoatHlfabricTxInit_exception);		
	}
	const mbedtls_x509_name *name = &m_certificate.subject;
	char value[64];
	size_t value_len;

	value_len = boat_find_oid_value_in_name(name, "CN", value, sizeof(value));
	if (value_len)
	{
		tx_ptr->var.creator_id = BoatMalloc(value_len + 1);
		if(NULL == tx_ptr->var.creator_id){
			BoatLog(BOAT_LOG_CRITICAL, "BoatMalloc failed.");
			boat_throw(BOAT_ERROR_COMMON_OUT_OF_MEMORY, BoatHlfabricTxInit_exception);
		}
		memcpy(tx_ptr->var.creator_id,value,value_len);
	}


#endif

	/* boat catch handle */
	boat_catch(BoatHlfabricTxInit_exception)
	{
		BoatLog(BOAT_LOG_CRITICAL, "Exception: %d", boat_exception);
		result = boat_exception;
		/* free malloc param Deinit */
		for (i = 0; i < sizeof(paramDstList) / sizeof(paramDstList[0]); i++)
		{
			BoatFree(paramDstList[i]);
		}
	}

	return result;
}

void BoatHwbcsTxDeInit(BoatHwbcsTx *tx_ptr)
{
	BoatHlfabricTxDeInit(tx_ptr);
}

BOAT_RESULT BoatHwbcsTxSetTimestamp(BoatHwbcsTx *tx_ptr,
									const BUINT64 sec,
									const BUINT64 nanos)
{
	return BoatHlfabricTxSetTimestamp(tx_ptr, sec, nanos);
}

BOAT_RESULT BoatHwbcsTxEvaluate(BoatHwbcsTx *tx_ptr)
{
	// BoatHlfabricNodeInfo urlTmp[2]    = {{NULL,NULL}, {NULL,NULL}};
	BOAT_RESULT result = BOAT_SUCCESS;
	// boat_try_declare;

	if (tx_ptr == NULL)
	{
		BoatLog(BOAT_LOG_CRITICAL, "Arguments cannot be NULL.");
		return BOAT_ERROR_COMMON_INVALID_ARGUMENT;
	}
	if (tx_ptr->var.args.args[0] == NULL)
	{
		BoatLog(BOAT_LOG_CRITICAL, "Arguments args[0] cannot be NULL.");
		return BOAT_ERROR_COMMON_INVALID_ARGUMENT;
	}

	BoatLog(BOAT_LOG_NORMAL, "Evaluate will execute...");

	/* submit query */
	tx_ptr->var.type = (BoatHlfabricType)HWBCS_TYPE_PROPOSAL;
	// urlTmp[0] = tx_ptr->wallet_ptr->network_info.endorser[0];
	result = BoatHwbcsTxExec(tx_ptr, tx_ptr->wallet_ptr->network_info, HWBCS_FUN_EVALUATE);

	/* free the unpacked response data */
	for (int i = 0; i < tx_ptr->endorserResponse.responseCount; i++)
	{
		if (tx_ptr->endorserResponse.response[i].responseType == (BoatHlfabricType)HWBCS_TYPE_PROPOSAL)
		{
			common__transaction__free_unpacked(tx_ptr->endorserResponse.response[i].contentPtr, NULL);
			if (tx_ptr->endorserResponse.response[i].payload.field_len != 0)
			{
				BoatFree(tx_ptr->endorserResponse.response[i].payload.field_ptr);
				tx_ptr->endorserResponse.response[i].payload.field_len = 0;
			}
		}
	}
	tx_ptr->endorserResponse.responseCount = 0;

	return result;
}

BOAT_RESULT BoatHwbcsTxSubmit(BoatHwbcsTx *tx_ptr)
{
	BOAT_RESULT result = BOAT_SUCCESS;
	// boat_try_declare;

	if (tx_ptr == NULL)
	{
		BoatLog(BOAT_LOG_CRITICAL, "Arguments cannot be NULL.");
		return BOAT_ERROR_COMMON_INVALID_ARGUMENT;
	}
	if (tx_ptr->var.args.args[0] == NULL)
	{
		BoatLog(BOAT_LOG_CRITICAL, "Arguments args[0] cannot be NULL.");
		return BOAT_ERROR_COMMON_INVALID_ARGUMENT;
	}

	BoatLog(BOAT_LOG_NORMAL, "Submit will execute...");

	/* invoke-step1: submit proposal to endorer */
	tx_ptr->var.type = (BoatHlfabricType)HWBCS_TYPE_PROPOSAL;
	result = BoatHwbcsTxExec(tx_ptr, tx_ptr->wallet_ptr->network_info, HWBCS_FUN_SUBMIT);
	if (result != BOAT_SUCCESS)
	{
		return result;
	}
	BoatLog(BOAT_LOG_NORMAL, "Submit proposal OK ...");
	/* invoke-step2: submit transaction to orderer */
	tx_ptr->var.type = (BoatHlfabricType)HWBCS_TYPE_TRANSACTION;
	result = BoatHwbcsTxExec(tx_ptr, tx_ptr->wallet_ptr->network_info, HWBCS_FUN_SUBMIT);
	if (result != BOAT_SUCCESS)
	{
		return result;
	}
	/* free the unpacked response data */
	for (int i = 0; i < tx_ptr->endorserResponse.responseCount; i++)
	{
		if (tx_ptr->endorserResponse.response[i].responseType == (BoatHlfabricType)HWBCS_TYPE_PROPOSAL)
		{
			common__transaction__free_unpacked(tx_ptr->endorserResponse.response[i].contentPtr, NULL);
		}
		else
		{
			// orderer__submit_response__free_unpacked(tx_ptr->endorserResponse.response[i].contentPtr, NULL);
		}
	}
	tx_ptr->endorserResponse.responseCount = 0;

	return result;
}
#endif /* end of PROTOCOL_USE_HWBCS */
