#ifndef X3D_FI_COMPRESSIONTOOLS_H
#define X3D_FI_COMPRESSIONTOOLS_H

#include <vector>

namespace XIOT {

	/**
	 * Contains some not exported helper classes to compress/uncompress
	 * data using X3D specific encoding algorithms
	 */
	namespace FITools {

	struct BitPacker {
			BitPacker();

			void pack(unsigned long value, unsigned long num_bits_to_pack);
			void get_result(char **data_return, int *len_return);

			int next_bit_to_write;   
			std::vector<unsigned char> buffer;
	};

	struct BitUnpacker {
			BitUnpacker(unsigned char *data, size_t len);

			void set_input(unsigned char *data, int len);
			unsigned long unpack(unsigned long num_bits);

			unsigned int num_bits_remaining;
			int next_bit_to_read;

			std::vector<unsigned char> buffer;
	};

	class FloatPacker {

		public:
			FloatPacker(unsigned long exponentBits, unsigned long mantissaBits);
			float decode(unsigned long src, bool isSigned = true);

		protected:
			unsigned long _exponentBits;
			unsigned long _mantissaBits;

			unsigned int _exponent_bias;			unsigned int _sign_shift;			unsigned int _sign_mask;			unsigned int _exponent_mask;			unsigned int _mantissa_mask;			int _exponent_max;			int _exponent_min;

	};

	} // End namespace FI
} // End namespace X3D

#endif

