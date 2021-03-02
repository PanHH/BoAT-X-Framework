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

/*!@brief Perform RAW transaction

@file
ethereum.c contains functions to construct a raw transaction, serialize it with RLP,
perform it and wait for its receipt.
*/

#include "boatinternal.h"
#if PROTOCOL_USE_PLATON == 1
#include "web3intf.h"
#include "boatethereum.h"
#include "boatplaton.h"


BOAT_RESULT PlatonSendRawtx(BOAT_INOUT BoatPlatonTx *tx_ptr)
{
    return EthSendRawtx((BoatEthTx *) tx_ptr);
}

BOAT_RESULT PlatonSendRawtxWithReceipt(BOAT_INOUT BoatPlatonTx *tx_ptr)
{
    return EthSendRawtxWithReceipt((BoatEthTx *) tx_ptr);
}
#endif /* end of PROTOCOL_USE_PLATON*/
