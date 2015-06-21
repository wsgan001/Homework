#include "FrameExtractor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FRAMEX_CTX *FrameExtractorInit(FRAMEX_IN_TYPE type, unsigned char delimiter[], int delim_leng, int delim_insert)
{
	FRAMEX_CTX *pCTX;

	// parameter checking
	if (delimiter == NULL || (delim_leng <= 0 || delim_leng > QUEUE_CAPACITY))
		return NULL;


	pCTX = (FRAMEX_CTX *) malloc(sizeof(FRAMEX_CTX));
	memset(pCTX, 0, sizeof(FRAMEX_CTX));

	pCTX->in_type    = type;
	pCTX->delim_ptr  = (unsigned char *) malloc(delim_leng);
	pCTX->delim_leng = delim_leng;
	memcpy(pCTX->delim_ptr, delimiter, delim_leng);

	pCTX->delim_insert = delim_insert;
	pCTX->cont_offset  = 0;

	return pCTX;
}


#define Q_INIT(queue, qp_s, qp_e, q_size, q_capacity)	\
{	\
	memset(queue, 0xFF, q_capacity);	\
	qp_s = qp_e = q_size = 0;			\
}


#define Q_PUSH(value, queue, qp_s, qp_e, q_size, q_capacity)	\
{	\
	queue[qp_s] = value;	\
	qp_s = (qp_s + 1) % q_capacity;	\
	q_size++;	\
}

#define Q_POP(value, queue, qp_s, qp_e, q_size, q_capacity)	\
{	\
	value = queue[qp_e];	\
	queue[qp_e] = 0xFE;		\
	qp_e = (qp_e + 1) % q_capacity;	\
	q_size--;	\
}

#define Q_PEEK(value, queue, qp_s, qp_e)	\
{	\
	value = queue[qp_e];	\
}

// ´ÙÀ½ÀÇ delimiter¸¦ Ã£´Â´Ù.
static int next_delimiter(FRAMEX_CTX *pCTX, FILE *fpin, unsigned char *outbuf, const int outbuf_size, int *n_fill)
{
	int r = 0;
	int l;
	int nbytes_to_write;
	int qp_s, qp_e, q_size;
	unsigned char queue[QUEUE_CAPACITY], b, c;

	int offset;

	offset = 0;
	nbytes_to_write = outbuf_size;
	Q_INIT(queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)

	// ¸®ÅÏÇÒ outbuf ¹öÆÛÀÇ ¸Ç ¾Õ¿¡ delimiter¸¦ Ã¤¿ö³Ö´Â´Ù.
	if (outbuf != NULL) {
		if (pCTX->cont_offset == 0) {
			if (nbytes_to_write <= pCTX->delim_leng) {
				return FRAMEX_ERR_BUFSIZE_TOO_SMALL;
			}

			if (pCTX->delim_insert) {
				memcpy(outbuf, pCTX->delim_ptr, pCTX->delim_leng);
				outbuf += pCTX->delim_leng;
				nbytes_to_write -= pCTX->delim_leng;	// °¡´ÉÇÑ Å©±â¸¦ delimiter ±æÀÌ¸¸Å­ »©¾ßÇÔ
				*n_fill = pCTX->delim_leng;
			}
			else
				*n_fill = 0;
		}
		else {
			// continue·Î µé¾î¿Â °æ¿ì´Â (pCTX->cont_offset != 0)
			offset = pCTX->cont_offset;
			outbuf += (pCTX->cont_offset - pCTX->delim_leng);

			for (l=0; l<pCTX->delim_leng; l++) {
				Q_PUSH(*outbuf, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
				outbuf++;
			}

		}
	}

	for (l=0; l<pCTX->delim_leng; l++) {
		if (q_size == 0 || l == q_size) {

			if (outbuf != NULL && offset >= nbytes_to_write) {
				if (pCTX->cont_offset == 0)
					pCTX->cont_offset = (pCTX->delim_leng + offset);
				else
					pCTX->cont_offset = offset;
				return FRAMEX_CONTINUE;
			}

			r = fread(&b, 1, 1, fpin);
			if (r != 1)
				break;
			offset++;
			if (outbuf != NULL) {
				*outbuf = b; outbuf++; (*n_fill)++;
			}
		}
		else {
			r = -1;
			Q_PEEK(b, queue, qp_s, qp_e + l)
		}


		if (b == pCTX->delim_ptr[l]) {
			if (r == 1)	// »õ·Î ÀÐÀº °æ¿ì¿¡¸¸ PUSHÇÑ´Ù.
				Q_PUSH(b, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
		}
		else {
			if (r != 1)
				// »õ·Î ÀÐÀº °æ¿ì°¡ ¾Æ´Ï¶ó¸é, ÇÏ³ª »©¹ö¸°´Ù.
				Q_POP(c, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
			else if (l > 0) {
				// »õ·Î ÀÐÀº °æ¿ì Áß¿¡¼­, delimiterÀÇ Ã¹ octet(l=0)À» ºñ±³ÇÏ´Â °æ¿ì´Â
				// QUEUE¿¡ ³ÖÁö ¾Ê°í ¹Ù·Î ³Ñ¾î°£´Ù.
				// 2¹øÂ° octetÀÌ»ó(l>0)À» ºñ±³ÇÏ´Â °æ¿ì´Â,
				// ¸Ç ¾Õ¿¡ ÇÏ³ª¸¦ »©°í ¸Ç µÚ¿¡ »õ·Î ÇÏ³ª¸¦ ³Ö´Â´Ù.

				Q_POP(c, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
				Q_PUSH(b, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
			}

			l = -1;
		}

	}

	if (r == 1) {
		if (outbuf != NULL) {
			*n_fill -= pCTX->delim_leng;
			pCTX->cont_offset = 0;
		}

		return FRAMEX_OK;
	}
	else if (r == 0)
		return FRAMEX_ERR_EOS;

	return FRAMEX_ERR_NOTFOUND;
}


// ´ÙÀ½ÀÇ frame¿¡¼­ ¿øÇÏ´Â ¼öÀÇ octets °ªÀ» È®ÀÎ¸¸ ÇÑ´Ù.
static int next_frame_peek(FRAMEX_CTX *pCTX, FILE *fpin, unsigned char *peekbuf, const int peek_size, int *n_fill)
{
	int r = 0;
	int l;
	int nbytes_to_peek;
	int qp_s, qp_e, q_size;
	unsigned char queue[QUEUE_CAPACITY], b, c;

	int offset;

	offset = 0;
	nbytes_to_peek = peek_size;
	Q_INIT(queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)

	// ¸®ÅÏÇÒ outbuf ¹öÆÛÀÇ ¸Ç ¾Õ¿¡ delimiter¸¦ Ã¤¿ö³Ö´Â´Ù.
	if (peekbuf != NULL) {
		if (pCTX->cont_offset == 0) {
			if (nbytes_to_peek <= pCTX->delim_leng) {
				return FRAMEX_ERR_BUFSIZE_TOO_SMALL;
			}

			if (pCTX->delim_insert) {
				memcpy(peekbuf, pCTX->delim_ptr, pCTX->delim_leng);
				peekbuf += pCTX->delim_leng;
				nbytes_to_peek -= pCTX->delim_leng;	// °¡´ÉÇÑ Å©±â¸¦ delimiter ±æÀÌ¸¸Å­ »©¾ßÇÔ
				*n_fill = pCTX->delim_leng;
			}
			else
				*n_fill = 0;
		}
		else {
			// continue·Î µé¾î¿Â °æ¿ì´Â ERROR´Ù.

		}
	}

	for (l=0; l<pCTX->delim_leng; l++) {
		if (q_size == 0 || l == q_size) {

			r = fread(&b, 1, 1, fpin);
			if (r != 1)
				break;
			offset++;
			if (peekbuf != NULL) {
				*peekbuf = b; peekbuf++; (*n_fill)++;
			}

			// fread °³¼ö°¡ peek_size°¡ µÇ¸é, Å»ÃâÇÑ´Ù.
			if (offset >= nbytes_to_peek)
				break;
		}
		else {
			r = -1;
			Q_PEEK(b, queue, qp_s, qp_e + l)
		}


		if (b == pCTX->delim_ptr[l]) {
			if (r == 1)	// »õ·Î ÀÐÀº °æ¿ì¿¡¸¸ PUSHÇÑ´Ù.
				Q_PUSH(b, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
		}
		else {
			if (r != 1)
				// »õ·Î ÀÐÀº °æ¿ì°¡ ¾Æ´Ï¶ó¸é, ÇÏ³ª »©¹ö¸°´Ù.
				Q_POP(c, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
			else if (l > 0) {
				// »õ·Î ÀÐÀº °æ¿ì Áß¿¡¼­, delimiterÀÇ Ã¹ octet(l=0)À» ºñ±³ÇÏ´Â °æ¿ì´Â
				// QUEUE¿¡ ³ÖÁö ¾Ê°í ¹Ù·Î ³Ñ¾î°£´Ù.
				// 2¹øÂ° octetÀÌ»ó(l>0)À» ºñ±³ÇÏ´Â °æ¿ì´Â,
				// ¸Ç ¾Õ¿¡ ÇÏ³ª¸¦ »©°í ¸Ç µÚ¿¡ »õ·Î ÇÏ³ª¸¦ ³Ö´Â´Ù.

				Q_POP(c, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
				Q_PUSH(b, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
			}

			l = -1;
		}

	}

	if (r == 1) {
		if (peekbuf != NULL) {
			*n_fill -= pCTX->delim_leng;
			pCTX->cont_offset = 0;
		}

		fseek(fpin, -(peek_size-pCTX->delim_leng), SEEK_CUR);

		return FRAMEX_OK;
	}
	else if (r == 0)
		return FRAMEX_ERR_EOS;

	return FRAMEX_ERR_NOTFOUND;
}


// ´ÙÀ½ÀÇ delimiter¸¦ Ã£´Â´Ù.
static int next_delimiter_mem(FRAMEX_CTX *pCTX, FRAMEX_STRM_PTR *strm_ptr, unsigned char *outbuf, const int outbuf_size, int *n_fill)
{
	int r = 0;
	int l;
	int nbytes_to_write;
	int qp_s, qp_e, q_size;
	unsigned char queue[QUEUE_CAPACITY], b, c;

	int offset;

	offset = 0;
	nbytes_to_write = outbuf_size;
	Q_INIT(queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)

	// ¸®ÅÏÇÒ outbuf ¹öÆÛÀÇ ¸Ç ¾Õ¿¡ delimiter¸¦ Ã¤¿ö³Ö´Â´Ù.
	if (outbuf != NULL) {
		if (pCTX->cont_offset == 0) {
			if (nbytes_to_write <= pCTX->delim_leng) {
				return FRAMEX_ERR_BUFSIZE_TOO_SMALL;
			}

			if (pCTX->delim_insert) {
				memcpy(outbuf, pCTX->delim_ptr, pCTX->delim_leng);
				outbuf += pCTX->delim_leng;
				nbytes_to_write -= pCTX->delim_leng;	// °¡´ÉÇÑ Å©±â¸¦ delimiter ±æÀÌ¸¸Å­ »©¾ßÇÔ
				*n_fill = pCTX->delim_leng;
			}
			else
				*n_fill = 0;
		}
		else {
			// continue·Î µé¾î¿Â °æ¿ì´Â (pCTX->cont_offset != 0)
			offset = pCTX->cont_offset;
			outbuf += (pCTX->cont_offset - pCTX->delim_leng);

			for (l=0; l<pCTX->delim_leng; l++) {
				Q_PUSH(*outbuf, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
				outbuf++;
			}

		}
	}

	for (l=0; l<pCTX->delim_leng; l++) {
		if (q_size == 0 || l == q_size) {

			if (outbuf != NULL && offset >= nbytes_to_write) {
				if (pCTX->cont_offset == 0)
					pCTX->cont_offset = (pCTX->delim_leng + offset);
				else
					pCTX->cont_offset = offset;
				return FRAMEX_CONTINUE;
			}

			if ((int) strm_ptr->p_cur > (int) strm_ptr->p_end)
				break;
			b = *(strm_ptr->p_cur); strm_ptr->p_cur++; r = 1;
			offset++;
			if (outbuf != NULL) {
				*outbuf = b; outbuf++; (*n_fill)++;
			}
		}
		else {
			r = -1;
			Q_PEEK(b, queue, qp_s, qp_e + l)
		}


		if (b == pCTX->delim_ptr[l]) {
			if (r == 1)	// »õ·Î ÀÐÀº °æ¿ì¿¡¸¸ PUSHÇÑ´Ù.
				Q_PUSH(b, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
		}
		else {
			if (r != 1)
				// »õ·Î ÀÐÀº °æ¿ì°¡ ¾Æ´Ï¶ó¸é, ÇÏ³ª »©¹ö¸°´Ù.
				Q_POP(c, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
			else if (l > 0) {
				// »õ·Î ÀÐÀº °æ¿ì Áß¿¡¼­, delimiterÀÇ Ã¹ octet(l=0)À» ºñ±³ÇÏ´Â °æ¿ì´Â
				// QUEUE¿¡ ³ÖÁö ¾Ê°í ¹Ù·Î ³Ñ¾î°£´Ù.
				// 2¹øÂ° octetÀÌ»ó(l>0)À» ºñ±³ÇÏ´Â °æ¿ì´Â,
				// ¸Ç ¾Õ¿¡ ÇÏ³ª¸¦ »©°í ¸Ç µÚ¿¡ »õ·Î ÇÏ³ª¸¦ ³Ö´Â´Ù.

				Q_POP(c, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
				Q_PUSH(b, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
			}

			l = -1;
		}

	}

	if (r == 1) {
		if (outbuf != NULL) {
			*n_fill -= pCTX->delim_leng;
			pCTX->cont_offset = 0;
		}

		return FRAMEX_OK;
	}
	else if (r == 0)
		return FRAMEX_ERR_EOS;

	return FRAMEX_ERR_NOTFOUND;
}


// ´ÙÀ½ÀÇ frame¿¡¼­ ¿øÇÏ´Â ¼öÀÇ octets °ªÀ» È®ÀÎ¸¸ ÇÑ´Ù.
static int next_frame_peek_mem(FRAMEX_CTX *pCTX, FRAMEX_STRM_PTR *strm_ptr, unsigned char *peekbuf, const int peek_size, int *n_fill)
{
	int r = 0;
	int l;
	int nbytes_to_peek;
	int qp_s, qp_e, q_size;
	unsigned char queue[QUEUE_CAPACITY], b, c;

	int offset;

	offset = 0;
	nbytes_to_peek = peek_size;
	Q_INIT(queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)

	// ¸®ÅÏÇÒ outbuf ¹öÆÛÀÇ ¸Ç ¾Õ¿¡ delimiter¸¦ Ã¤¿ö³Ö´Â´Ù.
	if (peekbuf != NULL) {
		if (pCTX->cont_offset == 0) {
			if (nbytes_to_peek <= pCTX->delim_leng) {
				return FRAMEX_ERR_BUFSIZE_TOO_SMALL;
			}

			if (pCTX->delim_insert) {
				memcpy(peekbuf, pCTX->delim_ptr, pCTX->delim_leng);
				peekbuf += pCTX->delim_leng;
				nbytes_to_peek -= pCTX->delim_leng;	// °¡´ÉÇÑ Å©±â¸¦ delimiter ±æÀÌ¸¸Å­ »©¾ßÇÔ
				*n_fill = pCTX->delim_leng;
			}
			else
				*n_fill = 0;
		}
		else {
			// continue·Î µé¾î¿Â °æ¿ì´Â ERROR´Ù.

		}
	}

	for (l=0; l<pCTX->delim_leng; l++) {
		if (q_size == 0 || l == q_size) {

			if ((int) strm_ptr->p_cur > (int) strm_ptr->p_end)
				break;
			b = *(strm_ptr->p_cur); strm_ptr->p_cur++; r = 1;
			offset++;
			if (peekbuf != NULL) {
				*peekbuf = b; peekbuf++; (*n_fill)++;
			}

			// fread °³¼ö°¡ peek_size°¡ µÇ¸é, Å»ÃâÇÑ´Ù.
			if (offset >= nbytes_to_peek)
				break;
		}
		else {
			r = -1;
			Q_PEEK(b, queue, qp_s, qp_e + l)
		}


		if (b == pCTX->delim_ptr[l]) {
			if (r == 1)	// »õ·Î ÀÐÀº °æ¿ì¿¡¸¸ PUSHÇÑ´Ù.
				Q_PUSH(b, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
		}
		else {
			if (r != 1)
				// »õ·Î ÀÐÀº °æ¿ì°¡ ¾Æ´Ï¶ó¸é, ÇÏ³ª »©¹ö¸°´Ù.
				Q_POP(c, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
			else if (l > 0) {
				// »õ·Î ÀÐÀº °æ¿ì Áß¿¡¼­, delimiterÀÇ Ã¹ octet(l=0)À» ºñ±³ÇÏ´Â °æ¿ì´Â
				// QUEUE¿¡ ³ÖÁö ¾Ê°í ¹Ù·Î ³Ñ¾î°£´Ù.
				// 2¹øÂ° octetÀÌ»ó(l>0)À» ºñ±³ÇÏ´Â °æ¿ì´Â,
				// ¸Ç ¾Õ¿¡ ÇÏ³ª¸¦ »©°í ¸Ç µÚ¿¡ »õ·Î ÇÏ³ª¸¦ ³Ö´Â´Ù.

				Q_POP(c, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
				Q_PUSH(b, queue, qp_s, qp_e, q_size, QUEUE_CAPACITY)
			}

			l = -1;
		}

	}

	if (r == 1) {
		if (peekbuf != NULL) {
			*n_fill -= pCTX->delim_leng;
			pCTX->cont_offset = 0;
		}

//		fseek(fpin, -(peek_size-pCTX->delim_leng), SEEK_CUR);
		strm_ptr->p_cur = (strm_ptr->p_cur  -  (peek_size-pCTX->delim_leng));

		return FRAMEX_OK;
	}
	else if (r == 0)
		return FRAMEX_ERR_EOS;

	return FRAMEX_ERR_NOTFOUND;
}


//
// int FrameExtractorFirst(FRAMEX_CTX *pCTX, FILE *fpin)
//
// Description
//		Á¦ÀÏ Ã³À½ÀÇ delimiter°¡ ³ª¿À´Â °ÍÀ» Ã£´Â´Ù.
// Return
//
// Parameters
//
int FrameExtractorFirst(FRAMEX_CTX *pCTX, FRAMEX_IN in)
{
	if (pCTX == NULL)
		return FRAMEX_INVALID_PARAM;

	if (pCTX->in_type == FRAMEX_IN_TYPE_FILE)
		return next_delimiter(pCTX, (FILE *) in, NULL, 0, NULL);
	else
		return next_delimiter_mem(pCTX, (FRAMEX_STRM_PTR *) in, NULL, 0, NULL);
}

//
// int FrameExtractorNext(FRAMEX_CTX *pCTX, FILE *fpin, unsigned char outbuf[], int outbuf_size, int *n_fill)
//
// Description
//		FrameExtractorFirst() ÇÔ¼ö¸¦ È£ÃâÇÏ¿© Ã¹ frameÀÇ delimiter¸¦ Ã£Àº ÈÄºÎÅÍ È£ÃâÇÑ´Ù.
//		´ÙÀ½ frameÀÇ delimiter°¡ ³ª¿Ã¶§±îÁöÀÇ frameÀ» ÃßÃâÇÏ¿©, outbuf¿¡ º¹»çÇÑ´Ù.
// Return
//
// Parameters
//		outbuf     [OUT]: pointer to the array that will be filled with the frame data
//		outbuf_size[IN] : length of the outbuf size
//		n_fill     [OUT]: number of octets filled in the out_buf
//
int FrameExtractorNext(FRAMEX_CTX *pCTX, FRAMEX_IN in, unsigned char outbuf[], int outbuf_size, int *n_fill)
{
	if (pCTX == NULL)
		return FRAMEX_INVALID_PARAM;

	if (pCTX->in_type == FRAMEX_IN_TYPE_FILE)
		return next_delimiter(pCTX, (FILE *) in, outbuf, outbuf_size, n_fill);
	else
		return next_delimiter_mem(pCTX, (FRAMEX_STRM_PTR *) in, outbuf, outbuf_size, n_fill);
}


//
// int FrameExtractorPeek(FRAMEX_CTX *pCTX, FILE *fpin, unsigned char peekbuf[], int peek_size, int *n_fill)
//
// Description
//		FrameExtractorFirst() ÇÔ¼ö¸¦ È£ÃâÇÏ¿© Ã¹ frameÀÇ delimiter¸¦ Ã£Àº ÈÄºÎÅÍ È£ÃâÇÑ´Ù.
//		´ÙÀ½ frameÀÇ °ªÀ» peek_size¸¸Å­ peekbuf¿¡ º¹»çÇÑ´Ù.
//		ÀÌ ¶§, Buffer¸¦ PEEK¸¸ ÇÏ´Â °ÍÀÌ¹Ç·Î, frameÀ» ÃßÃâÇÏ´Â °ÍÀº ¾Æ´Ï´Ù.
// Return
//
// Parameters
//		peekbuf    [OUT]: pointer to the array that will be filled with the frame data
//		peek_size  [IN] : length of the outbuf size
//		n_fill     [OUT]: number of octets filled in the out_buf
//
int FrameExtractorPeek(FRAMEX_CTX *pCTX, FRAMEX_IN in, unsigned char peekbuf[], int peek_size, int *n_fill)
{
	if (pCTX == NULL)
		return FRAMEX_INVALID_PARAM;

	if (pCTX->in_type == FRAMEX_IN_TYPE_FILE)
		return next_frame_peek(pCTX, (FILE *) in, peekbuf, peek_size, n_fill);
	else
		return next_frame_peek_mem(pCTX, (FRAMEX_STRM_PTR *) in, peekbuf, peek_size, n_fill);
}



//
// int FrameExtractorFinal(FRAMEX_CTX *pCTX)
//
// Description
//		FRAMEX_CTX ±¸Á¶Ã¼ µ¥ÀÌÅÍ¸¦ ÇØÁ¦ÇÑ´Ù.
// Return
//
// Parameters
//
int FrameExtractorFinal(FRAMEX_CTX *pCTX)
{
	if (pCTX == NULL)
		return FRAMEX_INVALID_PARAM;

	free(pCTX->delim_ptr);
	free(pCTX);

	return 0;
}


