/**
 * Base 64 encoding functions 
 * 
 * Functions developed by user ryyst on StackOverflow.com
 * http://stackoverflow.com/questions/342409/
 * Accessed on April 28, 2016
 *
 * Comments added by Michael Carter
 */

#include <stdint.h>
#include <stdlib.h>


static const char encoding_table[] =
  {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
   'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
   'w', 'x', 'y', 'z', '0', '1', '2', '3',
   '4', '5', '6', '7', '8', '9', '+', '/'};

static int mod_table[] = {0, 2, 1};


char *base64_encode(const uint8_t *data,
                    size_t data_len,
                    size_t *out_len) {
  int i, j;
  char *encoding;
  uint32_t octet_a, octet_b, octet_c, triple;

  *out_len = 4 * ((data_len + 2) / 3);

  encoding = malloc(*out_len + 1);
  if (encoding == NULL) return NULL;

  for (i = 0, j = 0; i < data_len;) {

	octet_a = i < data_len ? (unsigned char)data[i++] : 0;
	octet_b = i < data_len ? (unsigned char)data[i++] : 0;
	octet_c = i < data_len ? (unsigned char)data[i++] : 0;

	triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

	encoding[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
	encoding[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
	encoding[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
	encoding[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
  }

  for (i = 0; i < mod_table[data_len % 3]; i++)
	encoding[*out_len - 1 - i] = '=';

  encoding[*out_len] = '\0';

  return encoding;
}


uint8_t *base64_decode(const char *encoding,
							 size_t enc_len,
							 size_t *data_len) { 
  int i, j;
  uint8_t *data, *decoding_table;
  uint32_t sextet_a, sextet_b, sextet_c, sextet_d;
  uint32_t triple;
  
  decoding_table = malloc(256);
  for (i = 0; i < 64; i++)
	  decoding_table[(unsigned char) encoding_table[i]] = i;

  if (enc_len % 4 != 0) return NULL;

  *data_len = enc_len / 4 * 3;
  if (encoding[enc_len - 1] == '=') (*data_len)--;
  if (encoding[enc_len - 2] == '=') (*data_len)--;

  data = malloc(*data_len);
  if (data == NULL) return NULL;

  for (i = 0, j = 0; i < enc_len;) {
	sextet_a = encoding[i] == '=' ?
	  0 & i++ : decoding_table[(int)encoding[i++]];
	sextet_b = encoding[i] == '=' ?
	  0 & i++ : decoding_table[(int)encoding[i++]];
	sextet_c = encoding[i] == '=' ?
	  0 & i++ : decoding_table[(int)encoding[i++]];
	sextet_d = encoding[i] == '=' ?
	  0 & i++ : decoding_table[(int)encoding[i++]];

	triple = (sextet_a << 3 * 6)
	  + (sextet_b << 2 * 6)
	  + (sextet_c << 1 * 6)
	  + (sextet_d << 0 * 6);

	if (j < *data_len) data[j++] = (triple >> 16) & 0xFF;
	if (j < *data_len) data[j++] = (triple >> 8) & 0xFF;
	if (j < *data_len) data[j++] = triple & 0xFF;
  }

  free(decoding_table);

  return data;
}
