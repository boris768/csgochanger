#pragma once

namespace SourceEngine
{
    typedef unsigned long CRC32_t;

	constexpr unsigned int MD5_DIGEST_LENGTH = 16;
	struct MD5Value_t
	{
		unsigned char bits[MD5_DIGEST_LENGTH];

		void Zero();
		bool IsZero() const;

		bool operator==(const MD5Value_t& src) const
		{
			for(unsigned int i = 0; i < MD5_DIGEST_LENGTH; ++i)
			{
				if (bits[i] != src.bits[i])
					return false;
			}
			return true;
		}
		bool operator!=(const MD5Value_t& src) const
		{
			for (unsigned int i = 0; i < MD5_DIGEST_LENGTH; ++i)
			{
				if (bits[i] != src.bits[i])
					return true;
			}
			return false;
			
		}

	};
	
    void      CRC32_Init(CRC32_t *pulCRC);
    void      CRC32_ProcessBuffer(CRC32_t *pulCRC, const void *p, int nBuffer);
    void      CRC32_Final(CRC32_t *pulCRC);
    CRC32_t	CRC32_GetTableEntry(unsigned int slot);

    inline CRC32_t CRC32_ProcessSingleBuffer(const void *p, int len)
    {
        CRC32_t crc;

        CRC32_Init(&crc);
        CRC32_ProcessBuffer(&crc, p, len);
        CRC32_Final(&crc);

        return crc;
    }

}