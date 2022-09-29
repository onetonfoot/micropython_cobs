// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"

// https://forum.micropython.org/viewtopic.php?t=5622
// https://micropython-usermod.readthedocs.io/en/latest/index.html
// https://github.com/micropython/micropython/issues/2784

/** COBS encode data to buffer
	@param data Pointer to input data to encode
	@param length Number of bytes to encode
	@param buffer Pointer to encoded output buffer
	@return Encoded buffer length in bytes
	@note Does not output delimiter byte
*/
size_t cobs_encode(const void *data, size_t length, uint8_t *buffer)
{
	assert(data && buffer);

	uint8_t *encode = buffer; // Encoded byte pointer
	uint8_t *codep = encode++; // Output code pointer
	uint8_t code = 1; // Code value

	for (const uint8_t *byte = (const uint8_t *)data; length--; ++byte)
	{
		if (*byte) // Byte not zero, write it
			*encode++ = *byte, ++code;

		if (!*byte || code == 0xff) // Input is zero or block completed, restart
		{
			*codep = code, code = 1, codep = encode;
			if (!*byte || length)
				++encode;
		}
	}
	*codep = code; // Write final code value
	size_t n = (size_t)(encode - buffer);
	buffer[n] = 0; // Add final 0
	n +=1;

	return n;
}

/** COBS decode data from buffer
	@param buffer Pointer to encoded input bytes
	@param length Number of bytes to decode
	@param data Pointer to decoded output data
	@return Number of bytes successfully decoded
	@note Stops decoding if delimiter byte is found
*/
size_t cobs_decode(const uint8_t *buffer, size_t length, void *data)
{
	assert(buffer && data);

	const uint8_t *byte = buffer; // Encoded input byte pointer
	uint8_t *decode = (uint8_t *)data; // Decoded output byte pointer

	for (uint8_t code = 0xff, block = 0; byte < buffer + length; --block)
	{
		if (block) // Decode block byte
			*decode++ = *byte++;
		else
		{
			if (code != 0xff) // Encoded zero, write it
				*decode++ = 0;
			block = code = *byte++; // Next block length
			if (!code) // Delimiter code found
				break;
		}
	}

	return (size_t)(decode - (uint8_t *)data);
}

STATIC mp_obj_t encode(mp_obj_t in_obj) {
    mp_buffer_info_t bufinfo;
    mp_uint_t flags = 1;
    // This be used to get at the data for a bytestring/bytearray/string.
    mp_get_buffer_raise(in_obj, &bufinfo, flags);
	// This should be celi(bufinfo.len / 254) + 1 but we are getting issues compiling
	size_t maxlen =  bufinfo.len + 2;
    uint8_t buf[(size_t)maxlen];
    size_t n = cobs_encode(bufinfo.buf, bufinfo.len, buf);
	return mp_obj_new_bytes(buf, n);
}

STATIC mp_obj_t decode(mp_obj_t in_obj) {
    mp_buffer_info_t bufinfo;
    mp_uint_t flags = 1;
    // Can be used to get at the data for a bytestring/bytearray/string.
    mp_get_buffer_raise(in_obj, &bufinfo, flags);
    uint8_t buf[bufinfo.len];
    size_t n = cobs_decode(bufinfo.buf, bufinfo.len, buf);
    return mp_obj_new_bytes(buf, n);
}

// Define a Python reference to the function above
STATIC MP_DEFINE_CONST_FUN_OBJ_1(encode_obj, encode);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(decode_obj, decode);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Make the function available in the module's namespace
    mp_store_global(MP_QSTR_encode, MP_OBJ_FROM_PTR(&encode_obj));
    mp_store_global(MP_QSTR_decode, MP_OBJ_FROM_PTR(&decode_obj));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}