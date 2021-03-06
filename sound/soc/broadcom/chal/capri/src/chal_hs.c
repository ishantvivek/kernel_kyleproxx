/*
 * Copyright 2012 Broadcom Corporation
*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation (the "GPL").
*
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
*
 * A copy of the GPL is available at 
 * http://www.broadcom.com/licenses/GPLv2.php, or by writing to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
/**
*
*  @file   chal_hs.c
*
*  @brief  chal layer driver for audio device driver
*
****************************************************************************/
#include "chal_caph_audioh.h"
#include "chal_audio_int.h"
#include <mach/rdb/brcm_rdb_audioh.h>
#include <chal/chal_util.h>


/*
 * ****************************************************************************
 *                         G L O B A L   S E C T I O N
 * ****************************************************************************
 */

/*
 * ****************************************************************************
 *  global variable definitions
 * ****************************************************************************
 */


/*
 * ****************************************************************************
 *                          L O C A L   S E C T I O N
 * ****************************************************************************
 */

/*
 * ****************************************************************************
 *  local macro declarations
 * ****************************************************************************
 */


/*
 * ****************************************************************************
 *  local typedef declarations
 * ****************************************************************************
 */



/*
 * ****************************************************************************
 *  local variable definitions
 * ****************************************************************************
 */


/*
 * ****************************************************************************
 *  local function declarations
 * ****************************************************************************
 */



/*
 * ****************************************************************************
 *  local function definitions
 * ****************************************************************************
 */
/* to take care of EP/HS use case */
static Boolean isHS = FALSE;




/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_enable(CHAL_HANDLE handle,
 *	cUInt16 enable)
 *
 *  Description:  Enable or Disable headset path
 *
 *  Parameters:   handle - audio chal handle.
 *                enable - enable_chn: bit1 (left channel)true : enable
 *							false : disable.
 *                         enable_chn : bit0 (rightchannel )true : enable
 *							false : disable.
 *  Return:       None.
 *
 */
/*============================================================================*/
void chal_audio_hspath_enable(CHAL_HANDLE handle, cUInt16 enable_chan)
{
	cUInt32 base =    ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32 reg_val;
	cUInt32 Channl_Ctrl = 0x00000000;


	if (enable_chan&CHAL_AUDIO_CHANNEL_LEFT)
		Channl_Ctrl |= AUDIOH_DAC_CTL_STEREO_L_ENABLE_MASK;

	if (enable_chan&CHAL_AUDIO_CHANNEL_RIGHT)
		Channl_Ctrl |= AUDIOH_DAC_CTL_STEREO_R_ENABLE_MASK;

	reg_val = BRCM_READ_REG(base, AUDIOH_DAC_CTL);
	reg_val &= ~(AUDIOH_DAC_CTL_STEREO_L_ENABLE_MASK |
				 AUDIOH_DAC_CTL_STEREO_R_ENABLE_MASK);
	reg_val |= Channl_Ctrl;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_DAC_CTL, reg_val);

	return;
}

/*
 * =====================================================================
 *
 *	Function Name: void chal_audio_hspath_int_enable(CHAL_HANDLE handle,
 *				cUInt16  thr_int_enable, cUInt16 err_int_enable)
 *
 *	Description:  Enable or Disable headset path interrupt
 *
 *	Parameters:   handle - audio chal handle.
 *                int_enable - true : enable interrupt
 *					false : disable interrupt
 *	Return:       None.
 *
 * ====================================================================
 */
void chal_audio_hspath_int_enable(CHAL_HANDLE handle, cUInt16 thr_int_enable,
					cUInt16 err_int_enable)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32 reg_val;

	reg_val = BRCM_READ_REG(base, AUDIOH_AUDIO_INTC);

	if (thr_int_enable == CHAL_AUDIO_ENABLE)
		reg_val |= AUDIOH_AUDIO_INTC_STEREO_INTEN_MASK;
	else
		reg_val &= ~AUDIOH_AUDIO_INTC_STEREO_INTEN_MASK;

	if (err_int_enable == CHAL_AUDIO_ENABLE)
		reg_val |= AUDIOH_AUDIO_INTC_STEREO_FIFO_ERRINT_EN_MASK;
	else
		reg_val &= ~AUDIOH_AUDIO_INTC_STEREO_FIFO_ERRINT_EN_MASK;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_AUDIO_INTC, reg_val);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_dma_enable(CHAL_HANDLE handle,
 *	cUInt16 dma_enable)
 *
 *	Description:  Enable or Disable DMA on headset path
 *
 *	Parameters:   handle     - audio chal handle.
 *                dma_enable - true: enable DMA, false : disable DMA.
 *	Return:       None.
 *
 * ============================================================================
 */

void chal_audio_hspath_dma_enable(CHAL_HANDLE handle, _Bool dma_enable)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32 reg_val;

	reg_val = BRCM_READ_REG(base, AUDIOH_DMA_CTL);

	if (dma_enable)
		reg_val |= AUDIOH_DMA_CTL_STEREO_DMA_EN_MASK;
	else
		reg_val &= ~AUDIOH_DMA_CTL_STEREO_DMA_EN_MASK;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_DMA_CTL, reg_val);

	return;
}
/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_mute(CHAL_HANDLE handle,
 *				_Bool mute)
 *
 *	Description:  Mute or unmute on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *                mute - true: mute opeation, false : unmute opeartion.
 *	Return:       None.
 *
 * ============================================================================
 */

void chal_audio_hspath_mute(CHAL_HANDLE handle,  _Bool mute)
{
	cUInt32     reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	reg_val = BRCM_READ_REG(base, AUDIOH_DAC_CTL);

	if (mute == CHAL_AUDIO_ENABLE)
		reg_val |= AUDIOH_DAC_CTL_STEREO_MUTE_MASK;
	else
		reg_val &= ~AUDIOH_DAC_CTL_STEREO_MUTE_MASK;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_DAC_CTL, reg_val);

	return;
}

/*
 * =========================================================================
 *
 *	Function Name: void chal_audio_hspath_set_gain(CHAL_HANDLE handle,
 *			cUInt32 gain)
 *
 *	Description:  Set the gain on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *                gain   - gain valaue.
 *	Return:       None.
 *
 * ==========================================================================
 */

void chal_audio_hspath_set_gain(CHAL_HANDLE handle, cUInt32 gain)
{
	cUInt32 reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *) handle)->audioh_base;

	reg_val = 0x100A00;	/* Might need to add in sysparm. */

	/* Set the required setting */
	BRCM_WRITE_REG(base, AUDIOH_HS_DAC_CTRL, reg_val);

	return;
}

/*
 * ==========================================================================
 *
 *	Function Name: void chal_audio_hspath_set_bits_per_sample
 *		(CHAL_HANDLE handle, cUInt16 bits)
 *
 *	Description:  Set the sample bits on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *                bits   - sample bits value.
 *	Return:       None.
 *
 * ============================================================================
 */

void chal_audio_hspath_set_bits_per_sample(CHAL_HANDLE handle, cUInt16 bits)
{
	cUInt32     reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	reg_val = BRCM_READ_REG(base, AUDIOH_STEREO_FIFO_CTRL);

	if (bits >= 24)
		reg_val |= AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_24BIT_MASK;
	else
		reg_val &= ~AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_24BIT_MASK;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_STEREO_FIFO_CTRL, reg_val);

	return;
}

/*
 * ======================================================================
 *
 *	Function Name: void chal_audio_hspath_set_mono_stereo
 *			(CHAL_HANDLE handle, _Bool mode)
 *
 *	Description:  Set the playback mode on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *                  mode   - true : mono, false : stereo
 *	Return:       None.
 *
 * =========================================================================
 */

void chal_audio_hspath_set_mono_stereo(CHAL_HANDLE handle, _Bool mode)
{
	cUInt32     reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	reg_val = BRCM_READ_REG(base, AUDIOH_STEREO_FIFO_CTRL);

	if (mode == CHAL_AUDIO_ENABLE)
		reg_val |= AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_MONO_MASK;
	else /* stereo */
		reg_val &= ~AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_MONO_MASK;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_STEREO_FIFO_CTRL, reg_val);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_set_pack
 *	(CHAL_HANDLE handle, _Bool pack)
 *
 *	Description:  Set the sample pack/upack mode on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *                pack   - true : pack, false : unpack.
 *	Return:       None.
 *
 * ============================================================================
 */

void chal_audio_hspath_set_pack(CHAL_HANDLE handle, _Bool pack)
{
	cUInt32     reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	reg_val = BRCM_READ_REG(base, AUDIOH_STEREO_FIFO_CTRL);

	if (pack == CHAL_AUDIO_ENABLE)
		reg_val |= AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_PACK_MASK;
	else
		reg_val &= ~AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_PACK_MASK;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_STEREO_FIFO_CTRL, reg_val);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_clr_fifo(CHAL_HANDLE handle)
 *
 *	Description:  Clhs the FIFO on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *
 *	Return:       None.
 *
 * ============================================================================
 */

void chal_audio_hspath_clr_fifo(CHAL_HANDLE handle)
{
	cUInt32     reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	reg_val = BRCM_READ_REG(base, AUDIOH_STEREO_FIFO_CTRL);
	reg_val |= AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_CLEAR_MASK;
	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_STEREO_FIFO_CTRL, reg_val);

	reg_val &= ~AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_CLEAR_MASK;
	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_STEREO_FIFO_CTRL, reg_val);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_set_fifo_thres
 *	(CHAL_HANDLE handle, cUInt16 thres, cUInt16 thres_2)
 *
 *	Description:  Set the threshold for FIFO on headset path
 *
 *	Parameters:   handle  - audio chal handle
 *                  thres   - threshold 1
 *                  thres_2 - threshold 2
 *	Return:       None.
 *
 * ============================================================================
 */

void chal_audio_hspath_set_fifo_thres(CHAL_HANDLE handle, cUInt16 thres,
	cUInt16 thres_2)
{
	cUInt32     reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	reg_val = BRCM_READ_REG(base, AUDIOH_STEREO_FIFO_CTRL);
	reg_val &= ~(AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_THRES_MASK);
	reg_val |= thres;
	reg_val &= ~(AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_THRES2_MASK);
	reg_val |= thres_2 << AUDIOH_STEREO_FIFO_CTRL_STEREO_FIFO_THRES2_SHIFT;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_STEREO_FIFO_CTRL, reg_val);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: cUInt32 chal_audio_hspath_read_fifo_status
 *	(CHAL_HANDLE handle)
 *
 *	Description:  Read the FIFO status (including interrupt) on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *
 *	Return:       the status.
 *
 * ============================================================================
 */

cUInt32 chal_audio_hspath_read_fifo_status(CHAL_HANDLE handle)
{
	cUInt32     reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	/* Read the FIFO status*/
	reg_val = BRCM_READ_REG(base, AUDIOH_STEREO_FIFO_STATUS);

	/* Read the FIFO interrupt satatus*/
	reg_val |= chal_audio_hspath_read_int_status(handle);

	return    reg_val;
}

/*
 * ============================================================================
 *
 *	Function Name: cUInt32 chal_audio_hspath_read_int_status
 *	(CHAL_HANDLE handle)
 *
 *	Description:  Read the interrupt status on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *
 *	Return:       the interrupt status.
 *
 * ============================================================================
 */

cUInt32 chal_audio_hspath_read_int_status(CHAL_HANDLE handle)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32     status = 0;
	cUInt32     reg_val = 0;

	/* Read the Audio FIFO interrupt status of all paths*/
	reg_val = BRCM_READ_REG(base, AUDIOH_AUDIO_INT_STATUS);

	/* Check for Stereo FIFO error interrupt*/
	if (reg_val & AUDIOH_AUDIO_INT_STATUS_STEREO_FIFO_ERR_MASK)
		status |= CHAL_AUDIO_FIFO_STATUS_ERR_INT;

	/* Check for Stereo FIFO threshold interrupt*/
	if (reg_val & AUDIOH_AUDIO_INT_STATUS_STEREO_INT_MASK)
		status |= CHAL_AUDIO_FIFO_STATUS_THR_INT;

	return    status;
}

/*
 * ============================================================================
 *
 *	Function Name: chal_audio_hspath_write_fifo(CHAL_HANDLE handle,
 *				cUInt32 *src, cUInt32 length)
 *
 *	Description:  Write the data to FIFO on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *				*src   - the data buffer address
 *				length - the data buffer length
 *				ign_ovf - keep writing to the fifo even
 *						on near overflow
 *
 *	Return:       None.
 *
 * ============================================================================
 */

cUInt32 chal_audio_hspath_write_fifo(CHAL_HANDLE handle, cUInt32 *src,
	cUInt32 length, _Bool ign_ovf)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32 n;

	if (ign_ovf == CHAL_AUDIO_STOP_ON_FIFO_OVERFLOW) {
		for (n = 0; n < length; n++) {
			/* Check if the FIFO is getting underflow or not,
			if ign_udf flag is not set */
			if (chal_audio_hspath_read_fifo_status(handle)
				& CHAL_AUDIO_FIFO_STATUS_NEAR_FULL) {
				break;
			}
			BRCM_WRITE_REG(base, AUDIOH_STEREO_FIFO_DATA,
				((cUInt32) *src++));
		}
	} else {
		for (n = 0; n < length; n++)
			BRCM_WRITE_REG(base, AUDIOH_STEREO_FIFO_DATA,
				((cUInt32) *src++));
	}
	return n;
}

/*
 * ============================================================================
 *
 *	Function Name: chal_audio_hspath_eanc_in
 *		(CHAL_HANDLE handle, cUInt16 enable)
 *
 *	Description:  control the eanc into headset path
 *
 *	Parameters:   handle - audio chal handle.
 *                  mix_in - true : with in, false : without
 *
 *	Return:       None.
 *
 * ============================================================================
 */

/* Check ASIC if it could be controlled for each output channel */
void chal_audio_hspath_eanc_in(CHAL_HANDLE handle, cUInt16 enable)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32     eanc_ctrl;

	/* Set the required settings*/
	eanc_ctrl = BRCM_READ_REG(base, AUDIOH_EANC_CTL);
	eanc_ctrl &= (~AUDIOH_EANC_CTL_STEREO_EANC_ENABLE_MASK);
	if (enable == CHAL_AUDIO_ENABLE)
		eanc_ctrl |= AUDIOH_EANC_CTL_STEREO_EANC_ENABLE_MASK;

	BRCM_WRITE_REG(base, AUDIOH_EANC_CTL, eanc_ctrl);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: chal_audio_hspath_sidetone_in
 *	(CHAL_HANDLE handle, cUInt16 enable)
 *
 *	Description:  control the sidetone into heaset path
 *
 *	Parameters:	handle - audio chal handle.
 *				enable - true : with in, false : without
 *
 *	Return:       None.
 *
 * ============================================================================
 */

/* Confirmed with ASIC that this feature will be added*/

void chal_audio_hspath_sidetone_in(CHAL_HANDLE handle, cUInt16 enable)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32     reg_val = 0;

	reg_val = BRCM_READ_REG(base, AUDIOH_DAC_CTL);
	reg_val &= (~AUDIOH_DAC_CTL_STEREO_SIDETONE_EN_MASK);
	if (enable == CHAL_AUDIO_ENABLE)
		reg_val |= AUDIOH_DAC_CTL_STEREO_SIDETONE_EN_MASK;
	BRCM_WRITE_REG(base, AUDIOH_DAC_CTL, reg_val);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: chal_audio_hspath_sdm_enable_dither(CHAL_HANDLE handle,
 *		cUInt16 enable)
 *
 *	Description:  enable dither to sigma delta module on headset path
 *	set/clear on SDM_DITHER_CTL : Bit0 /Bit4
 *
 *	Parameters:   handle - audio chal handle.
 *		enable - gain_enable : bit 1 for right channel enable/disbale
 *		enable - gain_enable : bit 0 for left channel enable/disbale
 *
 *  Return: None.
 *
 * ============================================================================
 */

void chal_audio_hspath_sdm_enable_dither(CHAL_HANDLE handle,
	cUInt16 enable)
{
	cUInt32 reg_val;
	cUInt32 hs_dither_enable = 0x00000000;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	if (enable&CHAL_AUDIO_CHANNEL_LEFT)
		hs_dither_enable |=
			AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_EN_L_MASK;

	if (enable&CHAL_AUDIO_CHANNEL_RIGHT)
		hs_dither_enable |=
			AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_EN_R_MASK;

	reg_val = BRCM_READ_REG(base, AUDIOH_SDM_DITHER_CTL);

	reg_val &= ~AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_EN_L_MASK;
	reg_val &= ~AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_EN_R_MASK;

	reg_val |= hs_dither_enable;

	BRCM_WRITE_REG(base, AUDIOH_SDM_DITHER_CTL, reg_val);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name:
 *	chal_audio_hspath_sdm_is_dither_enabled(CHAL_HANDLE handle)
 *
 *	Description:  Check if dither to sigma delta module on headset path
 *	enabled
 *
 *	Parameters:   handle - audio chal handle.
 *
 *	 Return:
 *		enable : bit 1 for right channel enable/disable
 *		enable : bit 0 for left channel enable/disable
 *
 * ============================================================================
 */
cUInt16 chal_audio_hspath_sdm_is_dither_enabled(CHAL_HANDLE handle)
{
	cUInt32 reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt16 ret = 0;

	reg_val = BRCM_READ_REG(base, AUDIOH_SDM_DITHER_CTL);

	if (reg_val & AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_EN_L_MASK)
		ret |= AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_EN_L_MASK;

	if (reg_val & AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_EN_R_MASK)
		ret |= AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_EN_R_MASK;

	return ret;
}

/*
 * ============================================================================
 *
 *	Function Name: void hal_audio_hspath_sdm_enable_dither_gain
 *		(CHAL_HANDLE handle, cUInt16 gain_enable)
 *
 *	Description:  set the dither strength to sigma delta module on
 *		headset path set/clear on SDM_DITHER_CTL : Bit1 /Bit5
 *
 *	Parameters:   handle - audio chal handle.
 *		gain_enable - gain_enable : bit 1 for right ch noise gain cntrl
 *		gain_enable - gain_enable : bit 0 for left ch noise gain cntrl
 *
 *	Return:       None.
 *
 * ============================================================================
 */

void chal_audio_hspath_sdm_enable_dither_gain(CHAL_HANDLE handle,
	cUInt16 gain_enable)
{
	cUInt32 reg_val;
	cUInt32 hs_dither_gain = 0x00000000;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	if (gain_enable&CHAL_AUDIO_CHANNEL_LEFT)
		hs_dither_gain |=
			AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_NOISE_GAIN_L_MASK;

	if (gain_enable&CHAL_AUDIO_CHANNEL_RIGHT)
		hs_dither_gain |=
			AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_NOISE_GAIN_R_MASK;

	reg_val = BRCM_READ_REG(base, AUDIOH_SDM_DITHER_CTL);

	reg_val &= ~AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_NOISE_GAIN_L_MASK;
	reg_val &= ~AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_NOISE_GAIN_R_MASK;

	reg_val |= hs_dither_gain;

	BRCM_WRITE_REG(base, AUDIOH_SDM_DITHER_CTL, reg_val);

	return;
}


/*
 * ===========================================================================
 *
 *	Function Name: void chal_audio_hspath_sdm_set_dither_strength
 *		(CHAL_HANDLE handle, cUInt16 dither_streng_L,
 *		cUInt16 dither_streng_R);
 *
 *	Description: set the dither strength to sigma delta module
 *	on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *		dither_streng_L - Stereo Left Path Dither Stength
 *		dither_streng_R - Stereo Right Path Dither Stength
 *
 *	Return:       None.
 *
 * ============================================================================
 */

void chal_audio_hspath_sdm_set_dither_strength(CHAL_HANDLE handle,
	cUInt16 dither_streng_L, cUInt16 dither_streng_R)
{
	cUInt32 reg_val;
	cUInt32 hs_dither_strngth = 0x00000000;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	hs_dither_strngth |=
		(dither_streng_L & (CHAL_AUDIO_DITHER_STRENGTH_MASK))
		<< AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_STRENGTH_L_SHIFT;
	hs_dither_strngth |=
		(dither_streng_R & (CHAL_AUDIO_DITHER_STRENGTH_MASK))
		<< AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_STRENGTH_R_SHIFT;

	reg_val = BRCM_READ_REG(base, AUDIOH_SDM_DITHER_CTL);

	reg_val &= ~(AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_STRENGTH_L_MASK);
	reg_val &= ~(AUDIOH_SDM_DITHER_CTL_STEREO_DITHER_STRENGTH_R_MASK);

	reg_val |= hs_dither_strngth;
	BRCM_WRITE_REG(base, AUDIOH_SDM_DITHER_CTL, reg_val);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_sdm_set_coef(CHAL_HANDLE handle,
 *		cUInt32 hs_coef_L, cUInt32 hs_coef_R)
 *
 *	Description:  Write the coeff to sigma delta module on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *		hs_coef_L - STEREO Left channe Coefficient
 *		[31:16] :   STEREO Left channe Coefficient 1
 *		[15:00] :   STEREO Left channe Coefficient 0
 *			hs_coef_R - STEREO Right channel Coefficient
 *		[31:16] :   STEREO Right channe Coefficient 1
 *		[15:00] :   STEREO Right channe Coefficient 0
 *
 *	Return:       None.
 *
 * ============================================================================
 */
void chal_audio_hspath_sdm_set_coef(CHAL_HANDLE handle, cUInt32 hs_coef_L,
	cUInt32 hs_coef_R)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	BRCM_WRITE_REG(base, AUDIOH_STEREO_SDM_COEF_L, hs_coef_L);
	BRCM_WRITE_REG(base, AUDIOH_STEREO_SDM_COEF_R, hs_coef_R);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_sdm_set_dither_poly
 *		(CHAL_HANDLE handle, cUInt32 hs_dither_poly_L,
 *		cUInt32 hs_dither_poly_R)
 *
 *	Description: Write the dither poly to sigma delta module on
 *	headset path
 *
 *	Parameters:   handle - audio chal handle.
 *		hs_dither_poly_L - left channel dither poly value
 *		hs_dither_poly_R - right channel dither poly value
 *
 *	Return:       None.
 *
 * ============================================================================
 */
void chal_audio_hspath_sdm_set_dither_poly(CHAL_HANDLE handle,
	cUInt32 hs_dither_poly_L, cUInt32 hs_dither_poly_R)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32 reg_value;

	reg_value = BRCM_READ_REG(base, AUDIOH_STEREO_DITHER_POLY_L);
	/* clear the B31 to prepare to se dither poly value */
	reg_value &= ~AUDIOH_STEREO_DITHER_POLY_L_STEREO_DITHER_SET_L_MASK;
	BRCM_WRITE_REG(base, AUDIOH_STEREO_DITHER_POLY_L, reg_value);

	reg_value = hs_dither_poly_L |
		AUDIOH_STEREO_DITHER_POLY_L_STEREO_DITHER_SET_L_MASK;
	BRCM_WRITE_REG(base, AUDIOH_STEREO_DITHER_POLY_L, reg_value);

	reg_value = BRCM_READ_REG(base, AUDIOH_STEREO_DITHER_POLY_R);
	/* clear the B31 to prepare to se dither poly value */
	reg_value &= ~AUDIOH_STEREO_DITHER_POLY_R_STEREO_DITHER_SET_R_MASK;
	BRCM_WRITE_REG(base, AUDIOH_STEREO_DITHER_POLY_R, reg_value);

	reg_value = hs_dither_poly_R |
		AUDIOH_STEREO_DITHER_POLY_R_STEREO_DITHER_SET_R_MASK;
	BRCM_WRITE_REG(base, AUDIOH_STEREO_DITHER_POLY_R, reg_value);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: chal_audio_hspath_sdm_set_dither_seed(CHAL_HANDLE handle,
 *		cUInt32 hs_dither_seed_L, cUInt32 hs_dither_seed_R)
 *
 *	Description: Write the dither seed to sigma delta module on headset path
 *
 *	Parameters: handle - audio chal handle.
 *			hs_dither_seed_L - left channel dither seed value
 *			hs_dither_seed_R - right channel dither seed value
 *
 *	Return:       None.
 *
 * ============================================================================
 */
void chal_audio_hspath_sdm_set_dither_seed(CHAL_HANDLE handle,
	cUInt32 hs_dither_seed_L, cUInt32 hs_dither_seed_R)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	BRCM_WRITE_REG(base, AUDIOH_STEREO_DITHER_SEED_L,
		(hs_dither_seed_L &
		 AUDIOH_STEREO_DITHER_SEED_L_STEREO_DITHER_SEED_L_MASK));
	BRCM_WRITE_REG(base, AUDIOH_STEREO_DITHER_SEED_R,
		(hs_dither_seed_R &
		 AUDIOH_STEREO_DITHER_SEED_R_STEREO_DITHER_SEED_R_MASK));

	return;
}


/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_hs_supply_ctrl
 *	(CHAL_HANDLE handle, cUInt32 vout_supply_ctrl)
 *
 *	Description:  Control the headset supply on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *		hs_ds_pol - Driver Supply Indicator Polarity,
 *					FALSE: Active High, TRUE: Active Low
 *		hs_ds_delay - Signal Delay in step of 8 samples, 3-bit integer,
 *					000--no delay, 111--56 symbols delay
 *		hs_ds_lag - Lag control, 8-bit, in unit of audio samples (48KHz)
 *
 *  Return:       None.
 *
 * ============================================================================
 */
void chal_audio_hspath_hs_supply_ctrl(CHAL_HANDLE handle,
	_Bool hs_ds_pol, cUInt8 hs_ds_delay, cUInt8 hs_ds_lag)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32 hs_supply_ctl = 0x00000000;

	if (hs_ds_pol == TRUE)
		hs_supply_ctl =
		AUDIOH_HS_DRIVER_SUPPLY_CTRL_HS_DS_POLARITY_MASK;

	hs_supply_ctl |=
		((hs_ds_delay << AUDIOH_HS_DRIVER_SUPPLY_CTRL_HS_DS_DELAY_SHIFT)
		 & AUDIOH_HS_DRIVER_SUPPLY_CTRL_HS_DS_LAG_MASK);
	hs_supply_ctl |=
		((hs_ds_lag << AUDIOH_HS_DRIVER_SUPPLY_CTRL_HS_DS_LAG_SHIFT)
		 & AUDIOH_HS_DRIVER_SUPPLY_CTRL_HS_DS_LAG_MASK);

	BRCM_WRITE_REG(base, AUDIOH_HS_DRIVER_SUPPLY_CTRL, hs_supply_ctl);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_hs_supply_thres
 *	(CHAL_HANDLE handle, cUInt32 hs_supply_thres)
 *
 *	Description:  set threshold for headset supply on headset path
 *
 *	Parameters:   handle - audio chal handle.
 *		hs_supply_theshold  - headset supply control threshold value
 *
 *	Return:       None.
 *
 * ============================================================================
 */
void chal_audio_hspath_hs_supply_thres(CHAL_HANDLE handle,
	cUInt32 hs_supply_thres)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	BRCM_WRITE_REG(base, AUDIOH_HS_DRIVER_SUPPLY_TH, hs_supply_thres);

	return;
}
/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_get_dma_port_addr
 *		(CHAL_HANDLE handle, cUInt32 *dma_addr)
 *
 *	Description:  Get the DMA port physical address on HS path
 *
 *	Parameters:   handle - audio chal handle.
 *				dma_addr - point to address to return
 *				DMA physical address
 *	Return:       None.
 *
 * ============================================================================
 */

void chal_audio_hspath_get_dma_port_addr(CHAL_HANDLE handle, cUInt32 *dma_addr)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	*dma_addr = (cUInt32)(base) + AUDIOH_STEREO_FIFO_DATA_OFFSET;

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: cUInt32 chal_audio_hspath_get_dma_port_addr_offset()
 *
 *	Description:  Get the DMA address offset on HS path
 *
 *	Return:       Offset for the DMA address.
 *
 * ============================================================================
 */
cUInt32 chal_audio_hspath_get_dma_port_addr_offset(void)
{
	return AUDIOH_STEREO_FIFO_DATA_OFFSET;
}

/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_int_clear(CHAL_HANDLE handle,
 *	_Bool thr_int, _Bool err_int )
 *
 *	Description:  Clear interrupt on Headset path
 *
 *	Parameters:   handle      : the Headset pathhandle.
 *		thr_int      : Clear FIFO threshold interrupt
 *		err_int      : Clear FIFO Error interrupt
 *
 *	Return:       None.
 *
 * ============================================================================
 */
void chal_audio_hspath_int_clear(CHAL_HANDLE handle,
	_Bool thr_int, _Bool err_int)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32 reg_val = 0;

	if (thr_int == TRUE)
		reg_val |= AUDIOH_AUDIO_INT_STATUS_STEREO_INT_MASK;

	if (err_int == TRUE)
		reg_val |= AUDIOH_AUDIO_INT_STATUS_STEREO_FIFO_ERR_MASK;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_AUDIO_INT_STATUS, reg_val);

	return;
}

/*
 * ============================================================================
 *
 * Function Name: void chal_audio_hspath_set_dac_pwr(CHAL_HANDLE handle,
 *	cUInt16 enable_chan)
 *
 *	Description:  Set DAC power for the HS DAC
 *
 *	Parameters:   handle - audio chal handle.
 *					enable_chan - Specifies which DAC
 *					channel power need to be enabled
 *					or disabled
 *  Return:       None.
 *
 * ============================================================================
 */
void chal_audio_hspath_set_dac_pwr(CHAL_HANDLE handle, cUInt16 enable_chan)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32 reg_value;

	/* clearing i_hs_dac_ctl[9] bit to shorten settle time
	for DAC/Headset	AUDIOH_HS_DAC */
	reg_value = BRCM_READ_REG(base, AUDIOH_HS_DAC_CTRL);
	reg_value &= ~(1<<9);
	BRCM_WRITE_REG(base, AUDIOH_HS_DAC_CTRL, reg_value);

	reg_value = BRCM_READ_REG(base, AUDIOH_HS_PWR);

	/* According to ASIC team, we need to power on
	AUDIOH_HS_PWR_AUDIOTX_HS_REF_PD	and
	AUDIOH_EP_PWR_AUDIOTX_EP_IHF_REF_PD	to get EP
	working with loud volume. */

	if (enable_chan & (CHAL_AUDIO_CHANNEL_LEFT|CHAL_AUDIO_CHANNEL_RIGHT))
		isHS = TRUE;
	else
		isHS = FALSE;

	if (enable_chan&CHAL_AUDIO_CHANNEL_LEFT) {
		/* Clear power down */
		reg_value &= ~AUDIOH_HS_PWR_AUDIOTX_HS_DACL_PD_MASK;
	} else {
		/* Set power down */
		if (!chal_audio_isEPpath_active())
			reg_value |= AUDIOH_HS_PWR_AUDIOTX_HS_DACL_PD_MASK;
	}

	if (enable_chan&CHAL_AUDIO_CHANNEL_RIGHT) {
		/* Clear power down */
		reg_value &= ~AUDIOH_HS_PWR_AUDIOTX_HS_DACR_PD_MASK;
	} else {
		/* Set power down */
		if (!chal_audio_isEPpath_active())
			reg_value |= AUDIOH_HS_PWR_AUDIOTX_HS_DACR_PD_MASK;
	}

	if (enable_chan)
		reg_value &= ~AUDIOH_HS_PWR_AUDIOTX_HS_REF_PD_MASK;
	else {
		if (!chal_audio_isEPpath_active()) /* if EP not active */
			reg_value |= AUDIOH_HS_PWR_AUDIOTX_HS_REF_PD_MASK;
	}

	BRCM_WRITE_REG(base, AUDIOH_HS_PWR, reg_value);

	return;
}

/*
 * ============================================================================
 *
 *	Function Name: void chal_audio_hspath_set_filter(CHAL_HANDLE handle,
 *		cUInt16 filter)
 *
 *	Description:  set Filter type for the heaset  path
 *
 *	Parameters:   handle    - audio chal handle.
 *				filter    -  0: Linear Phase, 1 Minimum Phase
 *	Return:       None.
 *
 */
/*============================================================================*/
void chal_audio_hspath_set_filter(CHAL_HANDLE handle, cUInt16 filter)
{
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;
	cUInt32 reg_val;

	reg_val = BRCM_READ_REG(base, AUDIOH_MIN_PHASE);
	reg_val &= ~(AUDIOH_MIN_PHASE_HS_MIN_PHASE_MASK);

	if (filter & CHAL_AUDIO_MINIMUM_PHASE_FILTER)
		reg_val |= AUDIOH_MIN_PHASE_HS_MIN_PHASE_MASK;

	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_MIN_PHASE, reg_val);

	return;
}

	/* According to ASIC team, we need to power on
	AUDIOH_HS_PWR_AUDIOTX_HS_REF_PD	and
	AUDIOH_EP_PWR_AUDIOTX_EP_IHF_REF_PD to get EP
	working with loud volume. EP path will call this
	function to check the HS path status and based on
	the status, HS_PWR register will be configured
	accordingly */

Boolean chal_audio_isHSpath_active(void)
{
	return isHS;
}

/*
 * ============================================================================
 *
 *	Function Name: chal_audio_hspath_sdm_mute(CHAL_HANDLE handle,
 *		cUInt16 enable)
 *
 *	Description:  enable dither to sigma delta module on headset path
 *	set/clear on SDM_DITHER_CTL : Bit0 /Bit4
 *
 *	Parameters:   handle - audio chal handle.
 *		mute - 1 to enable mute, 0 to disable
 *
 *		lr_ch: bit 1 for right channel operation
 *		lr_ch: bit 0 for left channel operation
 *
 *  Return: None.
 *
 * ============================================================================
 */
void chal_audio_hspath_sdm_mute(CHAL_HANDLE handle, _Bool mute, cUInt16 lr_ch)
{
	cUInt32     reg_val;
	cUInt32 base = ((ChalAudioCtrlBlk_t *)handle)->audioh_base;

	reg_val = BRCM_READ_REG(base, AUDIOH_DAC_CTL);

	if (mute == CHAL_AUDIO_ENABLE) {
		if (lr_ch & CHAL_AUDIO_CHANNEL_LEFT)
			reg_val |= AUDIOH_DAC_CTL_STEREO_SDM_MUTE_L_MASK;
		if (lr_ch & CHAL_AUDIO_CHANNEL_RIGHT)
			reg_val |= AUDIOH_DAC_CTL_STEREO_SDM_MUTE_R_MASK;
	} else {
		if (lr_ch & CHAL_AUDIO_CHANNEL_LEFT)
			reg_val &= ~AUDIOH_DAC_CTL_STEREO_SDM_MUTE_L_MASK;
		if (lr_ch & CHAL_AUDIO_CHANNEL_RIGHT)
			reg_val &= ~AUDIOH_DAC_CTL_STEREO_SDM_MUTE_R_MASK;
	}
	/* Set the required setting */
	BRCM_WRITE_REG(base,  AUDIOH_DAC_CTL, reg_val);

	return;
}

