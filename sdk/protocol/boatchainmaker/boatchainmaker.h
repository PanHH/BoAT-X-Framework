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
boatchainamaker.h is header file for RAW transaction construction and performing.
*/

#ifndef __BOATCHAINMAKER_H__
#define __BOATCHAINMAKER_H__

#include "boatconfig.h"
#include "boatinternal.h"
BOAT_RESULT hlchainmakerTransactionPacked(BoatHlchainmakerTx *tx_ptr, char* method,BoatTransactionPara *transaction_para, TxType tx_type);
void get_time_string(char* time_buf);
void array_to_str(char* array, char *str, char lenth);

#ifdef __cplusplus
extern "C" {
#endif

/*! @}*/

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif
