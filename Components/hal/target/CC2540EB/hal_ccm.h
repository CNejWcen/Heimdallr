/**************************************************************************************************
  Filename:       hal_ccm.h
  Revised:        $Date: 2007-07-06 10:42:24 -0700 (Fri, 06 Jul 2007) $
  Revision:       $Revision: 13579 $

  Description:    Describe the purpose and contents of the file.
**************************************************************************************************/

#ifndef     HAL_CCM_H_
#define     HAL_CCM_H_

Status_t SSP_CCM_Auth (uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 *, uint8 *, uint8);
Status_t SSP_CCM_Encrypt (uint8, uint8 *, uint8 *, uint16, uint8 *, uint8 *, uint8);
Status_t SSP_CCM_Decrypt (uint8, uint8 *, uint8 *, uint16, uint8 *, uint8 *, uint8);
Status_t SSP_CCM_InvAuth (uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 *, uint8 *, uint8);
Status_t SSP_CCM_Auth_Encrypt (bool, uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 *, uint8 *, uint8);
Status_t SSP_CCM_InvAuth_Decrypt (bool, uint8, uint8 *, uint8 *, uint16, uint8 *, uint16, uint8 *, uint8 *, uint8);

#endif  // HAL_CCM_H_

