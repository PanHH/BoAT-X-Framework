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

/*!@brief Header file for performing RAW transaction

@file
boatcita.h is header file for RAW transaction construction and performing.
*/

#ifndef __BOATCITA_H__
#define __BOATCITA_H__

#include "boatinternal.h"


/*! @defgroup cita protocol-cita 
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * @brief Construct a raw cita transacton asynchronously.
 *
 * @details
 *   This function constructs a raw transacton and sends it asynchronously (i.e.
 *   don't wait for it being mined).   
 *   \n AN INTRODUCTION OF HOW RAW TRANSACTION IS CONSTRUCTED
 *   \n <b>[FIELDS IN A RAW TRANSACTION]</b>
 *                                          
 *   A RAW transaction consists of following 9 fields:
 *       1. nonce;
 *       2. quota;
 *       3. valid_until_block;
 *       4. data;
 *       5. value(optional);
 *       6. chain_id;
 *       7. version;
 *       8. to_v1;
 *       9. chain_id_v1;
 *   \n These transaction fields are encoded as elements of a LIST in above order
 *   as per RLP encoding rules. "LIST" is a type of RLP field.
 *
 *   EXCEPTION:   
 *   For Cita any fields (except <recipient>) having a value of zero are
 *   treated as NULL stream in RLP encoding instead of 1-byte-size stream whose
 *   value is 0. For example, nonce = 0 is encoded as 0x80 which represents NULL
 *   instead of 0x00 which represents a 1-byte-size stream whose value is 0.
 *
 *   <b>[HOW TO CONSTRUCT A RAW TRANSACTION]</b>  
 *   A RAW transaction is constructed in 4 steps in different ways according to
 *   the blockchain network's EIP-155 compatibility. 
 *  
 *   <I>CASE 1: If the blockchain network does NOT support EIP-155:</I> 
 *       \n - Step 1: Encode a LIST containing only the first 6 fields.
 *       \n - Step 2: Calculate SHA3 hash of the encoded stream in Step 1.
 *       \n - Step 3: Sign the hash in Step 2. This generates r, s and parity (0 or 1) for recovery identifier.
 *       \n - Step 4: Encode a LIST containing all 9 fields, where
 *               First 6 fields are same as what they are;
 *               v = parity + 27, where parity is given in Step 3;
 *               r and s are given in Step 3.
 
 * @param[in] tx_ptr
 *       A pointer to the context of the transaction.
 *		
 * @return
 *   This function returns BOAT_SUCCESS if successful. Otherwise it returns one
 *   of the error codes.
 ******************************************************************************/

BOAT_RESULT CitaSendRawtx(BOAT_INOUT BoatCitaTx *tx_ptr);

/*!*****************************************************************************
@brief Parse RPC method RESPONSE

   This function Parse "result" segment.
   If "result" object is string, this function will returns contents of "result" . 
   If "result" object is still json object, the parameter named "child_name" will actived,
   if "child_name" object is string, this function will returns contents of "child_name"; 
   if "child_name" object is other types, this function will prompt "un-implemention yet".
   For other types of "result" this function is not support yet.

@param[in] json_string
     The json to be parsed.

@param[in] child_name
     if "result" item is json object, this param will actived.

@param[out] result_out
     The buffer to store parse result.
     Caller can allocate memory for this param, or can initial it with {NULL, 0},
     this function will expand the memory if it too small to store parse result.
     
@return
    This function returns BOAT_SUCCESS if parse successed. Otherwise
    it returns an error code.
*******************************************************************************/
BOAT_RESULT cita_parse_json_result(const BCHAR *json_string, 
                                  const BCHAR *child_name, 
                                  BoatFieldVariable *result_out);

/*! @}*/

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif
