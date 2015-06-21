#include <stdio.h>
#include <string.h>
#include "FrameExtractor.h"
#include "H264Frames.h"


#define NAL_UNIT_TYPE_TYPE(n)	((0x001F) & (n))


typedef struct tagH264_SLICE_INFO
{
	unsigned int   first_mb;
	unsigned short slice_type;
	unsigned short pic_parameter_set_id;
	unsigned int   frame_num;
} H264_SLICE_INFO;


// SPS µ¥ÀÌÅÍ¿¡ µé¾î°¡ ÀÖ´Â ÆÄ¶ó¹ÌÅÍ
static unsigned int   log2_max_frame_num_minus4 = 0;
static unsigned int   frame_mbs_only_flag = 0;
// PPS ID ¹è¿­
static unsigned short pic_parameter_set_id_arr[32] = {0xFFFF, };
static void add_pps_id(unsigned short pic_parameter_set_id)
{
	int  i;

	for (i=0; i<32; i++) {
		if (pic_parameter_set_id_arr[i] == pic_parameter_set_id)
			break;
		if (pic_parameter_set_id_arr[i] == 0xFFFF) {
			pic_parameter_set_id_arr[i] = pic_parameter_set_id;
			break;
		}
	}
}
static int find_pps_id(unsigned short pic_parameter_set_id)
{
	int  i;

	for (i=0; i<32; i++) {
		if (pic_parameter_set_id_arr[i] == pic_parameter_set_id)
			return 1;
	}
	return 0;
}


static int NextIFrameH264(FRAMEX_CTX  *pFrameExCtx, void *fp, unsigned char buf[], int buf_size);


static unsigned int read_bits(unsigned char bytes[], int num_read, int *bit_offset)
{
	unsigned int   bits;
	unsigned int   utmp;

	int            i;
	int            bit_shift;

	int   byte_offset, bit_offset_in_byte;
	int   num_bytes_copy;


	if (num_read > 24)	// Max 24 bits±îÁö¸¸ Áö¿øµÈ´Ù.
		return 0xFFFFFFFF;
	if (num_read == 0)
		return 0;


	// byte_offset°ú
	// ±× byte ³»¿¡¼­ bit_offsetÀ» ±¸ÇÑ´Ù.
	byte_offset = (*bit_offset) >> 3;	// byte_offset = (*bit_offset) / 8
	bit_offset_in_byte = (*bit_offset) - (byte_offset << 3);


	num_bytes_copy = ((*bit_offset + num_read) >> 3) - (*bit_offset >> 3) + 1;
	bits = 0;
	for (i=0; i<num_bytes_copy; i++) {
		utmp = bytes[byte_offset + i];
		bits = (bits << 8) | (utmp);
	}

	bit_shift = (num_bytes_copy << 3) - (bit_offset_in_byte + num_read);
	bits >>= bit_shift;
	bits &= (0xFFFFFFFF >> (32 - num_read));

	*bit_offset += num_read;

	return bits;
}

// unsigned integer Exp-Golomb-codec element with the left bit first.
// The parsing process for this descriptor is specified in subclause 9.1 (ITU-T Rec. H.264).
static unsigned int ue_v(unsigned char bytes[], int *bit_offset)
{
	unsigned int   b;
	int            leadingZeroBits = -1;
	unsigned int   codeNum;


	for (b=0; !b; leadingZeroBits++) {
		b = read_bits(bytes, 1, bit_offset);
	}

	// codeNum = 2^(leadingZeroBits) - 1 + read_bits(leadingZeroBits)
	codeNum = (1 << leadingZeroBits) - 1 + read_bits(bytes, leadingZeroBits, bit_offset);


	return codeNum;
}

// signed integer Exp-Golomb-codec element with the left bit first.
// The parsing process for this descriptor is specified in subclause 9.1 (ITU-T Rec. H.264).
static signed int se_v(unsigned char bytes[], int *bit_offset)
{
	signed int   b;
	int          leadingZeroBits = -1;
	signed int   codeNum;


	for (b=0; !b; leadingZeroBits++) {
		b = read_bits(bytes, 1, bit_offset);
	}

	// codeNum = 2^(leadingZeroBits) - 1 + read_bits(leadingZeroBits)
	codeNum = (1 << leadingZeroBits) - 1 + read_bits(bytes, leadingZeroBits, bit_offset);


	return codeNum;
}

static unsigned int u_n(unsigned char bytes[], int n_bits, int *bit_offset)
{
	return read_bits(bytes, n_bits, bit_offset);
}


static void get_h264_slice_info(unsigned char slice_header[], H264_SLICE_INFO *h264_slice_info)
{
	unsigned int   utmp = 0;
	int            bit_offset;

	bit_offset = 0;

	h264_slice_info->first_mb              = ue_v(slice_header, &bit_offset);
	h264_slice_info->slice_type            = ue_v(slice_header, &bit_offset);
	h264_slice_info->pic_parameter_set_id  = ue_v(slice_header, &bit_offset);
	h264_slice_info->frame_num             = u_n(slice_header, log2_max_frame_num_minus4 + 4, &bit_offset);

#if 1
	if (!frame_mbs_only_flag) {
		utmp = u_n(slice_header, 1, &bit_offset);		// field_pic_flag
		if (utmp)
			utmp = u_n(slice_header, 1, &bit_offset);	// bottom_field_flag
	}

#endif
}




int ExtractConfigStreamH264(FRAMEX_CTX  *pFrameExCtx,
				void *fp,
				unsigned char buf[],
				int buf_size,
				H264_CONFIG_DATA *conf_data)
{
	int                i, j;
	int                ret;
	unsigned char      frame_type[10];
	unsigned char      nal_type;
	int                nStreamSize, nFrameSize;
	int                includeIframe=1;

	int                bit_offset;
	unsigned char      profile_idc, utmp, utmp2, *ptmp;
	int                itmp;


	unsigned char      PicHeightInMapUnits;
	unsigned char      FrameHeightInMbs;
	unsigned char      PicHeightInMbs;


	// PPS IDÀÇ ¹è¿­À» ÃÊ±âÈ­ ÇÑ´Ù.
	memset(pic_parameter_set_id_arr, 0xFF, sizeof(pic_parameter_set_id_arr));

	// SPS/PPS/SEI ºÎºÐ ÃßÃâ
	for (i=0, nStreamSize=0; i<100; i++) {

		ret = FrameExtractorPeek(pFrameExCtx, fp, frame_type, sizeof(frame_type), (int *)&nFrameSize);
		nal_type = NAL_UNIT_TYPE_TYPE(frame_type[4]);

		if ((nal_type != 6) && (nal_type != 7) && (nal_type != 8) && (nal_type != 9)) {
			break;
		}

		ret = FrameExtractorNext(pFrameExCtx, fp, buf + nStreamSize, buf_size - nStreamSize, (int *)&nFrameSize);
		if (ret != FRAMEX_OK)
			break;

		// SPS case
		// (Extracting 'log2_max_frame_num_minus4' value for SPS NAL.)
		if (nal_type == 7) {
			ptmp = buf + nStreamSize + 5;

			bit_offset = 0;
			profile_idc = u_n(ptmp, 8, &bit_offset);		// profile_idc
			if ((profile_idc == 100) || (profile_idc == 110) ||
				(profile_idc == 122) || (profile_idc == 144)) {
				return -1;
			}

			utmp = u_n(ptmp, 1, &bit_offset);		// constraint_set0_flag
			utmp = u_n(ptmp, 1, &bit_offset);		// constraint_set1_flag
			utmp = u_n(ptmp, 1, &bit_offset);		// constraint_set2_flag
			utmp = u_n(ptmp, 1, &bit_offset);		// constraint_set3_flag
			utmp = u_n(ptmp, 4, &bit_offset);		// reserved_zero_4bits
			utmp = u_n(ptmp, 8, &bit_offset);		// level_idc
			utmp = ue_v(ptmp, &bit_offset);			// seq_parameter_set_id
			log2_max_frame_num_minus4
			    = ue_v(ptmp, &bit_offset);			// log2_max_frame_num_minus4

			if (conf_data != NULL) {
				utmp = ue_v(ptmp, &bit_offset);		// pic_order_cnt_type
				if (utmp == 0)
					utmp = ue_v(ptmp, &bit_offset);	// log2_max_pic_order_cnt_lsb_minus4
				else if (utmp == 1) {
					utmp = u_n(ptmp, 1, &bit_offset);	// delta_pic_order_always_zero_flag
					itmp = se_v(ptmp, &bit_offset);	// offset_for_non_ref_pic
					itmp = se_v(ptmp, &bit_offset);	// offset_for_non_ref_pic
					utmp = ue_v(ptmp, &bit_offset);	// num_ref_frames_in_pic_order_cnt_cycle
					for (j = 0; j < (int) utmp; j++)
						se_v(ptmp, &bit_offset);
				}
				utmp = ue_v(ptmp, &bit_offset);			// num_ref_frames
				utmp = u_n(ptmp, 1, &bit_offset);		// gaps_in_frame_num_value_allowed_flag

				// Picture Width
				utmp = ue_v(ptmp, &bit_offset);			// pic_width_in_mbs_minus1
				conf_data->width = ((unsigned int) (utmp + 1)) << 4;	// width = (pic_width_in_mbs_minus1 + 1) * 16

				// Picture Height
				utmp  = ue_v(ptmp, &bit_offset);		// pic_height_in_map_units_minus1
				utmp2 = u_n(ptmp, 1, &bit_offset);		// frame_mbs_only_flag
				frame_mbs_only_flag = utmp2;			// frame_mbs_only_flag

				PicHeightInMapUnits = utmp + 1;
				FrameHeightInMbs    = (2 - utmp2) * PicHeightInMapUnits;
				PicHeightInMbs      = FrameHeightInMbs;// / (1 + field_pic_flag);
				conf_data->height   = ((unsigned int) PicHeightInMbs) << 4;
			}
		}
		// PPS case
		// (Extracting 'log2_max_frame_num_minus4' value for SPS NAL.)
		else if (nal_type == 8) {
			ptmp = buf + nStreamSize + 5;

			bit_offset = 0;
			utmp = ue_v(ptmp, &bit_offset);			// pic_parameter_set_id
			add_pps_id(utmp);
		}


		nStreamSize += nFrameSize;
	}


	// includeIframe °¡ 1ÀÎ °æ¿ì´Â
	// ¸®ÅÏµÇ´Â config stream¿¡ I-frameÀ» Æ÷ÇÔ½ÃÅ³Áö °áÁ¤ÇÑ´Ù.
	if (includeIframe == 1) {
		nFrameSize = NextIFrameH264(pFrameExCtx, fp, buf + nStreamSize, buf_size - nStreamSize);

		nStreamSize += nFrameSize;
	}


	return nStreamSize;
}

static int NextIFrameH264(FRAMEX_CTX  *pFrameExCtx, void *fp, unsigned char buf[], int buf_size)
{
	int                i, j;
	int                ret;
	unsigned char      frame_type[12];
	unsigned char      nal_type = 255;
	int                nFrameSize, nFrameSize2;

	H264_SLICE_INFO    h264_slice_info, h264_slice_info_next;
	int                bit_offset;


	for (i=0; i<200; i++) {

		ret = FrameExtractorNext(pFrameExCtx, fp, buf, buf_size, (int *)&nFrameSize);
		if (ret != FRAMEX_OK)
			return 0;
		nal_type = NAL_UNIT_TYPE_TYPE(buf[4]);
		if (nal_type == 5) {
			break;
		}
		// PPS case
		// (Extracting 'log2_max_frame_num_minus4' value for SPS NAL.)
		else if (nal_type == 8) {
			bit_offset = 0;
			add_pps_id(ue_v(buf + 5, &bit_offset));		// pic_parameter_set_id
		}
	}

	// I frame was not found for 200 NALs.
	if (nal_type != 5)
		return 0;


	// NAL with type == 5 is now found.
	// Multi-slice checking
	get_h264_slice_info(buf + 5, &h264_slice_info);
	for (j=0; j<200; j++) {
		ret = FrameExtractorPeek(pFrameExCtx, fp, frame_type, sizeof(frame_type), (int *)&nFrameSize2);
		if (ret != FRAMEX_OK)
			break;

		nal_type = NAL_UNIT_TYPE_TYPE(frame_type[4]);
		if (nal_type != 5) {
			// Áß°£¿¡ PPS°¡ ³¢¿©ÀÖ´ÂÁö È®ÀÎÇÑ´Ù.
			if (nal_type == 8) {
				bit_offset = 0;
				// Áß°£¿¡ ³¤ PPSÀÇ pps_id°¡ »õ·Î¿î ³ðÀÌ¸é, multi-slice°¡ ¿Ï·áµÈ °ÍÀÌ´Ù.
				if (!find_pps_id(ue_v(frame_type + 5, &bit_offset)))
					break;
				// Áß°£¿¡ ³¤ PPSÀÇ pps_id°¡ ¹Ýº¹µÇ¾î ³ª¿Â ³ðÀÌ¸é,
				// ÀÌ PPS´Â ¹ö·Á¹ö¸°´Ù.
				else {
					FrameExtractorNext(pFrameExCtx, fp, buf + nFrameSize, buf_size - nFrameSize, (int *)&nFrameSize2);
					continue;
				}
			}
			else
				break;
		}


		// first_mb¿Í frame_numÀ» ºñ±³ÇÑ´Ù.
		get_h264_slice_info(frame_type + 5, &h264_slice_info_next);
		if (h264_slice_info.frame_num != h264_slice_info_next.frame_num)
			break;
//		if (h264_slice_info.first_mb == h264_slice_info_next.first_mb)
//			break;
		if (h264_slice_info.pic_parameter_set_id != h264_slice_info_next.pic_parameter_set_id)
			break;

		ret = FrameExtractorNext(pFrameExCtx, fp, buf + nFrameSize, buf_size - nFrameSize, (int *)&nFrameSize2);

		nFrameSize += nFrameSize2;
	}


	return nFrameSize;
}


int NextFrameH264(FRAMEX_CTX  *pFrameExCtx, void *fp, unsigned char buf[], int buf_size, unsigned int *coding_type)
{
	int                i;
	int                ret;
	unsigned char      nal_first_12bytes[12];
	unsigned char      nal_type, old_nal_type;
	int                nStreamSize, nFrameSize;

	H264_SLICE_INFO    h264_slice_info, h264_slice_info_next;
	int                bit_offset;


	nStreamSize=0;

	///////////////////////////////
	///  SPS/PPS/SEI ºÎºÐ ÃßÃâ  ///
	///////////////////////////////
	for (i=0; i<100; i++) {

		ret = FrameExtractorPeek(pFrameExCtx, fp, nal_first_12bytes, sizeof(nal_first_12bytes), (int *)&nFrameSize);
		nal_type = NAL_UNIT_TYPE_TYPE(nal_first_12bytes[4]);

		// SPS/PPS/SEI ºÎºÐÀÌ ¾Æ´Ï¶ó¸é ±×³É ³Ñ¾î°£´Ù.
		if ((nal_type != 6) && (nal_type != 7) && (nal_type != 8) && (nal_type != 9) && (nal_type != 12)) {
			break;
		}
		// PPS case
		// (Extracting 'log2_max_frame_num_minus4' value for SPS NAL.)
		if (nal_type == 8) {
			bit_offset = 0;
			add_pps_id(ue_v(nal_first_12bytes + 5, &bit_offset));		// pic_parameter_set_id
		}

		ret = FrameExtractorNext(pFrameExCtx, fp, buf + nStreamSize, buf_size - nStreamSize, (int *)&nFrameSize);
		if (ret != FRAMEX_OK)
			break;

		// Filler dataÀÎ °æ¿ì, ±×³É ¹ö·Á¹ö¸°´Ù.
		if (nal_type == 12)
			continue;

		nStreamSize += nFrameSize;
	}


	// SPS/PPS/SEI ´ÙÀ½¿¡ ¿À´Â nal_unitÀÇ typeÀÌ
	// IDR slice(5¹ø)ÀÌ³ª non-IDR slice(1¹ø)ÀÌ ¾Æ´Ñ °æ¿ì´Â
	// ÇöÀç Áö¿øÇÏÁö ¾Ê´Â´Ù.
	// Âü°í·Î 5¹ø,1¹øÀÇ °æ¿ì´Â slice_layer°¡ slice_layer_without_partitioning_rbsp ÀÌ´Ù.
	if ((nal_type != 1) && (nal_type != 5)) {
		return 0;
	}
	old_nal_type = nal_type;


	/////////////////////////////////
	///  I ¶Ç´Â P NAL UNIT ÃßÃâ   ///
	/////////////////////////////////
	nFrameSize=0;
	ret = FrameExtractorNext(pFrameExCtx, fp, buf + nStreamSize, buf_size - nStreamSize, (int *)&nFrameSize);
	if (ret != FRAMEX_OK) {
		return 0;
	}

	get_h264_slice_info(buf + nStreamSize + 5, &h264_slice_info);
	if (coding_type) {
		*coding_type = h264_slice_info.slice_type;
	}


	nStreamSize += nFrameSize;

	///////////////////////////////
	///  ´ÙÀ½ÀÇ NAL UNIT ÃßÃâ   ///
	///   (Multi-slice Æ÷ÇÔ)    ///
	///////////////////////////////
	for (i=0; i<200; i++) {

		// ´ÙÀ½ NAL UNITÀÇ nal_unit_type È®ÀÎ
		ret = FrameExtractorPeek(pFrameExCtx, fp, nal_first_12bytes, sizeof(nal_first_12bytes), (int *)&nFrameSize);
		if (ret != FRAMEX_OK)
			break;
		else if (nFrameSize < (sizeof(nal_first_12bytes)- 4))
			break;

		// ´ÙÀ½ NAL UNITÀÇ typeÀÌ Ã³À½ type°ú °°Áö ¾Ê´Ù¸é,
		// multi-sliceÀÏ °æ¿ì°¡ ¾Æ´Ï¹Ç·Î ¸®ÅÏÇÑ´Ù.
		nal_type = NAL_UNIT_TYPE_TYPE(nal_first_12bytes[4]);
		if (nal_type != old_nal_type) {
			// Áß°£¿¡ PPS°¡ ³¢¿©ÀÖ´ÂÁö È®ÀÎÇÑ´Ù.
			if (nal_type == 8) {
				bit_offset = 0;
				// Áß°£¿¡ ³¤ PPSÀÇ pps_id°¡ »õ·Î¿î ³ðÀÌ¸é, multi-slice°¡ ¿Ï·áµÈ °ÍÀÌ´Ù.
				if (!find_pps_id(ue_v(nal_first_12bytes + 5, &bit_offset)))
					break;
				// Áß°£¿¡ ³¤ PPSÀÇ pps_id°¡ ¹Ýº¹µÇ¾î ³ª¿Â ³ðÀÌ¸é,
				// ÀÌ PPS´Â ¹ö·Á¹ö¸°´Ù.
				else {
					FrameExtractorNext(pFrameExCtx, fp, buf + nStreamSize, buf_size - nStreamSize, (int *)&nFrameSize);
					continue;
				}
			}
			else
				break;
		}
		old_nal_type = nal_type;


		// first_mb¿Í frame_numÀ» ºñ±³ÇÑ´Ù.
		get_h264_slice_info(nal_first_12bytes + 5, &h264_slice_info_next);
		if (h264_slice_info.frame_num != h264_slice_info_next.frame_num)
			break;
//		if (h264_slice_info.first_mb == h264_slice_info_next.first_mb)
//			break;
		if (h264_slice_info.pic_parameter_set_id != h264_slice_info_next.pic_parameter_set_id)
			break;


		// ´ÙÀ½ NAL UNITÀÌ Multi-sliceÀÌ¹Ç·Î Ãß°¡·Î ÃßÃâÇÑ´Ù.
		ret = FrameExtractorNext(pFrameExCtx, fp, buf + nStreamSize, buf_size - nStreamSize, (int *)&nFrameSize);
		if (ret != FRAMEX_OK)
			return 0;

		nStreamSize += nFrameSize;
	}


	return nStreamSize;
}




